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
  // Seems there is no way to set the backend worker affinity for bqlog
  // I have edited `bg_log/log/log_worker.cpp:102` so that the backend thread pins to cpu 5

  bq::log log_obj = bq::log::create_log("text", R"(
        log.buffer_size=131072
        log.thread_mode=async
        log.reliable_level=normal
        appenders_config.appender_0.type=text_file
        appenders_config.appender_0.levels=[all]
        appenders_config.appender_0.file_name= bqlog_call_site_latency_percentile_linux_benchmark.log
        appenders_config.appender_0.capacity_limit= 1
    )");

#ifdef BENCH_INT_INT_DOUBLE
  auto log_func = [&log_obj](uint64_t i, uint64_t j, double d)
  { log_obj.info("Logging int: {}, int: {}, double: {}", i, j, d); };
#elif defined(BENCH_INT_INT_LARGESTR)
  auto log_func = [&log_obj](uint64_t i, uint64_t j, std::string const& s)
  { log_obj.info("Logging int: {}, int: {}, string: {}", i, j, s); };
#elif defined(BENCH_VECTOR_LARGESTR)
  auto log_func = [&log_obj](uint64_t i, uint64_t j, std::vector<std::string> const& s)
  { /** log_obj.info("Logging int: {}, int: {}, vector: {}", i, j, s); **/ };
#endif

  auto on_start = [&log_obj]()
  {
    log_obj.info("Warm up");
    log_obj.force_flush();
  };

  auto on_exit = []() { };

  // Run the benchmark for n threads
  for (auto thread_count : thread_count_array)
  {
    run_benchmark("Logger: bqlog - Benchmark: Caller Thread Latency", thread_count,
                  num_iterations_per_thread, on_start, log_func, on_exit);
  }
}

/***/
int main(int argc, char* argv[]) { bqlog_benchmark(THREAD_LIST_COUNT, ITERATIONS); }
