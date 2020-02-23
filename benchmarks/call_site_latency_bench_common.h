#pragma once
#include <chrono>

#define THREAD_LIST_COUNT                                                                          \
  std::vector<int32_t> { 1, 4 }
#define ITERATIONS                                                                                 \
  std::size_t { 100000 }
#define MIN_WAIT_DURATION                                                                          \
  std::chrono::microseconds { 1 }
#define MAX_WAIT_DURATION                                                                          \
  std::chrono::microseconds { 100 }