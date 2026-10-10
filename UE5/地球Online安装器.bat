@echo off
chcp 65001 >nul
title 地球Online v0.1.8 安装程序
echo ========================================
echo    地球Online v0.1.8 内测版 安装程序
echo ========================================
echo.
echo 本程序将自动完成：下载 - 安装 - 创建快捷方式
echo.
echo 按任意键开始安装...
pause >nul
powershell -NoProfile -ExecutionPolicy Bypass -Command "$content = Get-Content -LiteralPath '%~f0' -Raw -Encoding UTF8; $ps = $content -split '::PS_START::' | Select-Object -Last 1; Invoke-Expression $ps"
exit /b %ERRORLEVEL%
::PS_START::
$ErrorActionPreference = "Stop"
$GameName = "地球Online"
$Version = "0.1.8"
$DefaultInstallDir = "D:\Games\EarthOnline"
Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  地球Online v$Version 内测版 安装程序" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "[步骤 1/5] 选择安装目录" -ForegroundColor Yellow
$installDir = Read-Host "安装路径（默认: $DefaultInstallDir）"
if ([string]::IsNullOrWhiteSpace($installDir)) { $installDir = $DefaultInstallDir }
$installDir = $installDir.Trim('"').TrimEnd('\')
if (-not (Test-Path $installDir)) { New-Item -ItemType Directory -Path $installDir -Force | Out-Null }
Write-Host "  安装目录: $installDir" -ForegroundColor Green
Write-Host ""
Write-Host "[步骤 2/5] 正在下载游戏..." -ForegroundColor Yellow
$tempDir = Join-Path $env:TEMP "EarthOnline_Install_$([System.Guid]::NewGuid().ToString('N').Substring(0,8))"
New-Item -ItemType Directory -Path $tempDir -Force | Out-Null
$zipPath = Join-Path $tempDir "EarthOnline.zip"
try {
    Write-Host "  正在从Gitee下载..." -ForegroundColor Gray
    $ProgressPreference = 'SilentlyContinue'
    $urls = @(
        "https://gitee.com/Doustar/Earth-online/releases/download/v0.1.8/EarthOnline_v0.1.8_Windows.zip",
        "https://gitee.com/Doustar/Earth-online/repository/archive/v0.1.8.zip",
        "https://gitee.com/Doustar/Earth-online/repository/archive/master.zip"
    )
    $downloaded = $false
    foreach ($url in $urls) {
        try {
            Write-Host "  尝试下载..." -ForegroundColor Gray
            Invoke-WebRequest -Uri $url -OutFile $zipPath -TimeoutSec 30 -UseBasicParsing
            if ((Get-Item $zipPath).Length -gt 1000) { $downloaded = $true; break }
        } catch { }
    }
    $ProgressPreference = 'Continue'
    if (-not $downloaded) { throw "所有下载地址均失败" }
    $fileSize = (Get-Item $zipPath).Length / 1MB
    Write-Host "  下载完成: $([math]::Round($fileSize, 2)) MB" -ForegroundColor Green
} catch {
    Write-Host "  [错误] 下载失败: $_" -ForegroundColor Red
    Write-Host "  请手动下载: https://gitee.com/Doustar/Earth-online/releases" -ForegroundColor Yellow
    Read-Host "按回车键退出"
    exit 1
}
Write-Host ""
Write-Host "[步骤 3/5] 正在安装..." -ForegroundColor Yellow
try {
    Expand-Archive -Path $zipPath -DestinationPath $tempDir -Force
    $gameExe = Get-ChildItem $tempDir -Recurse -Filter "EarthOnline.exe" -ErrorAction SilentlyContinue | Select-Object -First 1
    if ($gameExe) {
        $gameRoot = $gameExe.DirectoryName
        Copy-Item -Path "$gameRoot\*" -Destination $installDir -Recurse -Force
    } else {
        $projectDir = Get-ChildItem $tempDir -Directory | Where-Object { Test-Path "$($_.FullName)\UE5\EarthOnline.uproject" } | Select-Object -First 1
        if ($projectDir) {
            Copy-Item -Path "$($projectDir.FullName)\*" -Destination $installDir -Recurse -Force
        } else {
            Copy-Item -Path "$tempDir\*" -Destination $installDir -Recurse -Force
        }
    }
    Write-Host "  安装完成" -ForegroundColor Green
} catch {
    Write-Host "  [错误] 安装失败: $_" -ForegroundColor Red
    Read-Host "按回车键退出"
    exit 1
}
Write-Host ""
Write-Host "[步骤 4/5] 创建快捷方式..." -ForegroundColor Yellow
$targetExe = Join-Path $installDir "EarthOnline.exe"
if (-not (Test-Path $targetExe)) { $targetExe = Join-Path $installDir "开始游戏.bat" }
if (-not (Test-Path $targetExe)) { $targetExe = Join-Path $installDir "UE5\启动编辑器.bat" }
try {
    $shell = New-Object -ComObject WScript.Shell
    $desktop = [Environment]::GetFolderPath("Desktop")
    $sc = $shell.CreateShortcut("$desktop\$GameName.lnk")
    $sc.TargetPath = $targetExe
    $sc.WorkingDirectory = Split-Path $targetExe
    $sc.Description = "$GameName v$Version"
    $sc.Save()
    Write-Host "  桌面快捷方式已创建" -ForegroundColor Green
    $programs = [Environment]::GetFolderPath("Programs")
    $sc2 = $shell.CreateShortcut("$programs\$GameName.lnk")
    $sc2.TargetPath = $targetExe
    $sc2.WorkingDirectory = Split-Path $targetExe
    $sc2.Description = "$GameName v$Version"
    $sc2.Save()
    Write-Host "  开始菜单快捷方式已创建" -ForegroundColor Green
} catch {
    Write-Host "  快捷方式创建失败（不影响游戏）" -ForegroundColor Yellow
}
Write-Host ""
Write-Host "[步骤 5/5] 安装完成！" -ForegroundColor Yellow
Write-Host ""
Write-Host "========================================" -ForegroundColor Green
Write-Host "  安装成功！" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host ""
Write-Host "  游戏位置: $installDir" -ForegroundColor White
Write-Host "  版本: v$Version Beta" -ForegroundColor White
Write-Host ""
Write-Host "  双击桌面'地球Online'图标启动游戏" -ForegroundColor Cyan
Write-Host "  首次启动需输入内测密钥激活" -ForegroundColor Yellow
Write-Host ""
try { Remove-Item $tempDir -Recurse -Force -ErrorAction SilentlyContinue } catch {}
$launch = Read-Host "是否现在启动游戏？(Y/N)"
if ($launch -eq "Y" -or $launch -eq "y") { Start-Process $targetExe }
Write-Host ""
Write-Host "感谢安装地球Online！" -ForegroundColor Cyan
Start-Sleep -Seconds 2
