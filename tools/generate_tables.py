#!/usr/bin/env python3

from __future__ import annotations

import argparse
import json
import sys
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
}

DISABLED_SCENARIOS = {
    "disabled_hot": "Disabled Statement, Hot",
    "disabled_cold": "Disabled Statement, Cold",
}

PAYLOADS = ("int_int_double", "int_int_largestr", "vector_largestr")
PERCENTILES = ("p50", "p75", "p90", "p95", "p99", "p999")


def repo_root() -> Path:
    return Path(__file__).resolve().parents[1]


def run_failed(content: str) -> bool:
    """
    A run that lost messages, or whose benchmark did not exit cleanly, is never published
    """
    if re.search(r"^delivery: .*FAIL$", content, re.MULTILINE):
        return True
    exit_code = re.search(r"^exit code: (-?\d+)$", content, re.MULTILINE)
    return exit_code is not None and exit_code.group(1) != "0"


def read_bench_file(filepath: Path) -> tuple[str, str]:
    content = filepath.read_text(encoding="utf-8")
    if run_failed(content):
        print(f"skipping {filepath.name}: a delivery check failed or the benchmark did not exit cleanly",
              file=sys.stderr)
        return "", ""
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
        return {**best(results_list), "runs": len(results_list)}

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
    for key, logger_results in samples.items():
        for logger, results_list in logger_results.items():
            combined[key][logger] = combine(
                results_list, ("throughput", "elapsed_ms", "producers_ms"), stat, best
            )

    return combined


# Output format of the tables, set from --format: markdown for READMEs, rst for the Quill docs page
OUTPUT_FORMAT = "markdown"


def link(name: str, url: str) -> str:
    return f"`{name} <{url}>`__" if OUTPUT_FORMAT == "rst" else f"[{name}]({url})"


def heading(title: str, level: int) -> str:
    if OUTPUT_FORMAT == "rst":
        return f"{title}\n{'-~^'[level - 2] * len(title)}"
    return f"{'#' * level} {title}"


def render_table(columns: list[str], rows: list[list[str]], first_width: int, widths: list[int]) -> str:
    """
    A table with a library column and centred value columns
    """
    if OUTPUT_FORMAT == "rst":
        table = ".. list-table::\n   :header-rows: 1\n   :class: bench-table bench-center\n\n"
        for row in [columns] + rows:
            table += f"   * - {row[0]}\n"
            for cell in row[1:]:
                table += f"     - {cell.strip()}\n"
        return table

    table = f"| {columns[0].ljust(first_width)} | " + " | ".join(
        column.center(width) for column, width in zip(columns[1:], widths)) + " |\n"
    table += f"|{'-' * (first_width + 2)}|" + "|".join(f":{'-' * width}:" for width in widths) + "|\n"
    for row in rows:
        table += f"| {row[0].ljust(first_width)} | " + " | ".join(
            cell.center(width) for cell, width in zip(row[1:], widths)) + " |\n"
    return table


def latency_rows(results: dict, scenario: str, payload: str, thread_count: int) -> list[dict]:
    """
    The rows of one latency table, sorted: name, url and the six percentiles
    """
    logger_results = results.get((scenario, payload, thread_count), {})
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
                "name": logger_info["name"],
                "url": logger_info["url"],
                "values": [data[key] for key in PERCENTILES],
                "runs": data.get("runs"),
                # Ties keep alphabetical order, never a library's own order
                "sort_key": (data["p90"], data["p95"], data["p99"], data["p999"], logger_info["name"].casefold()),
            }
        )

    rows.sort(key=lambda row: row["sort_key"])
    for row in rows:
        del row["sort_key"]
    return rows


def format_latency_table(results: dict, scenario: str, payload: str, thread_count: int, title: str) -> str:
    rows = latency_rows(results, scenario, payload, thread_count)
    if not rows:
        return ""

    columns = ["Library", "50th", "75th", "90th", "95th", "99th", "99.9th"]
    cells = [[link(row["name"], row["url"])] + [f"{value:.1f}" for value in row["values"]] for row in rows]
    return f"{title}\n\n" + render_table(columns, cells, 73, [6] * 6)


