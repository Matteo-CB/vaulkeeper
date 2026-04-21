param(
    [Parameter(Mandatory)] [string]$Version,
    [switch]$Sign,
    [string]$CertThumbprint = ''
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$repoRoot = Split-Path -Parent $PSScriptRoot
Set-Location $repoRoot

& "$PSScriptRoot/lint-text.ps1"
if ($LASTEXITCODE -ne 0) { throw "text lint failed" }

& "$PSScriptRoot/build-installer.ps1" -Sign:$Sign -CertThumbprint $CertThumbprint
if ($LASTEXITCODE -ne 0) { throw "installer build failed" }

& "$PSScriptRoot/build-portable.ps1"
if ($LASTEXITCODE -ne 0) { throw "portable build failed" }

$releaseDir = Join-Path 'dist' "v$Version"
if (Test-Path $releaseDir) { Remove-Item -Recurse -Force $releaseDir }
New-Item -ItemType Directory -Path $releaseDir | Out-Null

Copy-Item "build/msvc-release-installer/installer/VaulkeeperSetup.exe" $releaseDir
Copy-Item "build/msvc-release-installer/installer/VaulkeeperSetup.exe.sha256" $releaseDir -ErrorAction SilentlyContinue
Copy-Item "dist/vaulkeeper-portable.zip" $releaseDir
Copy-Item "dist/vaulkeeper-portable.zip.sha256" $releaseDir -ErrorAction SilentlyContinue

Get-ChildItem "build/msvc-release-installer/installer" -Filter '*.msi' -Recurse | ForEach-Object {
    Copy-Item $_.FullName $releaseDir
}

Write-Host "Release staged at $releaseDir"
