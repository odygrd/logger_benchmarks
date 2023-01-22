# include spdlog
include_directories(${CMAKE_SOURCE_DIR}/third_party)

# quill
set(TARGET_NAME benchmark_quill_dual_mode_bounded_call_site_latency_int_int_double)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_quill_dual_mode_bounded_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_INT_INT_DOUBLE)
target_link_libraries(${TARGET_NAME} PRIVATE quill_dual_bounded)

set(TARGET_NAME benchmark_quill_dual_mode_bounded_call_site_latency_int_int_largestr)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_quill_dual_mode_bounded_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_INT_INT_LARGESTR)
target_link_libraries(${TARGET_NAME} PRIVATE quill_dual_bounded)

set(TARGET_NAME benchmark_quill_dual_mode_unbounded_call_site_latency_int_int_double)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_quill_dual_mode_unbounded_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_INT_INT_DOUBLE)
target_link_libraries(${TARGET_NAME} PRIVATE quill_dual_unbounded)

set(TARGET_NAME benchmark_quill_dual_mode_unbounded_call_site_latency_int_int_largestr)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_quill_dual_mode_unbounded_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_INT_INT_LARGESTR)
target_link_libraries(${TARGET_NAME} PRIVATE quill_dual_unbounded)

set(TARGET_NAME benchmark_quill_no_dual_mode_unbounded_call_site_latency_int_int_double)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_quill_no_dual_mode_unbounded_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_INT_INT_DOUBLE)
target_link_libraries(${TARGET_NAME} PRIVATE quill_dual_disabled)

set(TARGET_NAME benchmark_quill_no_dual_mode_unbounded_call_site_latency_int_int_largestr)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_quill_no_dual_mode_unbounded_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_INT_INT_LARGESTR)
target_link_libraries(${TARGET_NAME} PRIVATE quill_dual_disabled)

set(TARGET_NAME benchmark_quill_v2_unbounded_call_site_latency_int_int_double)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_quill_v2_unbounded_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_INT_INT_DOUBLE)
target_link_libraries(${TARGET_NAME} PRIVATE quill_v2)

set(TARGET_NAME benchmark_quill_v2_unbounded_call_site_latency_int_int_largestr)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_quill_v2_unbounded_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_INT_INT_LARGESTR)
target_link_libraries(${TARGET_NAME} PRIVATE quill_v2)

# quill v27
set(TARGET_NAME benchmark_quill_v27_unbounded_call_site_latency_int_int_double)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_quill_v27_unbounded_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_INT_INT_DOUBLE)
target_link_libraries(${TARGET_NAME} PRIVATE quillv27)

set(TARGET_NAME benchmark_quill_v27_unbounded_call_site_latency_int_int_largestr)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_quill_v27_unbounded_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_INT_INT_LARGESTR)
target_link_libraries(${TARGET_NAME} PRIVATE quillv27)

# quill v27x86
set(TARGET_NAME benchmark_quill_v27x86_unbounded_call_site_latency_int_int_double)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_quill_v27x86_unbounded_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_INT_INT_DOUBLE)
target_link_libraries(${TARGET_NAME} PRIVATE quillv27x86)

set(TARGET_NAME benchmark_quill_v27x86_unbounded_call_site_latency_int_int_largestr)
add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE benchmark_quill_v27x86_unbounded_call_site_latency.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_INT_INT_LARGESTR)
target_link_libraries(${TARGET_NAME} PRIVATE quillv27x86)

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
