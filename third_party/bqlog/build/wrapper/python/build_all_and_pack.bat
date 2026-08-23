@echo off
setlocal EnableDelayedExpansion

rem Build BqLog Python Wrapper and pack into wheel (Windows)
rem Step 1: Build native .pyd with Python support
rem Step 2: Run wrapper/python/CMakeLists.txt (version sync + artifacts staging)
rem Step 3: Copy native lib into staged artifacts
rem Step 4: Build wheel (output to artifacts/wrapper/python/wheel/)
rem Step 5: cmake install — copies only .whl + docs to install/wrapper_python/

set "DIR=%~dp0"
set "PROJECT_ROOT=%DIR%..\..\.."
set "BUILD_LIB_DIR=%PROJECT_ROOT%\build\lib\win64"
set "ARTIFACTS_DIR=%PROJECT_ROOT%\artifacts"
set "WRAPPER_DIR=%PROJECT_ROOT%\wrapper\python"
set "WHEEL_DIR=%ARTIFACTS_DIR%\wrapper\python\wheel"

set "CONFIG=%~1"
if "%CONFIG%"=="" set "CONFIG=RelWithDebInfo"

rem Check if first arg is an arch (arm64) or a config
set "ARCH=native"
if /i "%CONFIG%"=="arm64" (
    set "ARCH=arm64"
    set "CONFIG=%~2"
    if "!CONFIG!"=="" set "CONFIG=RelWithDebInfo"
)

rem ===== Step 1: Build native library =====
echo ===== Building BqLog Dynamic Library with Python Support [%CONFIG%] [%ARCH%] =====
pushd "%BUILD_LIB_DIR%"
call dont_execute_this.bat build %ARCH% msvc OFF OFF ON dynamic_lib %CONFIG%
if %ERRORLEVEL% neq 0 (
    popd
    echo ERROR: Native library build failed!
    exit /b 1
)
popd

rem ===== Step 2: Run wrapper CMake (version sync + staging) =====
echo ===== Running wrapper/python CMake (version sync + staging) =====
set "WRAPPER_BUILD_DIR=%DIR%wrapperProj"
if exist "%WRAPPER_BUILD_DIR%" rd /s /q "%WRAPPER_BUILD_DIR%"
md "%WRAPPER_BUILD_DIR%"
pushd "%WRAPPER_BUILD_DIR%"
cmake "%WRAPPER_DIR%" || exit /b 1
cmake --build . || exit /b 1
popd

rem ===== Step 3: Copy native lib into staged artifacts =====
echo ===== Copying native library to wheel source =====
set "STAGING_DIR=%ARTIFACTS_DIR%\wrapper\python"
set "LIB_PATH=%ARTIFACTS_DIR%\dynamic_lib\lib\%CONFIG%"
if not exist "%LIB_PATH%" (
    if exist "%ARTIFACTS_DIR%\dynamic_lib\lib\Release" (
        set "LIB_PATH=%ARTIFACTS_DIR%\dynamic_lib\lib\Release"
    ) else if exist "%ARTIFACTS_DIR%\dynamic_lib\lib\RelWithDebInfo" (
        set "LIB_PATH=%ARTIFACTS_DIR%\dynamic_lib\lib\RelWithDebInfo"
    )
)

for %%f in ("%LIB_PATH%\_bqlog*.pyd") do (
    copy /Y "%%f" "%STAGING_DIR%\src\bq\" >nul
    echo Copied %%~nxf
)

rem ===== Step 3b: Remove .pdb files from staging (keep wheel small) =====
echo ===== Removing .pdb files from staging =====
del /q "%STAGING_DIR%\src\bq\*.pdb" 2>nul

rem ===== Step 4: Build wheel =====
echo ===== Building wheel package =====
if not exist "%WHEEL_DIR%" md "%WHEEL_DIR%"
pushd "%STAGING_DIR%"
py -m pip install --upgrade pip setuptools wheel
py -m pip wheel . -w "%WHEEL_DIR%"
popd

rem ===== Step 5: cmake install — only .whl + docs to install/wrapper_python/ =====
echo ===== Installing wheel + docs to install/wrapper_python/ =====
pushd "%WRAPPER_BUILD_DIR%"
cmake --build . --target install || exit /b 1
popd

rem ===== Step 6: Pack to dist/ (same pattern as Java/Node.js wrappers) =====
echo ===== Packing to dist/ =====
set "PACK_BUILD_DIR=%DIR%pack"
if exist "%PACK_BUILD_DIR%" rd /s /q "%PACK_BUILD_DIR%"
md "%PACK_BUILD_DIR%"
pushd "%PACK_BUILD_DIR%"
cmake "%PROJECT_ROOT%\pack" -DTARGET_PLATFORM:STRING=all -DPACKAGE_NAME:STRING=bqlog-python-wrapper || exit /b 1
cmake --build . --target package || exit /b 1
popd

echo ===== Done =====
echo Dist packages are in: %PROJECT_ROOT%\dist
dir "%PROJECT_ROOT%\dist" 2>nul
