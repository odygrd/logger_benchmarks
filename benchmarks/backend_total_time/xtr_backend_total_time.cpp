#define FMT_HEADER_ONLY
#define XTR_USE_IO_URING 0
// The 1 MiB queue per thread every library is given where it can be set (the default is 256 KiB)
#define XTR_SINK_CAPACITY (1024UL * 1024UL)

#include "fmt/format.h"
#include "xtr/io/posix_fd_storage.hpp"
#include "xtr/logger.hpp"

#include <fcntl.h>
#include <unistd.h>

#include <chrono>
#include <filesystem>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "utils.h"

namespace
{
/**
 * XTR's storage without the fsync in sync(). Every library's clock stops once its data has been
 * handed to the kernel; XTR's sink.sync() would also wait for the disk.
 */
class no_fsync_storage final : public xtr::posix_fd_storage
{
public:
  using xtr::posix_fd_storage::posix_fd_storage;

  void sync() noexcept override {}
};

xtr::logger* g_log = nullptr;

// Each producer's sink, created before timing starts. The sinks outlive the producer threads, so
// the main thread can wait on them after the producers have finished.
thread_local xtr::sink* tl_sink = nullptr;
std::mutex g_sinks_mutex;
std::vector<std::unique_ptr<xtr::sink>> g_sinks;
} // namespace

/**
 * The backend worker just spins, so we just measure the total time elapsed for total_iterations
 */
int main()
{
  set_thread_affinity(1);

  std::filesystem::path log_file{"benchmark_xtr_backend_total_time.log"};
  std::remove(log_file.c_str());

  int const fd = ::open(log_file.c_str(), O_CREAT | O_WRONLY | O_TRUNC | O_APPEND, 0644);
  if (fd == -1)
  {
    abort();
  }
  xtr::logger log(std::make_unique<no_fsync_storage>(fd, log_file.string()));
  ::close(fd);
  g_log = &log;

  set_pthread_affinity(log.consumer_thread_native_handle(), 5);

  auto sink = log.get_sink("main");

  XTR_LOGL_TSC(info, sink, "Warm up");
  sink.sync();

  // 256 distinct statements in a fixed pseudo-random order, see many_statements.h
  #define BENCH_LOG(...) XTR_LOGL_TSC(info, *tl_sink, __VA_ARGS__)
  auto log_statement = [&](size_t iteration)
  {
  #include "many_statements/fmt.inc"
  };

  // Each producer creates its sink and logs a warm-up line before timing starts
  auto on_producer_start = [](size_t)
  {
    auto producer_sink = std::make_unique<xtr::sink>(g_log->get_sink("main"));
    tl_sink = producer_sink.get();
    XTR_LOGL_TSC(info, *tl_sink, "Warm up");
    std::lock_guard<std::mutex> const lock{g_sinks_mutex};
    g_sinks.push_back(std::move(producer_sink));
  };

  ProducerTimes const times = run_producers(on_producer_start, log_statement);

  // Wait until the consumer has written every producer's messages, then the end line
  for (auto& producer_sink : g_sinks)
  {
    producer_sink->sync();
  }
  XTR_LOGL_TSC(error, sink, "End");
  sink.sync();

  auto const end_time = std::chrono::steady_clock::now();
  print_throughput(times, end_time, static_cast<double>(std::filesystem::file_size(log_file)) / (1024 * 1024));

  // The sinks must close before the logger is destroyed
  g_sinks.clear();
}
