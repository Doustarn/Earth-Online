@echo off
setlocal
title Earth Online Launcher

rem ============================================================
rem  Earth Online one-click launcher
rem  Auto-detect drive letter & folder via %~dp0
rem  Works no matter which drive the project is on.
rem ============================================================

set "ROOT=%~dp0"
set "EXEDIR=%ROOT%UE5\Packaged\WindowsClient\EarthOnline\Binaries\Win64"
set "EXE=%EXEDIR%\EarthOnline.exe"

if not exist "%EXE%" (
    echo [ERROR] Game client not found:
    echo   %EXE%
    echo.
    echo Make sure this .bat is placed in the Earth Online project root
    echo (the folder that directly contains the "UE5" subfolder).
    echo.
    pause
    exit /b 1
)

cd /d "%EXEDIR%"
echo Starting Earth Online ...
start "" "EarthOnline.exe"
endlocal
exit /b 0
