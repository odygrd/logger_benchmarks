#!/usr/bin/env python3
import re
import glob
from collections import defaultdict
from typing import Dict, List, Tuple

# Logger name mapping with URLs
LOGGER_MAP = {
    'Quill - Unbounded': {
        'name': 'Quill Unbounded Queue',
        'url': 'https://github.com/odygrd/quill'
    },
    'Quill - Bounded': {
        'name': 'Quill Bounded Dropping Queue',
        'url': 'https://github.com/odygrd/quill'
    },
    'Quill - Unbounded With Functions': {
        'name': 'Quill Unbounded Queue (Log Functions)',
        'url': 'https://github.com/odygrd/quill'
    },
    'fmtlog': {
        'name': 'fmtlog',
        'url': 'https://github.com/MengRao/fmtlog'
    },
    'PlatformLab NanoLog': {
        'name': 'PlatformLab NanoLog',
        'url': 'https://github.com/PlatformLab/NanoLog'
    },
    'Ms binLog': {
        'name': 'MS BinLog',
        'url': 'https://github.com/Morgan-Stanley/binlog'
    },
    'XTR': {
        'name': 'XTR',
        'url': 'https://github.com/choll/xtr'
    },
    'Reckless': {
        'name': 'Reckless',
        'url': 'https://github.com/mattiasflodin/reckless'
    },
    'Bqlog': {
        'name': 'BqLog',
        'url': 'https://github.com/Tencent/BqLog'
    },
    'bqlog': {
        'name': 'BqLog',
        'url': 'https://github.com/Tencent/BqLog'
    },
    'Iyengar NanoLog': {
        'name': 'Iyengar NanoLog',
        'url': 'https://github.com/Iyengar111/NanoLog'
    },
    'Spdlog': {
        'name': 'spdlog',
        'url': 'https://github.com/gabime/spdlog'
    },
    'G3Log': {
        'name': 'g3log',
        'url': 'https://github.com/KjellKod/g3log'
    },
    'Boost.Log': {
        'name': 'Boost.Log',
        'url': 'https://www.boost.org'
    }
}


def parse_bench_file(filepath: str) -> List[Dict]:
    """Parse a single benchmark results file."""
    results = []

    with open(filepath, 'r') as f:
        content = f.read()

    # Get the benchmark path to determine the actual benchmark type
    bench_path_match = re.search(r'bench: (.+)$', content, re.MULTILINE)
    bench_path = bench_path_match.group(1) if bench_path_match else ''

    # Determine benchmark type from the path
    benchmark_type = 'int_int_double'  # default
    if 'int_int_largestr' in bench_path or 'largestr' in bench_path:
        if 'vector' in bench_path:
            benchmark_type = 'vector_largestr'
        else:
            benchmark_type = 'int_int_largestr'
    elif 'vector_largestr' in bench_path or 'int_vector_largestr' in bench_path:
        benchmark_type = 'vector_largestr'

    # Split by thread count sections
    sections = re.split(r'Thread Count \d+', content)
    headers = re.findall(r'Thread Count (\d+) - Total messages \d+ - Logger: ([^-]+) - Benchmark: ([^\n]+)', content)

    for i, (thread_count, logger_raw, benchmark_info) in enumerate(headers):
        # Extract logger name
        logger = logger_raw.strip()

        # Determine queue type for Quill using the bench path
        # Must use bench path, not header, as headers can be misleading
        logger_key = logger
        if logger == 'Quill':
            if 'with_functions' in bench_path or 'quill_with_functions' in bench_path:
                logger_key = 'Quill - Unbounded With Functions'
            elif 'quill_unbounded' in bench_path:
                # Use more specific pattern to avoid substring issues
                logger_key = 'Quill - Unbounded'
            elif 'quill_bounded' in bench_path:
                logger_key = 'Quill - Bounded'

        # Find the percentile values in the corresponding section
        if i + 1 < len(sections):
            section_text = sections[i + 1]
            # Extract the table row with percentiles
            match = re.search(r'\|\s*(\d+)\s*\|\s*(\d+)\s*\|\s*(\d+)\s*\|\s*(\d+)\s*\|\s*(\d+)\s*\|\s*(\d+)\s*\|',
                              section_text)
            if match:
                p50, p75, p90, p95, p99, p999 = map(int, match.groups())
                results.append({
                    'logger': logger_key,
                    'thread_count': int(thread_count),
                    'benchmark_type': benchmark_type,
                    'p50': p50,
                    'p75': p75,
                    'p90': p90,
                    'p95': p95,
                    'p99': p99,
                    'p999': p999
                })

    return results


