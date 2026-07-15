#!/usr/bin/env python3

from pathlib import Path
import subprocess


def repo_root() -> Path:
    return Path(__file__).resolve().parents[1]


CALL_SITE_DIR = repo_root() / "cmake-build-release" / "benchmarks" / "call_site_latency"
BACKEND_TOTAL_DIR = repo_root() / "cmake-build-release" / "benchmarks" / "backend_total_time"

CALL_SITE_BENCHMARKS = [
    "benchmark_quill_with_functions_unbounded_call_site_latency_int_int_double",
    "benchmark_quill_unbounded_call_site_latency_int_int_double",
    "benchmark_quill_bounded_call_site_latency_int_int_double",
    "benchmark_fmtlog_call_site_latency_int_int_double",
    "benchmark_xtr_call_site_latency_int_int_double",
    "benchmark_platformlab_call_site_latency_int_int_double",
    "benchmark_ms_binlog_call_site_latency_int_int_double",
    "benchmark_spdlog_call_site_latency_int_int_double",
    "benchmark_g3log_call_site_latency_int_int_double",
    "benchmark_iyengar_nanolog_call_site_latency_int_int_double",
    "benchmark_reckless_call_site_latency_int_int_double",
    "benchmark_boost_log_call_site_latency_int_int_double",
    "benchmark_bqlog_call_site_latency_int_int_double",
    "benchmark_quill_with_functions_unbounded_call_site_latency_int_int_largestr",
    "benchmark_quill_unbounded_call_site_latency_int_int_largestr",
    "benchmark_quill_bounded_call_site_latency_int_int_largestr",
    "benchmark_fmtlog_call_site_latency_int_int_largestr",
    "benchmark_xtr_call_site_latency_int_int_largestr",
    "benchmark_platformlab_call_site_latency_int_int_largestr",
    "benchmark_ms_binlog_call_site_latency_int_int_largestr",
    "benchmark_spdlog_call_site_latency_int_int_largestr",
    "benchmark_g3log_call_site_latency_int_int_largestr",
    "benchmark_iyengar_nanolog_call_site_latency_int_int_largestr",
    "benchmark_reckless_call_site_latency_int_int_largestr",
    "benchmark_boost_log_call_site_latency_int_int_largestr",
    "benchmark_bqlog_call_site_latency_int_int_largestr",
    "benchmark_quill_unbounded_call_site_latency_vector_largestr",
    "benchmark_quill_bounded_call_site_latency_vector_largestr",
    "benchmark_fmtlog_call_site_latency_int_vector_largestr",
    "benchmark_xtr_call_site_latency_int_vector_largestr",
    "benchmark_ms_binlog_call_site_latency_int_vector_largestr",
    "benchmark_spdlog_call_site_latency_int_vector_largestr",
    "benchmark_boost_log_call_site_latency_int_vector_largestr",
]

BACKEND_TOTAL_BENCHMARKS = [
    "BENCHMARK_ms_binlog_backend_total_time",
    "BENCHMARK_bqlog_binary_backend_total_time",
    "BENCHMARK_xtr_backend_total_time",
    "BENCHMARK_quill_backend_total_time",
    "BENCHMARK_spdlog_backend_total_time",
    "BENCHMARK_fmtlog_backend_total_time",
    "BENCHMARK_reckless_backend_total_time",
    "BENCHMARK_quill_with_functions_backend_total_time",
    "BENCHMARK_bqlog_backend_total_time",
    "BENCHMARK_boost_log_backend_total_time",
    "BENCHMARK_nanolog_backend_total_time",
    "BENCHMARK_iyengar_nanolog_backend_total_time",
]


def benchmark_paths() -> list[Path]:
    paths = [CALL_SITE_DIR / name for name in CALL_SITE_BENCHMARKS]
    paths.extend(BACKEND_TOTAL_DIR / name for name in BACKEND_TOTAL_BENCHMARKS)
    return paths


def benchmark_command(bench: Path) -> list[str]:
    # BqLog creates its asynchronous worker before main() changes the caller
    # thread's affinity. Start it on CPU 5 so the worker inherits the backend
    # CPU; the benchmark then moves its caller threads to CPUs 0-4.
    cpu_set = "5" if "bqlog" in bench.name.lower() else "1-5"
    return ["taskset", "-c", cpu_set, str(bench)]


def main() -> int:
    benchmarks = benchmark_paths()

    missing_benchmarks = [bench for bench in benchmarks if not bench.exists()]
    if missing_benchmarks:
        missing = "\n".join(str(bench) for bench in missing_benchmarks)
        raise SystemExit(f"Missing benchmark binaries:\n{missing}")

    for i, bench in enumerate(benchmarks):
        output_path = repo_root() / f"bench_results_{i}.txt"
        print(f"Writing to {output_path.name} for {bench}")

        with output_path.open("w", encoding="utf-8") as output:
            output.write(f"bench: {bench}\n")
            output.flush()
            print(f"Running {bench}")
            subprocess.run(benchmark_command(bench), cwd=repo_root(), stdout=output, check=True)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
