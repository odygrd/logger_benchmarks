#include "call_site_latency_bench.h"

#ifndef BENCH_SCENARIO_DEFAULTS
  // Block when the queue is full instead of dropping the message, as every other library does
  #define FMTLOG_BLOCK 1
#endif
#include "fmt/ranges.h"
#include "fmtlog/fmtlog.h"
#include <atomic>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

/***/
void fmtlog_benchmark(std::vector<int32_t> thread_count_array, size_t num_iterations_per_thread)
{
  std::remove("fmtlog_call_site_latency_percentile_linux_benchmark.log");
  fmtlog::setLogFile("fmtlog_call_site_latency_percentile_linux_benchmark.log", true);

#ifdef BENCH_SCENARIO_DEFAULTS
  // As shipped: fmtlog's own polling thread, which polls once a second by default
  set_thread_affinity(5);
  fmtlog::startPollingThread();
#else
  // The application owns the polling loop. It never sleeps, the most responsive setting every
  // library is given where it has one.
  std::atomic<bool> done{false};

  std::thread backend(
    [&done]()
    {
      // pin to cpu backend logging thread.
      set_thread_affinity(5);

      do
      {
        fmtlog::poll(false);
      } while (!done.load(std::memory_order_acquire));

      fmtlog::poll(true);
    });
#endif

  // wait for the backend thread to start
  std::this_thread::sleep_for(std::chrono::seconds(1));

#ifdef BENCH_SCENARIO_DISABLED
  // A statement below the logger's level
  fmtlog::setLogLevel(fmtlog::INF);
  auto log_func = [](uint64_t i, uint64_t j, double d)
  { FMTLOG(fmtlog::DBG, "Logging int: {}, int: {}, double: {}", i, j, d); };
#elif defined(BENCH_INT_INT_DOUBLE)
  auto log_func = [](uint64_t i, uint64_t j, double d)
  { FMTLOG(fmtlog::INF, "Logging int: {}, int: {}, double: {}", i, j, d); };
#elif defined(BENCH_INT_INT_LARGESTR)
  auto log_func = [](uint64_t i, uint64_t j, std::string const& s)
  { FMTLOG(fmtlog::INF, "Logging int: {}, int: {}, string: {}", i, j, s); };
#elif defined(BENCH_VECTOR_LARGESTR)
  auto log_func = [](uint64_t i, uint64_t j, std::vector<std::string> const& s)
  { FMTLOG(fmtlog::INF, "Logging int: {}, int: {}, vector: {}", i, j, s); };
#endif

  auto on_start = []()
  {
    FMTLOG(fmtlog::INF, "Warm up");
    std::this_thread::sleep_for(std::chrono::seconds{1});
  };

  auto on_exit = []() { };

  // Run the benchmark for n threads
  for (auto thread_count : thread_count_array)
  {
    run_benchmark("Logger: fmtlog - Benchmark: Caller Thread Latency", thread_count,
                  num_iterations_per_thread, MESSAGES_PER_ITERATION, on_start, log_func, on_exit);
  }

  // stop the backend thread
#ifdef BENCH_SCENARIO_DEFAULTS
  fmtlog::stopPollingThread();
#else
  done.store(true, std::memory_order_release);
  backend.join();
#endif
}

/***/
int main(int argc, char* argv[]) { fmtlog_benchmark(THREAD_LIST_COUNT, ITERATIONS); }
