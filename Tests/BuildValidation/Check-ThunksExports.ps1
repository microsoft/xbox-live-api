<#
.SYNOPSIS
    Verifies that the Thunks .def file still matches the public C API headers.

.DESCRIPTION
    Microsoft.Xbox.Services.C.Thunks.dll exports the flat C API through a module
    definition (.def) file. That file is generated from the headers under
    Include\xsapi-c, but nothing forces the two to stay in sync: adding a new
    STDAPI to a header does not regenerate the .def, and a stale .def is still
    perfectly valid linker input. It simply exports fewer symbols, so the build
    stays green while the exported API surface drifts.

    A title using the dynamic XSAPI then silently falls back to the static
    library for the missing symbols, which surfaces as a runtime library
    mismatch at link time rather than as an obvious missing-export error.

    This script regenerates the .def from the current headers and compares the
    result to the checked-in copy, so the drift is caught at build time instead
    of by a game developer.

    Note that the generator used here is the one under
    Build\Microsoft.Xbox.Services.GDK.C.Thunks\generator. The similarly named
    Microsoft.Xbox.Services.ThunksGenerator project generates the Win32 .def and
    is not interchangeable with it.

    Requires msbuild on the PATH. Run from a Developer Command Prompt, or from a
    PowerShell session with the Visual Studio development environment imported.

.PARAMETER Fix
    Keep the regenerated .def instead of restoring the original. Use this to
    update the checked-in file after intentionally adding or removing an API.

.PARAMETER SelfTest
    Additionally verify that the check actually reports drift, by running the
    same comparison and pass/fail decision against a deliberately damaged export
    list. This guards against the check silently degrading into one that always
    passes.

.EXAMPLE
    .\Check-ThunksExports.ps1
    Fails if the .def no longer matches the headers.

.EXAMPLE
    .\Check-ThunksExports.ps1 -Fix
    Regenerates the .def so the change can be committed.
#>
[CmdletBinding()]
param(
    [switch]$Fix,
    [switch]$SelfTest
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

# Compare the exported names rather than the raw text. The generator writes LF
# line endings, so a byte-for-byte comparison would report a difference on any
# checkout that stores the file with CRLF.
function Get-Exports([string]$content) {
    $content -split "`r?`n" |
        ForEach-Object { $_.Trim() } |
        Where-Object { $_ -and $_ -ne 'EXPORTS' }
}

function Compare-Exports($expected, $actual) {
    [pscustomobject]@{
        Missing = @($expected | Where-Object { $_ -notin $actual })
        Extra   = @($actual   | Where-Object { $_ -notin $expected })
    }
}

# The pass/fail decision, kept in one place so the self-test can exercise the
# same code the real check runs. Asserting only on the comparison would leave
# this decision untested, and a change that made it always return success would
# disable the check without failing anything.
function Get-DriftExitCode($diff) {
    if ($diff.Missing.Count -eq 0 -and $diff.Extra.Count -eq 0) {
        return 0
    }
    return 1
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

$before = @(Get-Exports $original)
$after  = @(Get-Exports $regenerated)

if ($SelfTest) {
    # Drop one export from the generated list, then require both that the
    # comparison reports it and that the pass/fail decision treats it as failure.
    if ($after.Count -lt 2) {
        Write-Error "SELF-TEST FAILED: only $($after.Count) export(s) were generated, which is not a usable baseline."
    }

    $damaged  = @($after | Select-Object -Skip 1)
    $selfDiff = Compare-Exports -expected $after -actual $damaged

    if ($selfDiff.Missing.Count -ne 1 -or $selfDiff.Missing[0] -ne $after[0]) {
        Write-Error "SELF-TEST FAILED: removing '$($after[0])' was not reported as drift. The check is not detecting missing exports."
    }

    if ((Get-DriftExitCode $selfDiff) -ne 1) {
        Write-Error 'SELF-TEST FAILED: drift was detected but the check did not treat it as a failure.'
    }

    Write-Host 'Self-test passed: a removed export is reported as drift and fails the check.' -ForegroundColor Green
}

$diff = Compare-Exports -expected $after -actual $before

if ((Get-DriftExitCode $diff) -eq 0) {
    # Restore the original bytes so the checkout stays clean when only the
    # line endings differed.
    Set-Content -Path $defPath -Value $original -NoNewline
    Write-Host "PASS: the .def matches the public headers ($($after.Count) exports)." -ForegroundColor Green
    exit 0
}

Write-Host ''
Write-Host 'FAIL: the .def no longer matches the public headers.' -ForegroundColor Red
Write-Host 'Run Tests\BuildValidation\Check-ThunksExports.ps1 -Fix and commit the regenerated .def.'

if ($diff.Missing.Count -gt 0) {
    Write-Host ''
    Write-Host "Declared in the headers but not exported ($($diff.Missing.Count)):" -ForegroundColor Yellow
    $diff.Missing | ForEach-Object { Write-Host "    $_" }
}

if ($diff.Extra.Count -gt 0) {
    Write-Host ''
    Write-Host "Exported but no longer declared in the headers ($($diff.Extra.Count)):" -ForegroundColor Yellow
    $diff.Extra | ForEach-Object { Write-Host "    $_" }
}

if ($Fix) {
    Write-Host ''
    Write-Host "Updated $defPath. Review and commit the change." -ForegroundColor Cyan
    exit 0
}

Set-Content -Path $defPath -Value $original -NoNewline
exit 1
