@echo off
chcp 65001 >nul
title 地球Online - 一键打包发布

echo ========================================
echo   地球Online - 一键打包发布工具
echo ========================================
echo.

set "PROJECT_DIR=%~dp0"
if "%PROJECT_DIR%"=="" (
    echo [错误] 无法获取脚本所在目录
    pause
    exit /b 1
)

set "UPROJECT=%PROJECT_DIR%EarthOnline.uproject"
if not exist "%UPROJECT%" (
    echo [错误] 找不到项目文件: %UPROJECT%
    pause
    exit /b 1
)

set "OUTPUT_DIR=%PROJECT_DIR%Release"
set "PKG_DIR=%PROJECT_DIR%Packaged\WindowsClient"

:: 安全校验：确保目录路径非空且在项目目录内
if "%OUTPUT_DIR%"=="" (
    echo [错误] 输出目录路径为空，终止操作
    pause
    exit /b 1
)
if "%PKG_DIR%"=="" (
    echo [错误] 打包目录路径为空，终止操作
    pause
    exit /b 1
)

echo 此脚本将自动完成：
echo   1. 编译C++项目
echo   2. 打包Windows客户端
echo   3. 生成玩家安装包
echo.
echo 项目: %UPROJECT%
echo 输出: %OUTPUT_DIR%
echo.

:: 查找UE引擎
set "UE_DIR="
if exist "C:\Program Files\Epic Games\UE_5.8" set "UE_DIR=C:\Program Files\Epic Games\UE_5.8"
if exist "D:\EpicGames\UE_5.8" set "UE_DIR=D:\EpicGames\UE_5.8"
if exist "E:\EpicGames\UE_5.8" set "UE_DIR=E:\EpicGames\UE_5.8"
if exist "F:\EpicGames\UE_5.8" set "UE_DIR=F:\EpicGames\UE_5.8"

if "%UE_DIR%"=="" (
    echo [错误] 未找到UE5.8引擎
    echo 请先通过Epic Games Launcher安装UE 5.8.x
    pause
    exit /b 1
)

echo [信息] 引擎: %UE_DIR%
echo.

:: 步骤1: 编译
echo [1/4] 编译C++项目...
set "UBT=%UE_DIR%\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe"
if not exist "%UBT%" set "UBT=%UE_DIR%\Engine\Binaries\DotNET\UnrealBuildTool.exe"

"%UBT%" EarthOnlineEditor Win64 Development -project="%UPROJECT%" -waitmutex
if %ERRORLEVEL% neq 0 (
    echo [错误] 编译失败
    pause
    exit /b 1
)
echo 编译成功
echo.

:: 步骤2: 打包
echo [2/4] 打包Windows客户端...
set "UAT=%UE_DIR%\Engine\Build\BatchFiles\RunUAT.bat"

:: 安全删除旧打包目录（校验路径在项目目录内）
if exist "%PKG_DIR%" (
    echo %PKG_DIR% | findstr /I /C:"%PROJECT_DIR%Packaged" >nul
    if %ERRORLEVEL% equ 0 (
        rmdir /s /q "%PKG_DIR%"
    ) else (
        echo [错误] 打包目录路径异常，终止: %PKG_DIR%
        pause
        exit /b 1
    )
)

call "%UAT%" BuildCookRun -project="%UPROJECT%" -noP4 -platform=Win64 -clientconfig=Shipping -serverconfig=Shipping -cook -allmaps -build -stage -pak -archive -archivedirectory="%PKG_DIR%" -compressed

if %ERRORLEVEL% neq 0 (
    echo [错误] 打包失败
    pause
    exit /b 1
)
echo 打包成功
echo.

:: 步骤3: 生成玩家安装包
echo [3/4] 生成玩家安装包...

:: 安全删除旧输出目录
if exist "%OUTPUT_DIR%" (
    echo %OUTPUT_DIR% | findstr /I /C:"%PROJECT_DIR%Release" >nul
    if %ERRORLEVEL% equ 0 (
        rmdir /s /q "%OUTPUT_DIR%"
    ) else (
        echo [错误] 输出目录路径异常，终止: %OUTPUT_DIR%
        pause
        exit /b 1
    )
)
mkdir "%OUTPUT_DIR%"

:: 复制打包好的游戏
xcopy "%PKG_DIR%\Windows\*" "%OUTPUT_DIR%\Game\" /E /I /Y >nul

:: 复制安装器
copy "%PROJECT_DIR%地球Online安装器.bat" "%OUTPUT_DIR%\" >nul
copy "%PROJECT_DIR%installer.ps1" "%OUTPUT_DIR%\" >nul
copy "%PROJECT_DIR%一键安装.bat" "%OUTPUT_DIR%\Game\" >nul
copy "%PROJECT_DIR%开始游戏.bat" "%OUTPUT_DIR%\Game\" >nul

:: 生成压缩包
echo 正在压缩...
powershell -Command "Compress-Archive -Path '%OUTPUT_DIR%\*' -DestinationPath '%OUTPUT_DIR%\EarthOnline_v0.1.8_玩家安装包.zip' -Force"

echo 安装包已生成: %OUTPUT_DIR%\EarthOnline_v0.1.8_玩家安装包.zip
echo.

:: 步骤4: 完成
echo [4/4] 完成！
echo.
echo ========================================
echo   打包发布完成
echo ========================================
echo.
echo 玩家安装包: %OUTPUT_DIR%\EarthOnline_v0.1.8_玩家安装包.zip
echo.
echo 上传到Gitee发行版:
echo   1. 打开 https://gitee.com/Doustar/Earth-online/releases
echo   2. 编辑 v0.1.8 发行版
echo   3. 上传 EarthOnline_v0.1.8_玩家安装包.zip
echo   4. 保存
echo.
pause
