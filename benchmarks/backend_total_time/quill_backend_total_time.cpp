#include "quill/Backend.h"
#include "quill/Frontend.h"
#include "quill/LogMacros.h"
#include "quill/Logger.h"
#include "quill/sinks/FileSink.h"
#include "quill/bundled/fmt/format.h"

#include <filesystem>

#include "utils.h"

/**
 * A bounded blocking queue of 1 MiB per thread, the queue size every library is given where it can
 * be set. Quill's default unbounded queue would grow during the run instead.
 */
struct ThroughputFrontendOptions : quill::FrontendOptions
{
  static constexpr quill::QueueType queue_type = quill::QueueType::BoundedBlocking;
  static constexpr size_t initial_queue_capacity = 1024 * 1024;
};

using frontend_t = quill::FrontendImpl<ThroughputFrontendOptions>;
using logger_t = quill::LoggerImpl<ThroughputFrontendOptions>;

#include <chrono>
#include <iostream>

/**
 * The backend worker just spins, so we just measure the total time elapsed for total_iterations
 */
int main()
{
  std::filesystem::path log_file{"benchmark_quill_backend_total_time.log"};
  std::remove(log_file.string().c_str());

  // main thread affinity
  quill::detail::set_cpu_affinity({static_cast<uint16_t>(bench_cpu(1))});

  // Setup
  quill::BackendOptions backend_options;
  backend_options.cpu_affinity = {static_cast<uint16_t>(bench_cpu(5))};
  backend_options.sleep_duration = std::chrono::nanoseconds{0};
  quill::Backend::start(backend_options);

  // Frontend
  auto file_sink = frontend_t::create_or_get_sink<quill::FileSink>(
    log_file,
    []()
    {
      quill::FileSinkConfig cfg;
      cfg.set_open_mode('w');
      cfg.set_filename_append_option(quill::FilenameAppendOption::None);
      return cfg;
    }(),
    quill::FileEventNotifier{});

  logger_t* logger = frontend_t::create_or_get_logger(
    "root", std::move(file_sink),
    quill::PatternFormatterOptions{
      "%(time) [%(thread_id)] %(short_source_location) %(log_level) %(message)", "%H:%M:%S.%Qns",
      quill::Timezone::GmtTime, false});

  LOG_INFO(logger, "Warm up");
  logger->flush_log(0);

  // start counting the time until backend worker finishes
#if defined(BENCH_MANY_STATEMENTS)
  // 256 distinct statements, see many_statements.h
  #define BENCH_LOG(...) LOG_INFO(logger, __VA_ARGS__)
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
      LOG_INFO(logger, "Iteration: {} int: {} double: {}", iteration, iteration * 2,
               static_cast<double>(iteration) / 2);
      break;
    case 1:
      LOG_INFO(logger, "Order: {} quantity: {} price: {}", iteration, iteration * 2,
               static_cast<double>(iteration) / 2);
      break;
    case 2:
      LOG_INFO(logger, "Request: {} bytes: {} elapsed: {}", iteration, iteration * 2,
               static_cast<double>(iteration) / 2);
      break;
    case 3:
      LOG_INFO(logger, "Session: {} sequence: {} latency: {}", iteration, iteration * 2,
               static_cast<double>(iteration) / 2);
      break;
    default:
      LOG_INFO(logger, "Worker: {} processed: {} load: {}", iteration, iteration * 2,
               static_cast<double>(iteration) / 2);
      break;
    }
  };
#endif

  // Each producer logs a warm-up line first, so its per-thread state is created before timing
  auto on_producer_start = [&](size_t) { LOG_INFO(logger, "Warm up"); };

  ProducerTimes const times = run_producers(on_producer_start, log_statement);
  auto const start_time = times.start;
  LOG_ERROR(logger, "End");

  // block until all messages are flushed
  logger->flush_log(0);

  auto const end_time = std::chrono::steady_clock::now();
  print_throughput(times, end_time, static_cast<double>(std::filesystem::file_size(log_file)) / (1024 * 1024));
}
