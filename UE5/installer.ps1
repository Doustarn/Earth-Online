# 地球Online v0.1.8 在线安装器
# 自动下载、安装、创建快捷方式、启动游戏

$ErrorActionPreference = "Stop"

# 配置
$GameName = "地球Online"
$Version = "0.1.8"
$RepoUrl = "https://gitee.com/Doustar/Earth-online"
$ReleaseApi = "https://gitee.com/api/v5/repos/Doustar/Earth-online/releases/latest"
$DefaultInstallDir = "D:\EarthOnline"

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  地球Online v$Version 内测版 安装程序" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# 1. 选择安装目录
Write-Host "[步骤 1/5] 选择安装目录" -ForegroundColor Yellow
$installDir = Read-Host "安装路径（默认: $DefaultInstallDir）"
if ([string]::IsNullOrWhiteSpace($installDir)) {
    $installDir = $DefaultInstallDir
}
$installDir = $installDir.Trim('"')

if (-not (Test-Path $installDir)) {
    New-Item -ItemType Directory -Path $installDir -Force | Out-Null
}
Write-Host "  安装目录: $installDir" -ForegroundColor Green
Write-Host ""

# 2. 下载游戏
Write-Host "[步骤 2/5] 正在下载游戏..." -ForegroundColor Yellow

$tempDir = Join-Path $env:TEMP "EarthOnline_Install"
if (Test-Path $tempDir) {
    Remove-Item $tempDir -Recurse -Force
}
New-Item -ItemType Directory -Path $tempDir -Force | Out-Null

$zipPath = Join-Path $tempDir "EarthOnline.zip"

# 尝试从Gitee发行版下载
try {
    Write-Host "  正在获取最新版本信息..." -ForegroundColor Gray
    $release = Invoke-RestMethod -Uri $ReleaseApi -TimeoutSec 30
    $tag = $release.tag_name
    Write-Host "  最新版本: $tag" -ForegroundColor Green

    # 下载源码包（包含所有游戏文件）
    $downloadUrl = "https://gitee.com/Doustar/Earth-online/repository/archive/$tag.zip"
    Write-Host "  下载地址: $downloadUrl" -ForegroundColor Gray
    Write-Host "  正在下载，请稍候..." -ForegroundColor Gray

    $ProgressPreference = 'SilentlyContinue'
    Invoke-WebRequest -Uri $downloadUrl -OutFile $zipPath -TimeoutSec 600 -UseBasicParsing
    $ProgressPreference = 'Continue'

    $fileSize = (Get-Item $zipPath).Length / 1MB
    Write-Host "  下载完成: $([math]::Round($fileSize, 2)) MB" -ForegroundColor Green
}
catch {
    Write-Host "  自动下载失败，尝试备用方式..." -ForegroundColor Yellow
    # 备用：直接下载master分支
    $backupUrl = "https://gitee.com/Doustar/Earth-online/repository/archive/master.zip"
    try {
        $ProgressPreference = 'SilentlyContinue'
        Invoke-WebRequest -Uri $backupUrl -OutFile $zipPath -TimeoutSec 600 -UseBasicParsing
        $ProgressPreference = 'Continue'
        Write-Host "  备用下载完成" -ForegroundColor Green
    }
    catch {
        Write-Host "  [错误] 下载失败: $_" -ForegroundColor Red
        Write-Host "  请检查网络连接，或手动从以下地址下载:" -ForegroundColor Yellow
        Write-Host "  $RepoUrl/releases" -ForegroundColor Cyan
        Read-Host "按回车键退出"
        exit 1
    }
}
Write-Host ""

# 3. 解压安装
Write-Host "[步骤 3/5] 正在安装..." -ForegroundColor Yellow

