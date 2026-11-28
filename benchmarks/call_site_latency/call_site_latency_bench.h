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
#include <type_traits>
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
 *
 * A thread waits for its next start time in two stages: counted loops of one decrement and branch
 * for most of the gap, then a loop that reads the timestamp counter for the last 16 to 20 us. Both
 * are needed on the Intel Golden Cove cores these benchmarks run on:
 * - Reading the timestamp counter for the whole gap leaves the branch predictor in a state that an
 *   application does not log from. After about 40 us in one small loop that is fetched and predicted
 *   on every iteration, the first taken branches after the loop are mispredicted, on every sample,
 *   and one of them can be a branch of the timed log call: about 8 ns added to a call of 5 ns. The
 *   counted loop is replayed from the loop buffer of the CPU without the branch predictor, and the
 *   timestamp loop that follows it is shorter than that.
 * - A counted loop until the start time leaves a second effect: with four threads leaving it at the
 *   same time, the calls made in the first microseconds after it stall for about 10 ns, in up to a
 *   third of the samples. After 16 us or more of the timestamp loop they do not.
 */
class Schedule
{
public:
  Schedule(uint64_t start_tsc, double ns_rdtsc_tick)
    : _next(start_tsc), _ticks_per_ns(1.0 / ns_rdtsc_tick), _gen(0x51ee'50a7u),
      _dist(static_cast<int64_t>(std::chrono::nanoseconds{MIN_WAIT_DURATION}.count()),
            static_cast<int64_t>(std::chrono::nanoseconds{MAX_WAIT_DURATION}.count())),
      _fine_wait_ticks(static_cast<uint64_t>(fine_wait_ns * _ticks_per_ns))
  {
    // The speed of the counted loop on this core: the fastest of a few runs
    for (int trial = 0; trial < 3; ++trial)
    {
      uint64_t const begin = __rdtsc();
      bench_counted_spin(calibration_iterations);
      uint64_t const elapsed = __rdtsc() - begin;
      _iterations_per_tick =
        std::max(_iterations_per_tick, static_cast<double>(calibration_iterations) / static_cast<double>(elapsed));
    }
    _min_iterations_per_tick = _iterations_per_tick / 8;
  }

  /**
   * Waits until the next start time. A thread that fell behind starts at once.
   *
   * It is never inlined, so that the benchmark loop is compiled the same way whatever this
   * function does.
   */
  BENCH_NOINLINE void wait_next() noexcept
  {
    // Counted loops until less than a quarter more than the fine wait is left. Each one aims short
    // of the start of the fine wait, so that a loop that runs slower than the last one did does not
    // overrun it.
    for (;;)
    {
      uint64_t const now = __rdtsc();
      if ((now >= _next) || ((_next - now) <= _fine_wait_ticks + _fine_wait_ticks / 4))
      {
        break;
      }

      uint64_t const iterations = static_cast<uint64_t>(
        static_cast<double>(_next - now - _fine_wait_ticks) * _iterations_per_tick * coarse_wait_fraction);
      bench_counted_spin(iterations);

      if (iterations >= min_measured_iterations)
      {
        // Follow the speed of the core. The floor keeps one interrupted loop from shrinking the
        // loops that follow it to nothing.
        _iterations_per_tick = std::max(
          _min_iterations_per_tick, static_cast<double>(iterations) / static_cast<double>(__rdtsc() - now));
      }
    }

    while (__rdtsc() < _next)
    {
    }
    _next += static_cast<uint64_t>(static_cast<double>(_dist(_gen)) * _ticks_per_ns);
  }

private:
  // The timestamp loop runs for this and up to a quarter more: long enough for the second effect
  // above, and short of the first
  static constexpr double fine_wait_ns = 16'000.0;
  static constexpr double coarse_wait_fraction = 15.0 / 16.0;
  static constexpr uint64_t calibration_iterations = uint64_t{1} << 16;
  static constexpr uint64_t min_measured_iterations = uint64_t{1} << 12;

  uint64_t _next;
  double _ticks_per_ns;
  std::mt19937 _gen;
  std::uniform_int_distribution<int64_t> _dist;
  uint64_t _fine_wait_ticks;
  double _iterations_per_tick{0};
  double _min_iterations_per_tick{0};
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
 * Reads what the callable that makes the log call has captured, so that what the benchmark itself
 * keeps for the call is in the cache when the call starts, as the payload is: the pointer to the
 * logger, for the libraries that are given one. The logger it points to is not read.
 */
template <typename TLogFunc>
inline void touch_log_func([[maybe_unused]] TLogFunc const& log_func, [[maybe_unused]] std::atomic<size_t>& sink) noexcept
{
  if constexpr (!std::is_empty_v<TLogFunc>)
  {
    auto const* bytes = reinterpret_cast<unsigned char const*>(&log_func);
    size_t sum = 0;
    for (size_t i = 0; i < sizeof(TLogFunc); ++i)
    {
      sum += bytes[i];
    }
    sink.store(sum, std::memory_order_relaxed);
  }
}

/**
 * How a sample is measured. The number of calls per sample is a template argument of the functions
 * below, a constant of the scenario.
 */
struct SampleMode
{
  char const* name;
  bool cold;
};

/**
 * The timed region: CallsPerSample calls between two reads of the timestamp counter. Returns the
 * ticks between the two reads.
 *
 * It is a function of its own, never inlined, so that nothing of the harness is in the timed region:
 * - What the calls need from the harness arrives in registers, as arguments. Inlined into the
 *   benchmark loop, the compiler kept the number of calls per sample on the stack for some
 *   libraries and read it back between the two timestamps, from a cache line that the cold
 *   scenarios had evicted.
 * - The number of calls is a constant: with one call per sample the region is the call alone,
 *   without the branches of a loop around it.
 * - The region is compiled the same way whatever the rest of the harness does.
 */
template <size_t CallsPerSample, typename TLogFunc>
BENCH_NOINLINE uint64_t bench_timed_calls(TLogFunc const& log_func, size_t i, bench_payload_t const& v)
{
  uint64_t const start = serialized_rdtsc();
  for (size_t j = 0; j < CallsPerSample; ++j)
  {
    log_func(i, j, v);
    // Emits no instruction, and every scenario makes it after every call
    bench_compiler_barrier();
  }
  uint64_t const end = serialized_rdtsc();
  return end - start;
}

/***/
template <size_t CallsPerSample, typename TOnThreadStart, typename TLogFunc, typename TOnThreadExit>
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
      // The sweep also evicted what a running thread always has in its cache
      bench_touch_thread_state();
    }
    touch_payload(v, payload_sink);
    touch_log_func(log_func, payload_sink);

