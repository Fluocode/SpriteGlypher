Param(
  [Parameter(Mandatory = $true)]
  [string]$ExePath,

  [Parameter(Mandatory = $false)]
  [string]$DistRoot = "",

  [Parameter(Mandatory = $false)]
  [string]$AppName = "SpriteGlypher",

  [Parameter(Mandatory = $false)]
  [string]$QtDir = $env:QTDIR,

  [Parameter(Mandatory = $false)]
  [string]$WinDeployQt = $env:WINDEPLOYQT
)

$ErrorActionPreference = "Stop"

function Get-RepoRoot {
  # $PSScriptRoot can be empty while parsing param defaults; resolve based on the script file location.
  $scriptPath = $MyInvocation.MyCommand.Path
  if (-not $scriptPath) { return $null }
  $toolsDir = Split-Path -Parent $scriptPath
  return (Resolve-Path (Join-Path $toolsDir "..")).Path
}

function Resolve-Windeployqt {
  param([string]$QtDir, [string]$WinDeployQt)

  if ($WinDeployQt -and (Test-Path $WinDeployQt)) { return (Resolve-Path $WinDeployQt).Path }
  if ($QtDir) {
    $candidate = Join-Path $QtDir "bin\windeployqt.exe"
    if (Test-Path $candidate) { return (Resolve-Path $candidate).Path }
  }
  $cmd = Get-Command windeployqt.exe -ErrorAction SilentlyContinue
  if ($cmd) { return $cmd.Path }
  throw "windeployqt.exe not found. Set QTDIR (e.g. C:\Qt\6.11.0\mingw_64) or WINDEPLOYQT to full path."
}

if (!(Test-Path $ExePath)) {
  throw "Exe not found: $ExePath"
}

$exeFull = (Resolve-Path $ExePath).Path
$windeployqt = Resolve-Windeployqt -QtDir $QtDir -WinDeployQt $WinDeployQt

if (-not $DistRoot) {
  $repoRoot = Get-RepoRoot
  if ($repoRoot) {
    $DistRoot = Join-Path $repoRoot "dist\windows"
  } else {
    # Fallback: package next to the current working directory (should not happen when called from the .pro).
    $DistRoot = Join-Path (Get-Location).Path "dist\windows"
  }
}

$outDir = Join-Path $DistRoot $AppName
New-Item -ItemType Directory -Force -Path $outDir | Out-Null

Copy-Item -Force $exeFull (Join-Path $outDir "$AppName.exe")

Push-Location $outDir
try {
  & $windeployqt ".\$AppName.exe" --compiler-runtime | Out-Host
} finally {
  Pop-Location
}

Write-Host ""
Write-Host "Packaged Windows build to:"
Write-Host "  $outDir"
Write-Host ""
Write-Host "Tip: zip the folder (exe + dlls + plugins) for distribution."

