#define FMT_HEADER_ONLY
#define XTR_USE_IO_URING 0

#include "call_site_latency_bench.h"

#include "xtr/logger.hpp"

#include <string>

/***/
void xtr_benchmark(std::vector<int32_t> thread_count_array, size_t num_iterations_per_thread)
{
  std::remove("xtr_call_site_latency_percentile_linux_benchmark.log");

  xtr::logger log("xtr_call_site_latency_percentile_linux_benchmark.log");

  set_pthread_affinity(log.consumer_thread_native_handle(), 1);

  // wait for the backend thread to start
  std::this_thread::sleep_for(std::chrono::seconds(1));

  // Define a logging lambda
#ifdef BENCH_INT_INT_DOUBLE
  auto log_func = [&log](uint64_t i, uint64_t j, double d) mutable {
    thread_local auto sink = log.get_sink("main");
    XTR_LOGL_TSC(info, sink, "Logging int: {}, int: {}, double: {}", i, j, d);
  };
#elif defined(BENCH_INT_INT_LARGESTR)
  auto log_func = [&log](uint64_t i, uint64_t j, std::string const& s) mutable {
    thread_local auto sink = log.get_sink("main");
    XTR_LOGL_TSC(info, sink, "Logging int: {}, int: {}, string: {}", i, j, s);
  };
#elif defined(BENCH_VECTOR_LARGESTR)
  auto log_func = [&log](uint64_t i, uint64_t j, std::vector<std::string> const& s) mutable
  {
    thread_local auto sink = log.get_sink("main");
    XTR_LOGL_TSC(info, sink, "Logging int: {}, int: {}, vector: {}", i, j, s);
  };
#endif

  auto on_start = []() {};

  auto on_exit = []() {};

  // Run the benchmark for n threads
  for (auto thread_count : thread_count_array)
  {
    run_benchmark("Logger: XTR - Benchmark: Caller Thread Latency", thread_count,
                  num_iterations_per_thread, on_start, log_func, on_exit);
  }
}

int main(int argc, char* argv[]) { xtr_benchmark(THREAD_LIST_COUNT, ITERATIONS); }