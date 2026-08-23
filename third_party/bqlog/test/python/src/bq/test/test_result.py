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
Test result tracking class for BqLog Python tests.
"""

import threading


class test_result:
    """Tracks pass/fail results for a test suite."""

    def __init__(self):
        self._success_count = 0
        self._total_count = 0
        self._mutex = threading.Lock()
        self._failed_infos = []

    def add_result(self, success, error_msg):
        """Add a test result. Thread-safe."""
        with self._mutex:
            self._total_count += 1
            if success:
                self._success_count += 1
            else:
                if len(self._failed_infos) < 128:
                    self._failed_infos.append(error_msg)
                elif len(self._failed_infos) == 128:
                    self._failed_infos.append(
                        "... Too many test case errors. A maximum of 128 can be displayed, and the rest are omitted. ....")

    def check_log_output_end_with(self, standard_log, error_msg):
        """Check if the latest console output ends with the expected string.
        Fetches console output internally (matches Java/C#/TS pattern)."""
        from bq.test.test_manager import test_manager
        output = test_manager.get_console_output()
        if output is None:
            self.add_result(False, error_msg)
        else:
            self.add_result(output.endswith(standard_log), error_msg)

    def is_all_pass(self):
        """Return True if all tests passed."""
        return self._success_count == self._total_count

    def output(self, test_name):
        """Print test results using bq.log.console (matches Java/C# pattern)."""
        from bq.log import log
        from bq.defs.log_level import log_level as ll
        level = ll.info if self.is_all_pass() else ll.error
        summary = "test case {} result: {}/{}".format(
            test_name, self._success_count, self._total_count)
        log.console(level, summary)
        for info in self._failed_infos:
            log.console(level, "\t" + info)
        log.console(ll.info, " ")
