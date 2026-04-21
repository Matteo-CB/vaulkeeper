param(
    [string]$Root = (Split-Path -Parent $PSScriptRoot),
    [switch]$FailOnWarning
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$extensions = @(
    '*.cpp','*.hpp','*.h','*.c','*.cc',
    '*.qml','*.js','*.ts',
    '*.json','*.yaml','*.yml','*.toml',
    '*.md','*.rc','*.wxs','*.xml',
    '*.ps1','*.cmake','*.cmd','*.bat'
)

$skipDirs = @('build','out','dist','third_party','vcpkg_installed','.git','.vs','.vscode','node_modules')

function Should-Skip([string]$path) {
    foreach ($d in $skipDirs) {
        if ($path -match "[\\/]$d[\\/]") { return $true }
        if ($path -match "[\\/]$d$") { return $true }
    }
    if ($path -match 'CLAUDE\.md$') { return $true }
    if ($path -match 'lint-text\.ps1$') { return $true }
    return $false
}

function Test-Emoji([string]$line) {
    if ([string]::IsNullOrEmpty($line)) { return $false }
    $i = 0
    while ($i -lt $line.Length) {
        $cp = [char]::ConvertToUtf32($line, $i)
        $step = if ([char]::IsSurrogatePair($line, $i)) { 2 } else { 1 }
        $i += $step
        if (($cp -ge 0x1F300 -and $cp -le 0x1FAFF) -or
            ($cp -ge 0x2600  -and $cp -le 0x27BF ) -or
            ($cp -ge 0x1F000 -and $cp -le 0x1F2FF) -or
            ($cp -ge 0x1F900 -and $cp -le 0x1F9FF) -or
            $cp -eq 0x2B50 -or $cp -eq 0x2B55) {
            return $true
        }
    }
    return $false
}

function Test-LongDash([string]$line) {
    foreach ($cp in [int[]](0x2012, 0x2013, 0x2014, 0x2015, 0x2212)) {
        if ($line.IndexOf([char]$cp) -ge 0) { return $true }
    }
    return $false
}

$aiPhrases = @(
    'je suis ravi', 'absolument', 'certainement', 'avec plaisir',
    'plongeons dans', 'explorons ensemble',
    'en resume', 'pour resumer', 'pour conclure',
    'il convient de noter', 'il est important de souligner',
    'il est essentiel de comprendre',
    'laissez moi vous expliquer', 'permettez moi de',
    'formidable', 'excellent choix', 'belle question', 'analysons ensemble',
    'liberez le potentiel', 'maximisez', 'boostez',
    'optimisez votre experience', 'revolutionnaire',
    'seamless experience', 'user friendly'
)

$findings = New-Object System.Collections.Generic.List[object]

$files = @()
foreach ($pat in $extensions) {
    $files += Get-ChildItem -Path $Root -Recurse -File -Filter $pat -ErrorAction SilentlyContinue
}
$files = $files | Where-Object { -not (Should-Skip $_.FullName) } | Sort-Object FullName -Unique

foreach ($f in $files) {
    $lineNo = 0
    foreach ($line in Get-Content -LiteralPath $f.FullName -Encoding UTF8) {
        $lineNo++
        if (Test-Emoji $line) {
            $findings.Add([pscustomobject]@{File=$f.FullName; Line=$lineNo; Kind='emoji'; Text=$line.Trim()})
        }
        if (Test-LongDash $line) {
            $findings.Add([pscustomobject]@{File=$f.FullName; Line=$lineNo; Kind='long-dash'; Text=$line.Trim()})
        }
        foreach ($p in $aiPhrases) {
            if ($line -match "(?i)$([regex]::Escape($p))") {
                $findings.Add([pscustomobject]@{File=$f.FullName; Line=$lineNo; Kind="phrase:$p"; Text=$line.Trim()})
            }
        }
    }
}

if ($findings.Count -eq 0) {
    Write-Host "lint-text: clean across $($files.Count) files."
    exit 0
}

foreach ($g in $findings | Group-Object Kind) {
    Write-Host ""
    Write-Host "== $($g.Name) : $($g.Count) hit(s) =="
    foreach ($h in $g.Group) {
        $rel = Resolve-Path -Relative $h.File -ErrorAction SilentlyContinue
        if (-not $rel) { $rel = $h.File }
        Write-Host ("  {0}:{1}  {2}" -f $rel, $h.Line, $h.Text)
    }
}

Write-Host ""
Write-Host "lint-text: $($findings.Count) violation(s) across $($files.Count) files scanned."
exit 1
