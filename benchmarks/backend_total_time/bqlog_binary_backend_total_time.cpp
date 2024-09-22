#define FMTLOG_BLOCK 1

#include "spdlog/fmt/fmt.h"
#include "bq_log/bq_log.h"
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
  // we need to construct the filename because bqlog will append the date....

  std::time_t now = std::time(nullptr);
  std::tm* local_time = std::localtime(&now);

  char date_str[9];  // Buffer to hold the formatted date
  std::strftime(date_str, sizeof(date_str), "%Y%m%d", local_time);

  std::string log_file = fmt::format("benchmark_bqlog_backend_total_time_{}_1.logcompr", date_str);

  set_thread_affinity(1);

  // Seems there is no way to set the backend worker affinity for bqlog
  // I have edited `bg_log/log/log_worker.cpp:102` so that the backend thread pins to cpu 5

  bq::log log_obj = bq::log::create_log("text", R"(
        log.buffer_size=131072
        log.thread_mode=async
        log.reliable_level=normal
        appenders_config.appender_0.type=compressed_file
        appenders_config.appender_0.levels=[all]
        appenders_config.appender_0.file_name= benchmark_bqlog_backend_total_time
        appenders_config.appender_0.capacity_limit= 1
    )");

  log_obj.info("Warm up");
  log_obj.force_flush();

  // start counting the time until backend worker finishes
  auto const start_time = std::chrono::steady_clock::now();
  for (size_t iteration = 0; iteration < total_iterations; ++iteration)
  {
    log_obj.info("Iteration: {} int: {} double: {}", iteration, iteration * 2,
           static_cast<double>(iteration) / 2);
  }
  log_obj.info("End");

  // block until all messages are flushed
  log_obj.force_flush();

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