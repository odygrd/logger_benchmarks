#pragma once

#include <cstddef>
#include <cstdlib>
#include <system_error>
#include <thread>

#if defined(_WIN32)
  #ifndef NOMINMAX
    #define NOMINMAX
  #endif
  #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
  #endif
  #include <windows.h>

  #include <tlhelp32.h>

  #include <algorithm>
  #include <vector>
#else
  #include <cerrno>
  #include <pthread.h>
  #include <sched.h>
#endif

/**
 * Maps the CPU numbers used by the benchmarks (0 = main thread, 1-4 = caller threads,
 * 5 = backend thread) to an OS logical processor.
 *
 * On Linux this is the identity mapping.
 *
 * On Windows each benchmark CPU maps to the first logical processor of a distinct physical core,
 * preferring the highest performance efficiency class (P-cores on hybrid CPUs), so the benchmark
 * threads never share an SMT sibling or land on an E-core.
 */
inline size_t bench_cpu(size_t cpu)
{
#if defined(_WIN32)
  static std::vector<size_t> const cpu_map = []()
  {
    struct Core
    {
      int efficiency_class;
      size_t first_logical_cpu;
    };

    std::vector<Core> cores;

    DWORD length = 0;
    GetLogicalProcessorInformationEx(RelationProcessorCore, nullptr, &length);
    std::vector<char> buffer(length);

    if (GetLogicalProcessorInformationEx(
          RelationProcessorCore, reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(buffer.data()), &length))
    {
      for (char* ptr = buffer.data(); ptr < buffer.data() + length;)
      {
        auto const* info = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(ptr);

        // Only processor group 0 is used, as SetThreadAffinityMask works on the current group
        if ((info->Processor.GroupMask[0].Group == 0) && (info->Processor.GroupMask[0].Mask != 0))
        {
          unsigned long first_bit = 0;
          _BitScanForward64(&first_bit, info->Processor.GroupMask[0].Mask);
          cores.push_back(Core{info->Processor.EfficiencyClass, static_cast<size_t>(first_bit)});
        }

        ptr += info->Size;
      }
    }

    std::stable_sort(cores.begin(), cores.end(), [](Core const& lhs, Core const& rhs)
                     { return lhs.efficiency_class > rhs.efficiency_class; });

    std::vector<size_t> result;
    for (Core const& core : cores)
    {
      result.push_back(core.first_logical_cpu);
    }
    return result;
  }();

  return cpu < cpu_map.size() ? cpu_map[cpu] : cpu;
#else
  return cpu;
#endif
}

/**
 * Pins the calling thread to the given benchmark CPU
 */
inline void set_thread_affinity(size_t cpu_num)
{
#if defined(_WIN32)
  if (SetThreadAffinityMask(GetCurrentThread(), DWORD_PTR{1} << bench_cpu(cpu_num)) == 0)
  {
    throw std::system_error(static_cast<int>(GetLastError()), std::system_category());
  }
#else
  // Set this thread affinity
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(cpu_num, &cpuset);

  auto const err = sched_setaffinity(0, sizeof(cpuset), &cpuset);

  if (err == -1)
  {
    throw std::system_error(errno, std::system_category());
  }
#endif
}

/**
 * Pins another thread to the given benchmark CPU, aborts on failure
 */
inline void set_pthread_affinity(std::thread::native_handle_type thread, int cpu)
{
#if defined(_WIN32)
  if (SetThreadAffinityMask(static_cast<HANDLE>(thread), DWORD_PTR{1} << bench_cpu(static_cast<size_t>(cpu))) == 0)
    abort();
#else
  cpu_set_t cpus;
  CPU_ZERO(&cpus);
  CPU_SET(cpu, &cpus);
  if (::pthread_setaffinity_np(thread, sizeof(cpus), &cpus) != 0)
    abort();
#endif
}

/**
 * Some loggers do not expose their worker thread. On Linux the worker inherits the affinity of the
 * thread that creates it, so the benchmarks pin the creating thread (or tools/run_all.py starts the
 * process with taskset) before the logger is initialised, and this function is a no-op.
 *
 * Windows threads start with the process affinity instead of the creating thread's affinity, so on
 * Windows this pins every thread of the process except the calling thread to the given benchmark
 * CPU. It must be called after the logger is initialised and before any caller thread is started.
 */
inline void pin_other_threads_on_windows([[maybe_unused]] size_t cpu_num)
{
#if defined(_WIN32)
  DWORD const process_id = GetCurrentProcessId();
  DWORD const current_thread_id = GetCurrentThreadId();

  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
  if (snapshot == INVALID_HANDLE_VALUE)
  {
    abort();
  }

  THREADENTRY32 entry{};
  entry.dwSize = sizeof(entry);
  if (Thread32First(snapshot, &entry))
  {
    do
    {
      if ((entry.th32OwnerProcessID != process_id) || (entry.th32ThreadID == current_thread_id))
      {
        continue;
      }

      HANDLE thread = OpenThread(THREAD_SET_INFORMATION | THREAD_QUERY_INFORMATION, FALSE, entry.th32ThreadID);
      if (thread == nullptr)
      {
        // the thread exited after the snapshot was taken
        continue;
      }

      if (SetThreadAffinityMask(thread, DWORD_PTR{1} << bench_cpu(cpu_num)) == 0)
      {
        abort();
      }

      CloseHandle(thread);
    } while (Thread32Next(snapshot, &entry));
  }

  CloseHandle(snapshot);
#endif
}
