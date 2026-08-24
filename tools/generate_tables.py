#!/usr/bin/env python3

from __future__ import annotations

import re
from collections import defaultdict
from pathlib import Path

LATENCY_LOGGER_MAP = {
    "Quill - Unbounded": {
        "name": "Quill Unbounded Queue",
        "url": "https://github.com/odygrd/quill",
    },
    "Quill - Bounded": {
        "name": "Quill Bounded Dropping Queue",
        "url": "https://github.com/odygrd/quill",
    },
    "Quill - Unbounded With Functions": {
        "name": "Quill Unbounded Queue (Log Functions)",
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
        "name": "Quill - Macro Free Mode",
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
        "name": "PlatformLab NanoLog",
        "url": "https://github.com/PlatformLab/NanoLog",
    },
    "BENCHMARK_iyengar_nanolog_backend_total_time": {
        "name": "Iyengar NanoLog",
        "url": "https://github.com/Iyengar111/NanoLog",
    },
}

README_THROUGHPUT_LOGGERS = {
    "MS BinLog (binary log)",
    "BqLog (binary log)",
    "XTR",
    "Quill",
    "spdlog",
    "fmtlog",
    "Reckless",
    "Quill - Macro Free Mode",
    "BqLog",
    "Boost.Log",
}

LATENCY_SECTION_METADATA = {
    "int_int_double": {
        "title": "#### Logging Numbers",
        "loggers": {
            "Quill - Bounded",
            "Quill - Unbounded",
            "Quill - Unbounded With Functions",
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
        "title": "#### Logging Large Strings",
        "loggers": {
            "Quill - Bounded",
            "Quill - Unbounded",
            "Quill - Unbounded With Functions",
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
        "title": "#### Logging Complex Types",
        "loggers": {
            "Quill - Bounded",
            "Quill - Unbounded",
            "fmtlog",
            "Ms binLog",
            "XTR",
            "Spdlog",
            "Boost.Log",
        },
    },
}


def repo_root() -> Path:
    return Path(__file__).resolve().parents[1]


def read_bench_file(filepath: Path) -> tuple[str, str]:
    content = filepath.read_text(encoding="utf-8")
    bench_path_match = re.search(r"^bench: (.+)$", content, re.MULTILINE)
    bench_path = bench_path_match.group(1).strip() if bench_path_match else ""
    return bench_path, content


def parse_latency_bench(bench_path: str, content: str) -> list[dict]:
    results = []

    benchmark_type = "int_int_double"
    if "vector_largestr" in bench_path or "int_vector_largestr" in bench_path:
        benchmark_type = "vector_largestr"
    elif "int_int_largestr" in bench_path or "largestr" in bench_path:
        benchmark_type = "int_int_largestr"

    sections = re.split(r"Thread Count \d+", content)
    headers = re.findall(
        r"Thread Count (\d+) - Total messages \d+ - Logger: ([^-]+) - Benchmark: ([^\n]+)",
        content,
    )

    for index, (thread_count, logger_raw, _benchmark_info) in enumerate(headers):
        logger = logger_raw.strip()
        logger_key = logger

        if logger == "Quill":
            if "with_functions" in bench_path or "quill_with_functions" in bench_path:
                logger_key = "Quill - Unbounded With Functions"
            elif "quill_unbounded" in bench_path:
                logger_key = "Quill - Unbounded"
            elif "quill_bounded" in bench_path:
                logger_key = "Quill - Bounded"

        if index + 1 >= len(sections):
            continue

        match = re.search(
            r"\|\s*(\d+)\s*\|\s*(\d+)\s*\|\s*(\d+)\s*\|\s*(\d+)\s*\|\s*(\d+)\s*\|\s*(\d+)\s*\|",
            sections[index + 1],
        )
        if not match:
            continue

        p50, p75, p90, p95, p99, p999 = map(int, match.groups())
        results.append(
            {
                "logger": logger_key,
                "thread_count": int(thread_count),
                "benchmark_type": benchmark_type,
                "p50": p50,
                "p75": p75,
                "p90": p90,
                "p95": p95,
                "p99": p99,
                "p999": p999,
            }
        )

    return results


def parse_backend_bench(bench_path: str, content: str) -> dict | None:
    bench_name = Path(bench_path).name
    logger_info = BACKEND_LOGGER_MAP.get(bench_name)
    if not logger_info:
        return None

    match = re.search(
        r"throughput is\s+([0-9]+(?:\.[0-9]+)?)\s+million msgs/sec average,\s+total time elapsed:\s+(\d+)\s+ms",
        content,
    )
    if not match:
        return None

    return {
        "logger": logger_info["name"],
        "throughput": float(match.group(1)),
        "elapsed_ms": int(match.group(2)),
    }


def collect_latency_results() -> dict:
    samples = defaultdict(lambda: defaultdict(list))

    for filepath in sorted(repo_root().glob("bench_results_*.txt")):
        bench_path, content = read_bench_file(filepath)
        if "call_site_latency" not in bench_path:
            continue

        for result in parse_latency_bench(bench_path, content):
            key = (result["benchmark_type"], result["thread_count"])
            samples[key][result["logger"]].append(result)

    averaged = defaultdict(dict)
    for key, logger_results in samples.items():
        for logger, results_list in logger_results.items():
            averaged[key][logger] = {
                "p50": round(sum(item["p50"] for item in results_list) / len(results_list)),
                "p75": round(sum(item["p75"] for item in results_list) / len(results_list)),
                "p90": round(sum(item["p90"] for item in results_list) / len(results_list)),
                "p95": round(sum(item["p95"] for item in results_list) / len(results_list)),
                "p99": round(sum(item["p99"] for item in results_list) / len(results_list)),
                "p999": round(sum(item["p999"] for item in results_list) / len(results_list)),
            }

    return averaged


def collect_backend_results() -> dict:
    samples = defaultdict(list)

    for filepath in sorted(repo_root().glob("bench_results_*.txt")):
        bench_path, content = read_bench_file(filepath)
        if "backend_total_time" not in bench_path:
            continue

        result = parse_backend_bench(bench_path, content)
        if result:
            samples[result["logger"]].append(result)

    averaged = {}
    for logger, results_list in samples.items():
        averaged[logger] = {
            "throughput": sum(item["throughput"] for item in results_list) / len(results_list),
            "elapsed_ms": round(sum(item["elapsed_ms"] for item in results_list) / len(results_list)),
        }

    return averaged


def format_latency_table(results: dict, benchmark_type: str, thread_count: int) -> str:
    key = (benchmark_type, thread_count)
    logger_results = results.get(key, {})
    allowed_loggers = LATENCY_SECTION_METADATA[benchmark_type]["loggers"]

    rows = []
    for logger, data in logger_results.items():
        if logger not in allowed_loggers:
            continue

        logger_info = LATENCY_LOGGER_MAP.get(logger)
        if not logger_info:
            continue

        if logger == "Quill - Bounded":
            tie_breaker = 0
        elif logger == "Quill - Unbounded":
            tie_breaker = 1
        elif logger.startswith("Quill"):
            tie_breaker = 2
        else:
            tie_breaker = 3

        rows.append(
            {
                "name": f"[{logger_info['name']}]({logger_info['url']})",
                "p50": data["p50"],
                "p75": data["p75"],
                "p90": data["p90"],
                "p95": data["p95"],
                "p99": data["p99"],
                "p999": data["p999"],
                "sort_key": (
                    data["p90"],
                    data["p95"],
                    data["p99"],
                    data["p999"],
                    tie_breaker,
                    logger_info["name"].casefold(),
                ),
            }
        )

    rows.sort(key=lambda row: row["sort_key"])

    header = "##### 1 Thread Logging\n\n" if thread_count == 1 else "##### 4 Threads Logging Simultaneously\n\n"

    table = header
    table += "| Library                                                                   |  50th  |  75th  |  90th  |  95th  |  99th  | 99.9th |\n"
    table += "|---------------------------------------------------------------------------|:------:|:------:|:------:|:------:|:------:|:------:|\n"

    for row in rows:
        name_padded = row["name"].ljust(73)
        table += (
            f"| {name_padded} | {row['p50']:^6} | {row['p75']:^6} | {row['p90']:^6} | "
            f"{row['p95']:^6} | {row['p99']:^6} | {row['p999']:^6} |\n"
        )

    return table


def format_throughput_table(results: dict) -> str:
    rows = []

    for logger, data in results.items():
        if logger not in README_THROUGHPUT_LOGGERS:
            continue

        for bench_name, logger_info in BACKEND_LOGGER_MAP.items():
            if logger_info["name"] == logger:
                rows.append(
                    {
                        "name": f"[{logger_info['name']}]({logger_info['url']})",
                        "throughput": data["throughput"],
                        "elapsed_ms": data["elapsed_ms"],
                    }
                )
                break

    rows.sort(key=lambda row: (-row["throughput"], row["name"]))

    table = "### Throughput\n\n"
    table += "| Library                                                            | million msg/second | elapsed time |\n"
    table += "|--------------------------------------------------------------------|:------------------:|:------------:|\n"

    for row in rows:
        name_padded = row["name"].ljust(66)
        elapsed = f"{row['elapsed_ms']} ms"
        table += f"| {name_padded} | {row['throughput']:>18.2f} | {elapsed:^12} |\n"

    return table


def main() -> None:
    latency_results = collect_latency_results()
    backend_results = collect_backend_results()

    for benchmark_type in ("int_int_double", "int_int_largestr", "vector_largestr"):
        print(LATENCY_SECTION_METADATA[benchmark_type]["title"])
        print()
        print(format_latency_table(latency_results, benchmark_type, 1))
        print(format_latency_table(latency_results, benchmark_type, 4))

    print(format_throughput_table(backend_results))


if __name__ == "__main__":
    main()
