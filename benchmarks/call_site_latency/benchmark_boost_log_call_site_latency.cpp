#include "call_site_latency_bench.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/log/attributes/clock.hpp>
#include <boost/log/attributes/counter.hpp>
#include <boost/log/attributes/current_thread_id.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/async_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <fstream>
#include <sched.h>
#include <string>
#include <system_error>
#include <vector>

namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;
namespace src = boost::log::sources;
namespace keywords = boost::log::keywords;
namespace attrs = boost::log::attributes;

// Type definitions for async sink
typedef sinks::asynchronous_sink<sinks::text_ostream_backend> async_sink_t;

void boost_log_benchmark(std::vector<int32_t> thread_count_array, size_t num_iterations_per_thread)
{
  std::remove("benchmark_boost_log_call_site_latency.log");

  // Setup async sink
  boost::shared_ptr<sinks::text_ostream_backend> backend = boost::make_shared<sinks::text_ostream_backend>();

  backend->add_stream(
    boost::make_shared<std::ofstream>("benchmark_boost_log_call_site_latency.log"));
  backend->auto_flush(false);

  // Create async frontend with thread initialization to pin backend thread to CPU 5
  boost::shared_ptr<async_sink_t> sink = boost::make_shared<async_sink_t>(
    backend,
    keywords::start_thread =
      [](const std::function<void()>& thread_func)
    {
      std::thread t(
        [thread_func]()
        {
          // Pin backend thread to CPU 5
          set_thread_affinity(5);

          thread_func();
        });
      t.detach();
    });

  // Set formatter - using optimized format with timestamp
  sink->set_formatter(expr::stream
                      << expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%H:%M:%S.%f")
                      << " [" << expr::attr<attrs::current_thread_id::value_type>("ThreadID") << "] "
                      << expr::attr<unsigned int>("LineID") << " " << logging::trivial::severity
                      << " "
                      // Optional hard-coded logger name
                      << "root " << expr::smessage);

  // Add the sink to the core
  logging::core::get()->add_sink(sink);

  // Add necessary attributes - LineID, ThreadID, and TimeStamp
  logging::core::get()->add_global_attribute("LineID", attrs::counter<unsigned int>(1));
  logging::core::get()->add_global_attribute("ThreadID", attrs::current_thread_id());
  logging::core::get()->add_global_attribute("TimeStamp", attrs::local_clock());

  // Create a persistent severity logger to avoid recreation on each log call
  src::severity_logger<logging::trivial::severity_level> logger;

  // Give the backend thread time to start
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  BOOST_LOG_SEV(logger, logging::trivial::info) << "Warm up";
  sink->flush();

  // Define logging lambda based on benchmark type
#ifdef BENCH_INT_INT_DOUBLE
  char const* benchmark_type = "[benchmark_type: int_int_double]";

  auto log_func = [&logger](uint64_t i, uint64_t j, double d)
  {
    BOOST_LOG_SEV(logger, logging::trivial::info)
      << "Logging int: " << i << ", int: " << j << ", double: " << d;
  };
#elif defined(BENCH_INT_INT_LARGESTR)
  char const* benchmark_type = "[benchmark_type: int_int_largestr]";

  auto log_func = [&logger](uint64_t i, uint64_t j, std::string const& s)
  {
    BOOST_LOG_SEV(logger, logging::trivial::info)
      << "Logging int: " << i << ", int: " << j << ", string: " << s;
  };
#elif defined(BENCH_VECTOR_LARGESTR)
  char const* benchmark_type = "[benchmark_type: vector_largestr]";

  auto log_func = [&logger](uint64_t i, uint64_t j, std::vector<std::string> const& v)
  {
    BOOST_LOG_SEV(logger, logging::trivial::info) << "Logging int: " << i << ", int: " << j << ", vector: [";
    for (size_t idx = 0; idx < v.size(); ++idx)
    {
      if (idx > 0)
        BOOST_LOG_SEV(logger, logging::trivial::info) << ", ";
      BOOST_LOG_SEV(logger, logging::trivial::info) << v[idx];
    }
    BOOST_LOG_SEV(logger, logging::trivial::info) << "]";
  };
#else
  static_assert(false,
                "define BENCH_INT_INT_DOUBLE or BENCH_INT_INT_LARGESTR or BENCH_VECTOR_LARGESTR");
#endif

  auto on_start = [&logger, &sink]()
  {
    BOOST_LOG_SEV(logger, logging::trivial::info) << "Warm up";
    sink->flush();
  };

  auto on_exit = [&sink]()
  {
    sink->flush();
    // Give time for async processing to complete
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  };

  // Run the benchmark for n threads
  std::string benchmark_name = "Logger: Boost.Log - Benchmark: Caller Thread Latency, Async";
  benchmark_name += " - ";
  benchmark_name += benchmark_type;

  for (auto thread_count : thread_count_array)
  {
    run_benchmark(benchmark_name.c_str(), thread_count, num_iterations_per_thread, on_start, log_func, on_exit);
  }

  // Stop logging and cleanup
  logging::core::get()->remove_sink(sink);
  sink->stop();
  sink->flush();
}

/***/
int main(int argc, char* argv[]) { boost_log_benchmark(THREAD_LIST_COUNT, ITERATIONS); }
