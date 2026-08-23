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
Log decoder for decoding BqLog binary log files.
"""

from bq.impl import log_invoker


class appender_decode_result:
    """Decode result codes."""
    success = 0
    eof = 1
    failed_invalid_handle = 2
    failed_decode_error = 3
    failed_io_error = 4


class log_decoder:
    """
    Decoder for BqLog binary log files.
    """

    def __init__(self, log_file_path, priv_key=""):
        """
        Create a log decoder.

        Args:
            log_file_path: Absolute path to the binary log file.
            priv_key: Private key for encrypted logs (empty string if not encrypted).
        """
        self._handle = 0xFFFFFFFF
        self._result = appender_decode_result.success
        self._decode_text = ""

        result_code, handle = log_invoker.log_decoder_create(log_file_path, priv_key or "")
        if result_code == appender_decode_result.success:
            self._handle = handle
        else:
            self._result = result_code

    def decode(self):
        """
        Decode the next log entry.

        Returns:
            appender_decode_result code.
        """
        if self._result != appender_decode_result.success:
            return self._result
        result_code, text = log_invoker.log_decoder_decode(self._handle)
        self._result = result_code
        self._decode_text = text
        return self._result

    def get_last_decode_result(self):
        """Get the result of the last decode operation."""
        return self._result

    def get_last_decoded_log_entry(self):
        """Get the text of the last successfully decoded log entry."""
        return self._decode_text if self._decode_text else ""

    def destroy(self):
        """Destroy the decoder and release memory."""
        if self._handle != 0xFFFFFFFF:
            log_invoker.log_decoder_destroy(self._handle)
            self._handle = 0xFFFFFFFF

    def __del__(self):
        self.destroy()

    @staticmethod
    def decode_file(log_file_path, output_file_path, priv_key=""):
        """
        Directly decode a binary log file to a text file.

        Args:
            log_file_path: Path to the binary log file.
            output_file_path: Path for the output text file.
            priv_key: Private key for encrypted logs.

        Returns:
            True on success.
        """
        return log_invoker.log_decode(log_file_path, output_file_path, priv_key or "")
