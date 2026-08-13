<#
.SYNOPSIS
    Verifies that the Thunks .def file matches the public C API headers.

.DESCRIPTION
    Microsoft.Xbox.Services.C.Thunks.dll exports the flat C API through a module
    definition (.def) file. That file is generated from the headers under
    Include\xsapi-c by the ThunksGenerator project, but nothing forces the two to
    stay in sync: adding a new STDAPI to a header does not regenerate the .def.

    When they drift, the new API compiles and links against the static library
    but is simply absent from the DLL's export table. A title using the dynamic
    XSAPI then falls back to the static library for those symbols, which produces
    a runtime library mismatch rather than an obvious error.

    This script regenerates the .def from the current headers and compares the
    result to the checked-in copy. Any difference is reported as a failure, so
    the drift is caught at build time instead of by a game developer.

.PARAMETER Fix
    Keep the regenerated .def instead of restoring the original. Use this to
    update the checked-in file after intentionally adding or removing an API.

.EXAMPLE
    .\Check-ThunksExports.ps1
    Fails if the .def no longer matches the headers.

.EXAMPLE
    .\Check-ThunksExports.ps1 -Fix
    Regenerates the .def so the change can be committed.
#>
[CmdletBinding()]
param(
    [switch]$Fix
)

$ErrorActionPreference = 'Stop'

$repoRoot      = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$thunksRoot    = Join-Path $repoRoot 'Build\Microsoft.Xbox.Services.GDK.C.Thunks'
$defPath       = Join-Path $thunksRoot 'dll\Microsoft.Xbox.Services.GDK.C.Thunks.def'
$generatorDir  = Join-Path $thunksRoot 'generator\ThunksGenerator'
$generatorProj = Join-Path $generatorDir 'ThunksGenerator.csproj'

foreach ($required in @($defPath, $generatorProj)) {
    if (-not (Test-Path $required)) {
        Write-Error "Expected file not found: $required"
    }
}

if (-not (Get-Command msbuild -ErrorAction SilentlyContinue)) {
    Write-Error @'
msbuild was not found on PATH. Run this script from a Developer Command Prompt,
or from PowerShell after importing the Visual Studio development environment.
'@
}

Write-Host 'Building ThunksGenerator...'
# -restore is required because the project has not been restored in a clean clone.
$buildLog = & msbuild $generatorProj -restore /p:Configuration=Debug /v:quiet /nologo 2>&1
if ($LASTEXITCODE -ne 0) {
    $buildLog | Write-Host
    Write-Error "Failed to build ThunksGenerator (exit code $LASTEXITCODE)."
}

$generatorExe = Join-Path $generatorDir 'bin\Debug\ThunksGenerator.exe'
if (-not (Test-Path $generatorExe)) {
    Write-Error "ThunksGenerator built but the executable was not found at $generatorExe."
}

$original = Get-Content $defPath -Raw

Write-Host 'Regenerating the .def from the current headers...'
try {
    # The generator locates the .def with a path relative to its own directory,
    # so it has to run with that directory as the working directory.
    Push-Location (Split-Path $generatorExe -Parent)
    $generatorLog = & $generatorExe 2>&1
    $generatorExit = $LASTEXITCODE
}
finally {
    Pop-Location
}

if ($generatorExit -ne 0) {
    $generatorLog | Write-Host
    Set-Content -Path $defPath -Value $original -NoNewline
    Write-Error "ThunksGenerator failed (exit code $generatorExit). The .def was left unchanged."
}

$regenerated = Get-Content $defPath -Raw

# Compare the exported names rather than the raw text. The generator writes LF
# line endings, so a byte-for-byte comparison would report a difference on any
# checkout that stores the file with CRLF.
function Get-Exports([string]$content) {
    $content -split "`r?`n" |
        ForEach-Object { $_.Trim() } |
        Where-Object { $_ -and $_ -ne 'EXPORTS' }
}

$before = Get-Exports $original
$after  = Get-Exports $regenerated

$missing = @($after  | Where-Object { $_ -notin $before })
$extra   = @($before | Where-Object { $_ -notin $after })

if ($missing.Count -eq 0 -and $extra.Count -eq 0) {
    # Restore the original bytes so the checkout stays clean when only the
    # line endings differed.
    Set-Content -Path $defPath -Value $original -NoNewline
    Write-Host "PASS: the .def matches the public headers ($($after.Count) exports)." -ForegroundColor Green
    exit 0
}

Write-Host ''
Write-Host 'FAIL: the .def no longer matches the public headers.' -ForegroundColor Red

if ($missing.Count -gt 0) {
    Write-Host ''
    Write-Host "Declared in the headers but not exported ($($missing.Count)):" -ForegroundColor Yellow
    $missing | ForEach-Object { Write-Host "    $_" }
}

if ($extra.Count -gt 0) {
    Write-Host ''
    Write-Host "Exported but no longer declared in the headers ($($extra.Count)):" -ForegroundColor Yellow
    $extra | ForEach-Object { Write-Host "    $_" }
}

if ($Fix) {
    Write-Host ''
    Write-Host "Updated $defPath. Review and commit the change." -ForegroundColor Cyan
    exit 0
}

Set-Content -Path $defPath -Value $original -NoNewline
Write-Host ''
Write-Host 'Re-run with -Fix to update the .def, then commit the result.'
exit 1
