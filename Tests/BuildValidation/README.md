# Build validation

Checks that guard against defects that are easy to introduce and hard to notice.

## Check-ThunksExports.ps1

`Microsoft.Xbox.Services.C.Thunks.dll` exports the flat C API through the module
definition file at
`Build\Microsoft.Xbox.Services.GDK.C.Thunks\dll\Microsoft.Xbox.Services.GDK.C.Thunks.def`.
That file is generated from the headers in `Include\xsapi-c` by the
`Microsoft.Xbox.Services.ThunksGenerator` project, but nothing forces the two to
stay in sync — adding a new `STDAPI` to a header does not regenerate the `.def`.

When they drift, the missing APIs still compile and link, because the linker
resolves them from the static library instead. The result is a title that
unintentionally links both the dynamic and static XSAPI and hits a runtime
library mismatch, rather than a clear build error.

This script regenerates the `.def` from the current headers and compares the
export list to the checked-in copy:

```powershell
.\Tests\BuildValidation\Check-ThunksExports.ps1
```

It exits non-zero and names the affected APIs if the two disagree. Run it from a
Developer Command Prompt, or from a PowerShell session with the Visual Studio
development environment imported, so that `msbuild` is on the `PATH`.

After intentionally adding or removing a public API, update the checked-in file
and commit the result:

```powershell
.\Tests\BuildValidation\Check-ThunksExports.ps1 -Fix
```

Note that some APIs are declared in the headers but deliberately not exported —
platform-specific entry points and APIs that are not part of the GDK surface.
The generator owns that policy, so running it is the only accurate way to
compare the headers against the `.def`.
