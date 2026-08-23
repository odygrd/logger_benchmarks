@echo off
SETLOCAL ENABLEEXTENSIONS


md makeProj
cd makeProj

cmake ..\..\..\..\wrapper\java -G "Unix Makefiles"
make

cd ..

REM Inject copyright header and convert to LF
set "TARGET=%~dp0..\..\..\src\bq_log\api\bq_impl_log_invoker.h"
for %%F in ("%TARGET%") do set "TARGET=%%~fF"

if not exist "%TARGET%" (
    echo File not found, skipping copyright injection: %TARGET%
    goto :DONE
)

powershell -NoProfile -ExecutionPolicy Bypass -Command "$f='%TARGET:\=/%'; $cr='/* Copyright (C) 2025 Tencent.' + [char]10 + ' * BQLOG is licensed under the Apache License, Version 2.0.' + [char]10 + ' * You may obtain a copy of the License at' + [char]10 + ' *' + [char]10 + ' *     http://www.apache.org/licenses/LICENSE-2.0' + [char]10 + ' *' + [char]10 + ' * Unless required by applicable law or agreed to in writing, software' + [char]10 + ' * distributed under the License is distributed on an \"AS IS\" BASIS,' + [char]10 + ' * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.' + [char]10 + ' */' + [char]10; $c=[IO.File]::ReadAllText($f); $c=$c -replace \"`r`n\",\"`n\"; [IO.File]::WriteAllText($f, $cr+$c, (New-Object System.Text.UTF8Encoding $false))"
echo Copyright header injected and converted to LF: %TARGET%

:DONE
pause
