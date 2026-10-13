# TrsSocketServer MSBuild 빌드 스크립트 (VS2019)
# 사용: .\scripts\build.ps1 [-Config Release|Debug]
param(
    [ValidateSet("Release", "Debug")]
    [string]$Config = "Release"
)

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
$sln  = Join-Path $root "Source\TrsSocketServer_Lic_VS2019_Atalk\TrsSocketServer.sln"

$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (-not (Test-Path $vswhere)) { throw "vswhere.exe 없음: Visual Studio 2019 설치 필요" }

$msbuild = & $vswhere -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe | Select-Object -First 1
if (-not $msbuild) { throw "MSBuild.exe를 찾을 수 없음" }

$logDir = Join-Path $root "logs"
if (-not (Test-Path $logDir)) { New-Item -ItemType Directory $logDir | Out-Null }
$log = Join-Path $logDir ("build_log_{0}.log" -f (Get-Date -Format "yyyyMMdd_HHmmss"))

Write-Host "MSBuild : $msbuild"
Write-Host "Solution: $sln"
Write-Host "Config  : $Config / Win32"
Write-Host "Log     : $log"

& $msbuild $sln /p:Configuration=$Config /p:Platform=Win32 /m /v:minimal /fl "/flp:logfile=$log;encoding=utf-8"
if ($LASTEXITCODE -ne 0) { throw "빌드 실패 (exit $LASTEXITCODE). 로그: $log" }
Write-Host "빌드 성공"
