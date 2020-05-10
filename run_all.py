import subprocess

root_dir = ""

benchmark_quill = root_dir + "benchmark_quill_call_site_latency"
benchmark_g3log = root_dir + "benchmark_g3log_call_site_latency"
benchmark_iyengar_nanolog = root_dir + "benchmark_iyengar_nanolog_call_site_latency"
benchmark_ms_binlog = root_dir + "benchmark_ms_binlog_call_site_latency"
benchmark_platformlab = root_dir + "benchmark_platformlab_call_site_latency"
benchmark_reckless = root_dir + "benchmark_reckless_call_site_latency"
benchmark_spdlog = root_dir + "benchmark_spdlog_call_site_latency"

benchmarks = [benchmark_quill, benchmark_g3log, benchmark_iyengar_nanolog, benchmark_ms_binlog, benchmark_platformlab, benchmark_reckless, benchmark_spdlog]

i = 0
for bench in benchmarks:
  s = "bench_results_{}.txt".format(i)
  print("Writing to {} for {}".format(s, bench))
  output = open(s, "w")
  i = i + 1

  for x in range(4):
    print("Running {}".format(bench))
    subprocess.call(bench, stdout=output)
