#include "call_site_latency_bench.h"

#include "spdlog/async.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

#include <string>

/***/
void spdlog_benchmark(std::vector<int32_t> thread_count_array, size_t num_iterations_per_thread)
{
  std::remove("spdlog_call_site_latency_percentile_linux_benchmark.log");

  // Setup
  spdlog::set_automatic_registration(false);

  auto on_backend_start = []() {
    // Set the spdlog backend thread cpu affinity to zero
    set_thread_affinity(0);
  };

  spdlog::init_thread_pool(8388608, 1, on_backend_start);

  auto sink = std::make_shared<spdlog::sinks::basic_file_sink_st>(
    "spdlog_call_site_latency_percentile_linux_benchmark.log");
  auto logger = std::make_shared<spdlog::async_logger>("bench_logger", sink, spdlog::thread_pool(),
                                                       spdlog::async_overflow_policy::block);
  logger->set_pattern("%T.%F [%t] %s:%# %l     %n - %v");

  // wait for the backend thread to start
  std::this_thread::sleep_for(std::chrono::seconds(1));

  // Define a logging lambda
#ifdef BENCH_INT_INT_DOUBLE
  auto log_func = [logger](uint64_t i, uint64_t j, double d) {
    SPDLOG_LOGGER_INFO(logger, "Logging int: {}, int: {}, double: {}", i, j, d);
  };
#elif defined(BENCH_INT_INT_LARGESTR)
  auto log_func = [logger](uint64_t i, uint64_t j, std::string const& s) {
    SPDLOG_LOGGER_INFO(logger, "Logging int: {}, int: {}, string: {}", i, j, s);
  };
#endif

  auto on_start = []() {};

  auto on_exit = []() {};

  // Run the benchmark for n threads
  for (auto thread_count : thread_count_array)
  {
    run_benchmark("Logger: Spdlog - Benchmark: Caller Thread Latency", thread_count,
                  num_iterations_per_thread, on_start, log_func, on_exit);
  }
}
/***/
int main(int argc, char* argv[]) { spdlog_benchmark(THREAD_LIST_COUNT, ITERATIONS); }