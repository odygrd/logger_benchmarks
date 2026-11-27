#!/usr/bin/env python3

from __future__ import annotations

import argparse
import re
import statistics
from collections import defaultdict
from pathlib import Path

QUILL_MACRO_FREE_MODE = "Quill - Macro Free Mode"

LATENCY_LOGGER_MAP = {
    "Quill - Unbounded": {
        "name": "Quill Unbounded Queue",
        "url": "https://github.com/odygrd/quill",
    },
    "Quill - Bounded": {
        "name": "Quill Bounded Dropping Queue",
        "url": "https://github.com/odygrd/quill",
    },
    QUILL_MACRO_FREE_MODE: {
        "name": QUILL_MACRO_FREE_MODE,
        "url": "https://github.com/odygrd/quill",
    },
    "fmtlog": {
        "name": "fmtlog",
        "url": "https://github.com/MengRao/fmtlog",
    },
    "PlatformLab NanoLog": {
        "name": "PlatformLab NanoLog",
        "url": "https://github.com/PlatformLab/NanoLog",
    },
    "Ms binLog": {
        "name": "MS BinLog",
        "url": "https://github.com/Morgan-Stanley/binlog",
    },
    "XTR": {
        "name": "XTR",
        "url": "https://github.com/choll/xtr",
    },
    "Reckless": {
        "name": "Reckless",
        "url": "https://github.com/mattiasflodin/reckless",
    },
    "Bqlog": {
        "name": "BqLog",
        "url": "https://github.com/Tencent/BqLog",
    },
    "bqlog": {
        "name": "BqLog",
        "url": "https://github.com/Tencent/BqLog",
    },
    "Iyengar NanoLog": {
        "name": "Iyengar NanoLog",
        "url": "https://github.com/Iyengar111/NanoLog",
    },
    "Spdlog": {
        "name": "spdlog",
        "url": "https://github.com/gabime/spdlog",
    },
    "G3Log": {
        "name": "g3log",
        "url": "https://github.com/KjellKod/g3log",
    },
    "Boost.Log": {
        "name": "Boost.Log",
        "url": "https://www.boost.org",
    },
}

BACKEND_LOGGER_MAP = {
    "BENCHMARK_ms_binlog_backend_total_time": {
        "name": "MS BinLog (binary log)",
        "url": "https://github.com/Morgan-Stanley/binlog",
    },
    "BENCHMARK_bqlog_binary_backend_total_time": {
        "name": "BqLog (binary log)",
        "url": "https://github.com/Tencent/BqLog",
    },
    "BENCHMARK_xtr_backend_total_time": {
        "name": "XTR",
        "url": "https://github.com/choll/xtr",
    },
    "BENCHMARK_quill_backend_total_time": {
        "name": "Quill",
        "url": "https://github.com/odygrd/quill",
    },
    "BENCHMARK_spdlog_backend_total_time": {
        "name": "spdlog",
        "url": "https://github.com/gabime/spdlog",
    },
    "BENCHMARK_fmtlog_backend_total_time": {
        "name": "fmtlog",
        "url": "https://github.com/MengRao/fmtlog",
    },
    "BENCHMARK_reckless_backend_total_time": {
        "name": "Reckless",
        "url": "https://github.com/mattiasflodin/reckless",
    },
    "BENCHMARK_quill_with_functions_backend_total_time": {
        "name": QUILL_MACRO_FREE_MODE,
        "url": "https://github.com/odygrd/quill",
    },
    "BENCHMARK_bqlog_backend_total_time": {
        "name": "BqLog",
        "url": "https://github.com/Tencent/BqLog",
    },
    "BENCHMARK_boost_log_backend_total_time": {
        "name": "Boost.Log",
        "url": "https://www.boost.org",
    },
    "BENCHMARK_nanolog_backend_total_time": {
        "name": "PlatformLab NanoLog (binary log)",
        "url": "https://github.com/PlatformLab/NanoLog",
    },
    "BENCHMARK_iyengar_nanolog_backend_total_time": {
        "name": "Iyengar NanoLog",
        "url": "https://github.com/Iyengar111/NanoLog",
    },
}

