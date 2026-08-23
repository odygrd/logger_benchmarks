# Copyright (C) 2025 Tencent.
# BQLOG is licensed under the Apache License, Version 2.0.
# Namespace package - allows bq.test to coexist with bq wrapper.
from pkgutil import extend_path
__path__ = extend_path(__path__, __name__)
