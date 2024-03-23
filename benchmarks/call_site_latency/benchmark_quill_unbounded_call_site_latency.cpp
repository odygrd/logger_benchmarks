#include "call_site_latency_bench.h"
#include "quill/Quill.h"

#include <string>

void quill_benchmark(std::vector<int32_t> thread_count_array, size_t num_iterations_per_thread)
{
  std::remove("benchmark_quill_no_dual_mode_unbounded_call_site_latency.log");

  // Setup
  quill::Config cfg;
  cfg.backend_thread_cpu_affinity = 5;
  cfg.enable_huge_pages_hot_path = true;

  // Start the logging backend thread
  quill::configure(cfg);
  quill::start();

  // wait for the backend thread to start
  std::this_thread::sleep_for(std::chrono::seconds(1));

  // Create a file handler to write to a file
  std::shared_ptr<quill::Handler> file_handler =
    quill::file_handler("benchmark_quill_unbounded_call_site_latency.log",
                        []()
                        {
                          quill::FileHandlerConfig cfg;
                          cfg.set_open_mode('w');
                          return cfg;
                        }());

  quill::Logger* logger = quill::create_logger("bench_logger", std::move(file_handler));

  // Define a logging lambda
#ifdef BENCH_INT_INT_DOUBLE
  char const* benchmark_type = "[benchmark_type: int_int_double]";

  auto log_func = [logger](uint64_t i, uint64_t j, double d)
  { LOG_INFO(logger, "Logging int: {}, int: {}, double: {}", i, j, d); };
#elif defined(BENCH_INT_INT_LARGESTR)
  char const* benchmark_type = "[benchmark_type: int_int_largestr]";

  auto log_func = [logger](uint64_t i, uint64_t j, std::string const& s)
  { LOG_INFO(logger, "Logging int: {}, int: {}, string: {}", i, j, s); };
#else
  static_assert(false, "define BENCH_INT_INT_DOUBLE or BENCH_INT_INT_LARGESTR");
#endif

#ifdef QUILL_X86ARCH
  char const* quill_x86_arch = "[quill_x86_arch: on]";
#else
  char const* quill_x86_arch = "[quill_x86_arch: off]";
#endif

  auto on_start = []() { quill::preallocate(); };

  auto on_exit = []() { quill::flush(); };

  // Run the benchmark for n threads
  std::string benchmark_name = "Logger: Quill - Benchmark: Caller Thread Latency, Unbounded";
  benchmark_name += " - ";
  benchmark_name += quill_x86_arch;
  benchmark_name += " - ";
  benchmark_name += benchmark_type;

  for (auto thread_count : thread_count_array)
  {
    run_benchmark(benchmark_name.c_str(), thread_count,
                  num_iterations_per_thread, on_start, log_func, on_exit);
  }
}

/***/
int main(int argc, char* argv[]) { quill_benchmark(THREAD_LIST_COUNT, ITERATIONS); }