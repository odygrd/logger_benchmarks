#include "call_site_latency_bench.h"

#ifdef BENCH_SCENARIO_DISABLED
  // Compile the debug statement in, so its level is checked at runtime
  #define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#endif

#include "spdlog/async.h"
#include "spdlog/fmt/ranges.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

#include <string>
#include <vector>

/***/
void spdlog_benchmark(std::vector<int32_t> thread_count_array, size_t num_iterations_per_thread)
{
  std::remove("spdlog_call_site_latency_percentile_linux_benchmark.log");

  // Setup
  spdlog::set_automatic_registration(false);

  auto on_backend_start = []()
  {
    // Pin the spdlog backend thread away from the producers.
    set_thread_affinity(5);
  };

  // The default queue size and one worker thread, as spdlog::init_thread_pool() uses
  spdlog::init_thread_pool(spdlog::details::default_async_q_size, 1, on_backend_start);

  auto sink = std::make_shared<spdlog::sinks::basic_file_sink_st>(
    "spdlog_call_site_latency_percentile_linux_benchmark.log");
  auto logger = std::make_shared<spdlog::async_logger>("bench_logger", sink, spdlog::thread_pool(),
                                                       spdlog::async_overflow_policy::block);
  logger->set_pattern("%T.%F [%t] %s:%# %l     %n - %v");

  // wait for the backend thread to start
  std::this_thread::sleep_for(std::chrono::seconds(1));

  // Define a logging lambda
#ifdef BENCH_SCENARIO_DISABLED
  // A statement below the logger's level
  logger->set_level(spdlog::level::info);
  auto log_func = [logger](uint64_t i, uint64_t j, double d)
  { SPDLOG_LOGGER_DEBUG(logger, "Logging int: {}, int: {}, double: {}", i, j, BENCH_DISABLED_ARG(d)); };
#elif defined(BENCH_INT_INT_DOUBLE)
  auto log_func = [logger](uint64_t i, uint64_t j, double d)
  { SPDLOG_LOGGER_INFO(logger, "Logging int: {}, int: {}, double: {}", i, j, d); };
#elif defined(BENCH_INT_INT_LARGESTR)
  auto log_func = [logger](uint64_t i, uint64_t j, std::string const& s)
  { SPDLOG_LOGGER_INFO(logger, "Logging int: {}, int: {}, string: {}", i, j, s); };
#elif defined(BENCH_VECTOR_LARGESTR)
  auto log_func = [logger](uint64_t i, uint64_t j, std::vector<std::string> const& s)
  { SPDLOG_LOGGER_INFO(logger, "Logging int: {}, int: {}, vector: {}", i, j, s); };
#endif

  auto on_start = [logger]()
  {
    SPDLOG_LOGGER_INFO(logger, "Warm up");
    logger->flush();
  };

  auto on_exit = []() {};

  // Run the benchmark for n threads
  for (auto thread_count : thread_count_array)
  {
    run_benchmark("Logger: Spdlog - Benchmark: Caller Thread Latency", thread_count,
                  num_iterations_per_thread, MESSAGES_PER_ITERATION, on_start, log_func, on_exit);
  }
}
/***/
int main(int argc, char* argv[]) { spdlog_benchmark(THREAD_LIST_COUNT, ITERATIONS); }
