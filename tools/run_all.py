#!/usr/bin/env python3
"""
Runs every benchmark binary of a build and writes one bench_results_run<P>_<N>.txt per binary and
pass, for tools/generate_tables.py.

- Each pass runs the binaries in a random order, from a seed recorded in every results file, so
  no library always runs first or right after another library's large log file.
- Dirty pages are written back (os.sync) between binaries, so one benchmark's log file is not
  written to disk during the next benchmark.
- Every results file starts with the system configuration and ends with a delivery check: the
  lines of each text log the benchmark wrote, against the lines it should have written. The log
  files are deleted afterwards.
"""

from __future__ import annotations

import argparse
import os
import platform
import random
import re
import subprocess
import time
from pathlib import Path


def repo_root() -> Path:
    return Path(__file__).resolve().parents[1]


IS_WINDOWS = os.name == "nt"

# Libraries that are not built on Windows, see the Windows section in README.md
WINDOWS_EXCLUDED_LIBRARIES = ["xtr", "platformlab", "nanolog_backend", "iyengar", "reckless"]

# Files in the run directory that are never benchmark logs
NOT_LOGS = re.compile(r"^bench_results_.*\.txt$|^run-all.*\.log$|^build.*\.log$")


def find_benchmarks(build_dir: Path, scenarios: list[str] | None, include_huge_pages: bool) -> list[Path]:
    suffix = ".exe" if IS_WINDOWS else ""
    call_site = sorted((build_dir / "benchmarks" / "call_site_latency").glob(f"benchmark_*_call_site_latency_*{suffix}"))
    throughput = sorted((build_dir / "benchmarks" / "backend_total_time").glob(f"BENCHMARK_*_backend_total_time*{suffix}"))

    paths = [path for path in call_site + throughput if path.is_file() and os.access(path, os.X_OK)]

    if not include_huge_pages:
        paths = [path for path in paths if "huge_pages" not in path.name]

    if scenarios:
        def scenario_of(path: Path) -> str:
            if "backend_total_time" in path.name:
                return "throughput"
            return path.stem.rsplit("_", 1)[-1]

        paths = [path for path in paths if scenario_of(path) in scenarios]

    return paths


def benchmark_command(bench: Path) -> list[str]:
    if IS_WINDOWS:
        # There is no taskset on Windows. The benchmarks pin their caller and backend threads
        # in-process, mapping the benchmark CPUs to distinct physical cores (see bench_cpu() in
        # benchmarks/bench_platform.h)
        return [str(bench)]

    # BqLog creates its asynchronous worker before main() changes the caller
    # thread's affinity. Start it on CPU 5 so the worker inherits the backend
    # CPU; the benchmark then moves its caller threads to CPUs 0-4.
    cpu_set = "5" if "bqlog" in bench.name.lower() else "1-5"
    return ["taskset", "-c", cpu_set, str(bench)]


def read_text(path: str) -> str:
    try:
        return Path(path).read_text(encoding="utf-8").strip()
    except OSError:
        return "unavailable"


def system_fingerprint() -> str:
    lines = [f"platform: {platform.platform()}", f"python: {platform.python_version()}"]

    if IS_WINDOWS:
        lines.append(f"processor: {platform.processor()}")
        return "\n".join(lines)

    cpu_model = "unavailable"
    for line in read_text("/proc/cpuinfo").splitlines():
        if line.startswith("model name"):
            cpu_model = line.split(":", 1)[1].strip()
            break

    lines += [
        f"kernel: {platform.release()}",
        f"cpu: {cpu_model}",
        f"cmdline: {read_text('/proc/cmdline')}",
        f"governor cpu1: {read_text('/sys/devices/system/cpu/cpu1/cpufreq/scaling_governor')}",
        f"no_turbo: {read_text('/sys/devices/system/cpu/intel_pstate/no_turbo')}",
        f"transparent_hugepage: {read_text('/sys/kernel/mm/transparent_hugepage/enabled')}",
    ]

    idle_states = []
    for state in sorted(Path("/sys/devices/system/cpu/cpu1/cpuidle").glob("state*")):
        name = read_text(str(state / "name"))
        disabled = read_text(str(state / "disable"))
        idle_states.append(f"{name}={'off' if disabled == '1' else 'on'}")
    lines.append(f"cpuidle cpu1: {' '.join(idle_states) or 'unavailable'}")

    return "\n".join(lines)


def run_dir_files(run_dir: Path) -> dict[Path, float]:
    files = {}
    for path in run_dir.iterdir():
        if path.is_file() and not NOT_LOGS.match(path.name):
            files[path] = path.stat().st_mtime
    return files


