# Build validation

Checks that a build of XSAPI exposes the full public API surface. They exist to
catch a specific class of defect: an API that is declared in the headers but is
not actually reachable from the built library.

Both checks need `msbuild` and `cl` on the `PATH`. Run them from a Developer
Command Prompt, or from a PowerShell session with the Visual Studio development
environment imported.

## Test-Linkage.ps1

Compiles `BuildValidation.cpp` against a build of XSAPI produced from this
repository and links it. The consumer references a representative set of public
APIs, so the linker has to resolve each one.

Static linking is the recommended configuration for most titles, so it is the
default:

```powershell
.\Tests\BuildValidation\Test-Linkage.ps1
```

The Thunks DLL exists for titles that cannot link statically, or prefer not to,
so it is validated on request:

```powershell
.\Tests\BuildValidation\Test-Linkage.ps1 -Linkage Dynamic
```

The dynamic run also launches the executable. Linking only proves the import
library lists the export; loading proves the export is present in the DLL
itself, and an exit code of `0xC0000139` means it is not.

Build the configuration under test first:

```powershell
# Static
msbuild Build\Microsoft.Xbox.Services.143.GDK.C\Microsoft.Xbox.Services.143.GDK.C.vcxproj /p:Configuration=Debug /p:Platform=x64

# Dynamic
msbuild Build\Microsoft.Xbox.Services.GDK.C.Thunks\Microsoft.Xbox.Services.GDK.C.Thunks.vcxproj /p:Configuration=Debug /p:Platform=x64
```

`-Configuration`, `-Platform`, and `-Toolset` are also available. Note that the
static library is built against the dynamic C runtime, so the static
configuration compiles the consumer with `/MD`. The dynamic configuration uses
`/MT`, because the Thunks DLL keeps its own C runtime behind the DLL boundary.

## Check-ThunksExports.ps1

`Microsoft.Xbox.Services.C.Thunks.dll` exports the flat C API through the module
definition file at
`Build\Microsoft.Xbox.Services.GDK.C.Thunks\dll\Microsoft.Xbox.Services.GDK.C.Thunks.def`.
That file is generated from the headers in `Include\xsapi-c` by the
`Microsoft.Xbox.Services.ThunksGenerator` project, but nothing forces the two to
stay in sync - adding a new `STDAPI` to a header does not regenerate the `.def`.

When they drift, the missing APIs still compile and link, because the linker
resolves them from the static library instead. The result is a title that
unintentionally links both the dynamic and static XSAPI and hits a runtime
library mismatch, rather than a clear build error.

This script regenerates the `.def` from the current headers and compares the
export list to the checked-in copy:

```powershell
.\Tests\BuildValidation\Check-ThunksExports.ps1
```

It exits non-zero and names the affected APIs if the two disagree. After
intentionally adding or removing a public API, update the checked-in file and
commit the result:

```powershell
.\Tests\BuildValidation\Check-ThunksExports.ps1 -Fix
```

Some APIs are declared in the headers but deliberately not exported -
platform-specific entry points and APIs that are not part of the GDK surface.
The generator owns that policy, so running it is the only accurate way to
compare the headers against the `.def`.
