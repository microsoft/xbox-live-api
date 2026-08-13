## Welcome!

The Microsoft Xbox Live Service API (XSAPI) enables game developers to access Xbox Live. To get access to the Xbox Live service, you can join the Xbox Live Creators Program at https://aka.ms/xblcp, or apply to the ID@Xbox program at: http://www.xbox.com/en-us/Developers/id

To learn more about these programs, please refer to the [developer program overview](https://docs.microsoft.com/en-us/windows/uwp/xbox-live/developer-program-overview).

## What's in the API:

*   Xbox Live Features - profile, social, presence, leaderboards, achievements, multiplayer, matchmaking, title storage
*   Xbox Live Authentication Library (XAL) public headers - Note that this repository does not contain full XAL source, it only contains XAL source files needed to support building with the Microsoft GDK.
*   Platforms - Microsoft GDK (targeting both PC and Console). Installing the Microsoft GDK is a prerequisite for building XSAPI. Additionally, source and projects for XDK and UWP platforms can be found at https://github.com/microsoft/xbox-live-api/tree/1807_xdk_qfe_preview
*   Support for Visual Studio 2019 (v142) and Visual Studio 2022 (v143)

## How to build

Install the [Microsoft GDK](https://github.com/microsoft/GDK) first - it is a prerequisite, and the build resolves headers and libraries from the installed GDK.

Open the solution matching your toolset and build the library project you need:

| Visual Studio | Solution | Static library project |
| --- | --- | --- |
| 2019 (v142) | `Microsoft.Xbox.Services.GDK.VS2019.sln` | `Microsoft.Xbox.Services.142.GDK.C` |
| 2022 (v143) | `Microsoft.Xbox.Services.GDK.VS2022.sln` | `Microsoft.Xbox.Services.143.GDK.C` |

Build for the **`x64`** or **`ARM64`** platform. These are the only platforms the projects define - do not select a `Gaming.Desktop.*` or `Gaming.Xbox.*` platform, as those are not configured here and will fail to build.

From the command line:

    msbuild Build\Microsoft.Xbox.Services.143.GDK.C\Microsoft.Xbox.Services.143.GDK.C.vcxproj /p:Configuration=Debug /p:Platform=x64

Build output goes to `Bins\Binaries\<Configuration>\<Platform>\<ProjectName>\`, so the command above writes to `Bins\Binaries\Debug\x64\Microsoft.Xbox.Services.143.GDK.C\`.

### Building the Thunks DLL

**Linking XSAPI statically is the recommended configuration and is what most titles should use.** The Thunks DLL exists for titles that cannot link statically, or prefer not to.

The usual reason is the C runtime. The static library links the C runtime dynamically (`/MD`), so a title that uses the static C runtime (`/MT`) cannot link it without a runtime library mismatch. The Thunks DLL puts XSAPI behind a DLL boundary, which isolates its C runtime from your title's.

    msbuild Build\Microsoft.Xbox.Services.GDK.C.Thunks\Microsoft.Xbox.Services.GDK.C.Thunks.vcxproj /p:Configuration=Debug /p:Platform=x64

This produces `Microsoft.Xbox.Services.C.Thunks.dll` and its import library `Microsoft.Xbox.Services.C.Thunks.lib` in `Bins\Binaries\Debug\x64\Microsoft.Xbox.Services.GDK.C.Thunks\`. Link against the import library and ship the DLL alongside your title.

Note that the Microsoft GDK also ships a `Microsoft.Xbox.Services.C.Thunks.lib` under the same name. Link against the one you built here by full path, or the linker may quietly pick up the other.

The DLL depends on `libHttpClient.GDK.dll`, which is built by a separate project and must be deployed with it:

    msbuild External\Xal\External\libHttpClient\Build\libHttpClient.GDK\libHttpClient.GDK.vcxproj /p:Configuration=Debug /p:Platform=x64

That project writes to `External\Xal\External\libHttpClient\Out\<Platform>\<Configuration>\libHttpClient.GDK\` rather than to `Bins`.

Note that the Thunks DLL exports the XSAPI **C** API (`xsapi-c`) only. It does not export the C++ wrapper (`xsapi-cpp`), which is header-only and compiles into your title.

The exported function list is generated from the public headers by the `Microsoft.Xbox.Services.ThunksGenerator` project. That project is C#, so it does not restore automatically as part of a native solution build - pass `-restore` when building it directly:

    msbuild Build\Microsoft.Xbox.Services.GDK.C.Thunks\generator\ThunksGenerator\ThunksGenerator.csproj -restore

### Validating a build

`Tests\BuildValidation` contains checks that confirm a build exposes the full public API surface, and that the Thunks DLL's exports have not drifted from the headers. See [Tests/BuildValidation/README.md](Tests/BuildValidation/README.md).

## How to use the Xbox Live Services API (XSAPI)

The best way to learn the API and see the best practices is to look at the Xbox Live samples that ship with the Microsoft GDK, and the [Xbox Live developer docs](https://docs.microsoft.com/en-us/windows/uwp/xbox-live/)

## How to clone repo

This repo contains submodules.  There are two ways to make sure you get submodules.

When initially cloning, make sure you use the `--recursive` option. IE:

    git clone --recursive https://github.com/Microsoft/xbox-live-api.git

If you already cloned the repo, you can initialize submodules with:

    git submodule sync
    git submodule update --init --recursive

**Note that using GitHub's feature to "Download Zip" does not contain the submodules and will not properly build.  Please clone recursively instead.**

### Long paths on Windows

Some nested submodules contain paths longer than the legacy 260 character limit. If long path support is not enabled, the recursive clone fails partway through with `Filename too long` and leaves the submodules incompletely checked out:

    error: unable to create file ...: Filename too long
    fatal: Unable to checkout '...' in submodule path 'External/Xal/External/libHttpClient'

Enable long paths before cloning:

    git config --global core.longpaths true

If you already hit the error, enable the setting and then re-run the submodule update to finish the checkout:

    git config --global core.longpaths true
    git submodule update --init --recursive

## How to link your project against source

You might want to link against the XSAPI source if you want to debug an issue, or understand where an error code is coming from.  How to do this can be found at [How to link your project against source](LINKTOSOURCE.md)

## Contribute Back!

Is there a feature missing that you'd like to see, or found a bug that you have a fix for? Or do you have an idea or just interest in helping out in building the library? Let us know and we'd love to work with you. For a good starting point on where we are headed and feature ideas, take a look at our [requested features and bugs](https://github.com/Microsoft/xbox-live-api/issues).  

Big or small we'd like to take your contributions back to help improve the Xbox Live Service API for everyone.

## Having Trouble?

We'd love to get your review score, whether good or bad, but even more than that, we want to fix your problem. If you submit your issue as a Review, we won't be able to respond to your problem and ask any follow-up questions that may be necessary. The most efficient way to do that is to open an issue in our [issue tracker](https://github.com/Microsoft/xbox-live-api/issues).  The Xbox Live team will be engaged with the community and be continually improving our APIs, tools, and documentation based on the feedback received.

### Xbox Live GitHub projects
*   [Xbox Live Service API for C++](https://github.com/Microsoft/xbox-live-api)
*   [Xbox Live Samples (XDK/UWP)](https://github.com/Microsoft/xbox-live-samples)
*   [Xbox Live Resiliency Fiddler Plugin](https://github.com/Microsoft/xbox-live-resiliency-fiddler-plugin)
*   [Xbox Live Trace Analyzer](https://github.com/Microsoft/xbox-live-trace-analyzer)
*   [Xbox Live Developer Tools](https://github.com/Microsoft/xbox-live-developer-tools)
*   [libHttpClient](https://github.com/Microsoft/libHttpClient)

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/). For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.
