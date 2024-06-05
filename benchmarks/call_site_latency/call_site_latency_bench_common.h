#pragma once
#include <chrono>
#include <pthread.h>

#if defined(_WIN32)
#include <intrin.h>
#else
#include <x86intrin.h>
#endif
#include <array>
#include <algorithm>


// perf c2c record -g --call-graph dwarf,8192  ./benchmark_quill_call_site_latency
// perf c2c report -NN -g --call-graph -c pid,iaddr --stdio
#define BENCH_WITHOUT_PERF

#define THREAD_LIST_COUNT std::vector<int32_t> { 1, 4 }

#define ITERATIONS std::size_t { 10000 }
#define MESSAGES std::size_t { 20 }
#define MIN_WAIT_DURATION std::chrono::microseconds { 2000 }
#define MAX_WAIT_DURATION std::chrono::microseconds { 2200 }

//#define BENCH_INT_INT_DOUBLE
//#define BENCH_INT_INT_LARGESTR

inline void set_pthread_affinity(pthread_t thread, int cpu)
{
  cpu_set_t cpus;
  CPU_ZERO(&cpus);
  CPU_SET(cpu, &cpus);
  if (::pthread_setaffinity_np(thread, sizeof(cpus), &cpus) != 0)
    abort();
}

/** -------- **/
inline double ns_per_rdtsc_tick()
{
  // Convert rdtsc to wall time.
  // 1. Get real time and rdtsc current count
  // 2. Calculate how many rdtsc ticks can occur in one
  // calculate _ticks_per_ns as the median over a number of observations.
  constexpr std::chrono::milliseconds spin_duration = std::chrono::milliseconds{10};

  constexpr int trials = 13;
  std::array<double, trials> rates = {{0}};

  for (size_t i = 0; i < trials; ++i)
  {
    auto const beg_ts =
      std::chrono::nanoseconds{std::chrono::steady_clock::now().time_since_epoch().count()};
    uint64_t const beg_tsc = __rdtsc();

    std::chrono::nanoseconds elapsed_ns;
    uint64_t end_tsc;
    do
    {
      auto const end_ts =
        std::chrono::nanoseconds{std::chrono::steady_clock::now().time_since_epoch().count()};
      end_tsc = __rdtsc();

      elapsed_ns = end_ts - beg_ts;       // calculates ns between two timespecs
    } while (elapsed_ns < spin_duration); // busy spin for 10ms

    rates[i] = static_cast<double>(end_tsc - beg_tsc) / static_cast<double>(elapsed_ns.count());
  }

  std::nth_element(rates.begin(), rates.begin() + trials / 2, rates.end());

  double const ticks_per_ns = rates[trials / 2];
  return 1 / ticks_per_ns;
}