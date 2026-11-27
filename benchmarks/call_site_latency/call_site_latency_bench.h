#pragma once

#include "call_site_latency_bench_common.h"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <iostream>
#include <memory>
#include <numeric>
#include <random>
#include <string>
#include <thread>
#include <vector>

#if !defined(_WIN32)
  #include <unistd.h>
#endif

// Kept for the benchmarks that still test it; latency is always recorded now
#define BENCH_WITHOUT_PERF

/** -------- **/
inline double ns_per_rdtsc_tick()
{
  // Convert rdtsc to wall time: the median ticks per ns over a number of 10 ms observations
  constexpr std::chrono::milliseconds spin_duration = std::chrono::milliseconds{10};

  constexpr int trials = 13;
  std::array<double, trials> rates = {{0}};

  for (size_t i = 0; i < trials; ++i)
  {
    auto const beg_ts = std::chrono::nanoseconds{std::chrono::steady_clock::now().time_since_epoch().count()};
    uint64_t const beg_tsc = serialized_rdtsc();

    std::chrono::nanoseconds elapsed_ns;
    uint64_t end_tsc;
    do
    {
      auto const end_ts = std::chrono::nanoseconds{std::chrono::steady_clock::now().time_since_epoch().count()};
      end_tsc = serialized_rdtsc();

      elapsed_ns = end_ts - beg_ts;
    } while (elapsed_ns < spin_duration);

    rates[i] = static_cast<double>(end_tsc - beg_tsc) / static_cast<double>(elapsed_ns.count());
  }

  std::nth_element(rates.begin(), rates.begin() + trials / 2, rates.end());

  double const ticks_per_ns = rates[trials / 2];
  return 1 / ticks_per_ns;
}

/**
 * The size of the L2 cache of the calling thread's core, 2 MiB if it cannot be read
 */
inline size_t l2_cache_size() noexcept
{
  size_t size = 0;
#if defined(_WIN32)
  DWORD length = 0;
  GetLogicalProcessorInformationEx(RelationCache, nullptr, &length);
  std::vector<char> buffer(length);
  if (GetLogicalProcessorInformationEx(
        RelationCache, reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(buffer.data()), &length))
  {
    for (char* ptr = buffer.data(); ptr < buffer.data() + length;)
    {
      auto const* info = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(ptr);
      if (info->Cache.Level == 2)
      {
        size = std::max(size, static_cast<size_t>(info->Cache.CacheSize));
      }
      ptr += info->Size;
    }
  }
#elif defined(_SC_LEVEL2_CACHE_SIZE)
  long const value = sysconf(_SC_LEVEL2_CACHE_SIZE);
  size = value > 0 ? static_cast<size_t>(value) : 0;
#endif
  return size != 0 ? size : size_t{2} * 1024 * 1024;
}

/**
 * Evicts the caller's L1 and L2 caches by reading one byte of every cache line of a private
 * buffer four times the size of L2 (at least 4 MiB). The logger's data is then served from L3
 * or memory, as on a busy thread that logs every few events.
 */
class CacheSweeper
{
public:
  CacheSweeper() : _size(std::max(size_t{4} * 1024 * 1024, 4 * l2_cache_size())), _buffer(new char[_size])
  {
    // Touch every page so the sweep never faults
    for (size_t i = 0; i < _size; i += 64)
    {
      _buffer[i] = static_cast<char>(i);
    }
  }

  void sweep() noexcept
  {
    char sum = 0;
    for (size_t i = 0; i < _size; i += 64)
    {
      sum = static_cast<char>(sum + _buffer[i]);
    }
    _sink.store(sum, std::memory_order_relaxed);
  }

  size_t size() const noexcept { return _size; }

