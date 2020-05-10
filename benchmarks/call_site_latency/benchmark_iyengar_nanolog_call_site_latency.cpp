#include "call_site_latency_bench.h"

#include "Iyengar_NanoLog/NanoLog.hpp"

/***/
void iyengar_nanoLog_benchmark(std::vector<int32_t> thread_count_array, size_t num_iterations_per_thread)
{
  std::remove("iyengar_nanoLog_call_site_latency_percentile_linux_benchmark.log");

  // Guaranteed nano log.
  nanolog::initialize(nanolog::GuaranteedLogger(), "./",
                      "iyengar_nanoLog_call_site_latency_percentile_linux_benchmark.log", 500);

  std::this_thread::sleep_for(std::chrono::seconds(1));
  
  auto log_func = [](uint64_t i, uint64_t j, double d){
    IY_LOG_INFO << "Logging int: " << i << ", int: " << j << ", double: " << d;
  };

  auto on_start = []() {};

  auto on_exit = []() {};

  // Run the benchmark for n threads
  for (auto thread_count : thread_count_array)
  {
    run_benchmark("Logger: Iyengar NanoLog - Benchmark: Caller Thread Latency", thread_count,
                  num_iterations_per_thread, on_start, log_func, on_exit);
  }
}

/***/
int main(int argc, char* argv[]) { iyengar_nanoLog_benchmark(THREAD_LIST_COUNT, ITERATIONS); }