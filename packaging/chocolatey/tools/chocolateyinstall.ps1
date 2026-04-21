$ErrorActionPreference = 'Stop'
$toolsDir = "$(Split-Path -parent $MyInvocation.MyCommand.Definition)"

$packageArgs = @{
    packageName    = 'vaulkeeper'
    fileType       = 'exe'
    url64bit       = 'https://vaulkeeper.local/releases/v0.1.0/VaulkeeperSetup.exe'
    checksum64     = '0000000000000000000000000000000000000000000000000000000000000000'
    checksumType64 = 'sha256'
    silentArgs     = '/quiet'
    validExitCodes = @(0, 1641, 3010)
}

Install-ChocolateyPackage @packageArgs
