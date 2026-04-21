param(
    [string]$Preset = 'msvc-release-installer',
    [string]$OutputDir = 'dist',
    [switch]$Sign,
    [string]$CertThumbprint = ''
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$repoRoot = Split-Path -Parent $PSScriptRoot
Set-Location $repoRoot

if (-not (Test-Path $OutputDir)) { New-Item -ItemType Directory -Path $OutputDir | Out-Null }

& "$PSScriptRoot\bootstrap.ps1" -Preset $Preset
if ($LASTEXITCODE -ne 0) { throw "Bootstrap failed" }

$binDir = "build/$Preset/bin"
$stageDir = "$OutputDir/stage"
if (Test-Path $stageDir) { Remove-Item -Recurse -Force $stageDir }
New-Item -ItemType Directory -Path $stageDir | Out-Null

Copy-Item "$binDir/vaulkeeper.exe" $stageDir
Copy-Item "$binDir/vk.exe" $stageDir
Copy-Item "$binDir/VaulkeeperAgent.exe" $stageDir
Copy-Item "$binDir/*.dll" $stageDir -ErrorAction SilentlyContinue

if ($Sign) {
    & "$PSScriptRoot\sign.ps1" -BinaryDir $stageDir -CertThumbprint $CertThumbprint
}

$zipPath = "$OutputDir/vaulkeeper-portable.zip"
if (Test-Path $zipPath) { Remove-Item $zipPath }
Compress-Archive -Path "$stageDir/*" -DestinationPath $zipPath

Get-FileHash $zipPath -Algorithm SHA256 | Out-File "$zipPath.sha256" -Encoding ascii
Write-Host "Portable archive: $zipPath"
