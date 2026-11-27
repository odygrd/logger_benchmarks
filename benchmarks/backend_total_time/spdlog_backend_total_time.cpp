#include "spdlog/async.h"
#include "spdlog/fmt/ranges.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

#include <atomic>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include <chrono>
#include <filesystem>
#include <iostream>

#include "utils.h"

#include "spdlog/details/file_helper.h"
#include "spdlog/details/null_mutex.h"
#include "spdlog/sinks/base_sink.h"

/**
 * A file sink that records each completed flush, so the benchmark can stop the clock as soon as the
 * worker has written the last message, without timing the thread pool's shutdown.
 * spdlog's async flush() only queues the flush and returns.
 */
class flush_notifying_file_sink final : public spdlog::sinks::base_sink<spdlog::details::null_mutex>
{
public:
  explicit flush_notifying_file_sink(spdlog::filename_t const& filename)
  {
    _file_helper.open(filename, true);
  }

  size_t flush_count() const noexcept { return _flush_count.load(std::memory_order_acquire); }

protected:
  void sink_it_(spdlog::details::log_msg const& msg) override
  {
    spdlog::memory_buf_t formatted;
    formatter_->format(msg, formatted);
    _file_helper.write(formatted);
  }

  void flush_() override
  {
    _file_helper.flush();
    _flush_count.fetch_add(1, std::memory_order_release);
  }

private:
  spdlog::details::file_helper _file_helper;
  std::atomic<size_t> _flush_count{0};
};

/**
 * The backend worker just spins, so we just measure the total time elapsed for total_iterations
 */
int main()
{
  set_thread_affinity(1);

  std::filesystem::path log_file{"benchmark_spdlog_backend_total_time.log"};
  std::remove(log_file.string().c_str());

  // Setup
  spdlog::set_automatic_registration(false);

  auto on_backend_start = []()
  {
    // pin to cpu backend logging thread.
    set_thread_affinity(5);
  };

  // spdlog's default queue of 8,192 messages and one worker thread
  spdlog::init_thread_pool(spdlog::details::default_async_q_size, 1, on_backend_start);

  auto sink = std::make_shared<flush_notifying_file_sink>(log_file.string());
  auto logger = std::make_shared<spdlog::async_logger>("bench_logger", sink, spdlog::thread_pool(),
                                                       spdlog::async_overflow_policy::block);
  logger->set_pattern("%T.%F [%t] %s:%# %l %v");

  SPDLOG_LOGGER_INFO(logger, "Warm up");
  logger->flush();

  auto spdlog_tp = spdlog::thread_pool();

  // Wait for the flush to be processed (queue empty again)
  while (spdlog_tp->queue_size() != 0) {
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }

  // start counting the time until backend worker finishes
  auto log_statement = [&](size_t iteration)
  {
    // Cycle between five log statements: an application does not repeat a single one
    switch (iteration % 5)
    {
    case 0:
      SPDLOG_LOGGER_INFO(logger, "Iteration: {} int: {} double: {}", iteration, iteration * 2,
                         static_cast<double>(iteration) / 2);
      break;
    case 1:
      SPDLOG_LOGGER_INFO(logger, "Order: {} quantity: {} price: {}", iteration, iteration * 2,
                         static_cast<double>(iteration) / 2);
      break;
    case 2:
      SPDLOG_LOGGER_INFO(logger, "Request: {} bytes: {} elapsed: {}", iteration, iteration * 2,
                         static_cast<double>(iteration) / 2);
      break;
    case 3:
      SPDLOG_LOGGER_INFO(logger, "Session: {} sequence: {} latency: {}", iteration, iteration * 2,
                         static_cast<double>(iteration) / 2);
      break;
    default:
      SPDLOG_LOGGER_INFO(logger, "Worker: {} processed: {} load: {}", iteration, iteration * 2,
                         static_cast<double>(iteration) / 2);
      break;
    }
  };

  // Each producer logs a warm-up line first, so its per-thread state is created before timing
  auto on_producer_start = [&](size_t) { SPDLOG_LOGGER_INFO(logger, "Warm up"); };

  ProducerTimes const times = run_producers(on_producer_start, log_statement);
  auto const start_time = times.start;

  SPDLOG_LOGGER_ERROR(logger, "End");

  // Queue a flush after the final record and wait until the worker has done it. The queue is
  // FIFO with one worker, so every message has been written by then.
  size_t const flushes_before = sink->flush_count();
  logger->flush();
  while (sink->flush_count() == flushes_before)
  {
    std::this_thread::yield();
  }

  auto const end_time = std::chrono::steady_clock::now();

  logger.reset();
  spdlog_tp.reset();
  spdlog::shutdown();
  print_throughput(times, end_time, static_cast<double>(std::filesystem::file_size(log_file)) / (1024 * 1024));
}
