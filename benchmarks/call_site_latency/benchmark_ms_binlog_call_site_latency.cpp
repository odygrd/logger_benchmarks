#include "call_site_latency_bench.h"
#include <fstream>
#include <iostream>
#include <thread>

#include "binlog/binlog.hpp"

#include <string>

/***/
void binlog_benchmark(std::vector<int32_t> thread_count_array, size_t num_iterations_per_thread)
{
  std::remove("binlog_call_site_latency_percentile_linux_benchmark.blog");

  // Start a binlog backend logging threads to consume the logs
  std::atomic<bool> done{false};

  std::thread backend([&done]() {
    std::ofstream logfile("binlog_call_site_latency_percentile_linux_benchmark.blog", std::ofstream::out | std::ofstream::binary);

    // pin to cpu
    set_thread_affinity(5);

    // done is read before consuming, so once it is set one more consume writes every record logged
    // before it
    while (true)
    {
      bool const finished = done.load(std::memory_order_acquire);
      binlog::Session::ConsumeResult const consume_result = binlog::consume(logfile);
      if (finished && consume_result.bytesConsumed == 0)
      {
        break;
      }
    }
  });

  // wait for the backend thread to start
  std::this_thread::sleep_for(std::chrono::seconds(1));

  // --- Benchmark ---
  // Define a logging lambda
#ifdef BENCH_SCENARIO_DISABLED
  // A statement below the logger's level
  binlog::default_session().setMinSeverity(binlog::Severity::info);
  auto log_func = [](uint64_t i, uint64_t j, double d)
  { BINLOG_DEBUG("Logging int: {}, int: {}, double: {}", i, j, d); };
#elif defined(BENCH_INT_INT_DOUBLE)
  auto log_func = [](uint64_t i, uint64_t j, double d) {
    BINLOG_INFO("Logging int: {}, int: {}, double: {}", i, j, d);
  };
#elif defined(BENCH_INT_INT_LARGESTR)
  auto log_func = [](uint64_t i, uint64_t j, std::string const& s)
  { BINLOG_INFO("Logging int: {}, int: {}, string: {}", i, j, s); };
#elif defined(BENCH_VECTOR_LARGESTR)
  auto log_func = [](uint64_t i, uint64_t j, std::vector<std::string> const& s)
  { BINLOG_INFO("Logging int: {}, int: {}, vector: {}", i, j, s); };
#endif

  auto on_start = []()
  {
    BINLOG_INFO("Warm up");
    std::this_thread::sleep_for(std::chrono::seconds{1});
  };

  auto on_exit = []() {};

  // Run the benchmark for n threads
  for (auto thread_count : thread_count_array)
  {
    run_benchmark("Logger: Ms binLog - Benchmark: Caller Thread Latency", thread_count,
                  num_iterations_per_thread, MESSAGES_PER_ITERATION, on_start, log_func, on_exit);
  }

  // stop the backend thread
  done.store(true, std::memory_order_release);
  backend.join();
}

/***/
int main(int argc, char* argv[]) { binlog_benchmark(THREAD_LIST_COUNT, ITERATIONS); }
