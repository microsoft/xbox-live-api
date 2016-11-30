if "%XES_SERIALPOSTBUILDREADY%" == "True" goto serializeForPostbuild
goto done
:serializeForPostbuild

echo Running postBuildScript.cmd
echo on

if "%BUILD_DEFINITIONNAME%" == "XSAPI_Rolling" goto finalize

setlocal
call %TFS_SourcesDirectory%\Utilities\VSOBuildScripts\setBuildVersion.cmd

rem format release numbers
FOR /F "TOKENS=1 eol=/ DELIMS=. " %%A IN ("%TFS_VersionNumber%") DO SET SDK_POINT_NAME_YEARMONTH=%%A
FOR /F "TOKENS=2 eol=/ DELIMS=. " %%A IN ("%TFS_VersionNumber%") DO SET SDK_POINT_NAME_DAYVER=%%A
set SDK_POINT_NAME_YEAR=%SDK_POINT_NAME_YEARMONTH:~0,2%
set SDK_POINT_NAME_MONTH=%SDK_POINT_NAME_YEARMONTH:~2,2%
set SDK_POINT_NAME_DAY=%SDK_POINT_NAME_DAYVER:~0,2%
set SDK_POINT_NAME_VER=%SDK_POINT_NAME_DAYVER:~2,9%
set SDK_RELEASE_NAME=%SDK_RELEASE_YEAR:~2,2%%SDK_RELEASE_MONTH%
set LONG_SDK_RELEASE_NAME=%SDK_RELEASE_NAME%-%SDK_POINT_NAME_YEAR%%SDK_POINT_NAME_MONTH%%SDK_POINT_NAME_DAY%-%SDK_POINT_NAME_VER%
set NUGET_VERSION_NUMBER=%SDK_RELEASE_YEAR%.%SDK_RELEASE_MONTH%.%SDK_POINT_NAME_YEAR%%SDK_POINT_NAME_MONTH%%SDK_POINT_NAME_DAY%.%SDK_POINT_NAME_VER%

set

xcopy %TFS_SourcesDirectory%\Utilities\Nuget\* %TFS_DropLocation%\Nuget\ /s /e /q /y
robocopy /NJS /NJH /MT:16 /S /NP %TFS_SourcesDirectory%\Include %TFS_DropLocation%\Include
robocopy /NJS /NJH /MT:16 /S /NP %TFS_SourcesDirectory%\External\cpprestsdk\Release\include %TFS_DropLocation%\include

:skipTest

rem setup folders for XDK drop
set XDK_BINARIES_DROP=%TFS_DropLocation%\xdkservicesapi
set UWP_BUILD_SHARE=%TFS_DropLocation%
set XDK_BUILD_SHARE=%TFS_DropLocation%

set XDK_BIN_BUILD_SHARE_RELEA=%XDK_BUILD_SHARE%\Release\x64
set XDK_BIN_BUILD_SHARE_DEBUG=%XDK_BUILD_SHARE%\Debug\x64
set UWP_BIN_BUILD_SHARE_RELEA=%UWP_BUILD_SHARE%\Release\x64
set UWP_BIN_BUILD_SHARE_DEBUG=%UWP_BUILD_SHARE%\Debug\x64

mkdir %XDK_BINARIES_DROP%

rem copy NuGetPackages to build output folder
mkdir %TFS_DropLocation%\SDK\Binaries
copy %UWP_BUILD_SHARE%\NuGetBinaries\*.nupkg %TFS_DropLocation%\SDK\Binaries

rem copy includes to build output folder
robocopy /NJS /NJH /MT:16 /S /NP %UWP_BUILD_SHARE%\Include %XDK_BINARIES_DROP%\cpp\include

rem copy binaries to build output folder
copy %XDK_BIN_BUILD_SHARE_RELEA%\Microsoft.Xbox.Services.140.XDK.WinRT\Microsoft.Xbox.Services.dll %XDK_BINARIES_DROP%\winrt\binaries\CommonConfiguration\Microsoft.Xbox.Services.dll
copy %XDK_BIN_BUILD_SHARE_DEBUG%\Microsoft.Xbox.Services.140.XDK.WinRT\Microsoft.Xbox.Services.dll %XDK_BINARIES_DROP%\winrt\binaries\Debug\Microsoft.Xbox.Services.dll
copy %XDK_BIN_BUILD_SHARE_RELEA%\Microsoft.Xbox.Services.140.XDK.WinRT\Microsoft.Xbox.Services.winmd %XDK_BINARIES_DROP%\winrt\binaries\CommonConfiguration\
copy %XDK_BIN_BUILD_SHARE_DEBUG%\Microsoft.Xbox.Services.140.XDK.WinRT\Microsoft.Xbox.Services.winmd %XDK_BINARIES_DROP%\winrt\binaries\Debug\
copy %XDK_BIN_BUILD_SHARE_DEBUG%\Microsoft.Xbox.Services.140.XDK.WinRT\Microsoft.Xbox.Services.pdb %XDK_BINARIES_DROP%\winrt\binaries\Debug\Microsoft.Xbox.Services.pdb
copy %XDK_BIN_BUILD_SHARE_RELEA%\Microsoft.Xbox.Services.140.XDK.WinRT\Microsoft.Xbox.Services.pdb %XDK_BINARIES_DROP%\winrt\binaries\CommonConfiguration\Microsoft.Xbox.Services.pdb

