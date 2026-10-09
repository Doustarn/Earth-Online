@echo off
chcp 65001 >nul
title 地球Online - 内测服启动器

:MENU
cls
echo ========================================
echo   地球Online 内测服启动器 v0.1.0
echo ========================================
echo.
echo   [1] 启动游戏客户端
echo   [2] 创建监听服务器（本机做主机）
echo   [3] 加入指定服务器
echo   [4] 启动专用服务器（无画面）
echo   [5] 打开项目编辑器
echo   [6] 编译C++项目
echo   [7] 打包客户端
echo   [0] 退出
echo.
set /p choice=请选择操作: 

if "%choice%"=="1" goto CLIENT
if "%choice%"=="2" goto HOST
if "%choice%"=="3" goto JOIN
if "%choice%"=="4" goto DEDICATED
if "%choice%"=="5" goto EDITOR
if "%choice%"=="6" goto COMPILE
if "%choice%"=="7" goto PACKAGE
if "%choice%"=="0" goto EXIT
goto MENU

:CLIENT
cls
echo 启动游戏客户端...
cd /d "%~dp0"
if exist "Packaged\WindowsClient\Windows\EarthOnline.exe" (
    start "" "Packaged\WindowsClient\Windows\EarthOnline.exe"
) else (
    echo [提示] 未找到打包后的客户端，正在用编辑器模式启动...
    call "启动编辑器.bat"
)
goto MENU

:HOST
cls
set /p port=服务器端口（默认7777）: 
if "%port%"=="" set port=7777
echo.
echo 创建监听服务器，端口: %port%
echo 其他玩家可以通过 你的IP:%port% 加入
echo.
cd /d "%~dp0"
if exist "Packaged\WindowsClient\Windows\EarthOnline.exe" (
    start "" "Packaged\WindowsClient\Windows\EarthOnline.exe" TestMap?listen?port=%port%
) else (
    echo [提示] 请先打包客户端，或在编辑器中使用Play模式
)
pause
goto MENU

:JOIN
cls
set /p ip=服务器IP地址: 
set /p port=端口（默认7777）: 
if "%port%"=="" set port=7777
echo.
echo 正在连接 %ip%:%port% ...
cd /d "%~dp0"
if exist "Packaged\WindowsClient\Windows\EarthOnline.exe" (
    start "" "Packaged\WindowsClient\Windows\EarthOnline.exe" %ip%:%port%
) else (
    echo [提示] 请先打包客户端
)
pause
goto MENU

:DEDICATED
cls
set /p port=服务器端口（默认7777）: 
if "%port%"=="" set port=7777
echo.
echo 启动专用服务器，端口: %port%
echo 专用服务器无画面，仅运行游戏逻辑
echo.
cd /d "%~dp0"
if exist "Packaged\WindowsClient\Windows\EarthOnlineServer.exe" (
    start "" "Packaged\WindowsClient\Windows\EarthOnlineServer.exe" TestMap?port=%port%
) else (
    echo [提示] 未找到专用服务器，需要在打包时勾选"包含服务器"
)
pause
goto MENU

:EDITOR
call "启动编辑器.bat"
goto MENU

:COMPILE
call "编译项目.bat"
goto MENU

:PACKAGE
call "打包客户端.bat"
goto MENU

:EXIT
exit /b 0
