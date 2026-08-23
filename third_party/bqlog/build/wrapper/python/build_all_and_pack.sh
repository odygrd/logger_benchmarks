#!/usr/bin/env bash
set -euo pipefail

# Build BqLog Python Wrapper and pack into wheel (Linux / macOS)
# Step 1: Build native .so/.dylib with Python support
# Step 2: Run wrapper/python/CMakeLists.txt (version sync + artifacts staging)
# Step 3: Copy native lib into staged artifacts
# Step 4: Build wheel (output to artifacts/wrapper/python/wheel/)
# Step 5: (Linux only) auditwheel repair for manylinux tag
# Step 6: cmake install — copies only .whl + docs to install/wrapper_python/

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${DIR}/../../.." && pwd)"
WRAPPER_DIR="${PROJECT_ROOT}/wrapper/python"
ARTIFACTS_DIR="${PROJECT_ROOT}/artifacts"
WHEEL_DIR="${ARTIFACTS_DIR}/wrapper/python/wheel"

CONFIG="${1:-RelWithDebInfo}"

# ===== Step 1: Build native library =====
if [[ "$(uname)" == "Darwin" ]]; then
    BUILD_LIB_DIR="${PROJECT_ROOT}/build/lib/mac"
    echo "===== Building BqLog Dynamic Library with Python Support (macOS) [${CONFIG}] ====="
    pushd "${BUILD_LIB_DIR}" >/dev/null
    zsh dont_execute_this.sh build OFF OFF ON dynamic_lib dylib "${CONFIG}"
    popd >/dev/null
else
    BUILD_LIB_DIR="${PROJECT_ROOT}/build/lib/linux"
    echo "===== Building BqLog Dynamic Library with Python Support (Linux) [${CONFIG}] ====="
    pushd "${BUILD_LIB_DIR}" >/dev/null
    bash dont_execute_this.sh build native gcc OFF OFF ON dynamic_lib "${CONFIG}"
    popd >/dev/null
fi

# ===== Step 2: Run wrapper CMake (version sync + artifacts staging) =====
echo "===== Running wrapper/python CMake (version sync + staging) ====="
WRAPPER_BUILD_DIR="${DIR}/wrapperProj"
rm -rf "${WRAPPER_BUILD_DIR}"
mkdir -p "${WRAPPER_BUILD_DIR}"
pushd "${WRAPPER_BUILD_DIR}" >/dev/null
cmake "${WRAPPER_DIR}" -G "Unix Makefiles"
cmake --build .
popd >/dev/null

# ===== Step 3: Copy native lib into staged artifacts =====
echo "===== Copying native library to wheel source ====="
STAGING_DIR="${ARTIFACTS_DIR}/wrapper/python"
LIB_PATH="${ARTIFACTS_DIR}/dynamic_lib/lib/${CONFIG}"
if [ ! -d "${LIB_PATH}" ]; then
    if [ -d "${ARTIFACTS_DIR}/dynamic_lib/lib/Release" ]; then
        LIB_PATH="${ARTIFACTS_DIR}/dynamic_lib/lib/Release"
    elif [ -d "${ARTIFACTS_DIR}/dynamic_lib/lib/RelWithDebInfo" ]; then
        LIB_PATH="${ARTIFACTS_DIR}/dynamic_lib/lib/RelWithDebInfo"
    fi
fi

for f in "${LIB_PATH}"/_bqlog*.so "${LIB_PATH}"/_bqlog*.dylib; do
    if [ -f "$f" ]; then
        cp "$f" "${STAGING_DIR}/src/bq/"
        echo "Copied $(basename "$f")"
    fi
done

# ===== Step 4: Build wheel =====
echo "===== Building wheel package ====="
mkdir -p "${WHEEL_DIR}"
pushd "${STAGING_DIR}" >/dev/null
python3 -m pip install --upgrade pip setuptools wheel
python3 -m pip wheel . -w "${WHEEL_DIR}"
popd >/dev/null

# ===== Step 5: (Linux only) auditwheel repair =====
if [[ "$(uname -s)" == "Linux" && "${SKIP_AUDITWHEEL:-0}" != "1" ]]; then
    echo "===== Repairing Linux wheel (auditwheel) ====="
    python3 -m pip install auditwheel patchelf
    for whl in "${WHEEL_DIR}"/*.whl; do
        auditwheel repair "$whl" -w "${WHEEL_DIR}_repaired"
    done
    rm -f "${WHEEL_DIR}"/*.whl
    mv "${WHEEL_DIR}_repaired"/*.whl "${WHEEL_DIR}/"
    rm -rf "${WHEEL_DIR}_repaired"
fi

# ===== Step 6: cmake install — only .whl + docs to install/wrapper_python/ =====
echo "===== Installing wheel + docs to install/wrapper_python/ ====="
pushd "${WRAPPER_BUILD_DIR}" >/dev/null
cmake --build . --target install
popd >/dev/null

# ===== Step 7: Pack to dist/ (same pattern as Java/Node.js wrappers) =====
echo "===== Packing to dist/ ====="
PACK_BUILD_DIR="${DIR}/pack"
rm -rf "${PACK_BUILD_DIR}"
mkdir -p "${PACK_BUILD_DIR}"
pushd "${PACK_BUILD_DIR}" >/dev/null
cmake "${PROJECT_ROOT}/pack" -G "Unix Makefiles" \
    -DTARGET_PLATFORM:STRING=all \
    -DPACKAGE_NAME:STRING=bqlog-python-wrapper
cmake --build . --target package
popd >/dev/null

echo "===== Done ====="
echo "Dist packages are in: ${PROJECT_ROOT}/dist"
ls -la "${PROJECT_ROOT}/dist/" 2>/dev/null || true
