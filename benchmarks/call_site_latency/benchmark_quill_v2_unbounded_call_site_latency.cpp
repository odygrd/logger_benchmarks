#include "call_site_latency_bench.h"
#include "quill/Quill.h"

#include <string>

void quill_benchmark(std::vector<int32_t> thread_count_array, size_t num_iterations_per_thread)
{
  std::remove("benchmark_quill_no_dual_mode_unbounded_call_site_latency.log");

  // Setup
  quill::config::set_backend_thread_sleep_duration(std::chrono::nanoseconds{0});
  quill::config::set_backend_thread_cpu_affinity(6);

  // Start the logging backend thread
  quill::start();

  // wait for the backend thread to start
  std::this_thread::sleep_for(std::chrono::seconds(1));

  // Create a file handler to write to a file
  quill::Handler* file_handler =
    quill::file_handler("benchmark_quill_no_dual_mode_unbounded_call_site_latency.log", "w");

  quill::Logger* logger = quill::create_logger("bench_logger", file_handler);

  // Define a logging lambda
#ifdef BENCH_INT_INT_DOUBLE
  auto log_func = [logger](uint64_t i, uint64_t j, double d)
  { LOG_INFO(logger, "Logging int: {}, int: {}, double: {}", i, j, d); };
#elif defined(BENCH_INT_INT_LARGESTR)
  auto log_func = [logger](uint64_t i, uint64_t j, std::string const& s)
  { LOG_INFO(logger, "Logging int: {}, int: {}, string: {}", i, j, s); };
#else
  static_assert(false, "define BENCH_INT_INT_DOUBLE or BENCH_INT_INT_LARGESTR");
#endif

  auto on_start = []() { quill::preallocate(); };

  auto on_exit = []() { quill::flush(); };

  // Run the benchmark for n threads
  for (auto thread_count : thread_count_array)
  {
    run_benchmark("Logger: Quill - Benchmark: Caller Thread Latency - No Dual Mode, Unbounded",
                  thread_count, num_iterations_per_thread, on_start, log_func, on_exit);
  }
}

/***/
int main(int argc, char* argv[]) { quill_benchmark(THREAD_LIST_COUNT, ITERATIONS); }