#include "quill/Quill.h"
#include <chrono>

static constexpr size_t total_iterations = 100'000;

/**
 * The backend worker just spins, so we just measure the total time elapsed for total_iterations
 */
int main()
{
  // main thread affinity
  quill::detail::set_cpu_affinity(0);

  /** - Setup Quill **/
  quill::Config cfg;
  cfg.backend_thread_sleep_duration = std::chrono::nanoseconds{0};
  cfg.backend_thread_cpu_affinity = 1;

  quill::configure(cfg);

  // Start the logging backend thread and give it some time to init
  quill::start();
  std::this_thread::sleep_for(std::chrono::milliseconds{100});

  // Create a file handler to write to a file
  quill::Handler* file_handler = quill::file_handler("quill_backend_total_time.log", "w");

  quill::Logger* logger = quill::create_logger("bench_logger", file_handler);

  quill::preallocate();

  // start counting the time until backend worker finishes
  auto const start_time = std::chrono::steady_clock::now();
  for (size_t iteration = 0; iteration < total_iterations; ++iteration)
  {
    LOG_INFO(logger, "Iteration: {}, int: {}, double: {}", iteration, iteration * 2,
             static_cast<double>(iteration) / 2);
  }
  LOG_ERROR(logger, "End");

  // block until all messages are flushed
  quill::flush();
  auto const end_time = std::chrono::steady_clock::now();
  auto const delta = end_time - start_time;

  auto delta_d =  std::chrono::duration_cast<std::chrono::duration<double>>(delta).count();

  std::cout << fmt::format("throughput  is {:.2f} million msgs/sec average, total time elapsed: {} ms \n", total_iterations / delta_d / 1e6, std::chrono::duration_cast<std::chrono::milliseconds>(delta).count()) << std::endl;

}