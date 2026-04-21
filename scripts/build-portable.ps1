param(
    [string]$Preset = 'msvc-release',
    [string]$OutputDir = 'dist'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$repoRoot = Split-Path -Parent $PSScriptRoot
Set-Location $repoRoot

if (-not (Test-Path $OutputDir)) { New-Item -ItemType Directory -Path $OutputDir | Out-Null }

& "$PSScriptRoot/bootstrap.ps1" -Preset $Preset -NoVcpkgUpdate
if ($LASTEXITCODE -ne 0) { throw "bootstrap failed" }

$binDir = Join-Path $repoRoot "build/$Preset/bin"
$stage = Join-Path $OutputDir 'portable-stage'
if (Test-Path $stage) { Remove-Item -Recurse -Force $stage }
New-Item -ItemType Directory -Path $stage | Out-Null

Copy-Item "$binDir/vaulkeeper.exe" $stage
Copy-Item "$binDir/vk.exe" $stage
Copy-Item "$binDir/VaulkeeperAgent.exe" $stage
Copy-Item "$binDir/vk-updater.exe" $stage
Copy-Item "$binDir/*.dll" $stage -ErrorAction SilentlyContinue

New-Item -ItemType File -Path (Join-Path $stage 'portable.flag') -Force | Out-Null

$zipPath = Join-Path $OutputDir 'vaulkeeper-portable.zip'
if (Test-Path $zipPath) { Remove-Item $zipPath }
Compress-Archive -Path "$stage/*" -DestinationPath $zipPath
Get-FileHash $zipPath -Algorithm SHA256 | Out-File "$zipPath.sha256" -Encoding ascii

Write-Host "Portable zip: $zipPath"
