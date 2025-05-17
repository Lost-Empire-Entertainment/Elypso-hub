@echo off

set "PROJECT_ROOT=%~dp0"

set CLEAN_ARG=
if "%1" == "clean" set CLEAN_ARG=clean

echo =====================================
echo [INFO] Copying binaries...
echo =====================================
echo.

cd "%PROJECT_ROOT%\_external_shared"
cmd /c "copy_windows.bat"
if errorlevel 1 (
    echo [ERROR] Failed to copy binaries.
    pause
    exit /b 1
)

echo =====================================
echo [INFO] Building Elypso hub in Release mode...
echo =====================================
echo.

cd "%PROJECT_ROOT%"
cmd /c "build_windows_release.bat" %CLEAN_ARG%
if errorlevel 1 (
    echo [ERROR] Release build failed.
    pause
    exit /b 1
)

echo.
echo =====================================
echo [INFO] Building Elypso hub in Debug mode...
echo =====================================
echo.

cmd /c "build_windows_debug.bat" %CLEAN_ARG%
if errorlevel 1 (
    echo [ERROR] Debug build failed.
    pause
    exit /b 1
)

echo.
echo =====================================
echo [SUCCESS] Finished building and installing Elypso hub!
echo =====================================
echo.

pause
exit /b 0
