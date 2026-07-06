@echo off

:: -----------------------------
:: Build script for Windows
:: Supports:
::   - Build type: Debug / Release
::   - Runtime linkage: static / dynamic
:: Uses Conan + CMake toolchain
:: -----------------------------

setlocal

set "green=[32m"
set "blue=[36m"
set "yellow=[33m"
set "red=[31m"
set "white=[0m"

set BUILD_TYPE=Debug
set RUNTIME_LINK=dynamic

:: Handle -h or --help
if "%~1"=="-h" goto :help
if "%~1"=="--help" goto :help

:: Parse input arguments
if not "%~1"=="" set BUILD_TYPE=%~1
if not "%~2"=="" set RUNTIME_LINK=%~2

:: --- Validate inputs ---
if  "%BUILD_TYPE%" neq "Debug" if "%BUILD_TYPE%" neq "Release" (
    echo %red%[ERROR] Invalid build type. Use Debug or Release.%white%
    exit /b 1
)
if  "%RUNTIME_LINK%" neq "static" if  "%RUNTIME_LINK%" neq "dynamic" (
    echo %red%[ERROR] Invalid runtime linkage. Use static or dynamic.%white%
    exit /b 1
)

echo %blue%[INFO] Selected build type: (%BUILD_TYPE%)%white%
echo %blue%[INFO] Selected runtime linkage: (%RUNTIME_LINK%)%white%

echo %blue%[INFO] Checking Python installation%white%
python --version >nul 2>nul
if %errorlevel% neq 0 (
    echo %red%[ERROR] Python not found. Please install it from https://www.python.org/downloads/%white%
    echo %red%[ERROR] Then close your command line, re-open it and relaunch the script.%white%
    exit /b 1
) else (
    echo %green%[OK] Python already installed.%white%
)

echo %blue%[INFO] Checking pip installation%white%
python -m pip --version >nul 2>nul
if %errorlevel% neq 0 (
    echo %yellow%[WARN] pip not found. Installing...%white%
    python -m ensurepip --upgrade
    python -m pip install --upgrade pip
) else (
    echo %green%[OK] pip already installed.%white%
)

echo %blue%[INFO] Checking CMake installation%white%
where cmake >nul 2>nul
if %errorlevel% neq 0 (
    echo %red%[ERROR] CMake not found. Please install it from https://cmake.org/download/%white%
    echo %red%[ERROR] Then close your command line, re-open it and relaunch the script.%white%
    exit /b 1
) else (
    echo %green%[OK] CMake already installed.%white%
)

echo %blue%[INFO] Checking Conan installation%white%
call "./scripts/conan_installer.bat"

call "./scripts/clean_windows.bat"

echo %blue%[INFO] Configuring Conan profile%white%
conan profile detect --force

if %RUNTIME_LINK%==static (
    set IS_SHARED=False
) else (
    set IS_SHARED=True
)
conan install . --settings=build_type=%BUILD_TYPE% -s compiler.runtime=%RUNTIME_LINK% --build=missing -o isShared=%IS_SHARED%
if %errorlevel% neq 0 (
    echo %red%[ERROR] Conan install failed.%white%
    exit /b 1
)


echo %blue%[INFO] Configuring project with CMake (%BUILD_TYPE%, runtime=%RUNTIME_LINK%)%white%
cd build
cmake .. -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -DRUNTIME_LINK=%RUNTIME_LINK% -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=bin -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
if %errorlevel% neq 0 (
    echo %red%[ERROR] CMake configuration failed.%white%
    exit /b 1
)

echo %blue%[INFO] Building project (%BUILD_TYPE%)%white%
cmake --build . --config %BUILD_TYPE%
if %errorlevel% neq 0 (
    echo %red%[ERROR] Build failed.%white%
    exit /b 1
)

:: --- Install C++ runtime DLLs only if dynamic runtime selected ---
if "%RUNTIME_LINK%" == "dynamic" (
    echo %blue%[INFO] Installing C++ runtime DLLs%white%
    cmake --install . --config %BUILD_TYPE% --prefix "%cd%/.."
    if %errorlevel% neq 0 (
        echo %red%[ERROR] Failed to copy C++ runtime DLLs.%white%
        exit /b 1
    )
)  else (
    echo %yellow%[INFO] Static runtime selected - skipping C++ runtime DLLs installation step.%white%
)  

echo %green%[SUCCESS] Build completed successfully! (%BUILD_TYPE%, %RUNTIME_LINK%)%white%
goto :eof

:help
echo %green%Usage: build_windows.bat [BUILD_TYPE] [RUNTIME_LINK]%white%
echo.
echo BUILD_TYPE:
echo   Debug     - Build with debug symbols.
echo             - Dynamic debug builds will requires Visual Studio to run on target machines (debug runtime c++ libs are non-redistributable)
echo             - Static debug builds will produce larger executables but they will run on all machines.
echo   Release   - Optimized build for distribution.
echo             - Recommended for sharing or packaging.
echo.
echo RUNTIME_LINK:
echo   static    - Links C++ runtime statically (/MT or /MTd)
echo             - All conan dependencies will also be built with static runtime.
echo             - Fully portable binary in Debug and Release mode (no MSVC redistributable needed)
echo             - Larger executable size.
echo   dynamic   - Links C++ runtime dynamically (/MD or /MDd)
echo             - All conan dependencies will also be built with dynamic runtime.
echo             - Smaller executable size with shared runtime DLLs build alongside it.
echo             - Release build will work on all machines.
echo             - Debug builds won't run if the target system don't have visual studio and c++ installed.
echo.
echo Examples:
echo   build_windows.bat Debug dynamic
echo   build_windows.bat Release static
echo.
echo Typical usage:
echo   - Debug dynamic: For development and debugging on your own PC
echo   - Release dynamic: Fully self-contained for distribution with smaller executable
echo   - Release static: Fully self-contained for distribution with larger executable
echo.
echo %green%By default build_windows.bat uses: Debug dynamic%white%
exit /b 0

endlocal