def is_text_file(path: Path) -> bool:
    with path.open("rb") as file:
        head = file.read(4096)
    return b"\0" not in head


def count_lines(path: Path) -> int:
    count = 0
    with path.open("rb") as file:
        while chunk := file.read(1 << 20):
            count += chunk.count(b"\n")
    return count


def expected_lines(output: str) -> tuple[int, str]:
    """
    The lines a benchmark must have written: exact for throughput, a minimum for latency, which
    also writes a few warm-up lines
    """
    match = re.search(r"expected log lines: (\d+)", output)
    if match:
        return int(match.group(1)), "=="

    total = sum(int(value) for value in re.findall(r"Total messages (\d+)", output))
    return total, ">="


def delivery_check(output: str, written: list[Path]) -> list[str]:
    if "Scenario: disabled" in output:
        return ["delivery: not checked, disabled statements write nothing"]

    expected, relation = expected_lines(output)
    report = []

    for path in sorted(written):
        if not is_text_file(path):
            report.append(f"delivery: {path.name} binary, not checked")
            continue

        lines = count_lines(path)
        ok = lines == expected if relation == "==" else lines >= expected
        report.append(f"delivery: {path.name} lines {lines}, expected {relation} {expected}: {'OK' if ok else 'FAIL'}")

    if not written:
        report.append("delivery: no log file found")

    return report


def main() -> int:
    parser = argparse.ArgumentParser(description="Run every benchmark binary of a build.")
    parser.add_argument("--build-dir", type=Path, default=repo_root() / "cmake-build-release")
    parser.add_argument("--passes", type=int, default=5, help="number of passes over all binaries")
    parser.add_argument("--first-pass", type=int, default=1, help="number of the first pass, to resume a run")
    parser.add_argument("--seed", type=int, default=None, help="seed of the run order, random by default")
    parser.add_argument("--scenarios", nargs="*", default=None,
                        help="only these scenarios: burst single cold disabled defaults throughput")
    parser.add_argument("--include-huge-pages", action="store_true", help="also run the Quill huge pages variants")
    parser.add_argument("--keep-logs", action="store_true", help="keep the log files the benchmarks write")
    args = parser.parse_args()

    benchmarks = find_benchmarks(args.build_dir, args.scenarios, args.include_huge_pages)
    if IS_WINDOWS:
        benchmarks = [bench for bench in benchmarks
                      if not any(library in bench.name for library in WINDOWS_EXCLUDED_LIBRARIES)]
    if not benchmarks:
        raise SystemExit(f"No benchmark binaries found in {args.build_dir}")

    seed = args.seed if args.seed is not None else random.SystemRandom().randrange(1 << 32)
    print(f"Running {len(benchmarks)} benchmarks, {args.passes} passes, seed {seed}")

    # Run the benchmarks with high priority on Windows
    creation_flags = subprocess.HIGH_PRIORITY_CLASS if IS_WINDOWS else 0
    fingerprint = system_fingerprint()
    run_dir = repo_root()

    for pass_number in range(args.first_pass, args.first_pass + args.passes):
        order = list(benchmarks)
        random.Random(seed + pass_number).shuffle(order)

        for index, bench in enumerate(order):
            output_path = run_dir / f"bench_results_run{pass_number}_{index}.txt"
            print(f"[pass {pass_number} {index + 1}/{len(order)}] {bench.name}")

            if hasattr(os, "sync"):
                os.sync()
            before = run_dir_files(run_dir)
            start = time.time()

            result = subprocess.run(benchmark_command(bench), cwd=run_dir, capture_output=True, text=True,
                                    creationflags=creation_flags)

            after = run_dir_files(run_dir)
            written = [path for path, mtime in after.items() if path not in before or mtime > before[path]]
            report = delivery_check(result.stdout, written)

            with output_path.open("w", encoding="utf-8") as output:
                output.write(f"bench: {bench}\n")
                output.write(f"pass: {pass_number}\nseed: {seed}\nposition: {index}\n")
                output.write(f"{fingerprint}\n\n")
                output.write(result.stdout)
                if result.stderr:
                    output.write(f"\nstderr:\n{result.stderr}\n")
                output.write(f"\nexit code: {result.returncode}\nwall time: {time.time() - start:.1f} s\n")
                output.write("\n".join(report) + "\n")

            for line in report:
                if line.endswith("FAIL"):
                    print(f"  {line}")
            if result.returncode != 0:
                print(f"  exit code {result.returncode}")

            if not args.keep_logs:
                for path in written:
                    path.unlink(missing_ok=True)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
