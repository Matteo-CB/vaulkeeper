param(
    [Parameter(Mandatory)] [string]$BinaryDir,
    [Parameter(Mandatory)] [string]$CertThumbprint,
    [string]$TimestampUrl = 'http://timestamp.digicert.com'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$signtool = (Get-Command signtool.exe -ErrorAction Stop).Source

$targets = Get-ChildItem -Path $BinaryDir -Include *.exe, *.dll -Recurse
if (-not $targets) {
    throw "No signable binaries found under $BinaryDir"
}

foreach ($t in $targets) {
    & $signtool sign /sha1 $CertThumbprint /fd SHA256 /tr $TimestampUrl /td SHA256 /d 'Vaulkeeper' $t.FullName
    if ($LASTEXITCODE -ne 0) { throw "signtool failed on $($t.FullName)" }
    & $signtool verify /pa /q $t.FullName
    if ($LASTEXITCODE -ne 0) { throw "verification failed on $($t.FullName)" }
}

Write-Host "Signed $($targets.Count) binaries."
