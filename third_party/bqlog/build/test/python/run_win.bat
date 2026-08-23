@echo off
setlocal EnableDelayedExpansion

set "DIR=%~dp0"
set "PROJECT_ROOT=%DIR%..\..\.."
set "BUILD_SCRIPT=%PROJECT_ROOT%\build\wrapper\python\build_all_and_pack.bat"
set "INSTALL_DIR=%PROJECT_ROOT%\install\wrapper_python"
set "TEST_MAIN=%PROJECT_ROOT%\test\python\src\bq\test\test_main.py"

set FAILED=0

for %%B in (Debug Release RelWithDebInfo MinSizeRel) do (
    echo.
    echo ========================================
    echo  Testing build type: %%B
    echo ========================================

    echo ----- Building wheel [%%B] -----
    call "%BUILD_SCRIPT%" %%B
    if errorlevel 1 (
        echo Build FAILED for %%B
        set FAILED=1
        goto :end
    )

    echo ----- Installing wheel -----
    for %%W in ("%INSTALL_DIR%\*.whl") do (
        py -m pip install --force-reinstall "%%W"
    )

    echo ----- Running tests [%%B] -----
    py "%TEST_MAIN%"
    if errorlevel 1 (
        echo Tests FAILED for %%B!
        set FAILED=1
        goto :end
    )
    echo Tests PASSED for %%B
)

:end
echo.
if %FAILED%==0 (
    echo All Python tests PASSED!
    exit /b 0
) else (
    echo Some Python tests FAILED!
    exit /b 1
)
