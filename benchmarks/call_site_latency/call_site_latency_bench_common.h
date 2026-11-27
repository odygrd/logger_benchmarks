#pragma once
#include <chrono>
#include <cstdint>
#include <vector>
#include "../bench_platform.h"

#if defined(_WIN32)
  #include <intrin.h>
#else
  #include <x86intrin.h>
#endif
#include <algorithm>
#include <array>

/**
 * Every call site latency benchmark is built once per scenario. The scenario is selected with one
 * of the BENCH_SCENARIO_* definitions; without one the benchmark runs the burst scenario.
 *
 * - Burst: 20 back-to-back calls per thread, every 2 to 2.2 ms. Each sample is the mean of the 20
 *   calls.
 * - Single: one call per thread every 100 to 300 us. Each sample is one call.
 * - Cold: one call after the caller has read a buffer several times the size of its L2 cache, so
 *   the logger's data comes from L3 or memory. One thread only.
 * - Disabled: a debug statement while the logger's level is info, hot (the mean of 1,000 calls)
 *   and cold. One thread only.
 * - Defaults: the single scenario with every library configured as shipped.
 *
 * All threads follow the same schedule of start times, so with several threads the calls of each
 * sample are made at the same time.
 */
#if defined(BENCH_SCENARIO_SINGLE) || defined(BENCH_SCENARIO_DEFAULTS)
  #define THREAD_LIST_COUNT std::vector<int32_t>{1, 4}
  #define ITERATIONS std::size_t{100'000}
  #define MESSAGES_PER_ITERATION std::size_t{1}
  #define WARMUP_ITERATIONS std::size_t{1'000}
  #define MIN_WAIT_DURATION std::chrono::microseconds{100}
  #define MAX_WAIT_DURATION std::chrono::microseconds{300}
#elif defined(BENCH_SCENARIO_COLD)
  #define THREAD_LIST_COUNT std::vector<int32_t>{1}
  #define ITERATIONS std::size_t{20'000}
  #define MESSAGES_PER_ITERATION std::size_t{1}
  #define WARMUP_ITERATIONS std::size_t{500}
  // The cache sweep before every sample already takes longer than this
  #define MIN_WAIT_DURATION std::chrono::microseconds{100}
  #define MAX_WAIT_DURATION std::chrono::microseconds{100}
  #define BENCH_COLD_CACHE
#elif defined(BENCH_SCENARIO_DISABLED)
  #define THREAD_LIST_COUNT std::vector<int32_t>{1}
  #define ITERATIONS std::size_t{20'000}
  #define MESSAGES_PER_ITERATION std::size_t{1}
  #define WARMUP_ITERATIONS std::size_t{500}
  #define MIN_WAIT_DURATION std::chrono::microseconds{100}
  #define MAX_WAIT_DURATION std::chrono::microseconds{100}
  // Calls per sample of the hot measurement, the disabled check is too short to time one call
  #define DISABLED_HOT_CALLS std::size_t{1'000}
#else
  #define BENCH_SCENARIO_BURST
  #define THREAD_LIST_COUNT std::vector<int32_t>{1, 4}
  #define ITERATIONS std::size_t{10'000}
  // Messages per sample, per thread
  #define MESSAGES_PER_ITERATION std::size_t{20}
  #define WARMUP_ITERATIONS std::size_t{100}
  // Give the backend time to catch up between bursts. Sustained throughput is measured by the
  // backend_total_time benchmarks.
  #define MIN_WAIT_DURATION std::chrono::microseconds{2000}
  #define MAX_WAIT_DURATION std::chrono::microseconds{2200}
#endif

inline char const* bench_scenario_name() noexcept
{
#if defined(BENCH_SCENARIO_SINGLE)
  return "single";
#elif defined(BENCH_SCENARIO_DEFAULTS)
  return "defaults";
#elif defined(BENCH_SCENARIO_COLD)
  return "cold";
#elif defined(BENCH_SCENARIO_DISABLED)
  return "disabled";
#else
  return "burst";
#endif
}

// RDTSC is not a serializing instruction. Fence both sides so the measured
// logging calls cannot move into or out of the timed region.
inline uint64_t serialized_rdtsc() noexcept
{
  _mm_lfence();
  uint64_t const timestamp = __rdtsc();
  _mm_lfence();
  return timestamp;
}

/**
 * Stops the compiler from keeping values loaded by one logging call in registers for the next, so
 * a disabled statement reloads the logger's level on every call
 */
inline void bench_compiler_barrier() noexcept
{
#if defined(_MSC_VER) && !defined(__clang__)
  _ReadWriteBarrier();
#else
  asm volatile("" ::: "memory");
#endif
}
