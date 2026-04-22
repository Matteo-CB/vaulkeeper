param(
    [string]$Preset = 'msvc-release-installer',
    [switch]$SkipVcpkg,
    [switch]$Sign,
    [string]$CertThumbprint = '',
    [switch]$NoRedistCheck
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$repoRoot = Split-Path -Parent $PSScriptRoot
Set-Location $repoRoot

function Remove-MsysFromPath {
    $parts = $env:Path -split ';'
    $kept = $parts | Where-Object {
        $p = $_.ToLower()
        -not ($p -match 'msys2|msys64|mingw32|mingw64|devkitpro|cygwin')
    }
    $env:Path = ($kept -join ';')
}

function Import-VcVars {
    $vswhere = 'C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe'
    if (-not (Test-Path $vswhere)) {
        throw "vswhere.exe not found, cannot locate Visual Studio install"
    }
    $vsPath = & $vswhere -latest -products * -property installationPath 2>$null | Select-Object -First 1
    if (-not $vsPath) {
        throw "no Visual Studio installation detected by vswhere"
    }
    $vcvars = Join-Path $vsPath 'VC\Auxiliary\Build\vcvars64.bat'
    if (-not (Test-Path $vcvars)) {
        throw "vcvars64.bat not found at $vcvars"
    }
    Write-Host "loading vcvars64 from $vsPath"
    $output = cmd /c "`"$vcvars`" >NUL && set"
    foreach ($line in $output) {
        if ($line -match '^([^=]+)=(.*)$') {
            [Environment]::SetEnvironmentVariable($matches[1], $matches[2], 'Process')
        }
    }
}

Remove-MsysFromPath
Import-VcVars

if (-not $env:VCPKG_ROOT -or -not (Test-Path $env:VCPKG_ROOT)) {
    throw "VCPKG_ROOT is not set. Install vcpkg and export VCPKG_ROOT."
}

$wixCandle = Get-Command candle.exe -ErrorAction SilentlyContinue
$wixLight = Get-Command light.exe -ErrorAction SilentlyContinue
if (-not $wixCandle -or -not $wixLight) {
    throw "WiX Toolset 3.11 or newer is required. Install it and ensure candle.exe and light.exe are in PATH."
}

$redistDir = Join-Path $repoRoot 'third_party/vc_redist'
$redistExe = Join-Path $redistDir 'vc_redist.x64.exe'
if (-not $NoRedistCheck -and -not (Test-Path $redistExe)) {
    if (-not (Test-Path $redistDir)) { New-Item -ItemType Directory -Path $redistDir | Out-Null }
    Write-Host "Downloading vc_redist.x64.exe"
    Invoke-WebRequest -Uri 'https://aka.ms/vs/17/release/vc_redist.x64.exe' -OutFile $redistExe
}

$brandDir = Join-Path $repoRoot 'assets/brand'
$requiredBrand = @('vaulkeeper.ico', 'banner.bmp', 'dialog.bmp', 'banner-wide.png', 'logo-setup.png', 'license.rtf')
$missing = @()
foreach ($asset in $requiredBrand) {
    $path = Join-Path $brandDir $asset
    if (-not (Test-Path $path)) { $missing += $asset }
}
if ($missing.Count -gt 0) {
    Write-Warning ("Missing brand assets: {0}" -f ($missing -join ', '))
    Write-Warning "Generating minimal placeholders so the build can proceed."
    & "$PSScriptRoot/brand-placeholders.ps1" -BrandDir $brandDir
    if ($LASTEXITCODE -ne 0) { throw "brand placeholder generation failed" }
}

if (-not $env:VCPKG_INSTALLED_DIR) {
    $env:VCPKG_INSTALLED_DIR = 'D:/vk_installed'
}
if (-not $env:VCPKG_DEFAULT_BINARY_CACHE) {
    $env:VCPKG_DEFAULT_BINARY_CACHE = 'E:/vcpkg-cache'
}
if (-not $env:VCPKG_DOWNLOADS) {
    $env:VCPKG_DOWNLOADS = 'E:/vcpkg-downloads'
}
New-Item -ItemType Directory -Path $env:VCPKG_DEFAULT_BINARY_CACHE -Force | Out-Null
New-Item -ItemType Directory -Path $env:VCPKG_DOWNLOADS -Force | Out-Null

if (-not $SkipVcpkg) {
    $overlay = Join-Path $repoRoot 'vcpkg-triplets'
    $env:VCPKG_MAX_CONCURRENCY = '1'
    $env:CMAKE_BUILD_PARALLEL_LEVEL = '4'
    & "$env:VCPKG_ROOT/vcpkg.exe" install `
        --triplet x64-windows-static-md `
        --overlay-triplets="$overlay" `
        --x-manifest-root="$repoRoot" `
        --x-install-root="$env:VCPKG_INSTALLED_DIR"
    if ($LASTEXITCODE -ne 0) { throw "vcpkg install failed" }
}

$cmakeCmd = (Get-Command cmake).Source
& $cmakeCmd --preset $Preset "-DVCPKG_INSTALLED_DIR=$env:VCPKG_INSTALLED_DIR"
if ($LASTEXITCODE -ne 0) { throw "cmake configure failed" }

& $cmakeCmd --build --preset $Preset --target vaulkeeper_installer
if ($LASTEXITCODE -ne 0) { throw "installer build failed" }

$setupExe = Join-Path $repoRoot "build/$Preset/installer/VaulkeeperSetup.exe"
if (-not (Test-Path $setupExe)) { throw "VaulkeeperSetup.exe not produced" }

if ($Sign) {
    if (-not $CertThumbprint) { throw "Provide -CertThumbprint when using -Sign" }
    & "$PSScriptRoot/sign.ps1" -BinaryDir (Split-Path -Parent $setupExe) -CertThumbprint $CertThumbprint
}

Get-FileHash $setupExe -Algorithm SHA256 | Out-File "$setupExe.sha256" -Encoding ascii
Write-Host "Installer ready: $setupExe"
