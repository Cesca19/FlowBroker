@echo off

set "green=[32m"
set "blue=[36m"
set "yellow=[33m"
set "red=[31m"
set "white=[0m"

:: === 1. Set the custom installation folder for Conan ===
set conan_install_dir=.conan_install_dir

:: === 2. Check if the folder exists ===
if not exist %conan_install_dir% (
    echo %yellow%[INFO] Conan folder not found, installing Conan...%white%
    goto :install_conan
) 
echo %blue%[INFO] Conan folder found, checking Conan version ...%white%
    
:: === 3. Specify the folder that contains the Conan executable. ===
set "conan_exe_dir=%cd%\%conan_install_dir%\bin"

:: === 4. Check if conan.exe exists ===
if not exist "%conan_exe_dir%\conan.exe" (
    echo %yellow%[WARN] Conan folder exists but no executable found. Reinstalling Conan...%white%
    goto :install_conan
)

:: === 5. Modify the PATH and PYTHONPATH variables to use Conan ===
set "PATH=%conan_exe_dir%;%PATH%"
set "PYTHONPATH=%conan_install_dir%;%PYTHONPATH%"

echo %blue%[INFO] Checking package integrity%white%
conan --version >nul 2>nul
if %errorlevel% neq 0 (
echo %yellow%[WARN] Conan not found or broken in "%conan_install_dir%". Try to reinstall.%white%
    goto :install_conan
)

:: === 6. Check the current version of Conan installed ===
set "current_version="
for /f "tokens=2 delims= " %%v in ('pip show conan ^| findstr Version') do (
    set "current_version=%%v"
)

if "%current_version%"=="" (
    echo %yellow%[WARN] Conan not found or broken in "%conan_install_dir%". Try to reinstall.%white%
    goto :install_conan
)

echo %blue%[INFO] Conan currently installed version: %current_version%%white%

:: === 7. Retrieve the latest version available on PyPI ===
set "versions="
for /f "tokens=3*" %%a in ('python -m pip index versions conan ^| findstr "Available versions"') do (
    set "versions=%%a %%b"
)
for /f "tokens=1 delims=," %%v in ("%versions%") do (
    set "latest_version=%%v"
)
echo %blue%[INFO] Latest available Conan version: %latest_version%%white%

:: === 8. Compare the two versions ===
if "%current_version%" == "%latest_version%" (
    echo %green%[OK] Conan is up-to-date%white%
    goto :done
) else (
    echo %yellow%[INFO] Conan version mismatch. Updating...%white%
    goto :install_conan
)

:: === 9. Installation / reinstallation step ===
:install_conan
if exist "%conan_install_dir%" (
    echo %blue%[INFO] Cleaning old Conan folder...%white%
    rmdir /s /q "%conan_install_dir%"
)
mkdir "%conan_install_dir%"
python -m pip install conan --target "%conan_install_dir%" --upgrade
if %errorlevel% neq 0 (
    echo %red%[ERROR] Conan installation failed. Please check manually.%white%
    exit /b 1
)
echo %green%[OK] Conan successfully installed.%white%

:: === 10. Update PATH and PYTHONPATH after installation ===
set "conan_exe_dir=%cd%\%conan_install_dir%\bin"
set "PATH=%conan_exe_dir%;%PATH%"
set "PYTHONPATH=%conan_install_dir%;%PYTHONPATH%"

:done
echo %green%[DONE] Conan is ready to use.%white%
conan --version
