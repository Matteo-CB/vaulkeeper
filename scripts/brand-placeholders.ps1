param(
    [string]$BrandDir = (Join-Path (Split-Path -Parent $PSScriptRoot) 'assets/brand')
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

if (-not (Test-Path $BrandDir)) {
    New-Item -ItemType Directory -Path $BrandDir | Out-Null
}

function Write-Bytes($path, [byte[]]$data) {
    [System.IO.File]::WriteAllBytes($path, $data)
}

function New-SolidBmp {
    param(
        [Parameter(Mandatory)][int]$Width,
        [Parameter(Mandatory)][int]$Height,
        [Parameter(Mandatory)][string]$Path,
        [byte]$R = 0x0B,
        [byte]$G = 0x0D,
        [byte]$B = 0x10
    )

    $rowStride = (($Width * 3) + 3) -band -bnot 3
    $pixelData = $rowStride * $Height
    $fileSize = 54 + $pixelData

    $stream = New-Object System.IO.MemoryStream
    $writer = New-Object System.IO.BinaryWriter($stream)
    $writer.Write([char]'B')
    $writer.Write([char]'M')
    $writer.Write([int]$fileSize)
    $writer.Write([int]0)
    $writer.Write([int]54)
    $writer.Write([int]40)
    $writer.Write([int]$Width)
    $writer.Write([int]$Height)
    $writer.Write([int16]1)
    $writer.Write([int16]24)
    $writer.Write([int]0)
    $writer.Write([int]$pixelData)
    $writer.Write([int]2835)
    $writer.Write([int]2835)
    $writer.Write([int]0)
    $writer.Write([int]0)

    $row = New-Object 'byte[]' $rowStride
    for ($x = 0; $x -lt $Width; ++$x) {
        $row[($x * 3)]     = $B
        $row[($x * 3) + 1] = $G
        $row[($x * 3) + 2] = $R
    }
    for ($y = 0; $y -lt $Height; ++$y) {
        $writer.Write($row)
    }

    $writer.Flush()
    Write-Bytes -path $Path -data $stream.ToArray()
    $writer.Dispose()
    $stream.Dispose()
}

function New-SolidPng {
    param(
        [Parameter(Mandatory)][int]$Width,
        [Parameter(Mandatory)][int]$Height,
        [Parameter(Mandatory)][string]$Path,
        [byte]$R = 0x0B,
        [byte]$G = 0x0D,
        [byte]$B = 0x10
    )

    $bmpPath = [System.IO.Path]::Combine([System.IO.Path]::GetTempPath(), [System.IO.Path]::GetRandomFileName() + '.bmp')
    try {
        New-SolidBmp -Width $Width -Height $Height -Path $bmpPath -R $R -G $G -B $B
        Add-Type -AssemblyName System.Drawing
        $bitmap = [System.Drawing.Image]::FromFile($bmpPath)
        $bitmap.Save($Path, [System.Drawing.Imaging.ImageFormat]::Png)
        $bitmap.Dispose()
    } finally {
        if (Test-Path $bmpPath) { Remove-Item $bmpPath -Force }
    }
}

function New-MinimalIco {
    param(
        [Parameter(Mandatory)][string]$Path
    )

    $sizes = @(16, 32, 48, 64, 128, 256)
    $entries = @()
    $bitmaps = @()

    foreach ($size in $sizes) {
        $rowStride = (($size * 4) + 3) -band -bnot 3
        $pixelData = $rowStride * $size * 2
        $dibSize = 40 + $pixelData

        $dibStream = New-Object System.IO.MemoryStream
        $dibWriter = New-Object System.IO.BinaryWriter($dibStream)
        $dibWriter.Write([int]40)
        $dibWriter.Write([int]$size)
        $dibWriter.Write([int]($size * 2))
        $dibWriter.Write([int16]1)
        $dibWriter.Write([int16]32)
        $dibWriter.Write([int]0)
        $dibWriter.Write([int]$pixelData)
        $dibWriter.Write([int]0)
        $dibWriter.Write([int]0)
        $dibWriter.Write([int]0)
        $dibWriter.Write([int]0)

        for ($y = 0; $y -lt $size; ++$y) {
            for ($x = 0; $x -lt $size; ++$x) {
                $dibWriter.Write([byte]0xFF)
                $dibWriter.Write([byte]0x9E)
                $dibWriter.Write([byte]0x5B)
                $dibWriter.Write([byte]0xFF)
            }
        }

        $maskBytes = New-Object 'byte[]' ($rowStride * $size / 4)
        $dibWriter.Write($maskBytes)

        $bitmaps += ,($dibStream.ToArray())
        $entries += [pscustomobject]@{
            Size = $size
            Length = $dibStream.Length
        }
        $dibWriter.Dispose()
        $dibStream.Dispose()
    }

    $header = New-Object System.IO.MemoryStream
    $writer = New-Object System.IO.BinaryWriter($header)
    $writer.Write([int16]0)
    $writer.Write([int16]1)
    $writer.Write([int16]$entries.Count)

    $offset = 6 + (16 * $entries.Count)
    foreach ($entry in $entries) {
        $width = if ($entry.Size -ge 256) { 0 } else { $entry.Size }
        $height = $width
        $writer.Write([byte]$width)
        $writer.Write([byte]$height)
        $writer.Write([byte]0)
        $writer.Write([byte]0)
        $writer.Write([int16]1)
        $writer.Write([int16]32)
        $writer.Write([int]$entry.Length)
        $writer.Write([int]$offset)
        $offset += $entry.Length
    }

    foreach ($bitmap in $bitmaps) {
        $writer.Write($bitmap)
    }

    $writer.Flush()
    Write-Bytes -path $Path -data $header.ToArray()
    $writer.Dispose()
    $header.Dispose()
}

Write-Host "Generating brand placeholders in $BrandDir"

New-SolidBmp -Width 493 -Height 58 -Path (Join-Path $BrandDir 'banner.bmp')
New-SolidBmp -Width 493 -Height 312 -Path (Join-Path $BrandDir 'dialog.bmp') -R 0x15 -G 0x18 -B 0x1D
New-SolidPng -Width 640 -Height 140 -Path (Join-Path $BrandDir 'banner-wide.png')
New-SolidPng -Width 64 -Height 64 -Path (Join-Path $BrandDir 'logo-setup.png') -R 0x5B -G 0x9E -B 0xFF
New-SolidPng -Width 256 -Height 256 -Path (Join-Path $BrandDir 'vaulkeeper-256.png') -R 0x5B -G 0x9E -B 0xFF
New-MinimalIco -Path (Join-Path $BrandDir 'vaulkeeper.ico')

Write-Host "Placeholders written. Replace them before a public release using scripts/brand-render.ps1 (ImageMagick)."