README_THROUGHPUT_LOGGERS = {
    "MS BinLog (binary log)",
    "PlatformLab NanoLog (binary log)",
    "BqLog (binary log)",
    "XTR",
    "Quill",
    "spdlog",
    "fmtlog",
    "Reckless",
    QUILL_MACRO_FREE_MODE,
    "BqLog",
    "Boost.Log",
}

LATENCY_SECTION_METADATA = {
    "int_int_double": {
        "title": "Logging Numbers",
        "loggers": {
            "Quill - Bounded",
            "Quill - Unbounded",
            QUILL_MACRO_FREE_MODE,
            "fmtlog",
            "PlatformLab NanoLog",
            "Ms binLog",
            "XTR",
            "Reckless",
            "Bqlog",
            "bqlog",
            "Iyengar NanoLog",
            "Spdlog",
            "G3Log",
            "Boost.Log",
        },
    },
    "int_int_largestr": {
        "title": "Logging Large Strings",
        "loggers": {
            "Quill - Bounded",
            "Quill - Unbounded",
            QUILL_MACRO_FREE_MODE,
            "fmtlog",
            "PlatformLab NanoLog",
            "Ms binLog",
            "XTR",
            "Reckless",
            "Bqlog",
            "bqlog",
            "Iyengar NanoLog",
            "Spdlog",
            "G3Log",
            "Boost.Log",
        },
    },
    "vector_largestr": {
        "title": "Logging Complex Types",
        "loggers": {
            "Quill - Bounded",
            "Quill - Unbounded",
            QUILL_MACRO_FREE_MODE,
            "fmtlog",
            "Ms binLog",
            "XTR",
            "Spdlog",
            "Boost.Log",
        },
    },
}


SCENARIOS = {
    "single": "Single Call",
    "burst": "Burst of 20 Calls",
    "cold": "Cold Cache",
    "defaults": "Default Settings",
}

DISABLED_SCENARIOS = {
    "disabled_hot": "Disabled Statement, Hot",
    "disabled_cold": "Disabled Statement, Cold",
}

PAYLOADS = ("int_int_double", "int_int_largestr", "vector_largestr")
PERCENTILES = ("p50", "p75", "p90", "p95", "p99", "p999")


def repo_root() -> Path:
    return Path(__file__).resolve().parents[1]


def read_bench_file(filepath: Path) -> tuple[str, str]:
    content = filepath.read_text(encoding="utf-8")
    bench_path_match = re.search(r"^bench: (.+)$", content, re.MULTILINE)
    bench_path = bench_path_match.group(1).strip() if bench_path_match else ""
    return bench_path, content


def bench_name(bench_path: str) -> str:
    name = Path(bench_path).name
    return name[: -len(".exe")] if name.endswith(".exe") else name


def payload_of(name: str) -> str:
    if "vector_largestr" in name:
        return "vector_largestr"
    if "largestr" in name:
        return "int_int_largestr"
    return "int_int_double"


def parse_latency_bench(bench_path: str, content: str) -> list[dict]:
    results = []
    name = bench_name(bench_path)
    payload = payload_of(name)

    headers = list(
        re.finditer(
            r"Thread Count (\d+) - Total messages \d+ - Logger: ([^-]+) - Benchmark: ([^\n]*?)(?: - Scenario: (\w+))?\n",
            content,
        )
    )

    for index, header in enumerate(headers):
        thread_count, logger_raw, _benchmark_info, scenario = header.groups()
        # Results written before the scenarios existed are the burst scenario
        scenario = scenario or "burst"
        logger = logger_raw.strip()
        logger_key = logger

        if logger == "Quill":
            if "with_functions" in name:
                logger_key = QUILL_MACRO_FREE_MODE
            elif "quill_unbounded" in name:
                logger_key = "Quill - Unbounded"
            elif "quill_bounded" in name:
                logger_key = "Quill - Bounded"

        end = headers[index + 1].start() if index + 1 < len(headers) else len(content)
        number = r"\s*([0-9]+(?:\.[0-9]+)?)\s*\|"
        match = re.search(r"\|" + number * 6, content[header.end():end])
        if not match:
            continue

        values = list(map(float, match.groups()))
        result = {"logger": logger_key, "thread_count": int(thread_count), "payload": payload, "scenario": scenario}
        result.update(dict(zip(PERCENTILES, values)))
        results.append(result)

    return results


