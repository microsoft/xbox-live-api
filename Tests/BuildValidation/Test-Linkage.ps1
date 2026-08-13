<#
.SYNOPSIS
    Builds a small consumer against XSAPI to verify the public API surface links.

.DESCRIPTION
    Compiles and links Tests\BuildValidation\BuildValidation.cpp against a build
    of XSAPI produced from this repository. The consumer references a
    representative set of public APIs, so the linker has to resolve each one.
    That catches APIs which are declared in the headers but are not actually
    present in the library or, for the Thunks DLL, are missing from its export
    table.

    Static linking is the recommended configuration for most titles and is the
    default here. The Thunks DLL exists for titles that cannot or prefer not to
    link statically, so it is validated on request with -Linkage Dynamic. The
    dynamic run also launches the executable, which is what proves the exports
    resolve at load time rather than only at link time.

.PARAMETER Linkage
    Static links the static library and is the default. Dynamic links the Thunks
    import library and then runs the executable.

.PARAMETER Configuration
    Build configuration to validate against. Defaults to Debug.

.PARAMETER Platform
    Platform to validate against. Defaults to x64.

.PARAMETER Toolset
    Toolset version of the static library to link. Defaults to 143.

.EXAMPLE
    .\Test-Linkage.ps1
    Validates the static library, which is the recommended configuration.

.EXAMPLE
    .\Test-Linkage.ps1 -Linkage Dynamic
    Validates the Thunks DLL, including that it loads and its exports resolve.
#>
[CmdletBinding()]
param(
    [ValidateSet('Static', 'Dynamic')]
    [string]$Linkage = 'Static',

    [ValidateSet('Debug', 'Release')]
    [string]$Configuration = 'Debug',

    [ValidateSet('x64', 'ARM64')]
    [string]$Platform = 'x64',

    [string]$Toolset = '143'
)

$ErrorActionPreference = 'Stop'

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$source   = Join-Path $PSScriptRoot 'BuildValidation.cpp'
$binaries = Join-Path $repoRoot "Bins\Binaries\$Configuration\$Platform"
$outputDir = Join-Path $repoRoot "Bins\BuildValidation\$Configuration\$Platform\$Linkage"

if (-not (Get-Command cl -ErrorAction SilentlyContinue)) {
    Write-Error @'
cl.exe was not found on PATH. Run this script from a Developer Command Prompt,
or from PowerShell after importing the Visual Studio development environment.
'@
}

if (-not (Test-Path $binaries)) {
    Write-Error @"
No XSAPI build was found at $binaries.
Build the appropriate project first, for example:
    msbuild Build\Microsoft.Xbox.Services.$Toolset.GDK.C\Microsoft.Xbox.Services.$Toolset.GDK.C.vcxproj /p:Configuration=$Configuration /p:Platform=$Platform
"@
}

$gdkInclude = $null
$gdkLib = $null
if ($env:GameDK) {
    # GameDK points at the install root, which contains one folder per installed
    # GDK edition. Use the newest edition present.
    $edition = Get-ChildItem $env:GameDK -Directory -ErrorAction SilentlyContinue |
        Where-Object { $_.Name -match '^\d+$' } |
        Sort-Object { [int]$_.Name } |
        Select-Object -Last 1

    if ($edition) {
        $candidateInclude = Join-Path $edition.FullName 'windows\include'
        $candidateLib = Join-Path $edition.FullName "windows\lib\$Platform"
        if (Test-Path $candidateInclude) { $gdkInclude = $candidateInclude }
        if (Test-Path $candidateLib) { $gdkLib = $candidateLib }
    }
}
if (-not $gdkInclude -or -not $gdkLib) {
    Write-Error 'The GDK was not found. Ensure the Microsoft GDK is installed and the GameDK environment variable is set.'
}

# The static library and the Thunks DLL are produced by different projects, so
# each configuration has its own output folder and link inputs.
if ($Linkage -eq 'Static') {
    $libDir = Join-Path $binaries "Microsoft.Xbox.Services.$Toolset.GDK.C"
    $xsapiLib = Join-Path $libDir "Microsoft.Xbox.Services.$Toolset.C.lib"
    $defines = @()
}
else {
    $libDir = Join-Path $binaries 'Microsoft.Xbox.Services.GDK.C.Thunks'
    $xsapiLib = Join-Path $libDir 'Microsoft.Xbox.Services.C.Thunks.lib'
    $defines = @('/DXSAPI_BUILD_VALIDATION_DYNAMIC')
}

