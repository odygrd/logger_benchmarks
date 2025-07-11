import subprocess

root_dir = "/home/odygrd/git/logger_benchmarks/cmake-build-release/benchmarks/call_site_latency/"

benchmark_quill_unbounded_with_functions_call_site_latency_int_int_double = root_dir + "benchmark_quill_with_functions_unbounded_call_site_latency_int_int_double"
benchmark_quill_unbounded_call_site_latency_int_int_double = root_dir + "benchmark_quill_unbounded_call_site_latency_int_int_double"
benchmark_quill_bounded_call_site_latency_int_int_double = root_dir + "benchmark_quill_bounded_call_site_latency_int_int_double"
benchmark_g3log_int_int_double = root_dir + "benchmark_g3log_call_site_latency_int_int_double"
benchmark_iyengar_nanolog_int_int_double = root_dir + "benchmark_iyengar_nanolog_call_site_latency_int_int_double"
benchmark_ms_binlog_int_int_double = root_dir + "benchmark_ms_binlog_call_site_latency_int_int_double"
benchmark_fmtlog_int_int_double = root_dir + "benchmark_fmtlog_call_site_latency_int_int_double"
benchmark_platformlab_int_int_double = root_dir + "benchmark_platformlab_call_site_latency_int_int_double"
benchmark_reckless_int_int_double = root_dir + "benchmark_reckless_call_site_latency_int_int_double"
benchmark_spdlog_int_int_double = root_dir + "benchmark_spdlog_call_site_latency_int_int_double"
benchmark_xtr_int_int_double = root_dir + "benchmark_xtr_call_site_latency_int_int_double"

benchmark_quill_unbounded_with_functions_call_site_latency_int_int_largestr = root_dir + "benchmark_quill_with_functions_unbounded_call_site_latency_int_int_largestr"
benchmark_quill_unbounded_call_site_latency_int_int_largestr = root_dir + "benchmark_quill_unbounded_call_site_latency_int_int_largestr"
benchmark_quill_bounded_call_site_latency_int_int_largestr = root_dir + "benchmark_quill_bounded_call_site_latency_int_int_largestr"
benchmark_g3log_int_int_largestr = root_dir + "benchmark_g3log_call_site_latency_int_int_largestr"
benchmark_iyengar_nanolog_int_int_largestr = root_dir + "benchmark_iyengar_nanolog_call_site_latency_int_int_largestr"
benchmark_ms_binlog_int_int_largestr = root_dir + "benchmark_ms_binlog_call_site_latency_int_int_largestr"
benchmark_fmtlog_int_int_largestr = root_dir + "benchmark_fmtlog_call_site_latency_int_int_largestr"
benchmark_platformlab_int_int_largestr = root_dir + "benchmark_platformlab_call_site_latency_int_int_largestr"
benchmark_reckless_int_int_largestr = root_dir + "benchmark_reckless_call_site_latency_int_int_largestr"
benchmark_spdlog_int_int_largestr = root_dir + "benchmark_spdlog_call_site_latency_int_int_largestr"
benchmark_xtr_int_int_largestr = root_dir + "benchmark_xtr_call_site_latency_int_int_largestr"

benchmark_quill_unbounded_call_site_latency_vector_largestr = root_dir + "benchmark_quill_unbounded_call_site_latency_vector_largestr"
benchmark_quill_bounded_call_site_latency_vector_largestr = root_dir + "benchmark_quill_bounded_call_site_latency_vector_largestr"
benchmark_fmtlog_call_site_latency_int_vector_largestr = root_dir + "benchmark_fmtlog_call_site_latency_int_vector_largestr"
benchmark_ms_binlog_call_site_latency_int_vector_largestr = root_dir + "benchmark_ms_binlog_call_site_latency_int_vector_largestr"
benchmark_spdlog_call_site_latency_int_vector_largestr = root_dir + "benchmark_spdlog_call_site_latency_int_vector_largestr"
benchmark_xtr_call_site_latency_int_vector_largestr = root_dir + "benchmark_xtr_call_site_latency_int_vector_largestr"

benchmarks = [benchmark_quill_unbounded_with_functions_call_site_latency_int_int_double,
              benchmark_quill_unbounded_call_site_latency_int_int_double,
              benchmark_quill_bounded_call_site_latency_int_int_double,
              benchmark_fmtlog_int_int_double,
              benchmark_xtr_int_int_double,
              benchmark_platformlab_int_int_double,
              benchmark_ms_binlog_int_int_double,
              benchmark_spdlog_int_int_double,
              benchmark_g3log_int_int_double,
              benchmark_iyengar_nanolog_int_int_double,
              benchmark_reckless_int_int_double,
              benchmark_quill_unbounded_with_functions_call_site_latency_int_int_largestr,
              benchmark_quill_unbounded_call_site_latency_int_int_largestr,
              benchmark_quill_bounded_call_site_latency_int_int_largestr,
              benchmark_fmtlog_int_int_largestr,
              benchmark_xtr_int_int_largestr,
              benchmark_platformlab_int_int_largestr,
              benchmark_ms_binlog_int_int_largestr,
              benchmark_spdlog_int_int_largestr,
              benchmark_g3log_int_int_largestr,
              benchmark_iyengar_nanolog_int_int_largestr,
              benchmark_reckless_int_int_largestr,
              benchmark_quill_unbounded_call_site_latency_vector_largestr,
              benchmark_quill_bounded_call_site_latency_vector_largestr,
              benchmark_fmtlog_call_site_latency_int_vector_largestr,
              benchmark_xtr_call_site_latency_int_vector_largestr,
              benchmark_ms_binlog_call_site_latency_int_vector_largestr,
              benchmark_spdlog_call_site_latency_int_vector_largestr]

i = 0
for bench in benchmarks:
    s = "bench_results_{}.txt".format(i)
    print("Writing to {} for {}".format(s, bench))
    output = open(s, "w")
    output.write("bench: {}".format(bench))
    i = i + 1

    for x in range(1):
        print("Running {}".format(bench))
        subprocess.call(bench, stdout=output)