def collect_all_results() -> Dict:
    """Collect all benchmark results from all files."""
    all_results = defaultdict(lambda: defaultdict(list))

    # Parse all bench_results_*.txt files
    for filepath in glob.glob('bench_results_*.txt'):
        results = parse_bench_file(filepath)
        for result in results:
            key = (result['benchmark_type'], result['thread_count'])
            all_results[key][result['logger']].append(result)

    # Average results for each logger/benchmark/thread combination
    averaged_results = defaultdict(lambda: defaultdict(dict))
    for key, logger_results in all_results.items():
        for logger, results_list in logger_results.items():
            if results_list:
                avg_result = {
                    'logger': logger,
                    'p50': round(sum(r['p50'] for r in results_list) / len(results_list)),
                    'p75': round(sum(r['p75'] for r in results_list) / len(results_list)),
                    'p90': round(sum(r['p90'] for r in results_list) / len(results_list)),
                    'p95': round(sum(r['p95'] for r in results_list) / len(results_list)),
                    'p99': round(sum(r['p99'] for r in results_list) / len(results_list)),
                    'p999': round(sum(r['p999'] for r in results_list) / len(results_list))
                }
                averaged_results[key][logger] = avg_result

    return averaged_results


def format_table(results: Dict, benchmark_type: str, thread_count: int) -> str:
    """Format a single table."""
    key = (benchmark_type, thread_count)
    logger_results = results.get(key, {})

    # Create list of rows
    rows = []
    for logger, data in logger_results.items():
        if logger in LOGGER_MAP:
            logger_info = LOGGER_MAP[logger]

            # Determine sort priority for ties
            # Lower number = higher priority
            is_quill = 'Quill' in logger
            is_bounded = 'Bounded' in logger

            if is_quill and is_bounded:
                tie_breaker = 0  # Quill Bounded has highest priority
            elif is_quill and 'Unbounded' in logger and 'Macro Free' not in logger:
                tie_breaker = 1  # Quill Unbounded (regular) second
            elif is_quill:
                tie_breaker = 2  # Other Quill variants third
            else:
                tie_breaker = 3  # Non-Quill libraries last

            row = {
                'name': f"[{logger_info['name']}]({logger_info['url']})",
                'p50': data['p50'],
                'p75': data['p75'],
                'p90': data['p90'],
                'p95': data['p95'],
                'p99': data['p99'],
                'p999': data['p999'],
                'sort_key': (data['p95'], tie_breaker)  # Sort by p95 first, then tie_breaker
            }
            rows.append(row)

    # Sort by p95 (ascending), then by tie_breaker for same p95 values
    rows.sort(key=lambda x: x['sort_key'])

    # Generate header
    if thread_count == 1:
        header = "##### 1 Thread Logging\n\n"
    else:
        header = "##### 4 Threads Logging Simultaneously\n\n"

    # Generate table
    table = header
    table += "| Library                                                                   |  50th  |  75th  |  90th  |  95th  |  99th  | 99.9th |\n"
    table += "|---------------------------------------------------------------------------|:------:|:------:|:------:|:------:|:------:|:------:|\n"

    for row in rows:
        # Format name with proper padding
        name = row['name']
        # Pad to 73 characters (75 total with surrounding spaces)
        name_padded = name.ljust(73)

        table += f"| {name_padded} | {row['p50']:^6} | {row['p75']:^6} | {row['p90']:^6} | {row['p95']:^6} | {row['p99']:^6} | {row['p999']:^6} |\n"

    return table


def main():
    """Main function to generate all tables."""
    results = collect_all_results()

    # Generate tables in order
    benchmark_types = [
        ('int_int_double', ''),
        ('int_int_largestr', ''),
        ('vector_largestr', '')
    ]

    for benchmark_type, label in benchmark_types:
        # 1 thread table
        print(format_table(results, benchmark_type, 1))
        # 4 threads table
        print(format_table(results, benchmark_type, 4))


if __name__ == '__main__':
    main()
