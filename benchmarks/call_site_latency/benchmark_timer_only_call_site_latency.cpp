#include "call_site_latency_bench.h"

#include <cstdint>
#include <string>
#include <vector>

/**
 * The measurement floor: the same harness, schedule and payloads as every library, with a call that only stores its
 * arguments instead of logging them. Its numbers are the cost of timing a sample, which is part of every library's
 * numbers too.
 */
namespace
{
volatile uintptr_t g_sink = 0;
}

/***/
void timer_only_benchmark(std::vector<int32_t> thread_count_array, size_t num_iterations_per_thread)
{
  auto log_func = [](uint64_t i, uint64_t j, auto const& v)
  { g_sink = static_cast<uintptr_t>(i + j) + reinterpret_cast<uintptr_t>(&v); };

  auto on_start = []() {};
  auto on_exit = []() {};

  for (auto thread_count : thread_count_array)
  {
    run_benchmark("Logger: Timer only - Benchmark: Caller Thread Latency", thread_count,
                  num_iterations_per_thread, MESSAGES_PER_ITERATION, on_start, log_func, on_exit);
  }
}

/***/
int main(int argc, char* argv[]) { timer_only_benchmark(THREAD_LIST_COUNT, ITERATIONS); }
