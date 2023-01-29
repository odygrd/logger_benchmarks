#pragma once

#include "call_site_latency_bench_common.h"
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <functional>
#include <iostream>
#include <numeric>
#include <random>
#include <thread>

// Instead of sleep
inline void wait(std::chrono::nanoseconds min, std::chrono::nanoseconds max)
{
#ifdef BENCH_WITHOUT_PERF
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_int_distribution<> dis(min.count(), max.count());

  auto const start_time = std::chrono::steady_clock::now();
  auto const end_time = start_time.time_since_epoch() + std::chrono::nanoseconds{dis(gen)};
  std::chrono::nanoseconds time_now;
  do
  {
    time_now = std::chrono::steady_clock::now().time_since_epoch();
  } while (time_now < end_time);
#else
  // when in perf use sleep as the other variables add noise
  std::this_thread::sleep_for(max);
#endif
}

inline void set_thread_affinity(size_t cpu_num)
{
  // Set this thread affinity
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(cpu_num, &cpuset);

  auto const err = sched_setaffinity(0, sizeof(cpuset), &cpuset);

  if (err == -1)
  {
    throw std::system_error(errno, std::system_category());
  }
}

#ifdef BENCH_WITHOUT_PERF
/***/

  #ifdef BENCH_INT_INT_DOUBLE
inline void run_log_benchmark(size_t num_iterations,
                              std::function<void()> on_thread_start,
                              std::function<void(uint64_t, uint64_t, double)> log_func,
                              std::function<void()> on_thread_exit,
                              size_t current_thread_num,
                              std::vector<uint64_t>& latencies,
                              double rdtsc_ticks_per_ns)
  #elif defined(BENCH_INT_INT_LARGESTR)
inline void run_log_benchmark(size_t num_iterations,
                              std::function<void()> on_thread_start,
                              std::function<void(uint64_t, uint64_t, std::string const&)> log_func,
                              std::function<void()> on_thread_exit,
                              size_t current_thread_num,
                              std::vector<uint64_t>& latencies,
                              double rdtsc_ticks_per_ns)
  #endif
{
  // running thread affinity
  set_thread_affinity(current_thread_num);

  on_thread_start();

  // Always ignore the first log statement as it will be doing initialisation for most loggers - quill and nanolog don't need this as they have preallocate

  #ifdef BENCH_INT_INT_DOUBLE
  // generate a double from i
  log_func(100, 100, 1.1);
  #elif defined(BENCH_INT_INT_LARGESTR)
  log_func(100, 100, "init");
  #endif

  unsigned int aux;

  // Main Benchmark
  for (int i = 0; i < num_iterations; ++i)
  {

  #ifdef BENCH_INT_INT_DOUBLE
    // generate a double from i
    double const v = i + (0.1 * i);
  #elif defined(BENCH_INT_INT_LARGESTR)
    std::string v{"Lorem ipsum dolor sit amet, consectetur "};
    v += std::to_string(i);
  #endif

    auto const start = __rdtscp(&aux);
    for (size_t j = 0; j < MESSAGES; ++j)
    {
      log_func(i, j, v);
    }
    auto const end = __rdtscp(&aux);

    uint64_t const latency{static_cast<uint64_t>((end - start) / MESSAGES / rdtsc_ticks_per_ns)};
    latencies.push_back(latency);

    // send the next log after x time
    wait(MIN_WAIT_DURATION, MAX_WAIT_DURATION);
  }

  on_thread_exit();
}
#else
/***/
inline void run_log_benchmark(size_t num_iterations,
                              std::function<void()> on_thread_start,
                              std::function<void(uint64_t, uint64_t, double)> log_func,
                              std::function<void()> on_thread_exit,
                              size_t current_thread_num)
{
  // running thread affinity
  set_thread_affinity(current_thread_num);

  on_thread_start();

  // Always ignore the first log statement as it will be doing initialisation for most loggers - quill and nanolog don't need this as they have preallocate
  log_func(100, 100, 1.0);

  // Main Benchmark
  for (int i = 0; i < num_iterations; ++i)
  {
    // generate a double from i
    double const d = i + (0.1 * i);

    for (size_t j = 0; j < MESSAGES; ++j)
    {
      log_func(i, j, d);
    }

    // send the next log after x time
    wait(MIN_WAIT_DURATION, MAX_WAIT_DURATION);
  }

  on_thread_exit();
}
#endif

/***/
#ifdef BENCH_INT_INT_DOUBLE
inline void run_benchmark(char const* benchmark_name,
                          int32_t thread_count,
                          size_t num_iterations,
                          std::function<void()> on_thread_start,
                          std::function<void(uint64_t, uint64_t, double)> log_func,
                          std::function<void()> on_thread_exit)
#elif defined(BENCH_INT_INT_LARGESTR)
inline void run_benchmark(char const* benchmark_name,
                          int32_t thread_count,
                          size_t num_iterations,
                          std::function<void()> on_thread_start,
                          std::function<void(uint64_t, uint64_t, std::string const&)> log_func,
                          std::function<void()> on_thread_exit)
#endif
{
  // main thread affinity
  set_thread_affinity(6);

#ifdef BENCH_WITHOUT_PERF
  // each thread gets a vector of latencies
  std::vector<std::vector<uint64_t>> latencies;
  latencies.resize(thread_count);
  for (auto& elem : latencies)
  {
    elem.reserve(num_iterations);
  }
#endif

  std::vector<std::thread> threads;
  threads.reserve(thread_count);
  for (int thread_num = 0; thread_num < thread_count; ++thread_num)
  {
#ifdef BENCH_WITHOUT_PERF
    // Spawn num threads
    threads.emplace_back(run_log_benchmark, num_iterations, on_thread_start, log_func, on_thread_exit,
                         thread_num + 7, std::ref(latencies[thread_num]), rdtsc_ticks());
#else
    // Spawn num threads
    threads.emplace_back(run_log_benchmark, num_iterations, on_thread_start, log_func,
                         on_thread_exit, thread_num + 7);
#endif
  }

  // Wait for threads to finish
  for (int i = 0; i < thread_count; ++i)
  {
    threads[i].join();
  }

#ifdef BENCH_WITHOUT_PERF
  // All threads have finished we can read all latencies
  std::vector<uint64_t> latencies_combined;
  latencies_combined.reserve(num_iterations * thread_count);
  for (auto const& elem : latencies)
  {
    latencies_combined.insert(latencies_combined.end(), elem.begin(), elem.end());
  }

  // Sort all latencies
  std::sort(latencies_combined.begin(), latencies_combined.end());

  std::cout << "Thread Count " << thread_count << " - Total messages " << latencies_combined.size()
            << " - " << benchmark_name << "\n |  50th | 75th | 90th | 95th | 99th | 99.9th | Worst |\n"
            << " |  " << latencies_combined[(size_t)(num_iterations * thread_count) * 0.5]
            << "  |  " << latencies_combined[(size_t)(num_iterations * thread_count) * 0.75]
            << "  |  " << latencies_combined[(size_t)(num_iterations * thread_count) * 0.9]
            << "  |  " << latencies_combined[(size_t)(num_iterations * thread_count) * 0.95]
            << "  |  " << latencies_combined[(size_t)(num_iterations * thread_count) * 0.99]
            << "  |  " << latencies_combined[(size_t)(num_iterations * thread_count) * 0.999]
            << "  |  " << latencies_combined[latencies_combined.size() - 1] << "  |\n\n";
#endif
}