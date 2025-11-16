#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/async_frontend.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/async_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

#include "utils.h"

namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;
namespace src = boost::log::sources;
namespace keywords = boost::log::keywords;
namespace attrs = boost::log::attributes;

// Type definitions for async sink
typedef sinks::asynchronous_sink<sinks::text_ostream_backend> async_sink_t;

static constexpr size_t total_iterations = 4'000'000;

/**
 * The backend worker just spins, so we just measure the total time elapsed for total_iterations
 */
int main()
{
  std::filesystem::path log_file{"benchmark_boost_log_backend_total_time.log"};
  std::remove(log_file.c_str());

  // Set main thread affinity
  set_thread_affinity(1);

  // Setup async sink
  boost::shared_ptr<sinks::text_ostream_backend> backend = boost::make_shared<sinks::text_ostream_backend>();

  backend->add_stream(boost::make_shared<std::ofstream>(log_file));
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

  src::severity_logger<logging::trivial::severity_level> logger;

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  BOOST_LOG_SEV(logger, logging::trivial::info) << "Warm up";
  sink->flush();

  auto const start_time = std::chrono::steady_clock::now();

  for (size_t iteration = 0; iteration < total_iterations; ++iteration)
  {
    BOOST_LOG_SEV(logger, logging::trivial::info)
      << "Iteration: " << iteration << " int: " << (iteration * 2)
      << " double: " << (static_cast<double>(iteration) / 2);
  }

  BOOST_LOG_SEV(logger, logging::trivial::error) << "End";

  sink->flush();

  auto const end_time = std::chrono::steady_clock::now();
  auto const delta = end_time - start_time;

  auto delta_d = std::chrono::duration_cast<std::chrono::duration<double>>(delta).count();

  std::cout << "throughput is " << (total_iterations / delta_d / 1e6) << " million msgs/sec average, "
            << "total time elapsed: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(delta).count() << " ms, "
            << "log file size "
            << (static_cast<double>(std::filesystem::file_size(log_file)) / (1024 * 1024)) << " MB"
            << std::endl;

  // Cleanup
  logging::core::get()->remove_sink(sink);
}