  /**
   * The median time in ns to reload a cache line read before a sweep, measured with a chain of
   * dependent loads. About an L3 hit means the sweep evicts L2 as intended.
   */
  double reload_latency_ns(double ns_rdtsc_tick)
  {
    constexpr size_t lines = 64;
    // One line per 4 KiB page plus a line, so the canaries do not share cache sets
    constexpr size_t stride = 4096 + 64;
    std::unique_ptr<char[]> storage(new char[lines * stride + 64]);
    std::vector<void*> chain(lines);
    for (size_t i = 0; i < lines; ++i)
    {
      chain[i] = storage.get() + i * stride;
    }
    for (size_t i = 0; i < lines; ++i)
    {
      *static_cast<void**>(chain[i]) = chain[(i + 1) % lines];
    }

    std::vector<double> samples;
    for (size_t trial = 0; trial < 201; ++trial)
    {
      void* p = chain[0];
      for (size_t i = 0; i < lines; ++i)
      {
        p = *static_cast<void**>(p);
      }
      sweep();
      uint64_t const start = serialized_rdtsc();
      for (size_t i = 0; i < lines; ++i)
      {
        p = *static_cast<void**>(p);
      }
      uint64_t const end = serialized_rdtsc();
      _sink.store(static_cast<char>(reinterpret_cast<uintptr_t>(p)), std::memory_order_relaxed);
      samples.push_back(static_cast<double>(end - start) * ns_rdtsc_tick / lines);
    }
    std::nth_element(samples.begin(), samples.begin() + samples.size() / 2, samples.end());
    return samples[samples.size() / 2];
  }

private:
  size_t _size;
  std::unique_ptr<char[]> _buffer;
  std::atomic<char> _sink{0};
};

/**
 * A shared schedule of sample start times. Every thread draws the same sequence of gaps, so with
 * several threads the calls of a sample are made at the same time.
 */
