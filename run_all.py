import subprocess

root_dir = "/home/odygrd/CLionProjects/logger_benchmarks/cmake-build-release/benchmarks/call_site_latency/"

benchmark_quill_unbounded_int_int_double = root_dir + "benchmark_quill_unbounded_call_site_latency_int_int_double"
benchmark_quill_x86_unbounded_int_int_double = root_dir + "benchmark_quill_x86_unbounded_call_site_latency_int_int_double"
benchmark_g3log_int_int_double = root_dir + "benchmark_g3log_call_site_latency_int_int_double"
benchmark_iyengar_nanolog_int_int_double = root_dir + "benchmark_iyengar_nanolog_call_site_latency_int_int_double"
benchmark_ms_binlog_int_int_double = root_dir + "benchmark_ms_binlog_call_site_latency_int_int_double"
benchmark_fmtlog_int_int_double = root_dir + "benchmark_fmtlog_call_site_latency_int_int_double"
benchmark_platformlab_int_int_double = root_dir + "benchmark_platformlab_call_site_latency_int_int_double"
benchmark_reckless_int_int_double = root_dir + "benchmark_reckless_call_site_latency_int_int_double"
benchmark_spdlog_int_int_double = root_dir + "benchmark_spdlog_call_site_latency_int_int_double"

benchmark_quill_unbounded_int_int_largestr = root_dir + "benchmark_quill_unbounded_call_site_latency_int_int_largestr"
benchmark_quill_x86_unbounded_int_int_largestr = root_dir + "benchmark_quill_x86_unbounded_call_site_latency_int_int_largestr"
benchmark_g3log_int_int_largestr = root_dir + "benchmark_g3log_call_site_latency_int_int_largestr"
benchmark_iyengar_nanolog_int_int_largestr = root_dir + "benchmark_iyengar_nanolog_call_site_latency_int_int_largestr"
benchmark_ms_binlog_int_int_largestr = root_dir + "benchmark_ms_binlog_call_site_latency_int_int_largestr"
benchmark_fmtlog_int_int_largestr = root_dir + "benchmark_fmtlog_call_site_latency_int_int_largestr"
benchmark_platformlab_int_int_largestr = root_dir + "benchmark_platformlab_call_site_latency_int_int_largestr"
benchmark_reckless_int_int_largestr = root_dir + "benchmark_reckless_call_site_latency_int_int_largestr"
benchmark_spdlog_int_int_largestr = root_dir + "benchmark_spdlog_call_site_latency_int_int_largestr"

benchmarks = [benchmark_quill_unbounded_int_int_double,
              benchmark_quill_x86_unbounded_int_int_double,
              benchmark_fmtlog_int_int_double,
              benchmark_platformlab_int_int_double,
              benchmark_ms_binlog_int_int_double,
              benchmark_spdlog_int_int_double,
              benchmark_g3log_int_int_double,
              benchmark_iyengar_nanolog_int_int_double,
              benchmark_reckless_int_int_double,
              benchmark_quill_unbounded_int_int_largestr,
              benchmark_quill_x86_unbounded_int_int_double,
              benchmark_fmtlog_int_int_largestr,
              benchmark_platformlab_int_int_largestr,
              benchmark_ms_binlog_int_int_largestr,
              benchmark_spdlog_int_int_largestr,
              benchmark_g3log_int_int_largestr,
              benchmark_iyengar_nanolog_int_int_largestr,
              benchmark_reckless_int_int_largestr]

i = 0
for bench in benchmarks:
    s = "bench_results_{}.txt".format(i)
    print("Writing to {} for {}".format(s, bench))
    output = open(s, "w")
    i = i + 1

    for x in range(4):
        print("Running {}".format(bench))
        subprocess.call(bench, stdout=output)
