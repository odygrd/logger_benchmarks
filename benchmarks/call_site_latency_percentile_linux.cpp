#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <numeric>
#include <thread>

// Quill
#include "quill/Quill.h"

// #if 0

// Spdlog
#include "spdlog/async.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

// Iyengar Nanolog
#include "Iyengar_NanoLog/NanoLog.hpp"

// Reckless
#include "reckless/severity_log.hpp"
#include "reckless/file_writer.hpp"

#include "platformlab_nanolog/include/nanolog/NanoLogCpp17.h"

// #endif


/***/
template <typename Function>
void run_log_benchmark(Function&& f, char const* benchmark_name, std::mutex& m, int thread_num)
{
  // Always ignore the first log statement as it will be doing initialisation for most loggers
  f(100, 100, "initial");

  int iterations = 100'000;
  std::vector<uint64_t> latencies;
  constexpr char const* str = "benchmark";

  for (int i = 0; i < iterations; ++i)
  {
    // generate a double from i
    double const d = i + (0.1 * i);
    std::chrono::nanoseconds const latency = f(i, d, str);
    latencies.push_back(latency.count());
  }

  // Sort all latencies
  std::sort(latencies.begin(), latencies.end());

  // Calculate the sum of all latencies
  uint64_t sum = std::accumulate(latencies.begin(), latencies.end(), static_cast<uint64_t>(0));

  // protect access to cout
  std::lock_guard<std::mutex> lock{m};
  std::cout << "Thread: " << thread_num << std::setw(12) << "50th" << std::setw(20) << "75th"
            << std::setw(20) << "90th" << std::setw(19) << "99th" << std::setw(20) << "99.9th"
            << std::setw(20) << "Worst" << std::setw(21) << "Average\n"
            << std::setw(20) << latencies[(size_t)iterations * 0.5] << std::setw(20)
            << latencies[(size_t)iterations * 0.75] << std::setw(20) << latencies[(size_t)iterations * 0.9]
            << std::setw(20) << latencies[(size_t)iterations * 0.99] << std::setw(20)
            << latencies[(size_t)iterations * 0.999] << std::setw(20) << latencies[latencies.size() - 1]
            << std::setw(20) << (sum * 1.0) / latencies.size() << "\n\n";
}

/***/
template <typename Function>
void run_benchmark(Function&& f, int32_t thread_count, char const* benchmark_name)
{
  std::cout << "********************************* " << std::endl;
  std::cout << "Total thread count: " << thread_count << " - " << benchmark_name
            << " in nanoseconds " << std::endl;

  std::mutex m;
  std::vector<std::thread> threads;
  for (int i = 0; i < thread_count; ++i)
  {
    // Spawn num threads
    threads.emplace_back(run_log_benchmark<Function>, std::ref(f), benchmark_name, std::ref(m), i + 1);
  }

  // Wait for threads to finish
  for (int i = 0; i < thread_count; ++i)
  {
    threads[i].join();
  }
}

/***/
void quill_benchmark(std::array<int32_t, 4> threads_num)
{
  std::remove("quill_call_site_latency_percentile_linux_benchmark.log");

  // Setup
  // Pin the backend thread to cpu 0
  quill::config::set_backend_thread_cpu_affinity(0);
  quill::config::set_backend_thread_sleep_duration(std::chrono::nanoseconds{0});

  // Start the logging backend thread
  quill::start();

  // wait for the backend thread to start
  std::this_thread::sleep_for(std::chrono::seconds(2));

  // Create a file handler to write to a file
  quill::Handler* file_handler =
    quill::file_handler("quill_call_site_latency_percentile_linux_benchmark.log", "w");

  quill::Logger* logger = quill::create_logger("bench_logger", file_handler);

  // Define a logging lambda
  auto quill_benchmark = [logger](int32_t i, double d, char const* str) {
    auto const start = std::chrono::steady_clock::now();
    LOG_INFO(logger, "Logging str: {}, int: {}, double: {}", str, i, d);
    auto const end = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
  };

  // Run the benchmark for n threads
  for (auto threads : threads_num)
  {
    run_benchmark(quill_benchmark, threads, "Logger: Quill - Benchmark: Caller Thread Latency");
  }
}

// #if 0

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

  // Run the benchmark for n threads
  for (auto threads : threads_num)
  {
    run_benchmark(spdlog_benchmark, threads, "Logger: Spdlog - Benchmark: Caller Thread Latency");
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

  auto nanolog_benchmark = [](int32_t i, double d, char const* str)  {
    auto const start = std::chrono::steady_clock::now();
    IY_LOG_INFO << "Logging str: " << str << ", int: " << i << ", double: " << d;
    auto const end = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
  };

  // Run the benchmark for n threads
  for (auto threads : threads_num)
  {
    run_benchmark(nanolog_benchmark, threads,
                  "Logger: Iyengar_NanoLog - Benchmark: Caller Thread Latency");
  }
}

/***/
void reckless_benchmark(std::array<int32_t, 4> threads_num)
{
  std::remove("reckless_call_site_latency_percentile_linux_benchmark.log");

  using log_t = reckless::severity_log < reckless::indent < 4 >, ' ', reckless::severity_field, reckless::timestamp_field >;
  reckless::file_writer writer("reckless_call_site_latency_percentile_linux_benchmark.log");
  log_t g_log(&writer, 8388608, 65534, 8388608);
  g_log.permanent_error_policy(reckless::error_policy::block);
  g_log.temporary_error_policy(reckless::error_policy::block);

  std::this_thread::sleep_for(std::chrono::seconds(3));

  auto reckless_benchmark = [&g_log](int32_t i, double d, char const* str)  {
    auto const start = std::chrono::steady_clock::now();
    g_log.info("Logging str: %s, int: %d, double: %f", str, i, d);
    auto const end = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
  };

  // Run the benchmark for n threads
  for (auto threads : threads_num)
  {
    run_benchmark(reckless_benchmark, threads,
                  "Logger: Reckless - Benchmark: Caller Thread Latency");
  }
}

/***/
void platformlab_nanolog(std::array<int32_t, 4> threads_num)
{
  std::remove("platformlab_nanolog_call_site_latency_percentile_linux_benchmark.log");

  NanoLog::preallocate();

  NanoLog::setLogFile("platformlab_nanolog_call_site_latency_percentile_linux_benchmark.log");

  std::this_thread::sleep_for(std::chrono::seconds(3));

  auto platformlab_nanolog_benchmark = [](int32_t i, double d, char const* str)  {
    auto const start = std::chrono::steady_clock::now();
    PL_NANO_LOG(NOTICE, "Logging str: %s, int: %d, double: %f", str, i, d);
    auto const end = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
  };

  // Run the benchmark for n threads
  for (auto threads : threads_num)
  {
    run_benchmark(platformlab_nanolog_benchmark, threads,
                  "Logger: Reckless - Benchmark: Caller Thread Latency");
  }
}
// #endif

int main(int argc, char* argv[])
{

  if (argc != 2)
  {
    std::cerr << "Please provide the name of the logger as argument." << std::endl;
    return 0;
  }

  std::array<int32_t, 4> threads_num{{1, 2, 3, 4}};

  // Main thread is not important so set it to the same cpu as the backend
  quill::detail::set_cpu_affinity(0);

  if (strcmp(argv[1], "quill") == 0)
  {
    quill_benchmark(threads_num);
  }

  // #if 0
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
  // #endif

  return 0;
}