if (-not (Test-Path $xsapiLib)) {
    Write-Error @"
Expected to link $xsapiLib but it was not found.
Build the $Linkage configuration first, then re-run this script.
"@
}

New-Item -ItemType Directory -Force -Path $outputDir | Out-Null

$exe = Join-Path $outputDir 'BuildValidation.exe'

# The static library is built against the dynamic CRT, so a consumer has to
# match it. The Thunks DLL keeps its CRT behind the DLL boundary, which is one
# of the reasons to choose it, so the consumer is free to use the static CRT.
if ($Linkage -eq 'Static') {
    $crt = if ($Configuration -eq 'Debug') { '/MDd' } else { '/MD' }
}
else {
    $crt = if ($Configuration -eq 'Debug') { '/MTd' } else { '/MT' }
}

# Link the XSAPI library by full path. The GDK ships a library with the same
# name, and linking that one instead would defeat the purpose of the test.
$linkInputs = @(
    "/LIBPATH:$gdkLib",
    $xsapiLib,
    (Join-Path $libDir 'libHttpClient.GDK.lib'),
    'xgameruntime.lib',
    'appnotify.lib',
    'winhttp.lib',
    'crypt32.lib',
    'ole32.lib',
    'advapi32.lib'
)

Write-Host "Validating the $($Linkage.ToLower()) configuration ($Configuration|$Platform)..."

$clArgs = @(
    '/nologo', '/EHsc', '/std:c++17', $crt,
    # The repository's own Xal and libHttpClient headers come first, because the
    # copies shipped in the GDK are built for a different platform flavor.
    "/I$(Join-Path $repoRoot 'External\Xal\Source\Xal\Include')",
    "/I$(Join-Path $repoRoot 'External\Xal\External\libHttpClient\Include')",
    "/I$(Join-Path $repoRoot 'Include')",
    "/I$gdkInclude"
) + $defines + @(
    $source,
    "/Fo$outputDir\",
    "/Fe$exe",
    '/link',
    '/NOLOGO'
) + $linkInputs

$buildLog = & cl @clArgs 2>&1
if ($LASTEXITCODE -ne 0) {
    $buildLog | Write-Host
    Write-Error @"
FAIL: the $($Linkage.ToLower()) configuration did not link.
Unresolved external symbols here mean the API is declared in the headers but is
missing from the library or, for the Thunks DLL, from its export table.
"@
}

Write-Host 'Link succeeded.' -ForegroundColor Green

if ($Linkage -eq 'Static') {
    Write-Host 'PASS: the static library exposes every referenced API.' -ForegroundColor Green
    exit 0
}

# Linking against the import library only proves the .def lists the export. The
# executable has to load for the export to be proven present in the DLL itself.
Copy-Item (Join-Path $libDir 'Microsoft.Xbox.Services.C.Thunks.dll') $outputDir -Force
$httpClientDll = Join-Path $libDir 'libHttpClient.GDK.dll'
if (Test-Path $httpClientDll) {
    Copy-Item $httpClientDll $outputDir -Force
}

Write-Host 'Loading the Thunks DLL...'
$process = Start-Process -FilePath $exe -WorkingDirectory $outputDir -PassThru -Wait -NoNewWindow
if ($process.ExitCode -ne 0) {
    $code = '0x{0:X8}' -f $process.ExitCode
    $hint = switch ($code) {
        '0xC0000139' { 'An export named by the import library is missing from the DLL, which is exactly the drift this test exists to catch.' }
        '0xC0000135' { 'A dependent DLL could not be found. Confirm libHttpClient.GDK.dll is alongside the executable.' }
        default      { 'The executable failed to start.' }
    }
    Write-Error "FAIL: the consumer exited with $code. $hint"
}

Write-Host 'PASS: the Thunks DLL loaded and every referenced export resolved.' -ForegroundColor Green
exit 0
