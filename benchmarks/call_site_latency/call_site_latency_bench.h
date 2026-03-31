#pragma once

#include "call_site_latency_bench_common.h"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <numeric>
#include <random>
#include <sched.h>
#include <string>
#include <thread>
#include <vector>

// Instead of sleep
inline void wait([[maybe_unused]] std::chrono::nanoseconds min, std::chrono::nanoseconds max)
{
#ifdef BENCH_WITHOUT_PERF
  thread_local std::mt19937 gen{std::random_device{}()};
  std::uniform_int_distribution<int64_t> dis(min.count(), max.count());

  auto const start_time = std::chrono::steady_clock::now();
  auto const end_time = start_time + std::chrono::nanoseconds{dis(gen)};
  std::chrono::nanoseconds time_now;
  do
  {
    time_now = std::chrono::steady_clock::now().time_since_epoch();
  } while (time_now < end_time.time_since_epoch());
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

inline size_t messages_for_thread(size_t messages_per_iteration, size_t thread_count, size_t thread_num) noexcept
{
  if (thread_count == 0)
  {
    return 0;
  }

  size_t const base_messages_per_thread = messages_per_iteration / thread_count;
  size_t const remainder = messages_per_iteration % thread_count;
  return base_messages_per_thread + (thread_num < remainder ? 1u : 0u);
}

inline void wait_for_all_threads_to_start(std::atomic<size_t>& started_threads, size_t participant_count) noexcept
{
  started_threads.fetch_add(1, std::memory_order_acq_rel);
  while (started_threads.load(std::memory_order_acquire) != participant_count) {}
}

#ifdef BENCH_WITHOUT_PERF
/***/

  #ifdef BENCH_INT_INT_DOUBLE
inline void run_log_benchmark(size_t num_iterations,
                              size_t messages_per_iteration,
                              std::function<void()> on_thread_start,
                              std::function<void(uint64_t, uint64_t, double)> log_func,
                              std::function<void()> on_thread_exit,
                              std::atomic<size_t>& started_threads,
                              size_t participant_count,
                              size_t current_thread_num,
                              std::vector<uint64_t>& latencies,
                              double ns_rdtsc_tick)
  #elif defined(BENCH_INT_INT_LARGESTR)
inline void run_log_benchmark(size_t num_iterations,
                              size_t messages_per_iteration,
                              std::function<void()> on_thread_start,
                              std::function<void(uint64_t, uint64_t, std::string const&)> log_func,
                              std::function<void()> on_thread_exit,
                              std::atomic<size_t>& started_threads,
                              size_t participant_count,
                              size_t current_thread_num,
                              std::vector<uint64_t>& latencies,
                              double ns_rdtsc_tick)
  #elif defined(BENCH_VECTOR_LARGESTR)
inline void run_log_benchmark(size_t num_iterations,
                              size_t messages_per_iteration,
                              std::function<void()> on_thread_start,
                              std::function<void(uint64_t, uint64_t, std::vector<std::string> const&)> log_func,
                              std::function<void()> on_thread_exit,
                              std::atomic<size_t>& started_threads,
                              size_t participant_count,
                              size_t current_thread_num,
                              std::vector<uint64_t>& latencies,
                              double ns_rdtsc_tick)
  #endif
{
  // running thread affinity
  set_thread_affinity(current_thread_num);

  //  sched_param param{};
  //  param.sched_priority = 99;
  //  if (sched_setscheduler(0, SCHED_FIFO, &param) != 0)
  //    perror("sched_setscheduler");

  on_thread_start();
  wait_for_all_threads_to_start(started_threads, participant_count);

  if (messages_per_iteration == 0)
  {
    on_thread_exit();
    return;
  }

  #ifdef BENCH_INT_INT_DOUBLE
  #elif defined(BENCH_INT_INT_LARGESTR)
  #elif defined(BENCH_VECTOR_LARGESTR)
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> length_dist(50, 60); // large strings
  std::uniform_int_distribution<int> char_dist(65, 90);   // ASCII uppercase letters
  #endif

  // Main Benchmark
  for (size_t i = 0; i < num_iterations; ++i)
  {
  #ifdef BENCH_INT_INT_DOUBLE
    // generate a double from i
    double const v = static_cast<double>(i) + (0.1 * static_cast<double>(i));
  #elif defined(BENCH_INT_INT_LARGESTR)
    std::string v{"Lorem ipsum dolor sit amet, consectetur "};
    v += std::to_string(i);
  #elif defined(BENCH_VECTOR_LARGESTR)
    std::vector<std::string> v(16);

    for (std::string& str : v)
    {
      int length = length_dist(gen);
      str.reserve(length);
      for (int char_idx = 0; char_idx < length; ++char_idx)
      {
        str.push_back(static_cast<char>(char_dist(gen)));
      }
    }
  #endif

    auto const start = __rdtsc();
    for (size_t j = 0; j < messages_per_iteration; ++j)
    {
      log_func(i, j, v);
    }
    auto const end = __rdtsc();

    uint64_t const latency{static_cast<uint64_t>(
      (static_cast<double>(end - start) / static_cast<double>(messages_per_iteration)) * ns_rdtsc_tick)};
    latencies.push_back(latency);

    // send the next log after x time
    wait(MIN_WAIT_DURATION, MAX_WAIT_DURATION);
  }

  on_thread_exit();
}
#else
/***/
inline void run_log_benchmark(size_t num_iterations,
                              size_t messages_per_iteration,
                              std::function<void()> on_thread_start,
                              std::function<void(uint64_t, uint64_t, double)> log_func,
                              std::function<void()> on_thread_exit,
                              std::atomic<size_t>& started_threads,
                              size_t participant_count,
                              size_t current_thread_num)
{
  // running thread affinity
  set_thread_affinity(current_thread_num);

  on_thread_start();
  wait_for_all_threads_to_start(started_threads, participant_count);

  if (messages_per_iteration == 0)
  {
    on_thread_exit();
    return;
  }

  // Main Benchmark
  for (size_t i = 0; i < num_iterations; ++i)
  {
    // generate a double from i
    double const d = static_cast<double>(i) + (0.1 * static_cast<double>(i));

    for (size_t j = 0; j < messages_per_iteration; ++j)
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
#elif defined(BENCH_VECTOR_LARGESTR)
inline void run_benchmark(char const* benchmark_name,
                          int32_t thread_count,
                          size_t num_iterations,
                          std::function<void()> on_thread_start,
                          std::function<void(uint64_t, uint64_t, std::vector<std::string> const&)> log_func,
                          std::function<void()> on_thread_exit)
#endif
{
  if (thread_count <= 0)
  {
#ifdef BENCH_WITHOUT_PERF
    std::cout << "Thread Count 0 - Total messages 0 - " << benchmark_name << "\n | no latency samples |\n\n";
#endif
    return;
  }

  size_t const thread_count_size = static_cast<size_t>(thread_count);

  // main thread affinity
  set_thread_affinity(0);

  double const ns_rdtsc_tick = ns_per_rdtsc_tick();

#ifdef BENCH_WITHOUT_PERF
  // each thread gets a vector of latencies
  std::vector<std::vector<uint64_t>> latencies;
  latencies.resize(thread_count_size);
  for (auto& elem : latencies)
  {
    elem.reserve(num_iterations);
  }
#endif

  std::vector<std::thread> threads;
  threads.reserve(thread_count_size);
  std::atomic<size_t> started_threads{0};
  for (size_t thread_num = 0; thread_num < thread_count_size; ++thread_num)
  {
    size_t const thread_messages_per_iteration = messages_for_thread(MESSAGES, thread_count_size, thread_num);

#ifdef BENCH_WITHOUT_PERF
    // Spawn num threads
    threads.emplace_back(run_log_benchmark, num_iterations, thread_messages_per_iteration, on_thread_start,
                         log_func, on_thread_exit, std::ref(started_threads), thread_count_size,
                         thread_num + 1, std::ref(latencies[thread_num]), ns_rdtsc_tick);
#else
    // Spawn num threads
    threads.emplace_back(run_log_benchmark, num_iterations, thread_messages_per_iteration,
                         on_thread_start, log_func, on_thread_exit, std::ref(started_threads),
                         thread_count_size, thread_num + 1);
#endif
  }

  // Wait for threads to finish
  for (size_t i = 0; i < thread_count_size; ++i)
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

  size_t const total_messages = num_iterations * MESSAGES;
  if (latencies_combined.empty())
  {
    std::cout << "Thread Count " << thread_count << " - Total messages " << total_messages << " - "
              << benchmark_name << "\n | no latency samples |\n\n";
    return;
  }

  size_t const sample_count = latencies_combined.size();
  auto percentile_index = [sample_count](double percentile)
  {
    size_t const nearest_rank =
      static_cast<size_t>(std::ceil(static_cast<double>(sample_count) * percentile));
    return std::min(sample_count - 1, nearest_rank == 0 ? size_t{0} : (nearest_rank - 1));
  };

  std::cout << "Thread Count " << thread_count << " - Total messages " << total_messages << " - "
            << benchmark_name << "\n |  50th | 75th | 90th | 95th | 99th | 99.9th | Worst |\n"
            << " |  " << latencies_combined[percentile_index(0.5)] << "  |  "
            << latencies_combined[percentile_index(0.75)] << "  |  "
            << latencies_combined[percentile_index(0.9)] << "  |  "
            << latencies_combined[percentile_index(0.95)] << "  |  "
            << latencies_combined[percentile_index(0.99)] << "  |  "
            << latencies_combined[percentile_index(0.999)] << "  |  "
            << latencies_combined[latencies_combined.size() - 1] << "  |\n\n";
#endif
}
