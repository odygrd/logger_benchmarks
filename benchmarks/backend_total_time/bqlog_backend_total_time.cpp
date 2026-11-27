#define FMTLOG_BLOCK 1

#include "fmt/format.h"
#include "bq_log/bq_log.h"
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
  // we need to construct the filename because bqlog will append the date....

  std::time_t now = std::time(nullptr);
  std::tm* local_time = std::localtime(&now);

  char date_str[9];  // Buffer to hold the formatted date
  std::strftime(date_str, sizeof(date_str), "%Y%m%d", local_time);

  std::string log_file = fmt::format("benchmark_bqlog_backend_total_time_{}_1.log", date_str);

  set_thread_affinity(5);

  // BqLog does not expose its backend worker affinity. tools/run_all.py starts this process on
  // CPU 5 so the worker inherits that affinity before this caller thread moves to CPU 1.
  // On Windows the worker is pinned explicitly instead.

  bq::log log_obj = bq::log::create_log("text", R"(
        log.buffer_size=1048576
        log.worker_interval_ms=0
        log.thread_mode=async
        appenders_config.appender_0.type=text_file
        appenders_config.appender_0.levels=[all]
        appenders_config.appender_0.file_name= benchmark_bqlog_backend_total_time
    )");

  pin_other_threads_on_windows(5);
  set_thread_affinity(1);

  BQ_LOG_FAST_INFO(log_obj, "Warm up");
  log_obj.force_flush();

  // start counting the time until backend worker finishes
  auto log_statement = [&](size_t iteration)
  {
    // Cycle between five log statements: an application does not repeat a single one
    switch (iteration % 5)
    {
    case 0:
      BQ_LOG_FAST_INFO(log_obj, "Iteration: {} int: {} double: {}", iteration, iteration * 2,
             static_cast<double>(iteration) / 2);
      break;
    case 1:
      BQ_LOG_FAST_INFO(log_obj, "Order: {} quantity: {} price: {}", iteration, iteration * 2,
             static_cast<double>(iteration) / 2);
      break;
    case 2:
      BQ_LOG_FAST_INFO(log_obj, "Request: {} bytes: {} elapsed: {}", iteration, iteration * 2,
             static_cast<double>(iteration) / 2);
      break;
    case 3:
      BQ_LOG_FAST_INFO(log_obj, "Session: {} sequence: {} latency: {}", iteration, iteration * 2,
             static_cast<double>(iteration) / 2);
      break;
    default:
      BQ_LOG_FAST_INFO(log_obj, "Worker: {} processed: {} load: {}", iteration, iteration * 2,
             static_cast<double>(iteration) / 2);
      break;
    }
  };

  // Each producer logs a warm-up line first, so its per-thread state is created before timing
  auto on_producer_start = [&](size_t) { BQ_LOG_FAST_INFO(log_obj, "Warm up"); };

  ProducerTimes const times = run_producers(on_producer_start, log_statement);
  auto const start_time = times.start;
  BQ_LOG_FAST_INFO(log_obj, "End");

  // block until all messages are flushed
  log_obj.force_flush();

  auto const end_time = std::chrono::steady_clock::now();
  print_throughput(times, end_time, static_cast<double>(std::filesystem::file_size(log_file)) / (1024 * 1024));
}
