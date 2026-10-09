@echo off
chcp 65001 >nul
title 地球Online - 一键编译打包

echo ========================================
echo   地球Online - 一键编译打包客户端
echo ========================================
echo.

echo [步骤1/2] 编译C++项目...
call "%~dp0编译项目.bat"
if %ERRORLEVEL% neq 0 (
    echo [错误] 编译失败，终止打包
    pause
    exit /b 1
)

echo.
echo [步骤2/2] 打包客户端...
call "%~dp0打包客户端.bat"

echo.
echo ========================================
echo   全部完成！
echo ========================================
pause
