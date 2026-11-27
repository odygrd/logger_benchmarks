#define FMT_HEADER_ONLY
// Write with plain write(2) calls, as every other library does. As shipped, XTR uses io_uring
// when liburing is available.
#define XTR_USE_IO_URING 0

#include "call_site_latency_bench.h"

#include "fmt/ranges.h"
#include "xtr/logger.hpp"

#include <optional>
#include <string>

// The calling thread reads the CPU timestamp counter, as Quill and fmtlog do
#define BENCH_XTR_LOG(LEVEL, SINK, ...) XTR_LOGL_TSC(LEVEL, SINK, __VA_ARGS__)

namespace
{
xtr::logger* g_log = nullptr;

// Each thread's sink, created in the warm-up so the timed calls never create it
thread_local std::optional<xtr::sink> tl_sink;
} // namespace

/***/
void xtr_benchmark(std::vector<int32_t> thread_count_array, size_t num_iterations_per_thread)
{
  std::remove("xtr_call_site_latency_percentile_linux_benchmark.log");

  xtr::logger log("xtr_call_site_latency_percentile_linux_benchmark.log");
  g_log = &log;

  set_pthread_affinity(log.consumer_thread_native_handle(), 5);

  // wait for the backend thread to start
  std::this_thread::sleep_for(std::chrono::seconds(1));

  // Define a logging lambda
#ifdef BENCH_SCENARIO_DISABLED
  // A statement below the sink's level
  auto log_func = [](uint64_t i, uint64_t j, double d)
  { BENCH_XTR_LOG(debug, *tl_sink, "Logging int: {}, int: {}, double: {}", i, j, BENCH_DISABLED_ARG(d)); };
#elif defined(BENCH_INT_INT_DOUBLE)
  auto log_func = [](uint64_t i, uint64_t j, double d)
  { BENCH_XTR_LOG(info, *tl_sink, "Logging int: {}, int: {}, double: {}", i, j, d); };
#elif defined(BENCH_INT_INT_LARGESTR)
  auto log_func = [](uint64_t i, uint64_t j, std::string const& s)
  { BENCH_XTR_LOG(info, *tl_sink, "Logging int: {}, int: {}, string: {}", i, j, s); };
#elif defined(BENCH_VECTOR_LARGESTR)
  auto log_func = [](uint64_t i, uint64_t j, std::vector<std::string> const& s)
  { BENCH_XTR_LOG(info, *tl_sink, "Logging int: {}, int: {}, vector: {}", i, j, s); };
#endif

  auto on_start = []()
  {
    if (!tl_sink)
    {
      tl_sink.emplace(g_log->get_sink("main"));
      tl_sink->set_level(xtr::log_level_t::info);
    }
    BENCH_XTR_LOG(info, *tl_sink, "Warm up");
    tl_sink->sync();
  };

  auto on_exit = []() {};

  // Run the benchmark for n threads
  for (auto thread_count : thread_count_array)
  {
    run_benchmark("Logger: XTR - Benchmark: Caller Thread Latency", thread_count,
                  num_iterations_per_thread, MESSAGES_PER_ITERATION, on_start, log_func, on_exit);
  }
}

int main(int argc, char* argv[]) { xtr_benchmark(THREAD_LIST_COUNT, ITERATIONS); }
