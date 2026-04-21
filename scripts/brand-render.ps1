param(
    [string]$BrandDir = (Join-Path (Split-Path -Parent $PSScriptRoot) 'assets/brand'),
    [string]$MagickPath = 'magick'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Assert-Command($name, $path) {
    if (-not (Get-Command $path -ErrorAction SilentlyContinue)) {
        throw "Required command '$name' not found. Install ImageMagick or pass -MagickPath."
    }
}

Assert-Command 'ImageMagick' $MagickPath

$logo = Join-Path $BrandDir 'logo.svg'
if (-not (Test-Path $logo)) { throw "Missing $logo" }

& $MagickPath $logo -background 'transparent' -resize 256x256 "$BrandDir/vaulkeeper-256.png"
& $MagickPath $logo -background 'transparent' -define icon:auto-resize=16,24,32,48,64,128,256 "$BrandDir/vaulkeeper.ico"
& $MagickPath $logo -background '#0B0D10' -resize 64x64 "$BrandDir/logo-setup.png"
& $MagickPath -size 640x140 canvas:'#0B0D10' "$BrandDir/banner-wide.png"
& $MagickPath -size 640x140 canvas:'#0B0D10' $logo -geometry +24+38 -resize x80 -composite "$BrandDir/banner-wide.png"
& $MagickPath -size 493x58 canvas:'#0B0D10' "$BrandDir/banner.bmp"
& $MagickPath -size 493x58 canvas:'#0B0D10' $logo -geometry +18+10 -resize x38 -composite "$BrandDir/banner.bmp"
& $MagickPath -size 493x312 canvas:'#15181D' "$BrandDir/dialog.bmp"

Write-Host "Brand assets rendered into $BrandDir"
