# Every throughput benchmark is built twice: BENCHMARK_<library>_backend_total_time with one
# producer thread, and BENCHMARK_<library>_backend_total_time_4_threads with four.
function(add_throughput_benchmark LIBRARY SOURCE)
  set(TARGET_NAME BENCHMARK_${LIBRARY}_backend_total_time)
  add_executable(${TARGET_NAME} ${SOURCE})
  target_link_libraries(${TARGET_NAME} ${ARGN})

  add_executable(${TARGET_NAME}_4_threads ${SOURCE})
  target_compile_definitions(${TARGET_NAME}_4_threads PRIVATE BENCH_THROUGHPUT_THREADS=4)
  target_link_libraries(${TARGET_NAME}_4_threads ${ARGN})
endfunction()

add_throughput_benchmark(quill quill_backend_total_time.cpp quill fmt::fmt)
add_throughput_benchmark(quill_with_functions quill_with_functions_backend_total_time.cpp quill fmt::fmt)
add_throughput_benchmark(fmtlog fmtlog_backend_total_time.cpp fmtlog fmt::fmt)
add_throughput_benchmark(spdlog spdlog_backend_total_time.cpp spdlog fmt::fmt Threads::Threads)

if(NOT WIN32)
    # XTR is Linux only
    add_throughput_benchmark(xtr xtr_backend_total_time.cpp xtr fmt::fmt Threads::Threads)
endif()

add_throughput_benchmark(ms_binlog ms_binlog_backend_total_time.cpp binlog fmt::fmt)

if(NOT WIN32)
    # Reckless does not compile with MSVC and the current Windows SDK
    add_throughput_benchmark(reckless reckless_backend_total_time.cpp reckless fmt::fmt)
endif()

if(NOT WIN32)
    # Iyengar NanoLog does not compile with MSVC
    add_throughput_benchmark(iyengar_nanolog iyengar_nanolog_backend_total_time.cpp IyengarNanoLog fmt::fmt)
endif()

if(NOT WIN32)
    # PlatformLab NanoLog is Linux only
    add_throughput_benchmark(nanolog nanolog_backend_total_time.cpp platformlab_nanolog fmt::fmt)
endif()

add_throughput_benchmark(bqlog bqlog_backend_total_time.cpp bqlog fmt::fmt)
add_throughput_benchmark(bqlog_binary bqlog_binary_backend_total_time.cpp bqlog fmt::fmt)
add_throughput_benchmark(boost_log boost_log_backend_total_time.cpp boost_log fmt::fmt)
