#!/usr/bin/env python3
# Copyright (C) 2025 Tencent.
# BQLOG is licensed under the Apache License, Version 2.0.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.

"""
BqLog Python Category Log Demo - demonstrates category logging features.
"""

import sys
import os

# Ensure the wrapper source is in the path
script_dir = os.path.dirname(os.path.abspath(__file__))
project_root = os.path.abspath(os.path.join(script_dir, "..", ".."))
wrapper_src = os.path.join(project_root, "wrapper", "python", "src")
sys.path.insert(0, wrapper_src)
# Also add the demo directory so we can import the generated category log
sys.path.insert(0, script_dir)

from bq.log import log
from demo_category_log import demo_category_log


def demo_category_logging():
    """Demonstrate category logging with generated wrapper."""
    print("\n=== Category Logging Demo ===")

    # Create a sync category logger with console appender
    cat_log = demo_category_log.create_log("demo_cat_sync",
        "appenders_config.ConsoleAppender.type=console\n"
        "appenders_config.ConsoleAppender.time_zone=localtime\n"
        "appenders_config.ConsoleAppender.levels=[all]\n"
        "log.thread_mode=sync")

    print("Log valid: {}".format(cat_log.is_valid()))
    print("Log name: {}".format(cat_log.get_name()))
    print("Categories count: {}".format(cat_log.get_categories_count()))

    # Log without category
    cat_log.info("Hello BqLog category demo - no category")

    # Log with different categories
    cat_log.info(cat_log.cat.node_2, "This is node_2 category")
    cat_log.info(cat_log.cat.node_2.node_5, "This is node_2.node_5 category")
    cat_log.info(cat_log.cat.node_3.node_6, "This is node_3.node_6 category")
    cat_log.warning(cat_log.cat.node_4.node_7.node_8, "Warning from node_4.node_7.node_8")
    cat_log.error(cat_log.cat.node_4.node_7.node_9, "Error from node_4.node_7.node_9")

    # Log with format parameters and category
    cat_log.info(cat_log.cat.node_3, "Category with params: {}, {}", "hello", 42)

    cat_log.force_flush()


def demo_category_mask():
    """Demonstrate category mask filtering."""
    print("\n=== Category Mask Demo ===")

    # Create a logger with category mask - only node_2.node_5 and node_4 are allowed
    masked_log = demo_category_log.create_log("demo_cat_masked",
        "appenders_config.ConsoleAppender.type=console\n"
        "appenders_config.ConsoleAppender.time_zone=localtime\n"
        "appenders_config.ConsoleAppender.levels=[all]\n"
        "log.thread_mode=sync\n"
        "log.categories_mask=[node_2.node_5,node_4]")

    print("Masked log valid: {}".format(masked_log.is_valid()))

    # This should be filtered (node_2 is not in mask)
    masked_log.info(masked_log.cat.node_2, "This should be FILTERED (node_2 not in mask)")

    # This should pass (node_2.node_5 is in mask)
    masked_log.info(masked_log.cat.node_2.node_5, "This should PASS (node_2.node_5 in mask)")

    # This should pass (node_4 is in mask)
    masked_log.info(masked_log.cat.node_4, "This should PASS (node_4 in mask)")

    # This should pass (node_4.node_7 is child of node_4)
    masked_log.info(masked_log.cat.node_4.node_7, "This should PASS (node_4.node_7 child of node_4)")

    # This should be filtered (node_3 is not in mask)
    masked_log.info(masked_log.cat.node_3, "This should be FILTERED (node_3 not in mask)")

    masked_log.force_flush()


def main():
    print("BqLog Python Category Log Demo")
    print("=" * 50)

    demo_category_logging()
    demo_category_mask()

    log.force_flush_all_logs()

    print("\n" + "=" * 50)
    print("Category demo completed!")
    return 0


if __name__ == "__main__":
    sys.exit(main())
