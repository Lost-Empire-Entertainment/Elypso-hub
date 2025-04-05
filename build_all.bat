@echo off
setlocal

set "root_path=%~dp0"
echo Root path: %root_path%

set "bat_path=%root_path%build_windows.bat"
echo Bat path: %bat_path%

if not exist "%bat_path%" (
	echo Failed to find bat file at "%bat_path%"!
	pause
	exit /b 1
)

echo.
echo ==============================
echo Building Elypso hub in debug mode...
echo ==============================
echo.

call "%bat_path%" cmake debug

echo.
echo ==============================
echo Building Elypso hub in release mode...
echo ==============================
echo.

call "%bat_path%" cmake release

echo.
echo ==============================
echo Finished building Elypso hub!
echo ==============================
echo.

pause
endlocal
exit /b 0