def throughput_rows(results: dict, threads: int) -> list[dict]:
    """
    The rows of one throughput table, fastest first
    """
    rows = []

    for logger, data in results.get(threads, {}).items():
        if logger not in README_THROUGHPUT_LOGGERS:
            continue

        for logger_info in BACKEND_LOGGER_MAP.values():
            if logger_info["name"] == logger:
                rows.append({"name": logger_info["name"], "url": logger_info["url"], **data})
                break

    rows.sort(key=lambda row: (-row["throughput"], row["name"]))
    return rows


def format_throughput_table(results: dict, threads: int) -> str:
    rows = throughput_rows(results, threads)
    if not rows:
        return ""

    title = "1 Producer Thread" if threads == 1 else f"{threads} Producer Threads"
    columns = ["Library", "million msg/second", "elapsed time", "producers done"]
    cells = []
    for row in rows:
        producers = f"{row['producers_ms']:.0f} ms" if row.get("producers_ms") is not None else "-"
        cells.append([link(row["name"], row["url"]), f"{row['throughput']:.2f}", f"{row['elapsed_ms']:.0f} ms", producers])
    return f"**{title}**\n\n" + render_table(columns, cells, 74, [18, 12, 14])


def results_json(latency_results: dict, backend_results: dict, stat: str) -> dict:
    """
    Every table as data: the rows of each latency table by scenario, payload and threads, and of each
    throughput table by producer threads
    """
    latency = []
    for scenario in list(SCENARIOS) + list(DISABLED_SCENARIOS):
        for payload in PAYLOADS:
            for threads in (1, 4):
                rows = latency_rows(latency_results, scenario, payload, threads)
                if rows:
                    latency.append({"scenario": scenario, "payload": payload, "threads": threads, "rows": rows})

    throughput = []
    for threads in (1, 4):
        rows = throughput_rows(backend_results, threads)
        if rows:
            throughput.append({"threads": threads, "rows": rows})

    return {"stat": stat, "percentiles": list(PERCENTILES), "latency": latency, "throughput": throughput}


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
    parser.add_argument("--format", choices=("markdown", "rst", "json"), default="markdown",
                        help="markdown tables, reStructuredText list-tables, or json: every table's rows, which "
                        "the Quill docs build their benchmark pages from")
    args = parser.parse_args()
    global OUTPUT_FORMAT
    OUTPUT_FORMAT = args.format
    stat = "best" if args.best_of_runs else args.stat

    latency_results = collect_latency_results(stat)
    backend_results = collect_backend_results(stat)

    if OUTPUT_FORMAT == "json":
        print(json.dumps(results_json(latency_results, backend_results, stat), indent=1))
        return

    for scenario, scenario_title in SCENARIOS.items():
        sections = []
        for payload in PAYLOADS:
            tables = [
                format_latency_table(latency_results, scenario, payload, 1, "**1 Thread Logging**"),
                format_latency_table(latency_results, scenario, payload, 4, "**4 Threads Logging Simultaneously**"),
            ]
            tables = [table for table in tables if table]
            if tables:
                sections.append(heading(LATENCY_SECTION_METADATA[payload]["title"], 3) + "\n\n" + "\n".join(tables))
        if sections:
            print(heading(scenario_title, 2) + "\n")
            print("\n".join(sections))

    disabled = [
        format_latency_table(latency_results, scenario, "int_int_double", 1, f"**{title}**")
        for scenario, title in DISABLED_SCENARIOS.items()
    ]
    disabled = [table for table in disabled if table]
    if disabled:
        print(heading("Disabled Statement", 2) + "\n")
        print("\n".join(disabled))

    throughput = [format_throughput_table(backend_results, threads) for threads in (1, 4)]
    throughput = [table for table in throughput if table]
    if throughput:
        print(heading("Throughput", 2) + "\n")
        print("\n".join(throughput))


if __name__ == "__main__":
    main()
