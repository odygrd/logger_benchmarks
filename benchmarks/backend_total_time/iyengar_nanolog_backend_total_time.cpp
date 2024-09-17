#include "Iyengar_NanoLog/NanoLog.hpp"
#include "spdlog/fmt/fmt.h"
#include <atomic>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include <chrono>
#include <filesystem>
#include <iostream>

#include "utils.h"

static constexpr size_t total_iterations = 4'000'000;

/**
 * The backend worker just spins, so we just measure the total time elapsed for total_iterations
 */
int main()
{
  set_thread_affinity(1);

  std::filesystem::path log_file{"benchmark_iyengar_backend_total_time.log"};
  std::filesystem::path actual_log_file{"benchmark_iyengar_backend_total_time.log.1.txt"};
  std::remove(actual_log_file.c_str());

  // Guaranteed nano log.
  nanolog::initialize(nanolog::GuaranteedLogger(), "./", log_file.c_str(), 10 * 1024 /* 10GB */);

  IY_LOG_CRIT << "Warm up";
  std::this_thread::sleep_for(std::chrono::seconds(1));

  // start counting the time until backend worker finishes
  auto const start_time = std::chrono::steady_clock::now();
  for (size_t iteration = 0; iteration < total_iterations; ++iteration)
  {
    IY_LOG_INFO << "Iteration: " << iteration << " int: " << iteration * 2
                << " double: " << static_cast<double>(iteration) / 2;
  }
  IY_LOG_CRIT << "End";

  auto const end_time = std::chrono::steady_clock::now();
  auto const delta = end_time - start_time;

  auto delta_d = std::chrono::duration_cast<std::chrono::duration<double>>(delta).count();

  // NOTE: Library does not offer a way to flush and wait here in order to measure the time ..
  // !! This benchmark is not accurate !!

  std::cout << fmt::format(
                 "throughput is {:.2f} million msgs/sec average, total time elapsed: {} ms, log "
                 "file size {:2.f} MB \n",
                 total_iterations / delta_d / 1e6,
                 std::chrono::duration_cast<std::chrono::milliseconds>(delta).count(),
                 static_cast<double>(std::filesystem::file_size(actual_log_file)) / (1024 * 1024))
            << std::endl;

  std::cerr << "Benchmark is not accurate";
}