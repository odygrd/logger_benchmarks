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
Test advanced features: multi-thread logging, console callbacks.
Aligned with Java test_log_2.
"""

import threading
import time
from bq.test.test_base import test_base
from bq.test.test_result import test_result
from bq.log import log
from bq.defs.log_level import log_level


class test_log_2(test_base):
    def __init__(self, name):
        super().__init__(name)
        self._appender = "a" * 32

    def test(self):
        result = test_result()

        log_inst_sync = log.create_log("sync_log",
            "appenders_config.FileAppender.type=compressed_file\n"
            "appenders_config.FileAppender.time_zone=localtime\n"
            "appenders_config.FileAppender.max_file_size=100000000\n"
            "appenders_config.FileAppender.file_name=Output/sync_log\n"
            "appenders_config.FileAppender.levels=[info, info, error,info]\n"
            "log.thread_mode=sync")

        log_inst_async = log.create_log("async_log",
            "appenders_config.FileAppender.type=compressed_file\n"
            "appenders_config.FileAppender.time_zone=localtime\n"
            "appenders_config.FileAppender.max_file_size=100000000\n"
            "appenders_config.FileAppender.file_name=Output/async_log\n"
            "appenders_config.FileAppender.levels=[error,info]")

        appender = self._appender

        # Sync multi-thread test (100 threads, max 5 concurrent)
        # Matches Java pattern: throttled thread pool with accumulating log content
        live_thread = [0]
        left_thread = [100]
        lock = threading.Lock()

        def sync_worker():
            log_content = ""
            for _ in range(128):
                log_content += appender
                log_inst_sync.info(log_content)
            with lock:
                live_thread[0] -= 1

        while left_thread[0] > 0 or live_thread[0] > 0:
            with lock:
                can_start = left_thread[0] > 0 and live_thread[0] < 5
            if can_start:
                t = threading.Thread(target=sync_worker)
                t.daemon = False
                t.start()
                with lock:
                    live_thread[0] += 1
                    left_thread[0] -= 1
            else:
                time.sleep(0.001)

        print("Sync Test Finished")

        # Async multi-thread test (32 threads, max 5 concurrent)
        live_thread[0] = 0
        left_thread[0] = 32

        def async_worker():
            log_content = ""
            for _ in range(2048):
                log_content += appender
                log_inst_async.info(log_content)
            with lock:
                live_thread[0] -= 1

        while left_thread[0] > 0 or live_thread[0] > 0:
            with lock:
                can_start = left_thread[0] > 0 and live_thread[0] < 5
            if can_start:
                t = threading.Thread(target=async_worker)
                t.daemon = False
                t.start()
                with lock:
                    live_thread[0] += 1
                    left_thread[0] -= 1
            else:
                time.sleep(0.001)

        # Console callback test (matches Java pattern)
        log_inst_console = log.create_log("console_log",
            "appenders_config.Appender1.type=console\n"
            "appenders_config.Appender1.time_zone=localtime\n"
            "appenders_config.Appender1.levels=[all]\n"
            "log.thread_mode=sync")

        log.set_console_buffer_enable(False)

        log.register_console_callback(
            lambda cb_log_id, cb_category_idx, cb_level, cb_content:
                (
                    result.add_result(cb_log_id == log_inst_console.get_id(), "console callback test 1"),
                    result.add_result(cb_level == log_level.debug, "console callback test 2"),
                    result.add_result(
                        cb_content.endswith("ConsoleTest") if cb_content else False,
                        "console callback test 3"),
                ) if cb_log_id != 0 else None
        )
        log_inst_console.debug("ConsoleTest")
        log.register_console_callback(None)

        log_inst_async.force_flush()
        result.add_result(True, "")

        return result
