@echo off
REM =============================================================================
REM CI-ONLY SCRIPT
REM This script is designed to run in the GitHub Actions CI pipeline.
REM It assumes all native prebuilt binaries (.node files) have already been
REM built and staged by earlier CI jobs (nodejs_build_* and
REM nodejs_collect_and_stage_prebuilds). It only packages them into the
REM npm tarball — it does NOT compile any native code itself.
REM =============================================================================
SETLOCAL ENABLEEXTENSIONS

if exist "..\..\..\artifacts" rmdir /s /q "..\..\..\artifacts"
if exist "..\..\..\install" rmdir /s /q "..\..\..\install"

md makeProj
cd makeProj

cmake ..\..\..\..\wrapper\typescript -G "Unix Makefiles" || exit /b 1
cmake --build . --target install || exit /b 1


cd ..

rd /s/q pack
md pack
cd pack

cmake ../../../../pack %GEN_PLATFORM_ARG% -DTARGET_PLATFORM:STRING=all -DPACKAGE_NAME:STRING=bqlog-nodejs-npm || exit /b 1
cmake --build . --target package || exit /b 1
cd ..
