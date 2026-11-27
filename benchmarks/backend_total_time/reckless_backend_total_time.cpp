#include "fmt/format.h"

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
  // A 1 MiB input buffer, the queue size every library is given where it can be set, and the
  // default output buffer
  log_t g_log(&writer, 1024UL * 1024UL, 0);
  g_log.permanent_error_policy(reckless::error_policy::block);
  g_log.temporary_error_policy(reckless::error_policy::block);

  cpu_set_t cpus;
  CPU_ZERO(&cpus);
  CPU_SET(5, &cpus);
  if (::pthread_setaffinity_np(g_log.worker_thread().native_handle(), sizeof(cpus), &cpus) != 0)
  {
    abort();
  }

  g_log.info("Warm up");
  g_log.flush();

  // start counting the time until backend worker finishes
  auto log_statement = [&](size_t iteration)
  {
    // Cycle between five log statements: an application does not repeat a single one
    switch (iteration % 5)
    {
    case 0:
      g_log.info("Iteration: %d, int: %d, double: %f", iteration, iteration * 2,
                 static_cast<double>(iteration) / 2);
      break;
    case 1:
      g_log.info("Order: %d, quantity: %d, price: %f", iteration, iteration * 2,
                 static_cast<double>(iteration) / 2);
      break;
    case 2:
      g_log.info("Request: %d, bytes: %d, elapsed: %f", iteration, iteration * 2,
                 static_cast<double>(iteration) / 2);
      break;
    case 3:
      g_log.info("Session: %d, sequence: %d, latency: %f", iteration, iteration * 2,
                 static_cast<double>(iteration) / 2);
      break;
    default:
      g_log.info("Worker: %d, processed: %d, load: %f", iteration, iteration * 2,
                 static_cast<double>(iteration) / 2);
      break;
    }
  };

  // Each producer logs a warm-up line first, so its per-thread state is created before timing
  auto on_producer_start = [&](size_t) { g_log.info("Warm up"); };

  ProducerTimes const times = run_producers(on_producer_start, log_statement);
  auto const start_time = times.start;

  g_log.error("End");
  g_log.flush();

  auto const end_time = std::chrono::steady_clock::now();
  print_throughput(times, end_time, static_cast<double>(std::filesystem::file_size(log_file)) / (1024 * 1024));
}
