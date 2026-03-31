include_guard(GLOBAL)

function(logger_benchmarks_add_boost_log)
  if(TARGET boost_log)
    return()
  endif()

  set(boost_dir "${CMAKE_SOURCE_DIR}/third_party/boost")

  if(NOT EXISTS "${boost_dir}/CMakeLists.txt")
    find_package(Python3 REQUIRED COMPONENTS Interpreter)
    message(STATUS "Bootstrapping Boost.Log into ${boost_dir}")
    execute_process(
      COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/tools/setup_boost_simple.py"
      WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
      COMMAND_ECHO STDOUT
      RESULT_VARIABLE boost_setup_result
    )
    if(NOT boost_setup_result EQUAL 0)
      message(FATAL_ERROR "Failed to bootstrap Boost.Log with tools/setup_boost_simple.py")
    endif()
  endif()

  add_subdirectory("${boost_dir}" "${CMAKE_BINARY_DIR}/vendor/boost")

  if(NOT TARGET boost_log)
    message(FATAL_ERROR "Boost.Log bootstrap completed, but the boost_log target was not created")
  endif()
endfunction()

function(logger_benchmarks_add_spdlog)
  if(TARGET spdlog)
    return()
  endif()

  add_library(spdlog INTERFACE)
  add_library(spdlog::spdlog ALIAS spdlog)
  target_include_directories(spdlog INTERFACE "${CMAKE_SOURCE_DIR}/third_party/spdlog/include")
endfunction()

function(logger_benchmarks_add_fmt)
  if(TARGET fmt::fmt)
    return()
  endif()

  set(FMT_DOC OFF CACHE BOOL "" FORCE)
  set(FMT_FUZZ OFF CACHE BOOL "" FORCE)
  set(FMT_INSTALL OFF CACHE BOOL "" FORCE)
  set(FMT_TEST OFF CACHE BOOL "" FORCE)

  add_subdirectory(
    "${CMAKE_SOURCE_DIR}/third_party/fmt"
    "${CMAKE_BINARY_DIR}/vendor/fmt"
  )
endfunction()

function(logger_benchmarks_add_iyengar_nanolog)
  if(TARGET IyengarNanoLog)
    return()
  endif()

  add_library(IyengarNanoLog STATIC
    "${CMAKE_SOURCE_DIR}/third_party/Iyengar_NanoLog/NanoLog.cpp"
  )
  target_compile_features(IyengarNanoLog PUBLIC cxx_std_11)
  target_include_directories(IyengarNanoLog
    PUBLIC "${CMAKE_SOURCE_DIR}/third_party"
    PRIVATE "${CMAKE_SOURCE_DIR}/third_party/Iyengar_NanoLog"
  )
  target_link_libraries(IyengarNanoLog PUBLIC Threads::Threads)
endfunction()

function(logger_benchmarks_add_platformlab_nanolog)
  if(TARGET platformlab_nanolog)
    return()
  endif()

  add_library(platformlab_nanolog STATIC
    "${CMAKE_SOURCE_DIR}/cmake/platformlab_nanolog_generated_code_stub.cpp"
    "${CMAKE_SOURCE_DIR}/third_party/platformlab_nanolog/runtime/Cycles.cc"
    "${CMAKE_SOURCE_DIR}/third_party/platformlab_nanolog/runtime/Log.cc"
    "${CMAKE_SOURCE_DIR}/third_party/platformlab_nanolog/runtime/NanoLog.cc"
    "${CMAKE_SOURCE_DIR}/third_party/platformlab_nanolog/runtime/RuntimeLogger.cc"
    "${CMAKE_SOURCE_DIR}/third_party/platformlab_nanolog/runtime/TimeTrace.cc"
    "${CMAKE_SOURCE_DIR}/third_party/platformlab_nanolog/runtime/Util.cc"
  )
  target_compile_features(platformlab_nanolog PUBLIC cxx_std_17)
  target_include_directories(platformlab_nanolog
    PUBLIC "${CMAKE_SOURCE_DIR}/third_party"
    PRIVATE "${CMAKE_SOURCE_DIR}/third_party/platformlab_nanolog/runtime"
  )
  target_link_libraries(platformlab_nanolog PUBLIC Threads::Threads)
  if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(platformlab_nanolog PUBLIC rt)
  endif()
endfunction()

