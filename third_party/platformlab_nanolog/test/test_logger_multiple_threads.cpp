#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <iterator>
#include <thread>
#include <vector>
#include "catch2/catch.hpp"
#include <boost/dll/runtime_symbol_info.hpp>

#include "nanolog/NanoLogCpp17.h"


using namespace NanoLog::LogLevels;

#define NUMBER_OF_RUNS 100
#define NUMBER_OF_THREADS 5

void loggingFunction()
{
  NanoLog::preallocate();
  for (unsigned i = 0; i < NUMBER_OF_RUNS; ++ i) {
    NANO_LOG(NOTICE, "Simple log message with 1 parameters %d", i);
  }
}

TEST_CASE("LOG_static_string_integer_mt_same_file")
{

    // get the path the test is running
    // it should be under build/tests
    std::string test_exec_path(boost::dll::program_location().remove_filename().string());

    std::string output_path = test_exec_path + "/test_log_output";
    std::string compressed_output_log_file = output_path + "/test_logger_multiple_threads_compressed";

    // create directory to store the output
    boost::filesystem::create_directory(output_path);

    if(boost::filesystem::exists(compressed_output_log_file))
    {
        boost::filesystem::remove(compressed_output_log_file);
    }

    // Do the logging
    NanoLog::setLogFile(compressed_output_log_file.c_str());

    std::vector<std::thread> vec_threads;

    for(unsigned i = 0; i < NUMBER_OF_THREADS; ++i)
    {
        vec_threads.emplace_back(std::thread(loggingFunction));
    }

    for(auto &t : vec_threads)
    {
        if(t.joinable())
        {
          t.join();
        }
    }

    NanoLog::sync();

    // Decompression
    std::string decompressed_log_file = compressed_output_log_file + "_decompressed";

    if(boost::filesystem::exists(output_path))
    {
        boost::filesystem::remove(decompressed_log_file);
    }

    std::string decompressor_binary_path = test_exec_path;

    {
        // find the path to the log_decompressor binary
        std::string s_to_erase{"/test"};
        std::string::size_type index = decompressor_binary_path.find(s_to_erase);
        decompressor_binary_path.erase(index, s_to_erase.length());
        decompressor_binary_path += "/bin/log_decompressor";
    }

    // decompressor command
    std::string decompressor_cmd = decompressor_binary_path;
    decompressor_cmd += " decompress ";
    decompressor_cmd += compressed_output_log_file;
    decompressor_cmd += ">";
    decompressor_cmd += decompressed_log_file;

    std::cout << "decompressor_cmd: " << decompressor_cmd << std::endl;

    system(decompressor_cmd.c_str());

    // Check decompressed log
    REQUIRE(boost::filesystem::exists(decompressed_log_file));

    std::ifstream out_file(decompressed_log_file.c_str());

    unsigned line_count = 0;
    for(std::string current_line; getline(out_file, current_line);)
    {
        ++line_count;
    }

    //3 lines is the summary that the decompressor spits out everytime
    REQUIRE((NUMBER_OF_RUNS * NUMBER_OF_THREADS + 3) == line_count);

    out_file.clear();
    out_file.seekg(0, out_file.beg);

    unsigned line_number = 0;
    std::string to_search = "Simple log message with 1 parameters ";
    for(std::string current_line; getline(out_file, current_line) && (line_number < NUMBER_OF_RUNS * NUMBER_OF_THREADS); ++line_number)
    {
        REQUIRE(current_line.find(to_search) != std::string::npos);
    }
}


