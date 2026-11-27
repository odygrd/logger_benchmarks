#pragma once

#include "../bench_platform.h"

#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdio>
#include <thread>
#include <vector>

/**
 * Every throughput benchmark logs total_iterations messages, split evenly between
 * BENCH_THROUGHPUT_THREADS producer threads pinned to CPUs 1 to 4. The backend runs on CPU 5.
 */
#ifndef BENCH_THROUGHPUT_THREADS
  #define BENCH_THROUGHPUT_THREADS 1
#endif

static constexpr size_t total_iterations = 4'000'000;

/**
 * Lines each benchmark writes: the main thread's warm-up line, one warm-up line per producer, the
 * messages and the main thread's end line
 */
static constexpr size_t expected_log_lines = total_iterations + BENCH_THROUGHPUT_THREADS + 2;

struct ProducerTimes
{
  std::chrono::steady_clock::time_point start;
  std::chrono::steady_clock::time_point producers_done;
};

/**
 * Starts the producer threads, then times them from when they are all ready until the last one
 * has logged its share.
 *
 * on_thread_start(thread_index) runs in each producer before timing starts. It must log one
 * warm-up line, so per-thread queues and other state are created outside the timed region.
 * log_statement(iteration) logs one message.
 */
template <typename TOnThreadStart, typename TLogStatement>
ProducerTimes run_producers(TOnThreadStart const& on_thread_start, TLogStatement const& log_statement)
{
  constexpr size_t thread_count = BENCH_THROUGHPUT_THREADS;
  std::atomic<size_t> ready{0};
  std::atomic<bool> go{false};

  std::vector<std::thread> threads;
  threads.reserve(thread_count);

  for (size_t thread_index = 0; thread_index < thread_count; ++thread_index)
  {
    threads.emplace_back(
      [&, thread_index]()
      {
        set_thread_affinity(1 + thread_index);
        on_thread_start(thread_index);

        ready.fetch_add(1, std::memory_order_acq_rel);
        while (!go.load(std::memory_order_acquire))
        {
        }

        size_t const begin = total_iterations * thread_index / thread_count;
        size_t const end = total_iterations * (thread_index + 1) / thread_count;
        for (size_t iteration = begin; iteration < end; ++iteration)
        {
          log_statement(iteration);
        }
      });
  }

  while (ready.load(std::memory_order_acquire) != thread_count)
  {
    std::this_thread::yield();
  }

  ProducerTimes times;
  times.start = std::chrono::steady_clock::now();
  go.store(true, std::memory_order_release);

  for (auto& thread : threads)
  {
    thread.join();
  }
  times.producers_done = std::chrono::steady_clock::now();

  return times;
}

/**
 * Prints the result line that tools/generate_tables.py parses
 */
inline void print_throughput(ProducerTimes const& times, std::chrono::steady_clock::time_point end_time,
                             double file_size_mb)
{
  auto const elapsed = end_time - times.start;
  double const elapsed_s = std::chrono::duration_cast<std::chrono::duration<double>>(elapsed).count();
  auto const elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
  auto const producers_ms =
    std::chrono::duration_cast<std::chrono::milliseconds>(times.producers_done - times.start).count();

  std::printf(
    "throughput is %.2f million msgs/sec average, total time elapsed: %lld ms, producer threads: %d, "
    "producers done after: %lld ms, log file size %.2f MB, expected log lines: %zu\n\n",
    static_cast<double>(total_iterations) / elapsed_s / 1e6, static_cast<long long>(elapsed_ms),
    BENCH_THROUGHPUT_THREADS, static_cast<long long>(producers_ms), file_size_mb, expected_log_lines);
  std::fflush(stdout);
}
