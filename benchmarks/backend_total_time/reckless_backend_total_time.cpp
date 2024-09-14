#include "spdlog/fmt/fmt.h"

#include "reckless/file_writer.hpp"
#include "reckless/severity_log.hpp"

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

  std::filesystem::path log_file{"benchmark_reckless_backend_total_time.log"};
  std::remove(log_file.c_str());

  using log_t = reckless::severity_log<reckless::indent<4>, ' ', reckless::severity_field, reckless::timestamp_field>;
  reckless::file_writer writer(log_file.c_str());
  log_t g_log(&writer, 512UL * 1024UL, 128UL * 1024UL * 1024UL);
  g_log.permanent_error_policy(reckless::error_policy::block);
  g_log.temporary_error_policy(reckless::error_policy::block);

  cpu_set_t cpus;
  CPU_ZERO(&cpus);
  CPU_SET(5, &cpus);
  if (::pthread_setaffinity_np(g_log.worker_thread().native_handle(), sizeof(cpus), &cpus) != 0)
  {
    abort();
  }

  // wait for the backend thread to start
  std::this_thread::sleep_for(std::chrono::seconds(1));

  // start counting the time until backend worker finishes
  auto const start_time = std::chrono::steady_clock::now();
  for (size_t iteration = 0; iteration < total_iterations; ++iteration)
  {
    g_log.info("Iteration: %d, int: %d, double: %f", iteration, iteration * 2,
               static_cast<double>(iteration) / 2);
  }
  g_log.error("End");

  g_log.flush();

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