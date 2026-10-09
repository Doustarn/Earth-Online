@echo off
chcp 65001 >nul
title 地球Online - UE5 启动器

echo ========================================
echo   地球Online (Earth Online) - UE5 启动器
echo ========================================
echo.

:: 设置路径
set "PROJECT_DIR=%~dp0"
set "UPROJECT=%PROJECT_DIR%EarthOnline.uproject"
set "ENGINE_DIR=%PROJECT_DIR%UE_5.8"
set "EDITOR_EXE=%ENGINE_DIR%\Engine\Binaries\Win64\UnrealEditor.exe"

:: 检查项目文件
if not exist "%UPROJECT%" (
    echo [错误] 找不到项目文件: %UPROJECT%
    pause
    exit /b 1
)

:: 检查引擎是否存在
if not exist "%EDITOR_EXE%" (
    echo [提示] UE 5.8 引擎尚未下载完成或路径不正确
    echo.
    echo 预期路径: %EDITOR_EXE%
    echo.
    echo 请确认 UE_5.8 目录已完整下载，然后重新运行此脚本。
    echo.
    echo 如果引擎安装在其他位置，可以手动用 UnrealEditor.exe 打开:
    echo %UPROJECT%
    echo.
    pause
    exit /b 1
)

echo [信息] 项目文件: %UPROJECT%
echo [信息] 引擎路径: %EDITOR_EXE%
echo.
echo [启动] 正在打开 Unreal Editor...
echo.

:: 启动编辑器
start "" "%EDITOR_EXE%" "%UPROJECT%"

echo [完成] 编辑器已启动，首次打开需要编译 Shader，请耐心等待。
echo.
timeout /t 3 >nul
exit /b 0
