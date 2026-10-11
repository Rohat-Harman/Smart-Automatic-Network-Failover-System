
@echo off
setlocal

cd /d "%~dp0"

call "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x64
if errorlevel 1 exit /b 1

if not exist "build" mkdir "build"

cl /nologo /EHsc /std:c++17 /I include /Fo:build\ src\main.cpp src\NetworkMonitor.cpp src\SocketManager.cpp src\WiFiManager.cpp src\Checksum.cpp src\Logger.cpp /Fe:build\SmartNetworkFailover.exe

if errorlevel 1 (
    echo BUILD FAILED
    exit /b 1
)

echo BUILD SUCCESSFUL
exit /b 0