def parse_backend_bench(bench_path: str, content: str) -> dict | None:
    name = bench_name(bench_path)
    threads = 1
    if name.endswith("_4_threads"):
        threads = 4
        name = name[: -len("_4_threads")]

    logger_info = BACKEND_LOGGER_MAP.get(name)
    if not logger_info:
        return None

    match = re.search(
        r"throughput is\s+([0-9]+(?:\.[0-9]+)?)\s+million msgs/sec average,\s+total time elapsed:\s+(\d+)\s+ms",
        content,
    )
    if not match:
        return None

    producers = re.search(r"producers done after: (\d+) ms", content)
    return {
        "logger": logger_info["name"],
        "threads": threads,
        "throughput": float(match.group(1)),
        "elapsed_ms": float(match.group(2)),
        "producers_ms": float(producers.group(1)) if producers else None,
    }


def combine(results_list: list[dict], keys: tuple[str, ...], stat: str, best) -> dict:
    """
    Combines the runs of one library: the mean or median of every value, or the single best run
    """
    if stat == "best":
        return best(results_list)

    combined = {}
    for key in keys:
        values = [item[key] for item in results_list if item.get(key) is not None]
        if not values:
            combined[key] = None
        elif stat == "median":
            combined[key] = statistics.median(values)
        else:
            combined[key] = sum(values) / len(values)
    combined["runs"] = len(results_list)
    return combined


def collect_latency_results(stat: str = "mean") -> dict:
    samples = defaultdict(lambda: defaultdict(list))

    for filepath in sorted(repo_root().glob("bench_results_*.txt")):
        bench_path, content = read_bench_file(filepath)
        if "call_site_latency" not in bench_path:
            continue

        for result in parse_latency_bench(bench_path, content):
            key = (result["scenario"], result["payload"], result["thread_count"])
            samples[key][result["logger"]].append(result)

    def best(results_list):
        # The single run with the lowest 90th percentile, then 95th, 99th and 99.9th
        return min(results_list, key=lambda item: (item["p90"], item["p95"], item["p99"], item["p999"]))

    combined = defaultdict(dict)
    for key, logger_results in samples.items():
        for logger, results_list in logger_results.items():
            combined[key][logger] = combine(results_list, PERCENTILES, stat, best)

    return combined


def collect_backend_results(stat: str = "mean") -> dict:
    samples = defaultdict(lambda: defaultdict(list))

    for filepath in sorted(repo_root().glob("bench_results_*.txt")):
        bench_path, content = read_bench_file(filepath)
        if "backend_total_time" not in bench_path:
            continue

        result = parse_backend_bench(bench_path, content)
        if result:
            samples[result["threads"]][result["logger"]].append(result)

    def best(results_list):
        # The single run with the highest throughput
        return max(results_list, key=lambda item: item["throughput"])

    combined = defaultdict(dict)
    for threads, logger_results in samples.items():
        for logger, results_list in logger_results.items():
            combined[threads][logger] = combine(
                results_list, ("throughput", "elapsed_ms", "producers_ms"), stat, best
            )

    return combined


