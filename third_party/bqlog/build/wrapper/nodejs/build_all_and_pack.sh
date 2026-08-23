#!/usr/bin/env bash
# =============================================================================
# CI-ONLY SCRIPT
# This script is designed to run in the GitHub Actions CI pipeline.
# It assumes all native prebuilt binaries (.node files) have already been
# built and staged by earlier CI jobs (nodejs_build_* and
# nodejs_collect_and_stage_prebuilds). It only packages them into the
# npm tarball — it does NOT compile any native code itself.
# =============================================================================
set -euo pipefail

rm -rf "../../../artifacts"
rm -rf "../../../install"

mkdir -p makeProj
cd makeProj

cmake ../../../../wrapper/typescript -G "Unix Makefiles"
cmake --build . --target install

cd ..

rm -rf pack
mkdir -p pack
cd pack

cmake ../../../../pack -DTARGET_PLATFORM:STRING=all -DPACKAGE_NAME:STRING=bqlog-nodejs-npm
cmake --build . --target package
cd ..