@echo off
chcp 65001 >nul
title 地球Online v0.1.8 - 一键安装

echo ========================================
echo   地球Online v0.1.8 内测版 - 安装程序
echo ========================================
echo.
echo 正在安装，请稍候...
echo.

:: 获取脚本所在目录（游戏目录）
set "GAME_DIR=%~dp0"
set "GAME_EXE=%GAME_DIR%EarthOnline.exe"

:: 检查游戏主程序
if not exist "%GAME_EXE%" (
    echo [错误] 找不到 EarthOnline.exe
    echo 请确保将所有文件解压到同一个文件夹后再运行此安装程序
    echo.
    pause
    exit /b 1
)

:: 创建桌面快捷方式
echo [1/3] 创建桌面快捷方式...
powershell -Command "$s=(New-Object -COM WScript.Shell).CreateShortcut([Environment]::GetFolderPath('Desktop')+'\地球Online.lnk');$s.TargetPath='%GAME_EXE%';$s.WorkingDirectory='%GAME_DIR%';$s.Description='地球Online v0.1.8 内测版';$s.Save()"
echo       桌面快捷方式已创建

:: 创建开始菜单快捷方式
echo [2/3] 创建开始菜单快捷方式...
powershell -Command "$s=(New-Object -COM WScript.Shell).CreateShortcut([Environment]::GetFolderPath('Programs')+'\地球Online.lnk');$s.TargetPath='%GAME_EXE%';$s.WorkingDirectory='%GAME_DIR%';$s.Description='地球Online v0.1.8 内测版';$s.Save()"
echo       开始菜单快捷方式已创建

:: 检查VC++运行库（UE5需要）
echo [3/3] 检查运行环境...
reg query "HKLM\SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64" /v Installed >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo       提示：首次运行可能需要安装VC++运行库
    echo       如果游戏无法启动，请安装目录中的 vcredist_x64.exe
) else (
    echo       运行环境检查通过
)

echo.
echo ========================================
echo   安装完成！
echo ========================================
echo.
echo 游戏位置: %GAME_DIR%
echo.
echo 可以通过以下方式启动游戏：
echo   1. 双击桌面"地球Online"图标
echo   2. 开始菜单搜索"地球Online"
echo   3. 直接运行本目录下的 EarthOnline.exe
echo.
echo 首次启动需要输入内测密钥激活
echo.
set /p start=是否现在启动游戏？(Y/N): 
if /i "%start%"=="Y" (
    start "" "%GAME_EXE%"
)
exit /b 0
