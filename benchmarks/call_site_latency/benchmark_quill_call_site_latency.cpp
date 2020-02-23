#include "call_site_latency_bench.h"

#include "quill/Quill.h"

/***/
void quill_benchmark(std::vector<int32_t> thread_count_array, size_t num_iterations_per_thread)
{
  std::remove("quill_call_site_latency_percentile_linux_benchmark.log");

  // Setup
  quill::config::set_backend_thread_sleep_duration(std::chrono::nanoseconds{0});
  quill::config::set_backend_thread_cpu_affinity(0);

  // Start the logging backend thread
  quill::start();

  // wait for the backend thread to start
  std::this_thread::sleep_for(std::chrono::seconds(2));

  // Create a file handler to write to a file
  quill::Handler* file_handler =
    quill::file_handler("quill_call_site_latency_percentile_linux_benchmark.log", "w");

  quill::Logger* logger = quill::create_logger("bench_logger", file_handler);

  // Define a logging lambda
  auto log_func = [logger](int32_t i, double d, char const* str) {
    auto const start = std::chrono::steady_clock::now();
    LOG_INFO(logger, "Logging str: {}, int: {}, double: {}", str, i, d);
    auto const end = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
  };

  auto on_start = []() { quill::preallocate(); };

  auto on_exit = []() {};

  // Run the benchmark for n threads
  for (auto thread_count : thread_count_array)
  {
    run_benchmark("Logger: Quill - Benchmark: Caller Thread Latency", thread_count,
                  num_iterations_per_thread, on_start, log_func, on_exit);
  }
}

/***/
int main(int argc, char* argv[]) { quill_benchmark(THREAD_LIST_COUNT, ITERATIONS); }