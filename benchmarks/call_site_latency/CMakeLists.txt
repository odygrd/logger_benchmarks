# include spdlog
include_directories(${CMAKE_SOURCE_DIR}/third_party)

# ------- #
set(TARGET_NAME benchmark_quill_with_functions_unbounded_call_site_latency_int_int_double)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_quill_with_functions_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_INT_INT_DOUBLE)
target_link_libraries(${TARGET_NAME} PRIVATE quill)

set(TARGET_NAME benchmark_quill_with_functions_unbounded_call_site_latency_int_int_largestr)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_quill_with_functions_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_INT_INT_LARGESTR)
target_link_libraries(${TARGET_NAME} PRIVATE quill)

# ------- #
set(TARGET_NAME benchmark_quill_unbounded_call_site_latency_int_int_double)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_quill_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_INT_INT_DOUBLE)
target_link_libraries(${TARGET_NAME} PRIVATE quill)

set(TARGET_NAME benchmark_quill_unbounded_call_site_latency_int_int_largestr)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_quill_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_INT_INT_LARGESTR)
target_link_libraries(${TARGET_NAME} PRIVATE quill)

set(TARGET_NAME benchmark_quill_unbounded_call_site_latency_vector_largestr)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_quill_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_VECTOR_LARGESTR)
target_link_libraries(${TARGET_NAME} PRIVATE quill)

# ------- #
set(TARGET_NAME benchmark_quill_bounded_call_site_latency_int_int_double)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_quill_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_INT_INT_DOUBLE QUILL_USE_BOUNDED_DROPPING_QUEUE)
target_link_libraries(${TARGET_NAME} PRIVATE quill)

set(TARGET_NAME benchmark_quill_bounded_call_site_latency_int_int_largestr)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_quill_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_INT_INT_LARGESTR QUILL_USE_BOUNDED_DROPPING_QUEUE)
target_link_libraries(${TARGET_NAME} PRIVATE quill)

set(TARGET_NAME benchmark_quill_bounded_call_site_latency_vector_largestr)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_quill_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_VECTOR_LARGESTR QUILL_USE_BOUNDED_DROPPING_QUEUE)
target_link_libraries(${TARGET_NAME} PRIVATE quill)

# ------- #
set(TARGET_NAME benchmark_quill_bounded_call_site_latency_int_int_double_huge_pages)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_quill_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_INT_INT_DOUBLE QUILL_USE_BOUNDED_DROPPING_QUEUE QUILL_USE_HUGE_PAGES)
target_link_libraries(${TARGET_NAME} PRIVATE quill)

set(TARGET_NAME benchmark_quill_bounded_call_site_latency_int_int_largestr_huge_pages)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_quill_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_INT_INT_LARGESTR QUILL_USE_BOUNDED_DROPPING_QUEUE QUILL_USE_HUGE_PAGES)
target_link_libraries(${TARGET_NAME} PRIVATE quill)

set(TARGET_NAME benchmark_quill_bounded_call_site_latency_vector_largestr_huge_pages)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_quill_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_VECTOR_LARGESTR QUILL_USE_BOUNDED_DROPPING_QUEUE QUILL_USE_HUGE_PAGES)
target_link_libraries(${TARGET_NAME} PRIVATE quill)

# ------- #
# platform lab
set(TARGET_NAME benchmark_platformlab_call_site_latency_int_int_double)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_platformlab_nanolog_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_INT_INT_DOUBLE)
target_link_libraries(${TARGET_NAME} PRIVATE platformlab_nanolog rt Threads::Threads)

set(TARGET_NAME benchmark_platformlab_call_site_latency_int_int_largestr)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_platformlab_nanolog_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_INT_INT_LARGESTR)
target_link_libraries(${TARGET_NAME} PRIVATE platformlab_nanolog rt Threads::Threads)

# Iyengar NanoLog
set(TARGET_NAME benchmark_iyengar_nanolog_call_site_latency_int_int_double)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_iyengar_nanolog_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_INT_INT_DOUBLE)
target_link_libraries(${TARGET_NAME} PRIVATE IyengarNanoLog Threads::Threads)

set(TARGET_NAME benchmark_iyengar_nanolog_call_site_latency_int_int_largestr)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_iyengar_nanolog_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_INT_INT_LARGESTR)
target_link_libraries(${TARGET_NAME} PRIVATE IyengarNanoLog Threads::Threads)

# Reckless
set(TARGET_NAME benchmark_reckless_call_site_latency_int_int_double)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_reckless_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_INT_INT_DOUBLE)
target_link_libraries(${TARGET_NAME} PRIVATE reckless Threads::Threads)

set(TARGET_NAME benchmark_reckless_call_site_latency_int_int_largestr)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_reckless_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_INT_INT_LARGESTR)
target_link_libraries(${TARGET_NAME} PRIVATE reckless Threads::Threads)

# spdlog
set(TARGET_NAME benchmark_spdlog_call_site_latency_int_int_double)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_spdlog_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_INT_INT_DOUBLE)
target_link_libraries(${TARGET_NAME} PRIVATE Threads::Threads)

