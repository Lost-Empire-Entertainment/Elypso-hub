@echo off

echo Starting to copy Elypso hub binaries...
echo.

set "PROJECT_ROOT=%~dp0"

:: Origin paths

set "GLFW_RELEASE=%PROJECT_ROOT%\GLFW\release\glfw3.dll"
set "GLFW_DEBUG=%PROJECT_ROOT%\GLFW\debug\glfw3.dll"

:: Target paths

set "RELEASE_DLLS=%PROJECT_ROOT%..\files\external dlls\release"
set "DEBUG_DLLS=%PROJECT_ROOT%..\files\external dlls\debug"

:: Copy release dlls

copy /Y "%GLFW_RELEASE%" "%RELEASE_DLLS%\"

:: Copy debug dlls

copy /Y "%GLFW_DEBUG%" "%DEBUG_DLLS%\"

echo.
echo Finished copying Elypso hub DLLs!

pause
exit /b 0