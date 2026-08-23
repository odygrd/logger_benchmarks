#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

mkdir -p "$SCRIPT_DIR/EclipseProj"
cd "$SCRIPT_DIR/EclipseProj"

cmake ../../../../wrapper/java -G "Unix Makefiles"
make

cd "$SCRIPT_DIR"

# Inject copyright header and convert to LF
TARGET="$SCRIPT_DIR/../../../src/bq_log/api/bq_impl_log_invoker.h"
if [ -f "$TARGET" ]; then
    COPYRIGHT='/* Copyright (C) 2025 Tencent.
 * BQLOG is licensed under the Apache License, Version 2.0.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
'
    TMPFILE=$(mktemp)
    printf '%s' "$COPYRIGHT" > "$TMPFILE"
    cat "$TARGET" >> "$TMPFILE"
    # Convert to LF and write back
    tr -d '\r' < "$TMPFILE" > "$TARGET"
    rm -f "$TMPFILE"
    echo "Copyright header injected and converted to LF: $TARGET"
fi
