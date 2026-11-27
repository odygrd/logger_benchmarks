#include "binlog/binlog.hpp"
#include "fmt/format.h"
#include <atomic>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include <chrono>
#include <iostream>

#include "utils.h"

/**
 * The backend worker just spins, so we just measure the total time elapsed for total_iterations
 */
int main()
{
  set_thread_affinity(1);

  std::filesystem::path log_file{"benchmark_ms_binlog_backend_total_time.blog"};
  std::remove(log_file.string().c_str());

  // Start a binlog backend logging threads to consume the logs
  std::atomic<bool> done{false};

  std::thread backend(
    [&done, &log_file]()
    {
      std::ofstream logfile(log_file.c_str(), std::ofstream::out | std::ofstream::binary);

      // pin to cpu
      set_thread_affinity(5);

      binlog::Session::ConsumeResult consume_result;
      while (true)
      {
        consume_result = binlog::consume(logfile);

        if (done.load(std::memory_order_acquire) && consume_result.bytesConsumed == 0)
        {
          break;
        }
      }
    });

  BINLOG_INFO("Warm up");
  std::this_thread::sleep_for(std::chrono::seconds{1});

  // start counting the time until backend worker finishes
#if defined(BENCH_MANY_STATEMENTS)
  // 256 distinct statements, see many_statements.h
  #define BENCH_LOG(...) BINLOG_INFO(__VA_ARGS__)
  auto log_statement = [&](size_t iteration)
  {
  #include "many_statements/fmt.inc"
  };
#else
  auto log_statement = [&](size_t iteration)
  {
    // Cycle between five log statements: an application does not repeat a single one
    switch (iteration % 5)
    {
    case 0:
      BINLOG_INFO("Iteration: {} int: {} double: {}", iteration, iteration * 2,
                  static_cast<double>(iteration) / 2);
      break;
    case 1:
      BINLOG_INFO("Order: {} quantity: {} price: {}", iteration, iteration * 2,
                  static_cast<double>(iteration) / 2);
      break;
    case 2:
      BINLOG_INFO("Request: {} bytes: {} elapsed: {}", iteration, iteration * 2,
                  static_cast<double>(iteration) / 2);
      break;
    case 3:
      BINLOG_INFO("Session: {} sequence: {} latency: {}", iteration, iteration * 2,
                  static_cast<double>(iteration) / 2);
      break;
    default:
      BINLOG_INFO("Worker: {} processed: {} load: {}", iteration, iteration * 2,
                  static_cast<double>(iteration) / 2);
      break;
    }
  };
#endif

  // Each producer logs a warm-up line first, so its per-thread state is created before timing
  auto on_producer_start = [&](size_t) { BINLOG_INFO("Warm up"); };

  ProducerTimes const times = run_producers(on_producer_start, log_statement);
  auto const start_time = times.start;
  BINLOG_ERROR("End");

  // block until all messages are flushed
  done.store(true, std::memory_order_release);
  backend.join();

  auto const end_time = std::chrono::steady_clock::now();
  print_throughput(times, end_time, static_cast<double>(std::filesystem::file_size(log_file)) / (1024 * 1024));
}
