@echo off
chcp 65001 >nul
title 地球Online - UE5 启动器

echo ========================================
echo   地球Online (Earth Online) - UE5 启动器
echo ========================================
echo.

:: 设置项目路径
set "PROJECT_DIR=%~dp0"
set "UPROJECT=%PROJECT_DIR%EarthOnline.uproject"

:: 检查项目文件
if not exist "%UPROJECT%" (
    echo [错误] 找不到项目文件: %UPROJECT%
    pause
    exit /b 1
)

:: 尝试查找已安装的UE5引擎
set "EDITOR_EXE="
if exist "C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe" (
    set "EDITOR_EXE=C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe"
)
if exist "D:\EpicGames\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe" (
    set "EDITOR_EXE=D:\EpicGames\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe"
)
if exist "E:\EpicGames\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe" (
    set "EDITOR_EXE=E:\EpicGames\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe"
)

echo [信息] 项目文件: %UPROJECT%
echo.

if defined EDITOR_EXE (
    echo [信息] 找到引擎: %EDITOR_EXE%
    echo.
    echo [启动] 正在打开 Unreal Editor...
    start "" "%EDITOR_EXE%" "%UPROJECT%"
) else (
    echo [提示] 未找到UE5.8引擎，通过项目关联打开...
    echo.
    echo 如果提示选择引擎版本，请选择已安装的 UE 5.8.x
    echo.
    start "" "%UPROJECT%"
)

echo [完成] 编辑器已启动，首次打开需要编译 Shader，请耐心等待。
echo.
timeout /t 3 >nul
exit /b 0
