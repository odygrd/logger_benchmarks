# Every call site latency benchmark is built once per payload and scenario, see
# call_site_latency_bench_common.h for the scenarios. Targets are named
# benchmark_<library>_call_site_latency_<payload>_<scenario>, and the disabled level
# benchmark benchmark_<library>_call_site_latency_disabled.

set(BENCH_ALL_PAYLOADS int_int_double int_int_largestr vector_largestr)
set(BENCH_ALL_SCENARIOS burst single cold)

# add_call_site_latency_benchmark(<library> SOURCE <file> LIBRARIES <libs...>
#                                 [PAYLOADS <payloads...>] [SCENARIOS <scenarios...>]
#                                 [DEFINITIONS <definitions...>] [DISABLED])
function(add_call_site_latency_benchmark LIBRARY)
  cmake_parse_arguments(ARG "DISABLED" "SOURCE" "LIBRARIES;PAYLOADS;SCENARIOS;DEFINITIONS" ${ARGN})

  if (NOT ARG_PAYLOADS)
    set(ARG_PAYLOADS ${BENCH_ALL_PAYLOADS})
  endif ()
  if (NOT ARG_SCENARIOS)
    set(ARG_SCENARIOS ${BENCH_ALL_SCENARIOS})
  endif ()

  foreach (PAYLOAD ${ARG_PAYLOADS})
    string(TOUPPER "BENCH_${PAYLOAD}" PAYLOAD_DEFINITION)
    foreach (SCENARIO ${ARG_SCENARIOS})
      set(TARGET_NAME benchmark_${LIBRARY}_call_site_latency_${PAYLOAD}_${SCENARIO})
      add_executable(${TARGET_NAME} ${ARG_SOURCE})
      target_compile_definitions(${TARGET_NAME} PRIVATE ${PAYLOAD_DEFINITION} ${ARG_DEFINITIONS})
      if (NOT SCENARIO STREQUAL "burst")
        string(TOUPPER "BENCH_SCENARIO_${SCENARIO}" SCENARIO_DEFINITION)
        target_compile_definitions(${TARGET_NAME} PRIVATE ${SCENARIO_DEFINITION})
      endif ()
      target_link_libraries(${TARGET_NAME} PRIVATE ${ARG_LIBRARIES})
    endforeach ()
  endforeach ()

  if (ARG_DISABLED)
    set(TARGET_NAME benchmark_${LIBRARY}_call_site_latency_disabled)
    add_executable(${TARGET_NAME} ${ARG_SOURCE})
    target_compile_definitions(${TARGET_NAME} PRIVATE BENCH_INT_INT_DOUBLE BENCH_SCENARIO_DISABLED ${ARG_DEFINITIONS})
    target_link_libraries(${TARGET_NAME} PRIVATE ${ARG_LIBRARIES})
  endif ()
endfunction()

# The measurement floor: the harness with a call that logs nothing
add_call_site_latency_benchmark(timer_only DISABLED
  SOURCE benchmark_timer_only_call_site_latency.cpp LIBRARIES Threads::Threads)

# Quill
add_call_site_latency_benchmark(quill_unbounded DISABLED
  SOURCE benchmark_quill_call_site_latency.cpp LIBRARIES quill)

add_call_site_latency_benchmark(quill_with_functions_unbounded DISABLED
  SOURCE benchmark_quill_with_functions_call_site_latency.cpp LIBRARIES quill)

add_call_site_latency_benchmark(quill_bounded
  SOURCE benchmark_quill_call_site_latency.cpp LIBRARIES quill
  DEFINITIONS QUILL_USE_BOUNDED_DROPPING_QUEUE)

add_call_site_latency_benchmark(quill_bounded_huge_pages
  SOURCE benchmark_quill_call_site_latency.cpp LIBRARIES quill
  SCENARIOS burst DEFINITIONS QUILL_USE_BOUNDED_DROPPING_QUEUE QUILL_USE_HUGE_PAGES)

# PlatformLab NanoLog is Linux only and has no std::vector support
if (NOT WIN32)
  add_call_site_latency_benchmark(platformlab DISABLED
    SOURCE benchmark_platformlab_nanolog_call_site_latency.cpp
    LIBRARIES platformlab_nanolog rt Threads::Threads PAYLOADS int_int_double int_int_largestr)
endif ()

# Iyengar NanoLog does not compile with MSVC and has no std::vector support
if (NOT WIN32)
  add_call_site_latency_benchmark(iyengar_nanolog DISABLED
    SOURCE benchmark_iyengar_nanolog_call_site_latency.cpp
    LIBRARIES IyengarNanoLog Threads::Threads PAYLOADS int_int_double int_int_largestr)
endif ()

# Reckless does not compile with MSVC and the current Windows SDK, has no std::vector support and
# no runtime log level
if (NOT WIN32)
  add_call_site_latency_benchmark(reckless
    SOURCE benchmark_reckless_call_site_latency.cpp
    LIBRARIES reckless Threads::Threads PAYLOADS int_int_double int_int_largestr)
endif ()

add_call_site_latency_benchmark(spdlog DISABLED
  SOURCE benchmark_spdlog_call_site_latency.cpp LIBRARIES spdlog Threads::Threads)

# g3log has no std::vector support, and filters levels at runtime only when built with
# G3_DYNAMIC_LOGGING
add_call_site_latency_benchmark(g3log
  SOURCE benchmark_g3log_call_site_latency.cpp LIBRARIES g3log PAYLOADS int_int_double int_int_largestr)

add_call_site_latency_benchmark(ms_binlog DISABLED
  SOURCE benchmark_ms_binlog_call_site_latency.cpp LIBRARIES binlog Threads::Threads)

add_call_site_latency_benchmark(fmtlog DISABLED
  SOURCE benchmark_fmtlog_call_site_latency.cpp LIBRARIES fmtlog Threads::Threads)

# XTR is Linux only
if (NOT WIN32)
  add_call_site_latency_benchmark(xtr DISABLED
    SOURCE benchmark_xtr_call_site_latency.cpp LIBRARIES xtr Threads::Threads)
endif ()

# BqLog has no std::vector support
add_call_site_latency_benchmark(bqlog DISABLED
  SOURCE benchmark_bqlog_call_site_latency.cpp LIBRARIES bqlog Threads::Threads
  PAYLOADS int_int_double int_int_largestr)

add_call_site_latency_benchmark(boost_log DISABLED
  SOURCE benchmark_boost_log_call_site_latency.cpp LIBRARIES boost_log Threads::Threads)
