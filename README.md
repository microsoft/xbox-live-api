## Welcome!

The Microsoft Xbox Live Service API (XSAPI) enables game developers to access Xbox Live. To get access to the Xbox Live service, you can join the Xbox Live Creators Program at https://aka.ms/xblcp, or apply to the ID@Xbox program at: http://www.xbox.com/en-us/Developers/id

To learn more about these programs, please refer to the [developer program overview](https://docs.microsoft.com/en-us/windows/uwp/xbox-live/developer-program-overview).

## What's in the API:

*   Xbox Live Features - profile, social, presence, leaderboards, achievements, multiplayer, matchmaking, title storage
*   Xbox Live Authentication Library (XAL) public headers - Note that this repository does not contain full XAL source, it only contains XAL source files needed to support building with the Microsoft GDK.
*   Platforms - Microsoft GDK (targeting both PC and Console). Installing the Microsoft GDK is a prerequisite for building XSAPI. Additionally, source and projects for XDK and UWP platforms can be found at https://github.com/microsoft/xbox-live-api/tree/1807_xdk_qfe_preview
*   Support for Visual Studio 2017 and 2019

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

## How to link your project against source

You might want to link against the XSAPI source if you want to debug an issue, or understand where an error code is coming from.  How to do this can be found at [How to link your project against source](LINKTOSOURCE.md)

## Contribute Back!

Is there a feature missing that you'd like to see, or found a bug that you have a fix for? Or do you have an idea or just interest in helping out in building the library? Let us know and we'd love to work with you. For a good starting point on where we are headed and feature ideas, take a look at our [requested features and bugs](https://github.com/Microsoft/xbox-live-api/issues).  

Big or small we'd like to take your contributions back to help improve the Xbox Live Service API for everyone.

## Having Trouble?

We'd love to get your review score, whether good or bad, but even more than that, we want to fix your problem. If you submit your issue as a Review, we won't be able to respond to your problem and ask any follow-up questions that may be necessary. The most efficient way to do that is to open a an issue in our [issue tracker](https://github.com/Microsoft/xbox-live-api/issues).  The Xbox Live team will be engaged with the community and be continually improving our APIs, tools, and documentation based on the feedback received.

### Xbox Live GitHub projects
*   [Xbox Live Service API for C++](https://github.com/Microsoft/xbox-live-api)
*   [Xbox Live Samples (XDK/UWP)](https://github.com/Microsoft/xbox-live-samples)
*   [Xbox Live Resiliency Fiddler Plugin](https://github.com/Microsoft/xbox-live-resiliency-fiddler-plugin)
*   [Xbox Live Trace Analyzer](https://github.com/Microsoft/xbox-live-trace-analyzer)
*   [Xbox Live Developer Tools](https://github.com/Microsoft/xbox-live-developer-tools)
*   [libHttpClient](https://github.com/Microsoft/libHttpClient)

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/). For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.
