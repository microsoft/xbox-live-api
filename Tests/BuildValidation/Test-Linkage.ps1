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

    [string]$Toolset = '143',

    # Which libHttpClient the Thunks DLL is expected to import. Any accepts either
    # and only reports what it found, which is the right default for a developer
    # spot check. Source and InBox assert, which is what a build leg wants.
    [ValidateSet('Any', 'Source', 'InBox')]
    [string]$ExpectLibHttpClient = 'Any'
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
$gdkBin = $null
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
        $candidateBin = Join-Path $edition.FullName "windows\bin\$Platform"
        if (Test-Path $candidateInclude) { $gdkInclude = $candidateInclude }
        if (Test-Path $candidateLib) { $gdkLib = $candidateLib }
        if (Test-Path $candidateBin) { $gdkBin = $candidateBin }
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

# Start from an empty folder. Anything left from an earlier run, in particular a
# libHttpClient staged for a different linkage mode, would let the executable load
# against a stale dependency and report a pass that the current build did not earn.
if (Test-Path $outputDir) {
    Remove-Item $outputDir -Recurse -Force
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

# A cold build of a project that references libHttpClient does not always copy
# its outputs next to the XSAPI library, so fall back to libHttpClient's own
# output folder and build it on demand if it has not been produced yet.
function Resolve-LibHttpClientFile {
    param([Parameter(Mandatory)][string]$FileName)

    $candidates = @(
        (Join-Path $libDir $FileName),
        (Join-Path $repoRoot "External\Xal\External\libHttpClient\Out\$Platform\$Configuration\libHttpClient.GDK\$FileName")
    )

    foreach ($candidate in $candidates) {
        if (Test-Path $candidate) { return $candidate }
    }
    return $null
}

# The static configuration pulls libHttpClient symbols into the consumer, so it
# still links the source-built libHttpClient. The Thunks DLL now links the
# libHttpClient that ships in the GDK and keeps it behind the DLL boundary, so
# the dynamic consumer must not reference libHttpClient at all. That is the
# behavior a title sees, and linking it here would hide a regression.
$httpClientLib = $null
if ($Linkage -eq 'Static') {
    if (-not (Resolve-LibHttpClientFile 'libHttpClient.GDK.lib')) {
        Write-Host 'libHttpClient has not been built yet. Building it now...'
        $hcProject = Join-Path $repoRoot 'External\Xal\External\libHttpClient\Build\libHttpClient.GDK\libHttpClient.GDK.vcxproj'
        & msbuild $hcProject "/p:Configuration=$Configuration" "/p:Platform=$Platform" /v:minimal /nologo | Out-Null
        if ($LASTEXITCODE -ne 0) {
            Write-Error 'libHttpClient failed to build, so the link inputs could not be resolved.'
        }
    }

    $httpClientLib = Resolve-LibHttpClientFile 'libHttpClient.GDK.lib'
    if (-not $httpClientLib) {
        Write-Error 'libHttpClient.GDK.lib could not be located after building libHttpClient.'
    }
}

# Link the XSAPI library by full path. The GDK ships a library with the same
# name, and linking that one instead would defeat the purpose of the test.
$linkInputs = @(
    "/LIBPATH:$gdkLib",
    $xsapiLib
) + @($httpClientLib | Where-Object { $_ }) + @(
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
$thunksDll = Join-Path $libDir 'Microsoft.Xbox.Services.C.Thunks.dll'
Copy-Item $thunksDll $outputDir -Force

# Which libHttpClient the Thunks DLL imports depends on how it was built, so read
# it out of the binary rather than assuming. A source build imports
# libHttpClient.GDK.dll, and a build with LibHttpClientBuildFromSource=false
# imports the in-box libHttpClient.dll that ships in the GDK. A packaged title
# gets the in-box copy from the Gaming Runtime, but this consumer is a plain
# executable, so whichever one is needed has to be staged next to the exe.
$imports = dumpbin /dependents $thunksDll 2>&1 |
    Select-String -Pattern '^\s*(libHttpClient(\.GDK)?\.dll)\s*$' |
    ForEach-Object { $_.Matches[0].Groups[1].Value }

if (-not $imports) {
    Write-Error "$thunksDll imports no libHttpClient at all, so it was linked against neither the source build nor the GDK in-box copy."
}

foreach ($import in $imports) {
    if ($import -eq 'libHttpClient.dll') {
        Write-Host "The Thunks DLL imports the GDK in-box $import."
        $from = $gdkBin
    }
    else {
        Write-Host "The Thunks DLL imports the source-built $import."
        $from = $libDir
    }

    $dependency = if ($from) { Join-Path $from $import } else { $null }
    if ($dependency -and (Test-Path $dependency)) {
        Copy-Item $dependency $outputDir -Force
    }
    else {
        Write-Error "The Thunks DLL imports $import, but it was not found in $from. The in-box import library and DLL are present from GDK edition 251001 for x64 and from the 260400 release for arm64."
    }
}

if ($ExpectLibHttpClient -ne 'Any') {
    $expected = if ($ExpectLibHttpClient -eq 'InBox') { 'libHttpClient.dll' } else { 'libHttpClient.GDK.dll' }
    if ($imports -notcontains $expected) {
        Write-Error "Expected the Thunks DLL to import $expected, but it imports $($imports -join ', ')."
    }
    Write-Host "PASS: the Thunks DLL imports $expected as expected." -ForegroundColor Green
}

Write-Host 'Loading the Thunks DLL...'
$process = Start-Process -FilePath $exe -WorkingDirectory $outputDir -PassThru -Wait -NoNewWindow
if ($process.ExitCode -ne 0) {
    $code = '0x{0:X8}' -f $process.ExitCode
    $hint = switch ($code) {
        '0xC0000139' { 'An export named by the import library is missing from the DLL, which is exactly the drift this test exists to catch.' }
        '0xC0000135' { 'A dependent DLL could not be found. Confirm the GDK in-box libHttpClient.dll is alongside the executable.' }
        default      { 'The executable failed to start.' }
    }
    Write-Error "FAIL: the consumer exited with $code. $hint"
}

Write-Host 'PASS: the Thunks DLL loaded and every referenced export resolved.' -ForegroundColor Green
exit 0
