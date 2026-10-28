<#
Earth-online UE5.8 开发环境一键安装脚本
自动功能：自动管理员提权、安装.NET10 SDK、.NET Framework4.8开发包、VS Build Tools2022(C++编译工具+Windows SDK)、VC++运行库
#>
# ====================== 自动提权模块 ======================
# 检测当前是否为管理员权限
$currentPrincipal = New-Object Security.Principal.WindowsPrincipal([Security.Principal.WindowsIdentity]::GetCurrent())
$isAdmin = $currentPrincipal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)

if (-not $isAdmin)
{
    Write-Host "未检测到管理员权限，正在自动重新启动脚本【管理员模式】..." -ForegroundColor Yellow
    Start-Process powershell.exe "-NoProfile -ExecutionPolicy Bypass -File `"$PSCommandPath`"" -Verb RunAs
    exit
}
# =========================================================

Write-Host "=====================================" -ForegroundColor Cyan
Write-Host "Earth-online UE5.8 开发环境一键安装器（管理员模式）" -ForegroundColor Cyan
Write-Host "=====================================`n" -ForegroundColor Cyan

# 1. 检查winget是否可用
Write-Host "[1/5] 检查Windows包管理器 winget..." -ForegroundColor Yellow
$wingetCheck = Get-Command winget -ErrorAction SilentlyContinue
if(-not $wingetCheck){
    Write-Host "winget未找到，请先更新Microsoft App Installer，脚本退出" -ForegroundColor Red
    Read-Host "按回车关闭窗口"
    exit 1
}
Write-Host "winget正常`n"

# 2. 安装 .NET 10 SDK (UE5.8编译必需)
Write-Host "[2/5] 正在安装 .NET 10 SDK" -ForegroundColor Yellow
winget install --id Microsoft.DotNet.SDK.10 --silent --accept-package-agreements --accept-source-agreements
Write-Host ".NET10 SDK安装完成`n"

#3. 安装 .NET Framework 4.8 Developer Pack
Write-Host "[3/5] 正在安装 .NET Framework 4.8 开发包" -ForegroundColor Yellow
winget install --id Microsoft.DotNet.Framework.DeveloperPack_4.8 --silent --accept-package-agreements --accept-source-agreements
Write-Host ".NET4.8开发包安装完成`n"

#4. 安装 VS Build Tools 2022 带C++工作负载 + Windows10 SDK
Write-Host "[4/5] 安装 Visual Studio Build Tools 2022（C++编译工具+Windows SDK）" -ForegroundColor Yellow
winget install --id Microsoft.VisualStudio.BuildTools --silent --accept-package-agreements --accept-source-agreements `
--override "--passive --wait --add Microsoft.VisualStudio.Workload.VCTools;includeRecommended"
Write-Host "VS Build Tools 安装完成`n"

#5. 安装 VC++ 2015-2022运行库
Write-Host "[5/5] 安装VC++ 2015~2022 x64运行库" -ForegroundColor Yellow
winget install --id Microsoft.VCRedist.2015+.x64 --silent --accept-package-agreements --accept-source-agreements
Write-Host "VC++运行库安装完成`n"

Write-Host "✅ 全部依赖安装结束！重启电脑后，即可编译 Earth-online UE5.8项目" -ForegroundColor Green
Read-Host "按回车键关闭窗口"
