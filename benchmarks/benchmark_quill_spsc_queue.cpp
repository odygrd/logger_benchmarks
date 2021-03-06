#include "quill/detail/BoundedSPSCQueue.h"
#include "quill/detail/record/LogRecord.h"
#include "quill/detail/record/RecordBase.h"
#include <thread>

//#define NO_PERF

using namespace quill::detail;
using SPSCQueueT = quill::detail::BoundedSPSCQueue<RecordBase>;

template <typename... FmtArgs>
auto push_record(SPSCQueueT& spsc_queue, StaticLogRecordInfo const* log_line_info, FmtArgs&&... fmt_args)
{
  // Resolve the type of the record first
  using log_record_t = quill::detail::LogRecord<FmtArgs...>;

  // emplace to the spsc queue owned by the ctx
  bool retry;
  do
  {
    retry = spsc_queue.try_emplace<log_record_t>(log_line_info, nullptr, std::forward<FmtArgs>(fmt_args)...);
    // unlikely case if the queue gets full we will wait until we can log
  } while (QUILL_UNLIKELY(!retry));
}

int main(int argc, char* argv[])
{
  alignas(64) std::atomic<bool> backend_running{true};

  SPSCQueueT queue{262144};

  // start a backend
  std::thread backend([&queue, &backend_running]() {
    quill::detail::set_cpu_affinity(1);

    // size_t cnt {0};
    while (backend_running.load(std::memory_order_relaxed) || !queue.empty())
    {
      auto handle = queue.try_pop();
      if (handle.is_valid())
      {
        // std::cout << cnt++ << " " << handle.data()->timestamp() << std::endl;
      }
    }
  });

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // caller
  quill::detail::set_cpu_affinity(0);
  static constexpr StaticLogRecordInfo log_line_info{QUILL_STRINGIFY(__LINE__), __FILE__, __FUNCTION__,
                                                     "Logging str: {}, int: {}, double: {}",
                                                     quill::LogLevel::Debug};

#ifdef NO_PERF
  std::vector<std::chrono::nanoseconds> results;
#endif

  for (size_t j = 0; j <= 1024; ++j)
  {
#ifdef NO_PERF
    auto const start = std::chrono::steady_clock::now();
#endif

    push_record(queue, &log_line_info, uint64_t{13}, uint64_t{13}, uint64_t{13}, uint64_t{13});

#ifdef NO_PERF
    auto const end = std::chrono::steady_clock::now();
    results.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start));
#endif
  }

#ifdef NO_PERF
  for (size_t i = 0; i < results.size(); ++i)
  {
    // std::cout << i << " : " << results[i].count() << std::endl;
  }
#endif

  backend_running.store(false);
  backend.join();
  return 0;
}