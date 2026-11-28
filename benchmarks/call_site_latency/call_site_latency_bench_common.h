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
#include <atomic>
#include <cstring>

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
 * - Disabled argument: the hot disabled statement with an argument that is expensive to compute,
 *   which shows whether a logger evaluates the arguments of a statement it filters out. Built
 *   with BENCH_DISABLED_EXPENSIVE_ARG on top of the disabled scenario.
 *
 * All threads follow the same schedule of start times, so with several threads the calls of each
 * sample are made at the same time.
 */
#if defined(BENCH_SCENARIO_SINGLE)
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
 * a disabled statement reloads the logger's level on every call. Every scenario makes it after
 * every call.
 */
inline void bench_compiler_barrier() noexcept
{
#if defined(_MSC_VER) && !defined(__clang__)
  _ReadWriteBarrier();
#else
  asm volatile("" ::: "memory");
#endif
}

#if defined(_MSC_VER) && !defined(__clang__)
  #define BENCH_NOINLINE __declspec(noinline)
#else
  #define BENCH_NOINLINE __attribute__((noinline))
#endif

/**
 * Spins for the given number of iterations of a loop of one decrement and branch. The empty
 * statement keeps the compiler from removing the loop and adds no instruction with GCC and Clang.
 * It is never inlined, so every caller runs the same loop at the same speed.
 */
BENCH_NOINLINE inline void bench_counted_spin(uint64_t iterations) noexcept
{
  for (; iterations != 0; --iterations)
  {
#if defined(_MSC_VER) && !defined(__clang__)
    __nop();
#else
    asm volatile("");
#endif
  }
}

/**
 * Has no value of its own: computing its address makes a thread read its thread pointer
 */
inline thread_local char bench_thread_local_anchor = 0;

/**
 * Written by bench_touch_thread_state(), which puts the page of the benchmark's own globals in the
 * TLB: the variable that the timer only benchmark stores to is on it
 */
inline std::atomic<uintptr_t> g_bench_thread_state_sink{0};

/**
 * Puts back in the cache what a running thread always has there and the cache sweep of the cold
 * scenarios evicted with the rest:
 * - The 4 KiB of stack below the caller, where the timed function and the functions of the logger
 *   that it calls have their frames. One byte of every cache line is written, and the fence waits
 *   until the writes have reached the cache.
 * - The thread pointer, which computing the address of a thread local reads.
 * - The translation of the page of the benchmark's own globals.
 *
 * The data of the logger stays evicted. It is never inlined, so that its frame is where those
 * frames will be.
 */
BENCH_NOINLINE inline void bench_touch_thread_state() noexcept
{
  volatile char frame[4096];
  for (size_t i = 0; i < sizeof(frame); i += 64)
  {
    frame[i] = 0;
  }
  frame[sizeof(frame) - 1] = 0;

  g_bench_thread_state_sink.store(reinterpret_cast<uintptr_t>(&bench_thread_local_anchor), std::memory_order_relaxed);

  _mm_mfence();
}

/**
 * The calls of bench_expensive_argument() and the result of its last call
 */
inline std::atomic<uint64_t> g_bench_expensive_argument_calls{0};
inline std::atomic<uint64_t> g_bench_expensive_argument_state{0};

/**
 * An argument that is expensive to compute: 64 dependent xorshift rounds, several hundred CPU
 * cycles. It is never inlined and counts its calls, so the compiler cannot drop a call that the
 * logger makes, and the count tells whether the logger evaluated the argument. Each call starts
 * from the result of the call before it, so the CPU cannot run two calls at the same time and
 * every call takes the whole time. It is called by one thread.
 */
BENCH_NOINLINE inline double bench_expensive_argument(double value) noexcept
{
  g_bench_expensive_argument_calls.store(g_bench_expensive_argument_calls.load(std::memory_order_relaxed) + 1,
                                         std::memory_order_relaxed);

  uint64_t x;
  std::memcpy(&x, &value, sizeof(x));
  x = (x ^ g_bench_expensive_argument_state.load(std::memory_order_relaxed)) | 1;

  for (int round = 0; round < 64; ++round)
  {
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
  }

  g_bench_expensive_argument_state.store(x, std::memory_order_relaxed);
  return static_cast<double>(x >> 11);
}

/**
 * The last argument of the disabled statement: the payload, or an expensive function of it when
 * the benchmark is built with BENCH_DISABLED_EXPENSIVE_ARG
 */
#if defined(BENCH_DISABLED_EXPENSIVE_ARG)
  #define BENCH_DISABLED_ARG(value) bench_expensive_argument(value)
#else
  #define BENCH_DISABLED_ARG(value) (value)
#endif