function(logger_benchmarks_add_reckless)
  if(TARGET reckless)
    return()
  endif()

  set(reckless_sources
    "${CMAKE_SOURCE_DIR}/third_party/reckless/reckless/src/basic_log.cpp"
    "${CMAKE_SOURCE_DIR}/third_party/reckless/reckless/src/fd_writer.cpp"
    "${CMAKE_SOURCE_DIR}/third_party/reckless/reckless/src/file_writer.cpp"
    "${CMAKE_SOURCE_DIR}/third_party/reckless/reckless/src/lockless_cv.cpp"
    "${CMAKE_SOURCE_DIR}/third_party/reckless/reckless/src/mpsc_ring_buffer.cpp"
    "${CMAKE_SOURCE_DIR}/third_party/reckless/reckless/src/ntoa.cpp"
    "${CMAKE_SOURCE_DIR}/third_party/reckless/reckless/src/output_buffer.cpp"
    "${CMAKE_SOURCE_DIR}/third_party/reckless/reckless/src/platform.cpp"
    "${CMAKE_SOURCE_DIR}/third_party/reckless/reckless/src/policy_log.cpp"
    "${CMAKE_SOURCE_DIR}/third_party/reckless/reckless/src/template_formatter.cpp"
    "${CMAKE_SOURCE_DIR}/third_party/reckless/reckless/src/trace_log.cpp"
    "${CMAKE_SOURCE_DIR}/third_party/reckless/reckless/src/writer.cpp"
  )

  if(WIN32)
    list(APPEND reckless_sources
      "${CMAKE_SOURCE_DIR}/third_party/reckless/reckless/src/crash_handler_win32.cpp"
      "${CMAKE_SOURCE_DIR}/third_party/reckless/reckless/src/spsc_event_win32.cpp"
    )
  else()
    list(APPEND reckless_sources
      "${CMAKE_SOURCE_DIR}/third_party/reckless/reckless/src/crash_handler_unix.cpp"
    )
  endif()

  add_library(reckless STATIC ${reckless_sources})
  target_compile_features(reckless PUBLIC cxx_std_11)
  target_include_directories(reckless
    PUBLIC "${CMAKE_SOURCE_DIR}/third_party/reckless/reckless/include"
  )
  target_link_libraries(reckless PUBLIC Threads::Threads)
endfunction()

function(logger_benchmarks_add_g3log)
  if(TARGET g3log)
    return()
  endif()

  set(ADD_FATAL_EXAMPLE OFF CACHE BOOL "" FORCE)
  set(ADD_G3LOG_BENCH_PERFORMANCE OFF CACHE BOOL "" FORCE)
  set(ADD_G3LOG_UNIT_TEST OFF CACHE BOOL "" FORCE)
  set(G3_SHARED_LIB OFF CACHE BOOL "" FORCE)
  set(G3_SHARED_RUNTIME OFF CACHE BOOL "" FORCE)

  add_subdirectory(
    "${CMAKE_SOURCE_DIR}/third_party/g3log"
    "${CMAKE_BINARY_DIR}/vendor/g3log"
  )
endfunction()

