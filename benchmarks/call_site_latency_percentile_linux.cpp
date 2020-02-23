// Quill

// #if 0

// Spdlog
#include "spdlog/async.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

// Iyengar Nanolog
#include "Iyengar_NanoLog/NanoLog.hpp"

// Reckless
#include "reckless/file_writer.hpp"
#include "reckless/severity_log.hpp"


#include <chrono>
#include <cstdint>
#include <random>

// #endif

#if 0

/***/
void spdlog_benchmark(std::array<int32_t, 4> threads_num)
{
  std::remove("spdlog_call_site_latency_percentile_linux_benchmark.log");

  // Setup
  spdlog::set_automatic_registration(false);

  auto on_backend_start = []() {
    // Set the spdlog backend thread cpu affinity to zero
    quill::detail::set_cpu_affinity(0);
  };

  spdlog::init_thread_pool(8388608, 1, on_backend_start);

  auto sink = std::make_shared<spdlog::sinks::basic_file_sink_st>(
    "spdlog_call_site_latency_percentile_linux_benchmark.log");
  auto logger = std::make_shared<spdlog::async_logger>("bench_logger", sink, spdlog::thread_pool(),
                                                       spdlog::async_overflow_policy::block);
  logger->set_pattern("%T.%F [%t] %s:%# %l     %n - %v");

  std::this_thread::sleep_for(std::chrono::seconds(3));

  // Define a logging lambda
  auto spdlog_benchmark = [logger](int32_t i, double d, char const* str) {
    auto const start = std::chrono::steady_clock::now();
    SPDLOG_LOGGER_INFO(logger, "Logging str: {}, int: {}, double: {}", str, i, d);
    auto const end = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
  };

  auto on_init = []() {};

  // Run the benchmark for n threads
  for (auto threads : threads_num)
  {
    run_benchmark(spdlog_benchmark, on_init, threads,
                  "Logger: Spdlog - Benchmark: Caller Thread Latency");
  }
}

/***/
void iyengar_nanoLog_benchmark(std::array<int32_t, 4> threads_num)
{
  std::remove("iyengar_nanoLog_call_site_latency_percentile_linux_benchmark.log");

  // Guaranteed nano log.
  nanolog::initialize(nanolog::GuaranteedLogger(), "./",
                      "iyengar_nanoLog_call_site_latency_percentile_linux_benchmark.log", 500);

  std::this_thread::sleep_for(std::chrono::seconds(3));

  auto nanolog_benchmark = [](int32_t i, double d, char const* str) {
    auto const start = std::chrono::steady_clock::now();
    IY_LOG_INFO << "Logging str: " << str << ", int: " << i << ", double: " << d;
    auto const end = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
  };

  auto on_init = []() {};

  // Run the benchmark for n threads
  for (auto threads : threads_num)
  {
    run_benchmark(nanolog_benchmark, on_init, threads,
                  "Logger: Iyengar_NanoLog - Benchmark: Caller Thread Latency");
  }
}

/***/
void reckless_benchmark(std::array<int32_t, 4> threads_num)
{
  std::remove("reckless_call_site_latency_percentile_linux_benchmark.log");

  using log_t = reckless::severity_log<reckless::indent<4>, ' ', reckless::severity_field, reckless::timestamp_field>;
  reckless::file_writer writer("reckless_call_site_latency_percentile_linux_benchmark.log");
  log_t g_log(&writer);
  g_log.permanent_error_policy(reckless::error_policy::block);
  g_log.temporary_error_policy(reckless::error_policy::block);

  std::this_thread::sleep_for(std::chrono::seconds(3));

  auto reckless_benchmark = [&g_log](int32_t i, double d, char const* str) {
    auto const start = std::chrono::steady_clock::now();
    g_log.info("Logging str: %s, int: %d, double: %f", str, i, d);
    auto const end = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
  };

  auto on_init = []() {};

  // Run the benchmark for n threads
  for (auto threads : threads_num)
  {
    run_benchmark(reckless_benchmark, on_init, threads,
                  "Logger: Reckless - Benchmark: Caller Thread Latency");
  }
}

#endif

int main(int argc, char* argv[])
{
  if (argc != 2)
  {
    std::cerr << "Please provide the name of the logger as argument." << std::endl;
    return 0;
  }

  std::vector<int32_t> threads_count{1, 4};
  std::size_t num_iterations{100'000};

  if (strcmp(argv[1], "quill") == 0)
  {
    quill_benchmark(threads_count, num_iterations);
  }

#if 0
  else if (strcmp(argv[1], "spdlog") == 0)
  {
    spdlog_benchmark(threads_num);
  }
  else if (strcmp(argv[1], "iy_nanolog") == 0)
  {
    iyengar_nanoLog_benchmark(threads_num);
  }
  else if (strcmp(argv[1], "reckless") == 0)
  {
    reckless_benchmark(threads_num);
  }
  else if (strcmp(argv[1], "pl_nanolog") == 0)
  {
    platformlab_nanolog(threads_num);
  }
#endif

  return 0;
}