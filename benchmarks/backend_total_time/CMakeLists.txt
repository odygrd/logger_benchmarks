# Every throughput benchmark is built four times: BENCHMARK_<library>_backend_total_time with one
# producer thread, BENCHMARK_<library>_backend_total_time_4_threads with four, and both again with a
# _many_statements suffix, logging 256 distinct statements instead of five (see many_statements.h).
function(add_throughput_benchmark LIBRARY SOURCE)
  foreach (WORKLOAD "" "_many_statements")
    set(TARGET_NAME BENCHMARK_${LIBRARY}_backend_total_time${WORKLOAD})

    add_executable(${TARGET_NAME} ${SOURCE})
    add_executable(${TARGET_NAME}_4_threads ${SOURCE})
    target_compile_definitions(${TARGET_NAME}_4_threads PRIVATE BENCH_THROUGHPUT_THREADS=4)

    foreach (TARGET ${TARGET_NAME} ${TARGET_NAME}_4_threads)
      if (WORKLOAD)
        target_compile_definitions(${TARGET} PRIVATE BENCH_MANY_STATEMENTS)
      endif ()
      target_link_libraries(${TARGET} ${ARGN})
    endforeach ()
  endforeach ()
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
