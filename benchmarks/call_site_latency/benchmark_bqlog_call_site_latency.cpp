#include "call_site_latency_bench.h"
#include "bq_log/bq_log.h"
#include <atomic>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

/***/
void bqlog_benchmark(std::vector<int32_t> thread_count_array, size_t num_iterations_per_thread)
{
  // BqLog does not expose its backend worker affinity. tools/run_all.py starts this process on
  // CPU 5 so the worker inherits that affinity before the benchmark moves caller threads.
  // On Windows the worker is pinned explicitly instead.

// The worker never sleeps, the most responsive setting every library is given where it has one
#define BQLOG_WORKER_CONFIG "log.worker_interval_ms=0\n"
#if defined(BENCH_SCENARIO_DISABLED)
  #define BQLOG_LEVELS "[info,warning,error,fatal]"
#else
  #define BQLOG_LEVELS "[all]"
#endif
  bq::log log_obj = bq::log::create_log("text", BQLOG_WORKER_CONFIG R"(
        log.thread_mode=async
        log.reliable_level=normal
        appenders_config.appender_0.type=text_file
        appenders_config.appender_0.levels=)" BQLOG_LEVELS R"(
        appenders_config.appender_0.file_name= bqlog_call_site_latency_percentile_linux_benchmark.log
        appenders_config.appender_0.capacity_limit= 1
    )");

  pin_other_threads_on_windows(5);

#ifdef BENCH_SCENARIO_DISABLED
  // A statement below the logger's level
  auto log_func = [&log_obj](uint64_t i, uint64_t j, double d)
  { BQ_LOG_FAST_DEBUG(log_obj, "Logging int: {}, int: {}, double: {}", i, j, BENCH_DISABLED_ARG(d)); };
#elif defined(BENCH_INT_INT_DOUBLE)
  auto log_func = [&log_obj](uint64_t i, uint64_t j, double d)
  { BQ_LOG_FAST_INFO(log_obj, "Logging int: {}, int: {}, double: {}", i, j, d); };
#elif defined(BENCH_INT_INT_LARGESTR)
  auto log_func = [&log_obj](uint64_t i, uint64_t j, std::string const& s)
  { BQ_LOG_FAST_INFO(log_obj, "Logging int: {}, int: {}, string: {}", i, j, s); };
#elif defined(BENCH_VECTOR_LARGESTR)
  auto log_func = [&log_obj](uint64_t i, uint64_t j, std::vector<std::string> const& s)
  { /** BQ_LOG_FAST_INFO(log_obj, "Logging int: {}, int: {}, vector: {}", i, j, s); **/ };
#endif

  auto on_start = [&log_obj]()
  {
    BQ_LOG_FAST_INFO(log_obj, "Warm up");
    log_obj.force_flush();
  };

  auto on_exit = []() { };

  // Run the benchmark for n threads
  for (auto thread_count : thread_count_array)
  {
    run_benchmark("Logger: bqlog - Benchmark: Caller Thread Latency", thread_count,
                  num_iterations_per_thread, MESSAGES_PER_ITERATION, on_start, log_func, on_exit);
  }
}

/***/
int main(int argc, char* argv[]) { bqlog_benchmark(THREAD_LIST_COUNT, ITERATIONS); }
