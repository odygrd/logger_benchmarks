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
Category log base class for BqLog.
Extends log to support categorized logging.
"""

from bq.log import log
from bq.impl import log_invoker


class category_log(log):
    """
    Base class for category-aware logs.
    Subclasses should define category constants and call
    call_api_create_category_log() to create the log.
    """

    def __init__(self, log_id=0):
        super().__init__(log_id)

    def get_categories_count(self):
        """Get the number of categories."""
        return len(self._categories_name_array)

    def get_categories_name_array(self):
        """Get names of all categories."""
        return self._categories_name_array

    @staticmethod
    def call_api_create_category_log(name, config, categories_count, categories):
        """
        Create a category log via the C API.

        Args:
            name: Log name.
            config: Log configuration string.
            categories_count: Number of categories.
            categories: List of category name strings.

        Returns:
            log_id (0 means failure).
        """
        if not config:
            return 0
        return log_invoker.create_log(name, config, categories_count, categories)
