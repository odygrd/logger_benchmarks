#include "call_site_latency_bench.h"

#include "quill/Backend.h"
#include "quill/Frontend.h"
#include "quill/LogMacros.h"
#include "quill/Logger.h"
#include "quill/sinks/FileSink.h"
#include "quill/std/Vector.h"

#include <string>

struct CustomFrontendOptions
{
#ifdef QUILL_USE_BOUNDED_DROPPING_QUEUE
  static constexpr quill::QueueType queue_type = quill::QueueType::BoundedDropping;
#else
  static constexpr quill::QueueType queue_type = quill::QueueType::UnboundedBlocking;
#endif

  // Set small capacity to demonstrate dropping messages in this example
  static constexpr uint32_t initial_queue_capacity = 131'072;
  static constexpr uint32_t blocking_queue_retry_interval_ns = 800;

  static constexpr bool huge_pages_enabled = false;
};

using frontend_t = quill::FrontendImpl<CustomFrontendOptions>;
using logger_t = quill::LoggerImpl<CustomFrontendOptions>;

void quill_benchmark(std::vector<int32_t> thread_count_array, size_t num_iterations_per_thread)
{
  std::remove("benchmark_quill_unbounded_call_site_latency.log");

  // Setup
  quill::BackendOptions backend_options;
  backend_options.backend_cpu_affinity = 5;
  quill::Backend::start(backend_options);

  // wait for the backend thread to start
  std::this_thread::sleep_for(std::chrono::seconds(1));

  // Frontend
  auto file_sink = frontend_t::create_or_get_sink<quill::FileSink>(
    "benchmark_quill_unbounded_call_site_latency.log",
    []()
    {
      quill::FileSinkConfig cfg;
      cfg.set_open_mode('w');
      cfg.set_filename_append_option(quill::FilenameAppendOption::None);
      return cfg;
    }(),
    quill::FileEventNotifier{});

  logger_t* logger =
    frontend_t::create_or_get_logger("root", std::move(file_sink),
                                     "%(time) [%(thread_id)] %(short_source_location:<28) "
                                     "LOG_%(log_level:<9) %(logger:<12) %(message)",
                                     "%H:%M:%S.%Qns", quill::Timezone::GmtTime);

  // Define a logging lambda
#ifdef BENCH_INT_INT_DOUBLE
  char const* benchmark_type = "[benchmark_type: int_int_double]";

  auto log_func = [logger](uint64_t i, uint64_t j, double d)
  { LOG_INFO(logger, "Logging int: {}, int: {}, double: {}", i, j, d); };
#elif defined(BENCH_INT_INT_LARGESTR)
  char const* benchmark_type = "[benchmark_type: int_int_largestr]";

  auto log_func = [logger](uint64_t i, uint64_t j, std::string const& s)
  { LOG_INFO(logger, "Logging int: {}, int: {}, string: {}", i, j, s); };
#elif defined(BENCH_VECTOR_LARGESTR)
  char const* benchmark_type = "[benchmark_type: vector_largestr]";

  auto log_func = [logger](uint64_t i, uint64_t j, std::vector<std::string> const& s)
  { LOG_INFO(logger, "Logging int: {}, int: {}, vector: {}", i, j, s); };
#else
  static_assert(false, "define BENCH_INT_INT_DOUBLE or BENCH_INT_INT_LARGESTR");
#endif

#ifdef QUILL_X86ARCH
  char const* quill_x86_arch = "[quill_x86_arch: on]";
#else
  char const* quill_x86_arch = "[quill_x86_arch: off]";
#endif

  auto on_start = []() { frontend_t::preallocate(); };

  auto on_exit = []() { frontend_t::get_all_loggers().front()->flush_log(); };

  // Run the benchmark for n threads
  std::string benchmark_name = "Logger: Quill - Benchmark: Caller Thread Latency, Unbounded";
  benchmark_name += " - ";
  benchmark_name += quill_x86_arch;
  benchmark_name += " - ";
  benchmark_name += benchmark_type;

  for (auto thread_count : thread_count_array)
  {
    run_benchmark(benchmark_name.c_str(), thread_count, num_iterations_per_thread, on_start, log_func, on_exit);
  }
}

/***/
int main(int argc, char* argv[]) { quill_benchmark(THREAD_LIST_COUNT, ITERATIONS); }