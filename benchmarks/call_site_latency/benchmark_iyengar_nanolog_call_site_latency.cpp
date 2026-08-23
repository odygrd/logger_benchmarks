#include "call_site_latency_bench.h"

#include "Iyengar_NanoLog/NanoLog.hpp"

#include <string>

/***/
void iyengar_nanoLog_benchmark(std::vector<int32_t> thread_count_array, size_t num_iterations_per_thread)
{
  std::remove("iyengar_nanoLog_call_site_latency_percentile_linux_benchmark.log");

  // Iyengar NanoLog does not expose its worker thread handle. Pin this thread
  // before initialization so the worker inherits the backend CPU.
  set_thread_affinity(5);
  nanolog::initialize(nanolog::GuaranteedLogger(), "./",
                      "iyengar_nanoLog_call_site_latency_percentile_linux_benchmark.log", 10 * 1024 /* 10GB */);

  std::this_thread::sleep_for(std::chrono::seconds(1));

#ifdef BENCH_INT_INT_DOUBLE
  auto log_func = [](uint64_t i, uint64_t j, double d){
    LOG_INFO << "Logging int: " << i << ", int: " << j << ", double: " << d;
  };
#elif defined(BENCH_INT_INT_LARGESTR)
  auto log_func = [](uint64_t i, uint64_t j, std::string const& s){
    LOG_INFO << "Logging int: " << i << ", int: " << j << ", string: " << s;
  };
#endif

  auto on_start = []()
  {
    LOG_INFO << "Warm up";
    std::this_thread::sleep_for(std::chrono::seconds{1});
  };

  auto on_exit = []() {};

  // Run the benchmark for n threads
  for (auto thread_count : thread_count_array)
  {
    run_benchmark("Logger: Iyengar NanoLog - Benchmark: Caller Thread Latency", thread_count,
                  num_iterations_per_thread, MESSAGES_PER_ITERATION, on_start, log_func, on_exit);
  }
}

/***/
int main(int argc, char* argv[]) { iyengar_nanoLog_benchmark(THREAD_LIST_COUNT, ITERATIONS); }
