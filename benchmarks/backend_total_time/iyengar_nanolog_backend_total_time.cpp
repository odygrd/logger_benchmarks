#include "Iyengar_NanoLog/NanoLog.hpp"
#include "fmt/format.h"
#include <algorithm>
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

namespace
{
constexpr char end_marker[] = "IYENGAR_BACKEND_BENCHMARK_END";

bool file_tail_contains(std::filesystem::path const& path, char const* marker)
{
  std::ifstream input{path, std::ios::binary | std::ios::ate};
  if (!input)
  {
    return false;
  }

  std::streamoff const file_size = input.tellg();
  if (file_size <= 0)
  {
    return false;
  }

  constexpr std::streamoff max_tail_size = 4096;
  std::streamoff const tail_size = std::min(file_size, max_tail_size);
  input.seekg(file_size - tail_size);

  std::string tail(static_cast<size_t>(tail_size), '\0');
  input.read(tail.data(), tail_size);
  tail.resize(static_cast<size_t>(input.gcount()));
  return tail.find(marker) != std::string::npos;
}
} // namespace

/**
 * The backend worker just spins, so we just measure the total time elapsed for total_iterations
 */
int main()
{
  // The worker inherits the creating thread's affinity.
  set_thread_affinity(5);

  std::filesystem::path log_file{"benchmark_iyengar_backend_total_time.log"};
  std::filesystem::path actual_log_file{"benchmark_iyengar_backend_total_time.log.1.txt"};
  std::remove(actual_log_file.c_str());

  // Guaranteed nano log.
  nanolog::initialize(nanolog::GuaranteedLogger(), "./", log_file.c_str(), 10 * 1024 /* 10GB */);
  set_thread_affinity(1);

  LOG_CRIT << "Warm up";
  std::this_thread::sleep_for(std::chrono::seconds(1));

  // start counting the time until backend worker finishes
  auto const start_time = std::chrono::steady_clock::now();
  for (size_t iteration = 0; iteration < total_iterations; ++iteration)
  {
    LOG_INFO << "Iteration: " << iteration << " int: " << iteration * 2
             << " double: " << static_cast<double>(iteration) / 2;
  }
  LOG_CRIT << end_marker;

  // Iyengar NanoLog has no public flush API. A CRIT record flushes its output
  // stream, so observing this unique final marker is a reliable completion
  // signal without modifying the vendored library.
  while (!file_tail_contains(actual_log_file, end_marker))
  {
    std::this_thread::sleep_for(std::chrono::microseconds{100});
  }

  auto const end_time = std::chrono::steady_clock::now();
  auto const delta = end_time - start_time;

  auto delta_d = std::chrono::duration_cast<std::chrono::duration<double>>(delta).count();

  std::cout << fmt::format(
                 "throughput is {:.2f} million msgs/sec average, total time elapsed: {} ms, log "
                 "file size {:.2f} MB \n",
                 total_iterations / delta_d / 1e6,
                 std::chrono::duration_cast<std::chrono::milliseconds>(delta).count(),
                 static_cast<double>(std::filesystem::file_size(actual_log_file)) / (1024 * 1024))
            << std::endl;
}
