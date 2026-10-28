@echo off
chcp 65001 >nul
title 地球Online - 编译C++项目

echo ========================================
echo   地球Online - C++项目编译
echo ========================================
echo.

:: 设置路径
set "PROJECT_DIR=%~dp0"
set "UPROJECT=%PROJECT_DIR%EarthOnline.uproject"

:: 查找UE引擎
set "UE_DIR="
if exist "%PROJECT_DIR%UE_5.8" set "UE_DIR=%PROJECT_DIR%UE_5.8"
if exist "C:\Program Files\Epic Games\UE_5.8" set "UE_DIR=C:\Program Files\Epic Games\UE_5.8"
if exist "D:\EpicGames\UE_5.8" set "UE_DIR=D:\EpicGames\UE_5.8"
if exist "E:\EpicGames\UE_5.8" set "UE_DIR=E:\EpicGames\UE_5.8"
if exist "F:\EpicGames\UE_5.8" set "UE_DIR=F:\EpicGames\UE_5.8"

if "%UE_DIR%"=="" (
    echo [错误] 未找到UE5.8引擎
    echo 请通过Epic Games Launcher安装UE 5.8.x
    pause
    exit /b 1
)

set "UBT=%UE_DIR%\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe"
set "UBT_ALT=%UE_DIR%\Engine\Binaries\DotNET\UnrealBuildTool.exe"

echo [信息] 项目: %UPROJECT%
echo [信息] 引擎: %UE_DIR%
echo.

:: 生成项目文件
echo [1/2] 生成Visual Studio项目文件...
if exist "%UBT%" (
    "%UBT%" -projectfiles -project="%UPROJECT%" -game -rocket -progress
) else (
    "%UBT_ALT%" -projectfiles -project="%UPROJECT%" -game -rocket -progress
)

if %ERRORLEVEL% neq 0 (
    echo [错误] 项目文件生成失败
    pause
    exit /b 1
)

echo.
echo [2/2] 编译Development Editor...
if exist "%UBT%" (
    "%UBT%" EarthOnlineEditor Win64 Development -project="%UPROJECT%" -waitmutex
) else (
    "%UBT_ALT%" EarthOnlineEditor Win64 Development -project="%UPROJECT%" -waitmutex
)

if %ERRORLEVEL% neq 0 (
    echo [错误] 编译失败，请检查错误信息
    pause
    exit /b 1
)

echo.
echo ========================================
echo   编译成功！
echo ========================================
echo.
echo 可以运行 启动编辑器.bat 打开项目
echo.
pause
