#include "binlog/binlog.hpp"
#include "spdlog/fmt/fmt.h"
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

static constexpr size_t total_iterations = 4'000'000;

/**
 * The backend worker just spins, so we just measure the total time elapsed for total_iterations
 */
int main()
{
  set_thread_affinity(1);

  std::filesystem::path log_file{"benchmark_ms_binlog_backend_total_time.blog"};
  std::remove(log_file.c_str());

  // Start a binlog backend logging threads to consume the logs
  std::atomic<bool> done{false};

  std::thread backend(
    [&done, &log_file]()
    {
      std::ofstream logfile(log_file.c_str(), std::ofstream::out | std::ofstream::binary);

      // pin to cpu
      set_thread_affinity(5);

      binlog::Session::ConsumeResult consume_result;
      while (true)
      {
        consume_result = binlog::consume(logfile);

        if (done.load(std::memory_order_relaxed) && consume_result.bytesConsumed == 0)
        {
          break;
        }
      }
    });

  BINLOG_INFO("Warm up");
  std::this_thread::sleep_for(std::chrono::seconds{1});

  // start counting the time until backend worker finishes
  auto const start_time = std::chrono::steady_clock::now();
  for (size_t iteration = 0; iteration < total_iterations; ++iteration)
  {
    BINLOG_INFO("Iteration: {} int: {} double: {}", iteration, iteration * 2,
                static_cast<double>(iteration) / 2);
  }
  BINLOG_ERROR("End");

  // block until all messages are flushed
  done.store(true, std::memory_order_relaxed);
  backend.join();

  auto const end_time = std::chrono::steady_clock::now();
  auto const delta = end_time - start_time;

  auto delta_d = std::chrono::duration_cast<std::chrono::duration<double>>(delta).count();

  std::cout << fmt::format(
                 "throughput is {:.2f} million msgs/sec average, total time elapsed: {} ms, log "
                 "file size {:2.f} MB \n",
                 total_iterations / delta_d / 1e6,
                 std::chrono::duration_cast<std::chrono::milliseconds>(delta).count(),
                 static_cast<double>(std::filesystem::file_size(log_file)) / (1024 * 1024))
            << std::endl;
}