class Schedule
{
public:
  Schedule(uint64_t start_tsc, double ns_rdtsc_tick)
    : _next(start_tsc), _ticks_per_ns(1.0 / ns_rdtsc_tick), _gen(0x51ee'50a7u),
      _dist(static_cast<int64_t>(std::chrono::nanoseconds{MIN_WAIT_DURATION}.count()),
            static_cast<int64_t>(std::chrono::nanoseconds{MAX_WAIT_DURATION}.count()))
  {
  }

  /**
   * Spins until the next start time. A thread that fell behind starts at once.
   */
  void wait_next() noexcept
  {
    while (__rdtsc() < _next)
    {
    }
    _next += static_cast<uint64_t>(static_cast<double>(_dist(_gen)) * _ticks_per_ns);
  }

private:
  uint64_t _next;
  double _ticks_per_ns;
  std::mt19937 _gen;
  std::uniform_int_distribution<int64_t> _dist;
};

inline void wait_for_all_threads_to_start(std::atomic<size_t>& started_threads, size_t participant_count,
                                          std::atomic<uint64_t>& start_tsc, double ns_rdtsc_tick) noexcept
{
  if (started_threads.fetch_add(1, std::memory_order_acq_rel) + 1 == participant_count)
  {
    // The last thread to arrive sets the shared start time, 1 ms from now
    start_tsc.store(__rdtsc() + static_cast<uint64_t>(1'000'000.0 / ns_rdtsc_tick), std::memory_order_release);
  }
  while (start_tsc.load(std::memory_order_acquire) == 0)
  {
  }
}

/**
 * Payloads are built before the timed loop, so building them never runs between two samples.
 * A sample uses payloads[i % payload_count].
 */
constexpr size_t payload_count = 64;

#if defined(BENCH_VECTOR_LARGESTR)
using bench_payload_t = std::vector<std::string>;
#elif defined(BENCH_INT_INT_LARGESTR)
using bench_payload_t = std::string;
#else
using bench_payload_t = double;
#endif

inline std::vector<bench_payload_t> make_payloads([[maybe_unused]] size_t thread_num)
{
  std::vector<bench_payload_t> payloads;
  payloads.reserve(payload_count);

#if defined(BENCH_VECTOR_LARGESTR)
  // Keep vector contents identical for the same producer in every executable.
  // This matters for loggers that compress or defer formatting the payload.
  std::mt19937 gen{static_cast<std::mt19937::result_type>(0xc001'c0deu + thread_num)};
  std::uniform_int_distribution<int> length_dist(50, 60); // large strings
  std::uniform_int_distribution<int> char_dist(65, 90);   // ASCII uppercase letters
#endif

  for (size_t i = 0; i < payload_count; ++i)
  {
#if defined(BENCH_VECTOR_LARGESTR)
    std::vector<std::string> v(16);
    for (std::string& str : v)
    {
      int const length = length_dist(gen);
      str.reserve(static_cast<size_t>(length));
      for (int char_idx = 0; char_idx < length; ++char_idx)
      {
        str.push_back(static_cast<char>(char_dist(gen)));
      }
    }
    payloads.push_back(std::move(v));
#elif defined(BENCH_INT_INT_LARGESTR)
    std::string v{"Lorem ipsum dolor sit amet, consectetur "};
    v += std::to_string(i);
    payloads.push_back(std::move(v));
#else
    payloads.push_back(static_cast<double>(i) + (0.1 * static_cast<double>(i)));
#endif
  }

  return payloads;
}

/**
 * Reads the payload so it is in the cache when the call starts, as an argument the caller has
 * just computed would be
 */
inline void touch_payload(bench_payload_t const& payload, std::atomic<size_t>& sink) noexcept
{
#if defined(BENCH_VECTOR_LARGESTR)
  size_t sum = 0;
  for (std::string const& s : payload)
  {
    for (char c : s)
    {
      sum += static_cast<unsigned char>(c);
    }
  }
  sink.store(sum, std::memory_order_relaxed);
#elif defined(BENCH_INT_INT_LARGESTR)
  size_t sum = 0;
  for (char c : payload)
  {
    sum += static_cast<unsigned char>(c);
  }
  sink.store(sum, std::memory_order_relaxed);
#else
  sink.store(static_cast<size_t>(payload), std::memory_order_relaxed);
#endif
}

/**
 * How a sample is measured
 */
struct SampleMode
{
  char const* name;
  size_t calls_per_sample;
  bool cold;
  bool barrier_between_calls;
};

/***/
template <typename TOnThreadStart, typename TLogFunc, typename TOnThreadExit>
inline void run_log_benchmark(size_t num_iterations, SampleMode mode, TOnThreadStart const& on_thread_start,
                              TLogFunc const& log_func, TOnThreadExit const& on_thread_exit,
                              std::atomic<size_t>& started_threads, std::atomic<uint64_t>& start_tsc,
                              size_t participant_count, size_t current_thread_num,
                              std::vector<double>& latencies, double ns_rdtsc_tick)
{
  // running thread affinity
  set_thread_affinity(current_thread_num);

  std::vector<bench_payload_t> const payloads = make_payloads(current_thread_num);
  std::atomic<size_t> payload_sink{0};
  std::unique_ptr<CacheSweeper> sweeper;
  if (mode.cold)
  {
    sweeper = std::make_unique<CacheSweeper>();
  }

  on_thread_start();
  wait_for_all_threads_to_start(started_threads, participant_count, start_tsc, ns_rdtsc_tick);

  Schedule schedule{start_tsc.load(std::memory_order_acquire), ns_rdtsc_tick};
  size_t const total_iterations = WARMUP_ITERATIONS + num_iterations;
  latencies.reserve(num_iterations);

  for (size_t i = 0; i < total_iterations; ++i)
  {
    bench_payload_t const& v = payloads[i % payload_count];

    schedule.wait_next();

    if (sweeper)
    {
      sweeper->sweep();
    }
    touch_payload(v, payload_sink);

    auto const start = serialized_rdtsc();
    for (size_t j = 0; j < mode.calls_per_sample; ++j)
    {
      log_func(i, j, v);
      if (mode.barrier_between_calls)
      {
        bench_compiler_barrier();
      }
    }
    auto const end = serialized_rdtsc();

    // The first iterations warm up the logger and are not recorded
    if (i >= WARMUP_ITERATIONS)
    {
      latencies.push_back(static_cast<double>(end - start) * ns_rdtsc_tick /
                          static_cast<double>(mode.calls_per_sample));
    }
  }

  on_thread_exit();
}

inline void print_latency_results(char const* benchmark_name, SampleMode mode, int32_t thread_count,
                                  size_t total_messages, std::vector<double>& latencies)
{
  std::sort(latencies.begin(), latencies.end());

  std::cout << "Thread Count " << thread_count << " - Total messages " << total_messages << " - "
            << benchmark_name << " - Scenario: " << mode.name << "\n";

  if (latencies.empty())
  {
    std::cout << " | no latency samples |\n\n";
    return;
  }

  size_t const sample_count = latencies.size();
  auto percentile = [&latencies, sample_count](double p)
  {
    size_t const nearest_rank = static_cast<size_t>(std::ceil(static_cast<double>(sample_count) * p));
    return latencies[std::min(sample_count - 1, nearest_rank == 0 ? size_t{0} : (nearest_rank - 1))];
  };

  char line[256];
  std::snprintf(line, sizeof(line), " |  %.1f  |  %.1f  |  %.1f  |  %.1f  |  %.1f  |  %.1f  |  %.1f  |\n",
                percentile(0.5), percentile(0.75), percentile(0.9), percentile(0.95), percentile(0.99),
                percentile(0.999), latencies.back());

  std::cout << " |  50th | 75th | 90th | 95th | 99th | 99.9th | Worst |\n" << line << "\n";
}

/***/
template <typename TOnThreadStart, typename TLogFunc, typename TOnThreadExit>
inline void run_benchmark_mode(char const* benchmark_name, SampleMode mode, int32_t thread_count,
                               size_t num_iterations, TOnThreadStart const& on_thread_start,
                               TLogFunc const& log_func, TOnThreadExit const& on_thread_exit)
{
  size_t const thread_count_size = static_cast<size_t>(thread_count);

  // main thread affinity
  set_thread_affinity(0);

  double const ns_rdtsc_tick = ns_per_rdtsc_tick();

  if (mode.cold)
  {
    CacheSweeper sweeper;
    std::cout << "Cold cache check: sweeping " << sweeper.size() / 1024
              << " KiB, a line read before the sweep reloads in " << sweeper.reload_latency_ns(ns_rdtsc_tick)
              << " ns (median)\n";
  }

  std::vector<std::vector<double>> latencies(thread_count_size);
  std::vector<std::thread> threads;
  threads.reserve(thread_count_size);
  std::atomic<size_t> started_threads{0};
  std::atomic<uint64_t> start_tsc{0};

  for (size_t thread_num = 0; thread_num < thread_count_size; ++thread_num)
  {
    threads.emplace_back(run_log_benchmark<TOnThreadStart, TLogFunc, TOnThreadExit>, num_iterations, mode,
                         std::cref(on_thread_start), std::cref(log_func), std::cref(on_thread_exit),
                         std::ref(started_threads), std::ref(start_tsc), thread_count_size, thread_num + 1,
                         std::ref(latencies[thread_num]), ns_rdtsc_tick);
  }

  for (auto& thread : threads)
  {
    thread.join();
  }

  std::vector<double> latencies_combined;
  latencies_combined.reserve(num_iterations * thread_count_size);
  for (auto const& elem : latencies)
  {
    latencies_combined.insert(latencies_combined.end(), elem.begin(), elem.end());
  }

  size_t const total_messages = (WARMUP_ITERATIONS + num_iterations) * mode.calls_per_sample * thread_count_size;
  print_latency_results(benchmark_name, mode, thread_count, total_messages, latencies_combined);

#if defined(BENCH_DISABLED_EXPENSIVE_ARG)
  // A logger that evaluates the arguments of a disabled statement computes the argument on every call
  uint64_t const evaluations = g_bench_expensive_argument_calls.exchange(0, std::memory_order_relaxed);
  std::cout << "Disabled argument: " << (evaluations != 0 ? "evaluated" : "skipped") << ", computed "
            << evaluations << " times in " << total_messages << " calls\n\n";
#endif
}

/**
 * Runs the scenario the benchmark is built for. messages_per_iteration is the number of calls
 * each thread makes per sample.
 */
template <typename TOnThreadStart, typename TLogFunc, typename TOnThreadExit>
inline void run_benchmark(char const* benchmark_name, int32_t thread_count, size_t num_iterations,
                          size_t messages_per_iteration, TOnThreadStart const& on_thread_start,
                          TLogFunc const& log_func, TOnThreadExit const& on_thread_exit)
{
  if (thread_count <= 0)
  {
    std::cout << "Thread Count 0 - Total messages 0 - " << benchmark_name << "\n | no latency samples |\n\n";
    return;
  }

#if defined(BENCH_SCENARIO_DISABLED) && defined(BENCH_DISABLED_EXPENSIVE_ARG)
  run_benchmark_mode(benchmark_name, SampleMode{"disabled_argument", DISABLED_HOT_CALLS, false, true}, thread_count,
                     num_iterations / 10, on_thread_start, log_func, on_thread_exit);
#elif defined(BENCH_SCENARIO_DISABLED)
  run_benchmark_mode(benchmark_name, SampleMode{"disabled_hot", DISABLED_HOT_CALLS, false, true}, thread_count,
                     num_iterations / 10, on_thread_start, log_func, on_thread_exit);
  run_benchmark_mode(benchmark_name, SampleMode{"disabled_cold", 1, true, true}, thread_count, num_iterations,
                     on_thread_start, log_func, on_thread_exit);
#elif defined(BENCH_COLD_CACHE)
  run_benchmark_mode(benchmark_name, SampleMode{bench_scenario_name(), messages_per_iteration, true, false},
                     thread_count, num_iterations, on_thread_start, log_func, on_thread_exit);
#else
  run_benchmark_mode(benchmark_name, SampleMode{bench_scenario_name(), messages_per_iteration, false, false},
                     thread_count, num_iterations, on_thread_start, log_func, on_thread_exit);
#endif
}