copy %XDK_BIN_BUILD_SHARE_RELEA%\Microsoft.Xbox.Services.110.XDK.Ship.Cpp\Microsoft.Xbox.Services.110.XDK.Ship.Cpp.lib %XDK_BINARIES_DROP%\cpp\binaries\release\v110\Microsoft.Xbox.Services.110.XDK.Ship.Cpp.lib.remove
copy %XDK_BIN_BUILD_SHARE_DEBUG%\Microsoft.Xbox.Services.110.XDK.Ship.Cpp\Microsoft.Xbox.Services.110.XDK.Ship.Cpp.lib %XDK_BINARIES_DROP%\cpp\binaries\debug\v110\Microsoft.Xbox.Services.110.XDK.Ship.Cpp.lib.remove
copy %XDK_BIN_BUILD_SHARE_RELEA%\Microsoft.Xbox.Services.110.XDK.Ship.Cpp\Microsoft.Xbox.Services.110.XDK.Ship.Cpp.pdb %XDK_BINARIES_DROP%\cpp\binaries\release\v110\Microsoft.Xbox.Services.110.XDK.Ship.Cpp.pdb
copy %XDK_BIN_BUILD_SHARE_DEBUG%\Microsoft.Xbox.Services.110.XDK.Ship.Cpp\Microsoft.Xbox.Services.110.XDK.Ship.Cpp.pdb %XDK_BINARIES_DROP%\cpp\binaries\debug\v110\Microsoft.Xbox.Services.110.XDK.Ship.Cpp.pdb

copy %XDK_BIN_BUILD_SHARE_RELEA%\casablanca110.Xbox\casablanca110.xbox.lib %XDK_BINARIES_DROP%\cpp\binaries\release\v110\casablanca110.xbox.lib.remove
copy %XDK_BIN_BUILD_SHARE_DEBUG%\casablanca110.Xbox\casablanca110.xbox.lib %XDK_BINARIES_DROP%\cpp\binaries\debug\v110\casablanca110.xbox.lib.remove
copy %XDK_BIN_BUILD_SHARE_RELEA%\casablanca110.Xbox\casablanca110.xbox.pdb %XDK_BINARIES_DROP%\cpp\binaries\release\v110\casablanca110.xbox.pdb
copy %XDK_BIN_BUILD_SHARE_DEBUG%\casablanca110.Xbox\casablanca110.xbox.pdb %XDK_BINARIES_DROP%\cpp\binaries\debug\v110\casablanca110.xbox.pdb

copy %UWP_BIN_BUILD_SHARE_RELEA%\Microsoft.Xbox.Services.140.XDK.Ship.Cpp\Microsoft.Xbox.Services.140.XDK.Ship.Cpp.lib %XDK_BINARIES_DROP%\cpp\binaries\release\v140\Microsoft.Xbox.Services.140.XDK.Ship.Cpp.lib.remove
copy %UWP_BIN_BUILD_SHARE_DEBUG%\Microsoft.Xbox.Services.140.XDK.Ship.Cpp\Microsoft.Xbox.Services.140.XDK.Ship.Cpp.lib %XDK_BINARIES_DROP%\cpp\binaries\debug\v140\Microsoft.Xbox.Services.140.XDK.Ship.Cpp.lib.remove
copy %UWP_BIN_BUILD_SHARE_RELEA%\Microsoft.Xbox.Services.140.XDK.Ship.Cpp\Microsoft.Xbox.Services.140.XDK.Ship.Cpp.pdb %XDK_BINARIES_DROP%\cpp\binaries\release\v140\Microsoft.Xbox.Services.140.XDK.Ship.Cpp.pdb
copy %UWP_BIN_BUILD_SHARE_DEBUG%\Microsoft.Xbox.Services.140.XDK.Ship.Cpp\Microsoft.Xbox.Services.140.XDK.Ship.Cpp.pdb %XDK_BINARIES_DROP%\cpp\binaries\debug\v140\Microsoft.Xbox.Services.140.XDK.Ship.Cpp.pdb