function(logger_benchmarks_add_binlog)
  if(TARGET binlog)
    return()
  endif()

  unset(Boost_ROOT CACHE)
  unset(Boost_ROOT)

  if(EXISTS "${CMAKE_SOURCE_DIR}/third_party/boost/include/boost/version.hpp")
    set(BOOST_ROOT "${CMAKE_SOURCE_DIR}/third_party/boost" CACHE PATH "" FORCE)
    set(BOOST_INCLUDEDIR "${CMAKE_SOURCE_DIR}/third_party/boost/include" CACHE PATH "" FORCE)
    set(BOOST_LIBRARYDIR "${CMAKE_SOURCE_DIR}/third_party/boost/lib" CACHE PATH "" FORCE)
    set(Boost_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/third_party/boost/include" CACHE PATH "" FORCE)
  endif()

  set(BINLOG_BUILD_BREAD OFF CACHE BOOL "" FORCE)
  set(BINLOG_BUILD_BRECOVERY OFF CACHE BOOL "" FORCE)
  set(BINLOG_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  set(BINLOG_BUILD_INTEGRATION_TESTS OFF CACHE BOOL "" FORCE)
  set(BINLOG_BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
  set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

  add_subdirectory(
    "${CMAKE_SOURCE_DIR}/third_party/binlog"
    "${CMAKE_BINARY_DIR}/vendor/binlog"
  )
endfunction()

function(logger_benchmarks_add_fmtlog)
  if(TARGET fmtlog)
    return()
  endif()

  add_library(fmtlog STATIC
    "${CMAKE_SOURCE_DIR}/third_party/fmtlog/fmtlog.cc"
  )
  target_compile_features(fmtlog PUBLIC cxx_std_17)
  target_include_directories(fmtlog
    PUBLIC "${CMAKE_SOURCE_DIR}/third_party"
    PRIVATE "${CMAKE_SOURCE_DIR}/third_party/fmtlog"
  )
  target_link_libraries(fmtlog PUBLIC fmt::fmt Threads::Threads)
endfunction()

function(logger_benchmarks_add_xtr)
  if(TARGET xtr)
    return()
  endif()

  add_library(xtr STATIC
    "${CMAKE_SOURCE_DIR}/third_party/xtr/src/buffer.cpp"
    "${CMAKE_SOURCE_DIR}/third_party/xtr/src/command_dispatcher.cpp"
    "${CMAKE_SOURCE_DIR}/third_party/xtr/src/command_path.cpp"
    "${CMAKE_SOURCE_DIR}/third_party/xtr/src/consumer.cpp"
    "${CMAKE_SOURCE_DIR}/third_party/xtr/src/fd_storage.cpp"
    "${CMAKE_SOURCE_DIR}/third_party/xtr/src/fd_storage_base.cpp"
    "${CMAKE_SOURCE_DIR}/third_party/xtr/src/file_descriptor.cpp"
    "${CMAKE_SOURCE_DIR}/third_party/xtr/src/io_uring_fd_storage.cpp"
    "${CMAKE_SOURCE_DIR}/third_party/xtr/src/logger.cpp"
    "${CMAKE_SOURCE_DIR}/third_party/xtr/src/log_level.cpp"
    "${CMAKE_SOURCE_DIR}/third_party/xtr/src/matcher.cpp"
    "${CMAKE_SOURCE_DIR}/third_party/xtr/src/memory_mapping.cpp"
    "${CMAKE_SOURCE_DIR}/third_party/xtr/src/mirrored_memory_mapping.cpp"
    "${CMAKE_SOURCE_DIR}/third_party/xtr/src/pagesize.cpp"
    "${CMAKE_SOURCE_DIR}/third_party/xtr/src/posix_fd_storage.cpp"
    "${CMAKE_SOURCE_DIR}/third_party/xtr/src/regex_matcher.cpp"
    "${CMAKE_SOURCE_DIR}/third_party/xtr/src/sink.cpp"
    "${CMAKE_SOURCE_DIR}/third_party/xtr/src/throw.cpp"
    "${CMAKE_SOURCE_DIR}/third_party/xtr/src/tsc.cpp"
    "${CMAKE_SOURCE_DIR}/third_party/xtr/src/wildcard_matcher.cpp"
  )
  target_compile_features(xtr PUBLIC cxx_std_20)
  target_compile_definitions(xtr PUBLIC XTR_USE_IO_URING=0)
  target_compile_definitions(xtr PRIVATE XTR_FUNC=)
  target_include_directories(xtr
    PUBLIC "${CMAKE_SOURCE_DIR}/third_party/xtr/include"
  )
  target_link_libraries(xtr PUBLIC fmt::fmt Threads::Threads)
endfunction()

function(logger_benchmarks_add_bqlog)
  if(TARGET bqlog)
    return()
  endif()

  set(TARGET_PLATFORM "linux" CACHE STRING "" FORCE)
  set(BUILD_LIB_TYPE "static_lib" CACHE STRING "" FORCE)
  set(JAVA_SUPPORT OFF CACHE BOOL "" FORCE)
  set(NODE_API_SUPPORT OFF CACHE BOOL "" FORCE)

  add_subdirectory(
    "${CMAKE_SOURCE_DIR}/third_party/bqlog/src"
    "${CMAKE_BINARY_DIR}/vendor/bqlog"
  )

  if(TARGET BqLog)
    target_include_directories(BqLog
      PUBLIC "${CMAKE_SOURCE_DIR}/third_party/bqlog/include"
    )
  endif()

  if(TARGET BqLog AND NOT TARGET bqlog)
    add_library(bqlog ALIAS BqLog)
  endif()
endfunction()
