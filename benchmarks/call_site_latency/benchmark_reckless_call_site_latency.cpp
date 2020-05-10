#include "call_site_latency_bench.h"

#include "reckless/file_writer.hpp"
#include "reckless/severity_log.hpp"

/***/
void reckless_benchmark(std::vector<int32_t> thread_count_array, size_t num_iterations_per_thread)
{
  std::remove("reckless_call_site_latency_percentile_linux_benchmark.log");

  using log_t = reckless::severity_log<reckless::indent<4>, ' ', reckless::severity_field, reckless::timestamp_field>;
  reckless::file_writer writer("reckless_call_site_latency_percentile_linux_benchmark.log");
  log_t g_log(&writer);
  g_log.permanent_error_policy(reckless::error_policy::block);
  g_log.temporary_error_policy(reckless::error_policy::block);

  // wait for the backend thread to start
  std::this_thread::sleep_for(std::chrono::seconds(1));

  auto log_func = [&g_log](uint64_t i, uint64_t j, double d) {
    g_log.info("Logging int: %i, int: %d, double: %f", i, j, d);
  };

  auto on_start = []() {};

  auto on_exit = []() {};

  // Run the benchmark for n threads
  for (auto thread_count : thread_count_array)
  {
    run_benchmark("Logger: Reckless - Benchmark: Caller Thread Latency", thread_count,
                  num_iterations_per_thread, on_start, log_func, on_exit);
  }
}

/***/
int main(int argc, char* argv[]) { reckless_benchmark(THREAD_LIST_COUNT, ITERATIONS); }