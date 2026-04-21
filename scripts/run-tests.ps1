param(
    [string]$Preset = 'msvc-relwithdebinfo',
    [string]$Filter = '',
    [switch]$Verbose
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$repoRoot = Split-Path -Parent $PSScriptRoot
Set-Location $repoRoot

$ctestArgs = @('--preset', $Preset)
if ($Filter) { $ctestArgs += @('-R', $Filter) }
if ($Verbose) { $ctestArgs += '--verbose' }

ctest @ctestArgs
if ($LASTEXITCODE -ne 0) { throw "Tests failed with exit code $LASTEXITCODE" }
