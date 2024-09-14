#pragma once

#include <sched.h>
#include <system_error>

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