    uint64_t const ticks = bench_timed_calls<CallsPerSample>(log_func, i, v);

    // The first iterations warm up the logger and are not recorded
    if (i >= WARMUP_ITERATIONS)
    {
      latencies.push_back(static_cast<double>(ticks) * ns_rdtsc_tick / static_cast<double>(CallsPerSample));
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
template <size_t CallsPerSample, typename TOnThreadStart, typename TLogFunc, typename TOnThreadExit>
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
    threads.emplace_back(run_log_benchmark<CallsPerSample, TOnThreadStart, TLogFunc, TOnThreadExit>,
                         num_iterations, mode,
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

  size_t const total_messages = (WARMUP_ITERATIONS + num_iterations) * CallsPerSample * thread_count_size;
  print_latency_results(benchmark_name, mode, thread_count, total_messages, latencies_combined);

#if defined(BENCH_DISABLED_EXPENSIVE_ARG)
  // A logger that evaluates the arguments of a disabled statement computes the argument on every call
  uint64_t const evaluations = g_bench_expensive_argument_calls.exchange(0, std::memory_order_relaxed);
  std::cout << "Disabled argument: " << (evaluations != 0 ? "evaluated" : "skipped") << ", computed "
            << evaluations << " times in " << total_messages << " calls\n\n";
#endif
}

/**
 * Runs the scenario the benchmark is built for. Each thread makes MESSAGES_PER_ITERATION calls per
 * sample, a constant of the scenario: the messages_per_iteration argument is kept for the callers
 * and must be that constant.
 */
template <typename TOnThreadStart, typename TLogFunc, typename TOnThreadExit>
inline void run_benchmark(char const* benchmark_name, int32_t thread_count, size_t num_iterations,
                          [[maybe_unused]] size_t messages_per_iteration, TOnThreadStart const& on_thread_start,
                          TLogFunc const& log_func, TOnThreadExit const& on_thread_exit)
{
  if (thread_count <= 0)
  {
    std::cout << "Thread Count 0 - Total messages 0 - " << benchmark_name << "\n | no latency samples |\n\n";
    return;
  }

#if defined(BENCH_SCENARIO_DISABLED)
  constexpr size_t hot_calls = DISABLED_HOT_CALLS;
#else
  constexpr size_t calls_per_sample = MESSAGES_PER_ITERATION;
#endif

#if defined(BENCH_SCENARIO_DISABLED) && defined(BENCH_DISABLED_EXPENSIVE_ARG)
  run_benchmark_mode<hot_calls>(benchmark_name, SampleMode{"disabled_argument", false}, thread_count,
                                num_iterations / 10, on_thread_start, log_func, on_thread_exit);
#elif defined(BENCH_SCENARIO_DISABLED)
  run_benchmark_mode<hot_calls>(benchmark_name, SampleMode{"disabled_hot", false}, thread_count,
                                num_iterations / 10, on_thread_start, log_func, on_thread_exit);
  run_benchmark_mode<1>(benchmark_name, SampleMode{"disabled_cold", true}, thread_count, num_iterations,
                        on_thread_start, log_func, on_thread_exit);
#elif defined(BENCH_COLD_CACHE)
  run_benchmark_mode<calls_per_sample>(benchmark_name, SampleMode{bench_scenario_name(), true}, thread_count,
                                       num_iterations, on_thread_start, log_func, on_thread_exit);
#else
  run_benchmark_mode<calls_per_sample>(benchmark_name, SampleMode{bench_scenario_name(), false}, thread_count,
                                       num_iterations, on_thread_start, log_func, on_thread_exit);
#endif
}
