@echo off

set "PROJECT_ROOT=%~dp0"
cd "%PROJECT_ROOT%"

echo =====================================
echo [INFO] Building hub in Release mode...
echo =====================================
echo.

cmd /c "build_windows_release.bat"
if errorlevel 1 (
    echo [ERROR] Release build failed.
    pause
    exit /b 1
)

echo.
echo =====================================
echo [INFO] Building hub in Debug mode...
echo =====================================
echo.

cmd /c "build_windows_debug.bat"
if errorlevel 1 (
    echo [ERROR] Debug build failed.
    pause
    exit /b 1
)

echo.
echo =====================================
echo [SUCCESS] Finished building and installing hub!
echo =====================================
echo.

pause
exit /b 0
