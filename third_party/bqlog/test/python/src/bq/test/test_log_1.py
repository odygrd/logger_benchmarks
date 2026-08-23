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
Test log basic functionality: sync/async logging, level filtering, parameter types, large logs.
Aligned with Java test_log_1.
"""

from bq.test.test_base import test_base
from bq.test.test_result import test_result
from bq.test.test_manager import test_manager
from bq.log import log


class test_log_1(test_base):
    def __init__(self, name):
        super().__init__(name)

    def test(self):
        result = test_result()

        log_inst_sync = log.create_log("sync_log",
            "appenders_config.ConsoleAppender.type=console\n"
            "appenders_config.ConsoleAppender.time_zone=localtime\n"
            "appenders_config.ConsoleAppender.levels=[info, info, error,info]\n"
            "log.thread_mode=sync")

        empty_str = None
        full_str = "123"

        # Test 1: Log level filtering - debug should not output
        log_inst_sync.debug("AAAA")
        result.add_result(test_manager.get_console_output() is None, "log level test")

        # Test 2: Basic string test
        log_inst_sync.info("\u6d4b\u8bd5\u5b57\u7b26\u4e32")
        result.check_log_output_end_with("\u6d4b\u8bd5\u5b57\u7b26\u4e32", "basic test")

        # Test 3: Basic param test with None and string
        log_inst_sync.info("\u6d4b\u8bd5\u5b57\u7b26\u4e32{},{}", empty_str, full_str)
        result.check_log_output_end_with("\u6d4b\u8bd5\u5b57\u7b26\u4e32null,123", "basic param test 1")

        # Test 4: Float format test
        standard_output = "Float value result: 62.1564"
        log_inst_sync.info("Float value result: {}", 62.15645)
        result.add_result(
            standard_output in (test_manager.get_console_output() or ""),
            "Float format test")

        # Test 5: Multiple parameter types (matches Java exactly)
        standard_output = (
            "\u8fd9\u4e9b\u662f\u7ed3\u679c\uff0cabc, abcde, -32, FALSE, TRUE, null, 3, 3823823, -32354, "
            "\u6d4b\u8bd5\u5b57\u7b26\u4e32\u5b8c\u6574\u7684\uff0c \u7ed3\u679c\u5b8c\u6210\u4e86"
        )
        log_inst_sync.info(
            "\u8fd9\u4e9b\u662f\u7ed3\u679c\uff0c{}, {}, {}, {}, {}, {}, {}, {}, {}, {}\uff0c \u7ed3\u679c\u5b8c\u6210\u4e86",
            "abc", "abcde", -32, False, True, None, 3, 3823823, -32354, "\u6d4b\u8bd5\u5b57\u7b26\u4e32\u5b8c\u6574\u7684")
        result.check_log_output_end_with(standard_output, "basic param test 2")

        # Test 6: Large format string test (grow to 1MB+)
        format_prefix = "a"
        appender = "a" * 1024
        while len(format_prefix) <= 1024 * 1024 + 1024 + 4:
            log_inst_sync.info(
                format_prefix + "\u8fd9\u4e9b\u662f\u7ed3\u679c\uff0c{}, {}, {}, {}, {}, {}, {}, {}, {}, {}\uff0c \u7ed3\u679c\u5b8c\u6210\u4e86",
                "abc", "abcde", -32, False, True, None, 3, 3823823, -32354, "\u6d4b\u8bd5\u5b57\u7b26\u4e32\u5b8c\u6574\u7684")
            result.check_log_output_end_with(format_prefix + standard_output, "basic param test 2")
            format_prefix += appender

        # Test 7: Async logging test (matches Java pattern)
        log_inst_async = log.create_log("async_log",
            "appenders_config.ConsoleAppender.type=console\n"
            "appenders_config.ConsoleAppender.time_zone=localtime\n"
            "appenders_config.ConsoleAppender.levels=[error,info]")

        format_prefix = "a"
        while len(format_prefix) <= 1024 * 1024 + 1024 + 4:
            log_inst_async.info(
                format_prefix + "\u8fd9\u4e9b\u662f\u7ed3\u679c\uff0c{}, {}, {}, {}, {}, {}, {}, {}, {}, {}\uff0c \u7ed3\u679c\u5b8c\u6210\u4e86",
                "abc", "abcde", -32, False, True, None, 3, 3823823, -32354, "\u6d4b\u8bd5\u5b57\u7b26\u4e32\u5b8c\u6574\u7684")
            log_inst_async.force_flush()
            result.check_log_output_end_with(format_prefix + standard_output, "basic param test")
            format_prefix += appender

        return result
