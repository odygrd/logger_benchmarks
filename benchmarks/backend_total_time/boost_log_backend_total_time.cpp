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

/**
 * The backend worker just spins, so we just measure the total time elapsed for total_iterations
 */
int main()
{
  std::filesystem::path log_file{"benchmark_boost_log_backend_total_time.log"};
  std::remove(log_file.string().c_str());

  // Set main thread affinity
  set_thread_affinity(1);

  // Setup async sink
  boost::shared_ptr<sinks::text_ostream_backend> backend = boost::make_shared<sinks::text_ostream_backend>();

  backend->add_stream(boost::make_shared<std::ofstream>(log_file));
  backend->auto_flush(false);

  // The sink's feeding thread is started here and pinned to CPU 5. keywords::start_thread only takes
  // a bool, so the sink must not start its own thread.
  boost::shared_ptr<async_sink_t> sink = boost::make_shared<async_sink_t>(backend, keywords::start_thread = false);
  std::thread feeding_thread(
    [sink]()
    {
      set_thread_affinity(5);
      sink->run();
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

  // Shared by every logging thread, so the thread-safe logger
  src::severity_logger_mt<logging::trivial::severity_level> logger;

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  BOOST_LOG_SEV(logger, logging::trivial::info) << "Warm up";
  sink->flush();

  // 256 distinct statements in a fixed pseudo-random order, see many_statements.h
  #define BENCH_LOG_STREAM(stream) BOOST_LOG_SEV(logger, logging::trivial::info) stream
  auto log_statement = [&](size_t iteration)
  {
  #include "many_statements/stream.inc"
  };

  // Each producer logs a warm-up line first, so its per-thread state is created before timing
  auto on_producer_start = [&](size_t) { BOOST_LOG_SEV(logger, logging::trivial::info) << "Warm up"; };

  ProducerTimes const times = run_producers(on_producer_start, log_statement);
  auto const start_time = times.start;

  BOOST_LOG_SEV(logger, logging::trivial::error) << "End";

  sink->flush();

  auto const end_time = std::chrono::steady_clock::now();
  print_throughput(times, end_time, static_cast<double>(std::filesystem::file_size(log_file)) / (1024 * 1024));

  // Cleanup
  logging::core::get()->remove_sink(sink);
  sink->stop();
  feeding_thread.join();
}