try {
    Expand-Archive -Path $zipPath -DestinationPath $tempDir -Force

    # 找到解压后的项目目录
    $extractedDir = Get-ChildItem $tempDir -Directory | Select-Object -First 1
    if ($extractedDir) {
        $sourceDir = $extractedDir.FullName
    } else {
        $sourceDir = $tempDir
    }

    # 复制游戏文件到安装目录
    Write-Host "  复制文件中..." -ForegroundColor Gray
    Copy-Item -Path "$sourceDir\*" -Destination $installDir -Recurse -Force

    Write-Host "  安装完成" -ForegroundColor Green
}
catch {
    Write-Host "  [错误] 安装失败: $_" -ForegroundColor Red
    Read-Host "按回车键退出"
    exit 1
}
Write-Host ""

# 4. 创建快捷方式
Write-Host "[步骤 4/5] 创建快捷方式..." -ForegroundColor Yellow

$gameExe = Join-Path $installDir "UE5\EarthOnline.uproject"
$startScript = Join-Path $installDir "UE5\开始游戏.bat"

# 如果有打包好的exe就用exe，否则用启动脚本
$targetExe = $startScript
if (Test-Path (Join-Path $installDir "UE5\Packaged\WindowsClient\Windows\EarthOnline.exe")) {
    $targetExe = Join-Path $installDir "UE5\Packaged\WindowsClient\Windows\EarthOnline.exe"
}

try {
    # 桌面快捷方式
    $desktop = [Environment]::GetFolderPath("Desktop")
    $shortcutPath = Join-Path $desktop "$GameName.lnk"
    $shell = New-Object -ComObject WScript.Shell
    $shortcut = $shell.CreateShortcut($shortcutPath)
    $shortcut.TargetPath = $targetExe
    $shortcut.WorkingDirectory = Split-Path $targetExe
    $shortcut.Description = "$GameName v$Version 内测版"
    $shortcut.Save()
    Write-Host "  桌面快捷方式已创建" -ForegroundColor Green

    # 开始菜单快捷方式
    $programs = [Environment]::GetFolderPath("Programs")
    $startMenuPath = Join-Path $programs "$GameName.lnk"
    $shortcut2 = $shell.CreateShortcut($startMenuPath)
    $shortcut2.TargetPath = $targetExe
    $shortcut2.WorkingDirectory = Split-Path $targetExe
    $shortcut2.Description = "$GameName v$Version 内测版"
    $shortcut2.Save()
    Write-Host "  开始菜单快捷方式已创建" -ForegroundColor Green
}
catch {
    Write-Host "  快捷方式创建失败（不影响游戏运行）: $_" -ForegroundColor Yellow
}
Write-Host ""

# 5. 完成
Write-Host "[步骤 5/5] 安装完成！" -ForegroundColor Yellow
Write-Host ""
Write-Host "========================================" -ForegroundColor Green
Write-Host "  安装成功！" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host ""
Write-Host "  游戏位置: $installDir" -ForegroundColor White
Write-Host "  版本: v$Version Beta" -ForegroundColor White
Write-Host ""
Write-Host "  启动方式:" -ForegroundColor Cyan
Write-Host "    1. 双击桌面'地球Online'图标" -ForegroundColor White
Write-Host "    2. 开始菜单搜索'地球Online'" -ForegroundColor White
Write-Host "    3. 运行: $targetExe" -ForegroundColor White
Write-Host ""
Write-Host "  首次启动需要输入内测密钥激活" -ForegroundColor Yellow
Write-Host ""

# 清理临时文件
try {
    Remove-Item $tempDir -Recurse -Force -ErrorAction SilentlyContinue
} catch {}

# 询问是否启动
$launch = Read-Host "是否现在启动游戏？(Y/N)"
if ($launch -eq "Y" -or $launch -eq "y") {
    Write-Host "  正在启动游戏..." -ForegroundColor Green
    Start-Process $targetExe
}

Write-Host ""
Write-Host "感谢安装地球Online！" -ForegroundColor Cyan
Start-Sleep -Seconds 2
