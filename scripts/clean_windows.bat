@echo off
setlocal

set "green=[32m"
set "blue=[36m"
set "yellow=[33m"
set "red=[31m"
set "white=[0m"

set binary_dir=./bin
set build_dir=./build

if exist "%binary_dir%" (
    echo %blue%[INFO] Cleaning binary folder...%white%
    rmdir /s /q "%binary_dir%"
)

if %errorlevel% neq 0 (
    echo %red%[ERROR] Cleaning failed .%white%
    exit /b 1
)

if exist "%build_dir%" (
    echo %blue%[INFO] Cleaning build folder...%white%
    rmdir /s /q "%build_dir%"
)

if %errorlevel% neq 0 (
    echo %red%[ERROR] Cleaning failed .%white%
    exit /b 1
)

echo %green%[OK] Cleaning completed successfully%white%

endlocal