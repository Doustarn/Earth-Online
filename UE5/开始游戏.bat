@echo off
chcp 65001 >nul
title 地球Online v0.1.8

cd /d "%~dp0"

if exist "EarthOnline.exe" (
    start "" EarthOnline.exe
    exit
)

if exist "Windows\EarthOnline.exe" (
    start "" "Windows\EarthOnline.exe"
    exit
)

echo ========================================
echo   地球Online v0.1.8 内测版
echo ========================================
echo.
echo [错误] 找不到游戏主程序
echo.
echo 请确认你已经完整解压了游戏压缩包
echo 本启动器应和 EarthOnline.exe 在同一文件夹
echo.
pause
