@echo off
setlocal EnableDelayedExpansion

set "DIR=%~dp0"
set "PROJECT_ROOT=%DIR%..\..\.."
set "BUILD_LIB_DIR=%PROJECT_ROOT%\build\lib\win64"
set "TEST_SRC_DIR=%PROJECT_ROOT%\test\typescript"
set "ARTIFACTS_DIR=%PROJECT_ROOT%\artifacts"

set "CONFIG=%~1"
if "%CONFIG%"=="" set "CONFIG=RelWithDebInfo"

echo ===== Installing Node API Headers (Project Root) =====
pushd "%PROJECT_ROOT%"
call npm install node-api-headers --no-save
REM Windows needs node.lib for linking, which node-gyp install provides
call npx node-gyp install
popd

echo ===== Building BqLog Dynamic Library (Windows) =====
pushd "%BUILD_LIB_DIR%"
call dont_execute_this.bat build native msvc OFF ON OFF dynamic_lib
popd

echo ===== Installing TypeScript Test Dependencies =====
pushd "%TEST_SRC_DIR%"
call npm install
popd

echo ===== Building TypeScript Wrapper (for tests) =====
pushd "%PROJECT_ROOT%\wrapper\typescript"
call npm install
call npm run build
popd

echo ===== Finding and Staging .node Binary =====
REM Find .node file
set "NODE_LIB_DIR=%ARTIFACTS_DIR%\dynamic_lib\lib\%CONFIG%"
if not exist "%NODE_LIB_DIR%" (
     if exist "%ARTIFACTS_DIR%\dynamic_lib\lib\Release" (
        set "NODE_LIB_DIR=%ARTIFACTS_DIR%\dynamic_lib\lib\Release"
    ) else if exist "%ARTIFACTS_DIR%\dynamic_lib\lib\Debug" (
        set "NODE_LIB_DIR=%ARTIFACTS_DIR%\dynamic_lib\lib\Debug"
    )
)

set "NODE_LIB_PATH="
for /r "%NODE_LIB_DIR%" %%f in (*.node) do (
    set "NODE_LIB_PATH=%%f"
    goto :found_node
)

:found_node
if "%NODE_LIB_PATH%"=="" (
    echo Warning: .node file not found in %NODE_LIB_DIR%
) else (
    echo Found Node Lib: %NODE_LIB_PATH%
    set "DEST_DIR=%PROJECT_ROOT%\wrapper\typescript\dist"
    if not exist "!DEST_DIR!" mkdir "!DEST_DIR!"
    echo Copying to !DEST_DIR! ...
    copy /Y "%NODE_LIB_PATH%" "!DEST_DIR!\BqLog.node" >nul
)

echo ===== Packing @pippocao/bqlog into tgz =====
pushd "%PROJECT_ROOT%\wrapper\typescript"
call npm pack --pack-destination "%TEST_SRC_DIR%"
popd

echo ===== Installing @pippocao/bqlog tgz into test project =====
pushd "%TEST_SRC_DIR%"
set "BQLOG_TGZ="
for %%f in (pippocao-bqlog-*.tgz) do (
    set "BQLOG_TGZ=%%f"
    goto :found_tgz
)
:found_tgz
if "%BQLOG_TGZ%"=="" (
    echo Error: tgz file not found!
    exit /b 1
)
echo Installing %BQLOG_TGZ%...
call npm install ".\%BQLOG_TGZ%" --no-save
del /q "%BQLOG_TGZ%"
popd

echo ===== Running TypeScript Tests =====
pushd "%TEST_SRC_DIR%"
call npm test
popd
