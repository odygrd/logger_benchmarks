#define FMT_HEADER_ONLY
#define XTR_USE_IO_URING 0

#include "xtr/logger.hpp"

#include <atomic>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include <chrono>
#include <iostream>

#include "utils.h"
#include <filesystem>

static constexpr size_t total_iterations = 4'000'000;

/**
 * The backend worker just spins, so we just measure the total time elapsed for total_iterations
 */
int main()
{
  set_thread_affinity(1);

  std::filesystem::path log_file{"benchmark_xtr_backend_total_time.log"};
  std::remove(log_file.c_str());

  xtr::logger log(log_file.c_str());

  cpu_set_t cpus;
  CPU_ZERO(&cpus);
  CPU_SET(5, &cpus);
  if (::pthread_setaffinity_np(log.consumer_thread_native_handle(), sizeof(cpus), &cpus) != 0)
  {
    abort();
  }

  // wait for the backend thread to start
  std::this_thread::sleep_for(std::chrono::seconds(1));

  // start counting the time until backend worker finishes
  auto sink = log.get_sink("main");
  auto const start_time = std::chrono::steady_clock::now();
  for (size_t iteration = 0; iteration < total_iterations; ++iteration)
  {
    XTR_LOGL_TSC(info, sink, "Iteration: {} int: {} double: {}", iteration, iteration * 2,
                 static_cast<double>(iteration) / 2);
  }
  XTR_LOGL_TSC(error, sink, "End");

  sink.sync();

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