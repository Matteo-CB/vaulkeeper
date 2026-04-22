param(
    [string]$Preset = 'msvc-relwithdebinfo',
    [switch]$Clean,
    [switch]$NoVcpkgUpdate
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
    $winCmd = Join-Path $env:SystemRoot 'System32\cmd.exe'
    $output = & $winCmd /c "`"$vcvars`" >NUL && set"
    foreach ($line in $output) {
        if ($line -match '^([^=]+)=(.*)$') {
            [Environment]::SetEnvironmentVariable($matches[1], $matches[2], 'Process')
        }
    }
}

Remove-MsysFromPath
Import-VcVars

function Assert-Command($name) {
    if (-not (Get-Command $name -ErrorAction SilentlyContinue)) {
        throw "Required command '$name' not found in PATH."
    }
}

Assert-Command cmake
Assert-Command ninja
Assert-Command cl

$cmakeCmd = (Get-Command cmake).Source
Write-Host "cmake : $cmakeCmd"
Write-Host "ninja : $((Get-Command ninja).Source)"
Write-Host "cl    : $((Get-Command cl).Source)"

if (-not $env:VCPKG_ROOT -or -not (Test-Path $env:VCPKG_ROOT)) {
    throw "VCPKG_ROOT is not set or does not exist. Install vcpkg and export VCPKG_ROOT."
}

if ($Clean) {
    Write-Host "Removing build/$Preset"
    if (Test-Path "build/$Preset") {
        Remove-Item -Recurse -Force "build/$Preset"
    }
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

if (-not $NoVcpkgUpdate) {
    Write-Host "Updating vcpkg manifest dependencies (install root: $env:VCPKG_INSTALLED_DIR)"
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

& $cmakeCmd --preset $Preset "-DVCPKG_INSTALLED_DIR=$env:VCPKG_INSTALLED_DIR"
if ($LASTEXITCODE -ne 0) { throw "cmake configure failed" }

& $cmakeCmd --build --preset $Preset
if ($LASTEXITCODE -ne 0) { throw "cmake build failed" }

Write-Host "Build complete: build/$Preset/bin"