def format_latency_table(results: dict, scenario: str, payload: str, thread_count: int, heading: str) -> str:
    logger_results = results.get((scenario, payload, thread_count), {})
    if not logger_results:
        return ""

    allowed_loggers = LATENCY_SECTION_METADATA[payload]["loggers"]

    rows = []
    for logger, data in logger_results.items():
        if logger not in allowed_loggers:
            continue

        logger_info = LATENCY_LOGGER_MAP.get(logger)
        if not logger_info:
            continue

        rows.append(
            {
                "name": f"[{logger_info['name']}]({logger_info['url']})",
                "values": [data[key] for key in PERCENTILES],
                # Ties keep alphabetical order, never a library's own order
                "sort_key": (data["p90"], data["p95"], data["p99"], data["p999"], logger_info["name"].casefold()),
            }
        )

    rows.sort(key=lambda row: row["sort_key"])

    table = f"{heading}\n\n"
    table += "| Library                                                                   |  50th  |  75th  |  90th  |  95th  |  99th  | 99.9th |\n"
    table += "|---------------------------------------------------------------------------|:------:|:------:|:------:|:------:|:------:|:------:|\n"

    for row in rows:
        cells = " | ".join(f"{value:^6.1f}" for value in row["values"])
        table += f"| {row['name'].ljust(73)} | {cells} |\n"

    return table


def format_throughput_table(results: dict, threads: int) -> str:
    rows = []

    for logger, data in results.get(threads, {}).items():
        if logger not in README_THROUGHPUT_LOGGERS:
            continue

        for logger_info in BACKEND_LOGGER_MAP.values():
            if logger_info["name"] == logger:
                rows.append({"name": f"[{logger_info['name']}]({logger_info['url']})", **data})
                break

    if not rows:
        return ""

    rows.sort(key=lambda row: (-row["throughput"], row["name"]))

    heading = "1 Producer Thread" if threads == 1 else f"{threads} Producer Threads"
    table = f"**{heading}**\n\n"
    table += "| Library                                                                    | million msg/second | elapsed time | producers done |\n"
    table += "|----------------------------------------------------------------------------|:------------------:|:------------:|:--------------:|\n"

    for row in rows:
        elapsed = f"{row['elapsed_ms']:.0f} ms"
        producers = f"{row['producers_ms']:.0f} ms" if row.get("producers_ms") is not None else "-"
        table += f"| {row['name'].ljust(74)} | {row['throughput']:>18.2f} | {elapsed:^12} | {producers:^14} |\n"

    return table


def main() -> None:
    parser = argparse.ArgumentParser(description="Generate markdown tables from bench_results_*.txt files.")
    parser.add_argument(
        "--stat",
        choices=("mean", "median", "best"),
        default="mean",
        help="how the runs of each library are combined: the mean or median of every value, or its best run "
        "(the lowest 90th, then 95th, 99th and 99.9th percentile for latency, the highest msg/s for throughput)",
    )
    parser.add_argument("--best-of-runs", action="store_true", help="the same as --stat best")
    args = parser.parse_args()
    stat = "best" if args.best_of_runs else args.stat

    latency_results = collect_latency_results(stat)
    backend_results = collect_backend_results(stat)

    for scenario, scenario_title in SCENARIOS.items():
        sections = []
        for payload in PAYLOADS:
            tables = [
                format_latency_table(latency_results, scenario, payload, 1, "**1 Thread Logging**"),
                format_latency_table(latency_results, scenario, payload, 4, "**4 Threads Logging Simultaneously**"),
            ]
            tables = [table for table in tables if table]
            if tables:
                sections.append(f"### {LATENCY_SECTION_METADATA[payload]['title']}\n\n" + "\n".join(tables))
        if sections:
            print(f"## {scenario_title}\n")
            print("\n".join(sections))

    disabled = [
        format_latency_table(latency_results, scenario, "int_int_double", 1, f"**{title}**")
        for scenario, title in DISABLED_SCENARIOS.items()
    ]
    disabled = [table for table in disabled if table]
    if disabled:
        print("## Disabled Statement\n")
        print("\n".join(disabled))

    throughput = [format_throughput_table(backend_results, threads) for threads in (1, 4)]
    throughput = [table for table in throughput if table]
    if throughput:
        print("## Throughput\n")
        print("\n".join(throughput))


if __name__ == "__main__":
    main()
