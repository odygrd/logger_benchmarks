#pragma once
#include <chrono>

// perf c2c record -g --call-graph dwarf,8192  ./benchmark_quill_call_site_latency
// perf c2c report -NN -g --call-graph -c pid,iaddr --stdio
#define BENCH_WITHOUT_PERF

#define THREAD_LIST_COUNT                                                                          \
  std::vector<int32_t> { 1, 4 }
#define ITERATIONS                                                                                 \
  std::size_t { 100000 }
#define MIN_WAIT_DURATION                                                                          \
  std::chrono::microseconds { 2 }
#define MAX_WAIT_DURATION                                                                          \
  std::chrono::microseconds { 100 }
