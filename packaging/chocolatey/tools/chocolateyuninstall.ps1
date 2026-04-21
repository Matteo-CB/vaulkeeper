$ErrorActionPreference = 'Stop'

$uninstallArgs = @{
    packageName   = 'vaulkeeper'
    fileType      = 'exe'
    silentArgs    = '/uninstall /quiet'
    validExitCodes = @(0, 1641, 3010)
    file          = Get-UninstallRegistryKey -SoftwareName 'Vaulkeeper*' |
                    Select-Object -ExpandProperty UninstallString -First 1
}

if ($uninstallArgs.file) {
    Uninstall-ChocolateyPackage @uninstallArgs
}