set(TARGET_NAME benchmark_spdlog_call_site_latency_int_int_largestr)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_spdlog_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_INT_INT_LARGESTR)
target_link_libraries(${TARGET_NAME} PRIVATE Threads::Threads)

set(TARGET_NAME benchmark_spdlog_call_site_latency_int_vector_largestr)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_spdlog_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_VECTOR_LARGESTR)
target_link_libraries(${TARGET_NAME} PRIVATE Threads::Threads)

# g3log
set(TARGET_NAME benchmark_g3log_call_site_latency_int_int_double)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_g3log_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_INT_INT_DOUBLE)
target_link_libraries(${TARGET_NAME} PRIVATE g3log)

set(TARGET_NAME benchmark_g3log_call_site_latency_int_int_largestr)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_g3log_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_INT_INT_LARGESTR)
target_link_libraries(${TARGET_NAME} PRIVATE g3log)

# ms binlog
set(TARGET_NAME benchmark_ms_binlog_call_site_latency_int_int_double)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_ms_binlog_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_INT_INT_DOUBLE)
target_link_libraries(${TARGET_NAME} PRIVATE binlog Threads::Threads)

set(TARGET_NAME benchmark_ms_binlog_call_site_latency_int_int_largestr)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_ms_binlog_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_INT_INT_LARGESTR)
target_link_libraries(${TARGET_NAME} PRIVATE binlog Threads::Threads)

set(TARGET_NAME benchmark_ms_binlog_call_site_latency_int_vector_largestr)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_ms_binlog_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_VECTOR_LARGESTR)
target_link_libraries(${TARGET_NAME} PRIVATE binlog Threads::Threads)

# fmtlog
set(TARGET_NAME benchmark_fmtlog_call_site_latency_int_int_double)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_fmtlog_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_INT_INT_DOUBLE)
target_link_libraries(${TARGET_NAME} PRIVATE fmtlog Threads::Threads)

set(TARGET_NAME benchmark_fmtlog_call_site_latency_int_int_largestr)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_fmtlog_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_INT_INT_LARGESTR)
target_link_libraries(${TARGET_NAME} PRIVATE fmtlog Threads::Threads)

set(TARGET_NAME benchmark_fmtlog_call_site_latency_int_vector_largestr)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_fmtlog_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_VECTOR_LARGESTR)
target_link_libraries(${TARGET_NAME} PRIVATE fmtlog Threads::Threads)

# xtr
set(TARGET_NAME benchmark_xtr_call_site_latency_int_int_double)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_xtr_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_INT_INT_DOUBLE)
target_link_libraries(${TARGET_NAME} PRIVATE xtr Threads::Threads)

set(TARGET_NAME benchmark_xtr_call_site_latency_int_int_largestr)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_xtr_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_INT_INT_LARGESTR)
target_link_libraries(${TARGET_NAME} PRIVATE xtr Threads::Threads)

set(TARGET_NAME benchmark_xtr_call_site_latency_int_vector_largestr)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_xtr_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_VECTOR_LARGESTR)
target_link_libraries(${TARGET_NAME} PRIVATE xtr Threads::Threads)

# bqlog
set(TARGET_NAME benchmark_bqlog_call_site_latency_int_int_double)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_bqlog_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_INT_INT_DOUBLE)
target_link_libraries(${TARGET_NAME} PRIVATE bqlog Threads::Threads)

set(TARGET_NAME benchmark_bqlog_call_site_latency_int_int_largestr)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_bqlog_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_INT_INT_LARGESTR)
target_link_libraries(${TARGET_NAME} PRIVATE bqlog Threads::Threads)

# Boost.Log (conditional - only if boost_log target exists)
if (TARGET boost_log)
    set(TARGET_NAME benchmark_boost_log_call_site_latency_int_int_double)
    add_executable(${TARGET_NAME})
    target_sources(${TARGET_NAME} PRIVATE benchmark_boost_log_call_site_latency.cpp)
    target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_INT_INT_DOUBLE)
    target_link_libraries(${TARGET_NAME} PRIVATE boost_log Threads::Threads)

    set(TARGET_NAME benchmark_boost_log_call_site_latency_int_int_largestr)
    add_executable(${TARGET_NAME})
    target_sources(${TARGET_NAME} PRIVATE benchmark_boost_log_call_site_latency.cpp)
    target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_INT_INT_LARGESTR)
    target_link_libraries(${TARGET_NAME} PRIVATE boost_log Threads::Threads)

    set(TARGET_NAME benchmark_boost_log_call_site_latency_int_vector_largestr)
    add_executable(${TARGET_NAME})
    target_sources(${TARGET_NAME} PRIVATE benchmark_boost_log_call_site_latency.cpp)
    target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_VECTOR_LARGESTR)
    target_link_libraries(${TARGET_NAME} PRIVATE boost_log Threads::Threads)

    message(STATUS "Boost.Log benchmarks added to build")
else ()
    message(STATUS "Boost.Log benchmarks skipped - boost_log target not available")
endif ()