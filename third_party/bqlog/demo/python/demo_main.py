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
BqLog Python Demo - demonstrates basic and advanced logging features.
"""

import sys
import os
import time

# Ensure the wrapper source is in the path
script_dir = os.path.dirname(os.path.abspath(__file__))
project_root = os.path.abspath(os.path.join(script_dir, "..", ".."))
wrapper_src = os.path.join(project_root, "wrapper", "python", "src")
sys.path.insert(0, wrapper_src)

from bq.log import log
from bq.defs.log_level import log_level
from bq.tools.log_decoder import log_decoder, appender_decode_result


def demo_basic_logging():
    """Demonstrate basic sync and async logging."""
    print("\n=== Basic Logging Demo ===")

    # Create a sync logger with console appender
    sync_log = log.create_log("demo_sync",
        "appenders_config.ConsoleAppender.type=console\n"
        "appenders_config.ConsoleAppender.time_zone=localtime\n"
        "appenders_config.ConsoleAppender.levels=[all]\n"
        "log.thread_mode=sync")

    print("Log version: {}".format(log.get_version()))
    print("Log name: {}".format(sync_log.get_name()))
    print("Log valid: {}".format(sync_log.is_valid()))

    # Log at different levels
    sync_log.verbose("This is a verbose message")
    sync_log.debug("This is a debug message")
    sync_log.info("This is an info message")
    sync_log.warning("This is a warning message")
    sync_log.error("This is an error message")
    sync_log.fatal("This is a fatal message")

    # Log with format parameters
    sync_log.info("Hello {}, you are {} years old", "World", 25)
    sync_log.info("Float: {}, Bool: {}, None: {}", 3.14159, True, None)
    sync_log.info("Negative: {}, Large: {}", -42, 9999999999)
    sync_log.info("Chinese: {}", "\u4f60\u597d\u4e16\u754c")

    # Create an async logger
    async_log = log.create_log("demo_async",
        "appenders_config.ConsoleAppender.type=console\n"
        "appenders_config.ConsoleAppender.time_zone=localtime\n"
        "appenders_config.ConsoleAppender.levels=[all]")

    async_log.info("This is an async log message")
    async_log.force_flush()
    time.sleep(0.1)


def demo_file_appender():
    """Demonstrate file appenders."""
    print("\n=== File Appender Demo ===")

    # Compressed file appender
    file_log = log.create_log("demo_file",
        "appenders_config.FileAppender.type=compressed_file\n"
        "appenders_config.FileAppender.time_zone=localtime\n"
        "appenders_config.FileAppender.file_name=Output/demo_compressed\n"
        "appenders_config.FileAppender.levels=[all]\n"
        "log.thread_mode=sync")

    file_log.info("This log is written to a compressed file")
    file_log.info("With parameters: {}, {}, {}", "hello", 42, 3.14)
    file_log.force_flush()
    print("Compressed file log written to Output/demo_compressed.*")

    # Text file appender
    text_log = log.create_log("demo_text",
        "appenders_config.TextAppender.type=text_file\n"
        "appenders_config.TextAppender.time_zone=localtime\n"
        "appenders_config.TextAppender.file_name=Output/demo_text\n"
        "appenders_config.TextAppender.levels=[all]\n"
        "log.thread_mode=sync")

    text_log.info("This log is written to a text file")
    text_log.force_flush()
    print("Text file log written to Output/demo_text.*")


def demo_snapshot():
    """Demonstrate snapshot feature."""
    print("\n=== Snapshot Demo ===")

    snap_log = log.create_log("demo_snapshot",
        "appenders_config.ConsoleAppender.type=console\n"
        "appenders_config.ConsoleAppender.time_zone=localtime\n"
        "appenders_config.ConsoleAppender.levels=[all]\n"
        "snapshot.buffer_size=65536\n"
        "log.thread_mode=sync")

    snap_log.info("Snapshot entry 1")
    snap_log.info("Snapshot entry 2: {}", "hello")
    snap_log.warning("Snapshot entry 3: warning!")

    snapshot_text = snap_log.take_snapshot("localtime")
    print("Snapshot content:")
    print(snapshot_text)


def demo_console_callback():
    """Demonstrate console callback."""
    print("\n=== Console Callback Demo ===")

    cb_log = log.create_log("demo_callback",
        "appenders_config.ConsoleAppender.type=console\n"
        "appenders_config.ConsoleAppender.time_zone=localtime\n"
        "appenders_config.ConsoleAppender.levels=[all]\n"
        "log.thread_mode=sync")

    def my_callback(log_id, category_idx, level, content):
        level_names = ["verbose", "debug", "info", "warning", "error", "fatal"]
        level_name = level_names[level] if 0 <= level < len(level_names) else "unknown"
        print("  [Callback] level={}, content={}".format(level_name, content))

    log.register_console_callback(my_callback)
    cb_log.info("This triggers the callback")
    cb_log.warning("So does this")
    log.register_console_callback(None)
    print("Console callback demo completed")


def main():
    print("BqLog Python Demo")
    print("=" * 50)

    demo_basic_logging()
    demo_file_appender()
    demo_snapshot()
    demo_console_callback()

    print("\n" + "=" * 50)
    print("Demo completed!")
    return 0


if __name__ == "__main__":
    sys.exit(main())
