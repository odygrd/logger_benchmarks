#define FMTLOG_BLOCK 1

#include "fmt/format.h"
#include "fmtlog/fmtlog.h"
#include <atomic>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include <chrono>
#include <iostream>

#include "utils.h"

/**
 * The backend worker just spins, so we just measure the total time elapsed for total_iterations
 */
int main()
{
  set_thread_affinity(1);

  std::filesystem::path log_file{"benchmark_fmtlog_backend_total_time.log"};
  std::remove(log_file.string().c_str());

  fmtlog::setLogFile(log_file.string().c_str(), true);

  std::atomic<bool> done{false};

  std::thread backend(
    [&done]()
    {
      // pin to cpu backend logging thread.
      set_thread_affinity(5);

      do
      {
        fmtlog::poll(false);
      } while (!done.load(std::memory_order_acquire));

      fmtlog::poll(true);
    });

  FMTLOG(fmtlog::INF, "Warm up");
  std::this_thread::sleep_for(std::chrono::seconds{1});

  // start counting the time until backend worker finishes
  // 256 distinct statements in a fixed pseudo-random order, see many_statements.h
  #define BENCH_LOG(...) FMTLOG(fmtlog::INF, __VA_ARGS__)
  auto log_statement = [&](size_t iteration)
  {
  #include "many_statements/fmt.inc"
  };

  // Each producer logs a warm-up line first, so its per-thread state is created before timing
  auto on_producer_start = [&](size_t) { FMTLOG(fmtlog::INF, "Warm up"); };

  ProducerTimes const times = run_producers(on_producer_start, log_statement);
  auto const start_time = times.start;
  FMTLOG(fmtlog::ERR, "End");

  // block until all messages are flushed
  done.store(true, std::memory_order_release);
  backend.join();

  auto const end_time = std::chrono::steady_clock::now();
  print_throughput(times, end_time, static_cast<double>(std::filesystem::file_size(log_file)) / (1024 * 1024));
}
