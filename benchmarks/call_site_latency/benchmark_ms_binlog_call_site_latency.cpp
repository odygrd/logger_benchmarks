#include "call_site_latency_bench.h"
#include <fstream>
#include <iostream>
#include <thread>

#include "binlog/binlog.hpp"

/***/
void binlog_benchmark(std::vector<int32_t> thread_count_array, size_t num_iterations_per_thread)
{
  std::remove("binlog_call_site_latency_percentile_linux_benchmark.blog");

  // Start a binlog backend logging threads to consume the logs
  std::atomic<bool> done{false};

  std::thread backend([&done]() {
    std::ofstream logfile("binlog_call_site_latency_percentile_linux_benchmark.blog", std::ofstream::out | std::ofstream::binary);

    // pin to cpu
    set_thread_affinity(0);

    binlog::Session::ConsumeResult consume_result;
    do
    {
      if (consume_result.bytesConsumed == 0)
      {
        std::this_thread::sleep_for(std::chrono::nanoseconds{300});
      }

      consume_result = binlog::consume(logfile);
    } while (!done.load(std::memory_order_relaxed) || consume_result.bytesConsumed != 0);
  });

  // --- Benchmark ---
  // Define a logging lambda
  auto log_func = [](int32_t i, double d, char const* str) {
    auto const start = std::chrono::steady_clock::now();
    BINLOG_INFO("Logging str: {}, int: {}, double: {}", str, i, d);
    auto const end = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
  };

  auto on_start = []() {};

  auto on_exit = []() {};

  // Run the benchmark for n threads
  for (auto thread_count : thread_count_array)
  {
    run_benchmark("Logger: Ms binLog - Benchmark: Caller Thread Latency", thread_count,
                  num_iterations_per_thread, on_start, log_func, on_exit);
  }

  // stop the backend thread
  done.store(true, std::memory_order_relaxed);
  backend.join();
}

/***/
int main(int argc, char* argv[]) { binlog_benchmark(THREAD_LIST_COUNT, ITERATIONS); }