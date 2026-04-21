param(
    [string]$Preset = 'msvc-relwithdebinfo',
    [switch]$Clean,
    [switch]$NoVcpkgUpdate
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Assert-Command($name) {
    if (-not (Get-Command $name -ErrorAction SilentlyContinue)) {
        throw "Required command '$name' not found in PATH."
    }
}

$repoRoot = Split-Path -Parent $PSScriptRoot
Set-Location $repoRoot

Assert-Command cmake
Assert-Command ninja

if (-not $env:VCPKG_ROOT -or -not (Test-Path $env:VCPKG_ROOT)) {
    throw "VCPKG_ROOT is not set or does not exist. Install vcpkg and export VCPKG_ROOT."
}

if ($Clean) {
    Write-Host "Removing build/$Preset"
    if (Test-Path "build/$Preset") {
        Remove-Item -Recurse -Force "build/$Preset"
    }
}

if (-not $NoVcpkgUpdate) {
    Write-Host "Updating vcpkg manifest dependencies"
    $overlay = Join-Path $repoRoot 'vcpkg-triplets'
    $env:VCPKG_MAX_CONCURRENCY = '1'
    $env:CMAKE_BUILD_PARALLEL_LEVEL = '4'
    & "$env:VCPKG_ROOT/vcpkg.exe" install `
        --triplet x64-windows-static-md `
        --overlay-triplets="$overlay" `
        --x-manifest-root="$repoRoot"
    if ($LASTEXITCODE -ne 0) { throw "vcpkg install failed" }
}

cmake --preset $Preset
if ($LASTEXITCODE -ne 0) { throw "cmake configure failed" }

cmake --build --preset $Preset
if ($LASTEXITCODE -ne 0) { throw "cmake build failed" }

Write-Host "Build complete: build/$Preset/bin"
