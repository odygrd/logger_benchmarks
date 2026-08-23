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
Test manager for BqLog Python tests.
Manages console buffer fetching and test execution.
Matches Java/C# test_manager pattern with atomic counter handshake.
"""

import threading
from bq.log import log


class test_manager:
    """Manages test execution and console output capture."""

    _test_list = []
    _fetch_count = 0
    _fetch_lock = threading.Lock()
    _log_console_output = None

    @classmethod
    def add_test(cls, test_obj):
        """Add a test case to the test list."""
        cls._test_list.append(test_obj)

    @classmethod
    def test(cls):
        """Run all registered tests. Returns True if all passed."""
        log.set_console_buffer_enable(True)

        # Start background fetch thread (daemon, matches Java pattern)
        fetch_thread = threading.Thread(target=cls._fetch_loop, daemon=True)
        fetch_thread.start()

        success = True
        for test_obj in cls._test_list:
            result = test_obj.test()
            result.output(test_obj.get_name())
            if not result.is_all_pass():
                success = False

        return success

    @classmethod
    def _fetch_loop(cls):
        """Background thread that fetches console buffer entries.
        Matches Java test_manager pattern with fetch_count handshake."""
        last_fetch_count = 0
        while True:
            with cls._fetch_lock:
                new_fetch_count = cls._fetch_count
            # Drain the console buffer
            while True:
                fetch_result = log.fetch_and_remove_console_buffer(cls._on_fetch)
                if not fetch_result:
                    break
            # Handshake: if caller bumped fetch_count, acknowledge by bumping again
            if new_fetch_count != last_fetch_count:
                new_fetch_count += 1
                with cls._fetch_lock:
                    cls._fetch_count = new_fetch_count
            last_fetch_count = new_fetch_count
            import time
            time.sleep(0.001)

    @classmethod
    def _on_fetch(cls, log_id, category_idx, log_level, content, length):
        """Callback invoked for each fetched console buffer entry."""
        cls._log_console_output = content

    @classmethod
    def get_console_output(cls):
        """Get the latest console output using atomic counter handshake.
        Matches Java test_manager.get_console_output() pattern."""
        import time
        with cls._fetch_lock:
            prev = cls._fetch_count
            cls._fetch_count = prev + 1
        # Wait for background thread to acknowledge (bumps to prev + 2)
        while True:
            with cls._fetch_lock:
                current = cls._fetch_count
            if current != prev + 1:
                break
            time.sleep(0)  # yield
        return cls._log_console_output
