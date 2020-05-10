#include "call_site_latency_bench.h"

#include "platformlab_nanolog/include/nanolog/NanoLogCpp17.h"

/***/
void platformlab_nanolog(std::vector<int32_t> thread_count_array, size_t num_iterations_per_thread)
{
  std::remove("platformlab_nanolog_call_site_latency_percentile_linux_benchmark.log");

  NanoLog::setLogFile("platformlab_nanolog_call_site_latency_percentile_linux_benchmark.log");

  // wait for the backend thread to start
  std::this_thread::sleep_for(std::chrono::seconds(1));

  auto log_func = [](uint64_t i, uint64_t j, double d) {
    NANO_LOG(NOTICE, "Logging int: %lu, int: %lu, double: %f", i, j, d);
  };

  auto on_start = []() { NanoLog::preallocate(); };

  auto on_exit = []() {};

  // Run the benchmark for n threads
  for (auto thread_count : thread_count_array)
  {
    run_benchmark("Logger: PlatformLab NanoLog - Benchmark: Caller Thread Latency", thread_count,
                  num_iterations_per_thread, on_start, log_func, on_exit);
  }
}

/***/
int main(int argc, char* argv[]) { platformlab_nanolog(THREAD_LIST_COUNT, ITERATIONS); }