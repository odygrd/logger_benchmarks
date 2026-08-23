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
Test base class for BqLog Python tests.
"""

from bq.test.test_result import test_result


class test_base:
    """Abstract base class for test cases."""

    def __init__(self, name):
        self._name = name

    def get_name(self):
        """Get the test name."""
        return self._name

    def test(self):
        """Run the test. Must be overridden by subclasses. Returns a test_result."""
        raise NotImplementedError("Subclasses must implement test()")
