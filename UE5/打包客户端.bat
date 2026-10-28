@echo off
chcp 65001 >nul
title 地球Online - 打包客户端

echo ========================================
echo   地球Online - 客户端打包
echo ========================================
echo.

set "PROJECT_DIR=%~dp0"
set "UPROJECT=%PROJECT_DIR%EarthOnline.uproject"
set "OUTPUT_DIR=%PROJECT_DIR%Packaged\WindowsClient"

:: 查找UE引擎
set "UE_DIR="
if exist "%PROJECT_DIR%UE_5.8" set "UE_DIR=%PROJECT_DIR%UE_5.8"
if exist "C:\Program Files\Epic Games\UE_5.8" set "UE_DIR=C:\Program Files\Epic Games\UE_5.8"
if exist "D:\EpicGames\UE_5.8" set "UE_DIR=D:\EpicGames\UE_5.8"
if exist "E:\EpicGames\UE_5.8" set "UE_DIR=E:\EpicGames\UE_5.8"
if exist "F:\EpicGames\UE_5.8" set "UE_DIR=F:\EpicGames\UE_5.8"

if "%UE_DIR%"=="" (
    echo [错误] 未找到UE5.8引擎
    pause
    exit /b 1
)

set "UAT=%UE_DIR%\Engine\Build\BatchFiles\RunUAT.bat"

echo [信息] 项目: %UPROJECT%
echo [信息] 输出: %OUTPUT_DIR%
echo.
echo 正在打包Windows客户端（Development配置）...
echo 这可能需要10-30分钟，请耐心等待...
echo.

call "%UAT%" BuildCookRun -project="%UPROJECT%" -noP4 -platform=Win64 -clientconfig=Development -serverconfig=Development -cook -allmaps -build -stage -pak -skipiostore -archive -archivedirectory="%OUTPUT_DIR%" -compressed

if %ERRORLEVEL% neq 0 (
    echo.
    echo [错误] 打包失败
    pause
    exit /b 1
)

echo.
echo ========================================
echo   打包成功！
echo ========================================
echo.
echo 客户端位置: %OUTPUT_DIR%\Windows\EarthOnline.exe
echo.
echo 分发时将整个 Windows 文件夹压缩即可
echo.
pause
