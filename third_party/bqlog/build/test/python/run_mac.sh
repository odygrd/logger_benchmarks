#!/usr/bin/env bash
set -euo pipefail

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${DIR}/../../.." && pwd)"
BUILD_SCRIPT="${PROJECT_ROOT}/build/wrapper/python/build_all_and_pack.sh"
INSTALL_DIR="${PROJECT_ROOT}/install/wrapper_python"
TEST_MAIN="${PROJECT_ROOT}/test/python/src/bq/test/test_main.py"

for BUILD_TYPE in Debug Release RelWithDebInfo MinSizeRel; do
    echo ""
    echo "========================================"
    echo " Testing build type: ${BUILD_TYPE}"
    echo "========================================"

    echo "----- Building wheel [${BUILD_TYPE}] -----"
    bash "${BUILD_SCRIPT}" "${BUILD_TYPE}"

    echo "----- Installing wheel -----"
    pip3 install --force-reinstall "${INSTALL_DIR}"/*.whl

    echo "----- Running tests [${BUILD_TYPE}] -----"
    python3 "${TEST_MAIN}"
    echo "Tests PASSED for ${BUILD_TYPE}"
done

echo ""
echo "All Python tests PASSED!"
