[CmdletBinding()]
param(
    [switch]$Probe,
    [switch]$Run,
    [ValidateSet('Debug', 'Release')]
    [string]$Configuration = 'Debug',
    [switch]$Benchmark
)

$ErrorActionPreference = 'Stop'
if ($Benchmark) {
    if ($PSBoundParameters.ContainsKey('Configuration') -and $Configuration -ne 'Release') {
        throw '-Benchmark requires Release configuration.'
    }
    $Configuration = 'Release'
    $Run = $true
}
$repoRoot = Split-Path -Parent $PSScriptRoot
$vswherePath = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
if (-not (Test-Path -LiteralPath $vswherePath)) {
    throw 'vswhere was not found. Install Visual Studio C++ build tools.'
}

$vsRoot = & $vswherePath -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if ($LASTEXITCODE -ne 0 -or -not $vsRoot) {
    throw 'No Visual Studio installation with MSVC x64/x86 tools was found.'
}
$vsRoot = ($vsRoot | Select-Object -First 1).Trim()
$msbuildPath = Join-Path $vsRoot 'MSBuild/Current/Bin/MSBuild.exe'
if (-not (Test-Path -LiteralPath $msbuildPath)) {
    throw "MSBuild was not found at $msbuildPath"
}

$toolsetFiles = Get-ChildItem -Path (Join-Path $vsRoot 'MSBuild/Microsoft/VC/*/Platforms/x64/PlatformToolsets/v*/Toolset.props') -File |
    Where-Object { $_.Directory.Name -match '^v\d+$' } |
    Sort-Object { [int]$_.Directory.Name.Substring(1) } -Descending
$toolsetFile = $toolsetFiles | Select-Object -First 1
if (-not $toolsetFile) {
    throw 'No installed MSVC x64 PlatformToolset was found.'
}
$toolset = $toolsetFile.Directory.Name
if ($Probe) {
    [pscustomobject]@{
        VisualStudioRoot = $vsRoot
        MSBuildPath = $msbuildPath
        PlatformToolset = $toolset
    }
    return
}

$projectPath = Join-Path $repoRoot 'EngineProgramPractices.vcxproj'
$outputDir = (Join-Path $repoRoot "bin/x64/$Configuration/").Replace('\', '/')
$intermediateDir = (Join-Path $repoRoot "obj/x64/$Configuration/").Replace('\', '/')
$buildArgs = @(
    $projectPath, '/nologo', '/m', '/verbosity:minimal', '/t:Build',
    "/p:Configuration=$Configuration", '/p:Platform=x64', "/p:PlatformToolset=$toolset",
    "/p:OutDir=$outputDir", "/p:IntDir=$intermediateDir"
)
Write-Host "Building $Configuration/x64 with $toolset"
& $msbuildPath @buildArgs
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

if ($Run) {
    $exePath = Join-Path $outputDir 'EngineProgramPractices.exe'
    if (-not (Test-Path -LiteralPath $exePath)) {
        throw "Build succeeded but executable was not found at $exePath"
    }
    Push-Location $repoRoot
    try {
        $runArguments = @()
        if ($Benchmark) { $runArguments += '--benchmark' }
        & $exePath @runArguments
        $testExitCode = $LASTEXITCODE
    }
    finally {
        Pop-Location
    }
    exit $testExitCode
}
