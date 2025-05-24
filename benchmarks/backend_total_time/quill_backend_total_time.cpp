#include "quill/Backend.h"
#include "quill/Frontend.h"
#include "quill/LogMacros.h"
#include "quill/Logger.h"
#include "quill/sinks/FileSink.h"
#include "quill/bundled/fmt/format.h"

#include <filesystem>

#include <chrono>
#include <iostream>

static constexpr size_t total_iterations = 4'000'000;

/**
 * The backend worker just spins, so we just measure the total time elapsed for total_iterations
 */
int main()
{
  std::filesystem::path log_file{"benchmark_quill_backend_total_time.log"};
  std::remove(log_file.c_str());

  // main thread affinity
  quill::detail::set_cpu_affinity(1);

  // Setup
  quill::BackendOptions backend_options;
  backend_options.cpu_affinity = 5;
  backend_options.sleep_duration = std::chrono::nanoseconds{0};
  quill::Backend::start(backend_options);

  // Frontend
  auto file_sink = quill::Frontend::create_or_get_sink<quill::FileSink>(
    log_file,
    []()
    {
      quill::FileSinkConfig cfg;
      cfg.set_open_mode('w');
      cfg.set_filename_append_option(quill::FilenameAppendOption::None);
      return cfg;
    }(),
    quill::FileEventNotifier{});

  quill::Logger* logger = quill::Frontend::create_or_get_logger(
    "root", std::move(file_sink),
    quill::PatternFormatterOptions{
      "%(time) [%(thread_id)] %(short_source_location) %(log_level) %(message)", "%H:%M:%S.%Qns",
      quill::Timezone::GmtTime, false});

  LOG_INFO(logger, "Warm up");
  logger->flush_log(0);

  // start counting the time until backend worker finishes
  auto const start_time = std::chrono::steady_clock::now();
  for (size_t iteration = 0; iteration < total_iterations; ++iteration)
  {
    LOG_INFO(logger, "Iteration: {} int: {} double: {}", iteration, iteration * 2,
             static_cast<double>(iteration) / 2);
  }
  LOG_ERROR(logger, "End");

  // block until all messages are flushed
  logger->flush_log(0);

  auto const end_time = std::chrono::steady_clock::now();
  auto const delta = end_time - start_time;

  auto delta_d = std::chrono::duration_cast<std::chrono::duration<double>>(delta).count();

  std::cout << fmtquill::format("throughput is {:.2f} million msgs/sec average, total time elapsed: {} ms, log "
                 "file size {:.2f} MB \n",
                 total_iterations / delta_d / 1e6,
                 std::chrono::duration_cast<std::chrono::milliseconds>(delta).count(),
                 static_cast<double>(std::filesystem::file_size(log_file)) / (1024 * 1024))
            << std::endl;
}