#include "quill/Backend.h"
#include "quill/Frontend.h"
#include "quill/LogMacros.h"
#include "quill/Logger.h"
#include "quill/sinks/FileSink.h"

#include <chrono>
#include <iostream>

static constexpr size_t total_iterations = 4'000'000;

/**
 * The backend worker just spins, so we just measure the total time elapsed for total_iterations
 */
int main()
{
  // main thread affinity
  quill::detail::set_cpu_affinity(0);

  // Setup
  quill::BackendOptions backend_options;
  backend_options.cpu_affinity = 5;
  quill::Backend::start(backend_options);

  // wait for the backend thread to start
  std::this_thread::sleep_for(std::chrono::seconds(1));

  // Frontend
  auto file_sink = quill::Frontend::create_or_get_sink<quill::FileSink>(
    "benchmark_quill_unbounded_call_site_latency.log",
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

  quill::Frontend::preallocate();

  // start counting the time until backend worker finishes
  auto const start_time = std::chrono::steady_clock::now();
  for (size_t iteration = 0; iteration < total_iterations; ++iteration)
  {
    LOG_INFO(logger, "Iteration: {} int: {} double: {}", iteration, iteration * 2,
             static_cast<double>(iteration) / 2);
  }
  LOG_ERROR(logger, "End");

  // block until all messages are flushed
  quill::Frontend::get_all_loggers().front()->flush_log();

  auto const end_time = std::chrono::steady_clock::now();
  auto const delta = end_time - start_time;

  auto delta_d = std::chrono::duration_cast<std::chrono::duration<double>>(delta).count();

  std::cout << fmtquill::format(
                 "throughput is {:.2f} million msgs/sec average, total time elapsed: {} ms \n",
                 total_iterations / delta_d / 1e6,
                 std::chrono::duration_cast<std::chrono::milliseconds>(delta).count())
            << std::endl;
}