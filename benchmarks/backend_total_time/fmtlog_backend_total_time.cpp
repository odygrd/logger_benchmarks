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

static constexpr size_t total_iterations = 4'000'000;

/**
 * The backend worker just spins, so we just measure the total time elapsed for total_iterations
 */
int main()
{
  set_thread_affinity(1);

  std::filesystem::path log_file{"benchmark_fmtlog_backend_total_time.log"};
  std::remove(log_file.c_str());

  fmtlog::setLogFile(log_file.c_str(), true);

  std::atomic<bool> done{false};

  std::thread backend(
    [&done]()
    {
      // pin to cpu backend logging thread.
      set_thread_affinity(5);

      do
      {
        fmtlog::poll(false);
      } while (!done.load(std::memory_order_relaxed));

      fmtlog::poll(true);
    });

  FMTLOG(fmtlog::INF, "Warm up");
  std::this_thread::sleep_for(std::chrono::seconds{1});

  // start counting the time until backend worker finishes
  auto const start_time = std::chrono::steady_clock::now();
  for (size_t iteration = 0; iteration < total_iterations; ++iteration)
  {
    FMTLOG(fmtlog::INF, "Iteration: {} int: {} double: {}", iteration, iteration * 2,
           static_cast<double>(iteration) / 2);
  }
  FMTLOG(fmtlog::ERR, "End");

  // block until all messages are flushed
  done.store(true, std::memory_order_relaxed);
  backend.join();

  auto const end_time = std::chrono::steady_clock::now();
  auto const delta = end_time - start_time;

  auto delta_d = std::chrono::duration_cast<std::chrono::duration<double>>(delta).count();

  std::cout << fmt::format(
                 "throughput is {:.2f} million msgs/sec average, total time elapsed: {} ms, log "
                 "file size {:.2f} MB \n",
                 total_iterations / delta_d / 1e6,
                 std::chrono::duration_cast<std::chrono::milliseconds>(delta).count(),
                 static_cast<double>(std::filesystem::file_size(log_file)) / (1024 * 1024))
            << std::endl;
}