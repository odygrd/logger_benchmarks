#include "doctest/doctest.h"

#include "misc/TestUtilities.h"
#include "quill/Backend.h"
#include "quill/Frontend.h"
#include "quill/LogMacros.h"
#include "quill/core/Filesystem.h"
#include "quill/sinks/FileSink.h"

#include "quill/std/FilesystemPath.h"

#include <cstdio>
#include <string>
#include <string_view>
#include <vector>

using namespace quill;

/***/
TEST_CASE("wide_std_types_logging")
{
#if defined(_WIN32)
  static constexpr char const* filename = "wide_std_types_logging.log";
  static std::string const logger_name = "logger";

  // Start the logging backend thread
  Backend::start();

  // Set writing logging to a file
  auto file_sink = Frontend::create_or_get_sink<FileSink>(
    filename,
    []()
    {
      FileSinkConfig cfg;
      cfg.set_open_mode('w');
      return cfg;
    }(),
    FileEventNotifier{});

  Logger* logger = Frontend::create_or_get_logger(logger_name, std::move(file_sink));

  {
    fs::path wp{L"C:\\some\\path"};
    LOG_INFO(logger, "wp {}", wp);

    fs::path p{"C:\\another\\path"};
    LOG_INFO(logger, "p {}", p);

    LOG_INFO(logger, "wp {} p {}", wp, p);
  }

  logger->flush_log();
  Frontend::remove_logger(logger);

  // Wait until the backend thread stops for test stability
  Backend::stop();

  // Read file and check
  std::vector<std::string> const file_contents = quill::testing::file_contents(filename);

  REQUIRE(quill::testing::file_contains(
    file_contents, std::string{"LOG_INFO      " + logger_name + "       wp C:\\some\\path"}));

  REQUIRE(quill::testing::file_contains(
    file_contents, std::string{"LOG_INFO      " + logger_name + "       p C:\\another\\path"}));

  REQUIRE(quill::testing::file_contains(
    file_contents, std::string{"LOG_INFO      " + logger_name + "       wp C:\\some\\path p C:\\another\\path"}));

  testing::remove_file(filename);
#endif
}