copy %UWP_BIN_BUILD_SHARE_RELEA%\casablanca140.Xbox\casablanca140.xbox.lib %XDK_BINARIES_DROP%\cpp\binaries\release\v140\casablanca140.xbox.lib.remove
copy %UWP_BIN_BUILD_SHARE_DEBUG%\casablanca140.Xbox\casablanca140.xbox.lib %XDK_BINARIES_DROP%\cpp\binaries\debug\v140\casablanca140.xbox.lib.remove
copy %UWP_BIN_BUILD_SHARE_RELEA%\casablanca140.Xbox\casablanca140.xbox.pdb %XDK_BINARIES_DROP%\cpp\binaries\release\v140\casablanca140.xbox.pdb
copy %UWP_BIN_BUILD_SHARE_DEBUG%\casablanca140.Xbox\casablanca140.xbox.pdb %XDK_BINARIES_DROP%\cpp\binaries\debug\v140\casablanca140.xbox.pdb



if "%skipNuget%" == "1" goto :finalize

rem create UWP XBL nuget packages
\\scratch2\scratch\jasonsa\tools\nuget pack %TFS_DropLocation%\Nuget\Microsoft.Xbox.Live.SDK.Cpp.UWP.nuspec -BasePath %TFS_DropLocation% -OutputDirectory %TFS_DropLocation% -Verbosity normal -version %NUGET_VERSION_NUMBER%

rem setup the C++/WinRT headers for nuget
set WINSDK_OUTPUT_SRC=%TFS_DropLocation%\CppWinRT\XSAPI_WinSDK_Headers\winrt
set WINSDK_OUTPUT_DEST=%TFS_DropLocation%\include\cppwinrt\winrt
rmdir /s /q %WINSDK_OUTPUT_DEST%
robocopy /NJS /NJH /MT:16 /S /NP %WINSDK_OUTPUT_SRC% %WINSDK_OUTPUT_DEST%

\\scratch2\scratch\jasonsa\tools\nuget pack %TFS_DropLocation%\Nuget\Microsoft.Xbox.Live.SDK.WinRT.UWP.nuspec -BasePath %TFS_DropLocation% -OutputDirectory %TFS_DropLocation% -Verbosity normal -version %NUGET_VERSION_NUMBER%
rmdir /s /q %WINSDK_OUTPUT_DEST%

rem create Xbox One XBL nuget packages
\\scratch2\scratch\jasonsa\tools\nuget pack %TFS_DropLocation%\Nuget\Microsoft.Xbox.Live.SDK.Cpp.XboxOneXDK.nuspec -BasePath %TFS_DropLocation% -OutputDirectory %TFS_DropLocation% -Verbosity normal -version %NUGET_VERSION_NUMBER%

rem setup the C++/WinRT headers for nuget
set XDK_OUTPUT_SRC=%TFS_DropLocation%\CppWinRT\XSAPI_XDK_Headers\winrt
set XDK_OUTPUT_DEST=%TFS_DropLocation%\include\cppwinrt\winrt
rmdir /s /q %XDK_OUTPUT_DEST%
robocopy /NJS /NJH /MT:16 /S /NP %XDK_OUTPUT_SRC% %XDK_OUTPUT_DEST%
\\scratch2\scratch\jasonsa\tools\nuget pack %TFS_DropLocation%\Nuget\Microsoft.Xbox.Live.SDK.WinRT.XboxOneXDK.nuspec -BasePath %TFS_DropLocation% -OutputDirectory %TFS_DropLocation% -Verbosity normal -version %NUGET_VERSION_NUMBER%
rmdir /s /q %XDK_OUTPUT_DEST%

mkdir %TFS_DropLocation%\NuGetBinaries
move %TFS_DropLocation%\*.nupkg %TFS_DropLocation%\NuGetBinaries
copy %XDK_BUILD_SHARE%\NuGetBinaries\*.nupkg %TFS_DropLocation%\SDK\Binaries

:finalize
set MSGTITLE="BUILD: %BUILD_SOURCEVERSIONAUTHOR% %BUILD_DEFINITIONNAME% %BUILD_SOURCEBRANCH% = %agent.jobstatus%"
set MSGBODY="%TFS_DROPLOCATION%    https://microsoft.visualstudio.com/OS/_build/index?buildId=%BUILD_BUILDID%&_a=summary"
call \\scratch2\scratch\jasonsa\tools\send-build-email.cmd %MSGTITLE% %MSGBODY% 

echo.
echo Done postBuildScript.cmd
echo.
endlocal

:done
