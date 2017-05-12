if "%1" == "local" goto testlocal
goto start

:testlocal
set TFS_DropLocation=c:\test
mkdir %TFS_DropLocation%
set TFS_VersionNumber=1701.10000
set TFS_SourcesDirectory=%CD%\..\..
goto serializeForPostbuild

:start
if "%XES_SERIALPOSTBUILDREADY%" == "True" goto serializeForPostbuild
goto done
:serializeForPostbuild

echo Running postBuildScript.cmd
echo on

if "%BUILD_DEFINITIONNAME%" == "XSAPI_Rolling" goto finalize

setlocal
call %TFS_SourcesDirectory%\Utilities\VSOBuildScripts\setBuildVersion.cmd

rem format release numbers
for /f "tokens=2 delims==" %%G in ('wmic os get localdatetime /value') do set datetime=%%G
set DATETIME_YEAR=%datetime:~0,4%
set DATETIME_MONTH=%datetime:~4,2%
set DATETIME_DAY=%datetime:~6,2%

FOR /F "TOKENS=1 eol=/ DELIMS=. " %%A IN ("%TFS_VersionNumber%") DO SET SDK_POINT_NAME_YEARMONTH=%%A
FOR /F "TOKENS=2 eol=/ DELIMS=. " %%A IN ("%TFS_VersionNumber%") DO SET SDK_POINT_NAME_DAYVER=%%A
set SDK_POINT_NAME_YEAR=%DATETIME_YEAR%
set SDK_POINT_NAME_MONTH=%DATETIME_MONTH%
set SDK_POINT_NAME_DAY=%DATETIME_DAY%
set SDK_POINT_NAME_VER=%SDK_POINT_NAME_DAYVER:~2,9%
set SDK_RELEASE_NAME=%SDK_RELEASE_YEAR:~2,2%%SDK_RELEASE_MONTH%
set LONG_SDK_RELEASE_NAME=%SDK_RELEASE_NAME%-%SDK_POINT_NAME_YEAR%%SDK_POINT_NAME_MONTH%%SDK_POINT_NAME_DAY%-%SDK_POINT_NAME_VER%
set NUGET_VERSION_NUMBER=%SDK_RELEASE_YEAR%.%SDK_RELEASE_MONTH%.%SDK_POINT_NAME_YEAR%%SDK_POINT_NAME_MONTH%%SDK_POINT_NAME_DAY%.%SDK_POINT_NAME_VER%

set

xcopy %TFS_SourcesDirectory%\Utilities\Nuget\* %TFS_DropLocation%\Nuget\ /s /e /q /y
rem goto skipCopy
robocopy /NJS /NJH /MT:16 /S /NP %TFS_SourcesDirectory%\Include %TFS_DropLocation%\Include
robocopy /NJS /NJH /MT:16 /S /NP %TFS_SourcesDirectory%\External\cpprestsdk\Release\include %TFS_DropLocation%\include

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

rem copy includes to build output folder
robocopy /NJS /NJH /MT:16 /S /NP %UWP_BUILD_SHARE%\Include %XDK_BINARIES_DROP%\cpp\include

rem copy binaries to build output folder

mkdir %XDK_BINARIES_DROP%
mkdir %XDK_BINARIES_DROP%\winrt
mkdir %XDK_BINARIES_DROP%\winrt\binaries
mkdir %XDK_BINARIES_DROP%\winrt\binaries\Debug
mkdir %XDK_BINARIES_DROP%\winrt\binaries\CommonConfiguration
mkdir %XDK_BINARIES_DROP%\cpp\binaries\release\v110
mkdir %XDK_BINARIES_DROP%\cpp\binaries\debug\v110
mkdir %XDK_BINARIES_DROP%\cpp\binaries\release\v140
mkdir %XDK_BINARIES_DROP%\cpp\binaries\debug\v140

copy %XDK_BIN_BUILD_SHARE_RELEA%\Microsoft.Xbox.Services.110.XDK.WinRT\Microsoft.Xbox.Services.dll %XDK_BINARIES_DROP%\winrt\binaries\CommonConfiguration\Microsoft.Xbox.Services.dll
copy %XDK_BIN_BUILD_SHARE_DEBUG%\Microsoft.Xbox.Services.110.XDK.WinRT\Microsoft.Xbox.Services.dll %XDK_BINARIES_DROP%\winrt\binaries\Debug\Microsoft.Xbox.Services.dll
copy %XDK_BIN_BUILD_SHARE_RELEA%\Microsoft.Xbox.Services.110.XDK.WinRT\Microsoft.Xbox.Services.winmd %XDK_BINARIES_DROP%\winrt\binaries\CommonConfiguration\
copy %XDK_BIN_BUILD_SHARE_DEBUG%\Microsoft.Xbox.Services.110.XDK.WinRT\Microsoft.Xbox.Services.winmd %XDK_BINARIES_DROP%\winrt\binaries\Debug\
copy %XDK_BIN_BUILD_SHARE_DEBUG%\Microsoft.Xbox.Services.110.XDK.WinRT\Microsoft.Xbox.Services.pdb %XDK_BINARIES_DROP%\winrt\binaries\Debug\Microsoft.Xbox.Services.pdb
copy %XDK_BIN_BUILD_SHARE_RELEA%\Microsoft.Xbox.Services.110.XDK.WinRT\Microsoft.Xbox.Services.pdb %XDK_BINARIES_DROP%\winrt\binaries\CommonConfiguration\Microsoft.Xbox.Services.pdb

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

robocopy /NJS /NJH /MT:16 /S /NP %TFS_DropLocation%\ABI\include %XDK_BINARIES_DROP%\winrt\include\abi
robocopy /NJS /NJH /MT:16 /S /NP %TFS_SourcesDirectory% %XDK_BINARIES_DROP%\source /XD .git
rmdir /s /q %XDK_BINARIES_DROP%\source\.git
rmdir /s /q %XDK_BINARIES_DROP%\source\External\cpprestsdk\Intermediate
rmdir /s /q %XDK_BINARIES_DROP%\source\InProgressSamples
rmdir /s /q %XDK_BINARIES_DROP%\source\Tests
rmdir /s /q %XDK_BINARIES_DROP%\source\Utilities
rmdir /s /q %XDK_BINARIES_DROP%\source\External\cppwinrt
rmdir /s /q %XDK_BINARIES_DROP%\source\External\Packages
rmdir /s /q %XDK_BINARIES_DROP%\source\External\cpprestsdk\Release\samples
rmdir /s /q %XDK_BINARIES_DROP%\source\External\cpprestsdk\Release\tests
rmdir /s /q %XDK_BINARIES_DROP%\source\External\cpprestsdk\Release\libs
rmdir /s /q %XDK_BINARIES_DROP%\source\Binaries
rmdir /s /q %XDK_BINARIES_DROP%\source\Build\Microsoft.Xbox.Services.UnitTest.140.Shared
rmdir /s /q %XDK_BINARIES_DROP%\source\Build\Microsoft.Xbox.Services.UnitTest.140.TAEF
rmdir /s /q %XDK_BINARIES_DROP%\source\Build\Microsoft.Xbox.Services.UnitTest.140.TE
rmdir /s /q %XDK_BINARIES_DROP%\source\Build\Microsoft.Xbox.Services.140.UWP.Cpp
rmdir /s /q %XDK_BINARIES_DROP%\source\Build\Microsoft.Xbox.Services.140.UWP.WinRT
del /s %XDK_BINARIES_DROP%\source\*.log
del %XDK_BINARIES_DROP%\source\*.md
mkdir %XDK_BINARIES_DROP%\SourceDist
\\scratch2\scratch\jasonsa\tools\vZip.exe /FOLDER:%XDK_BINARIES_DROP%\source /OUTPUTNAME:%XDK_BINARIES_DROP%\SourceDist\Xbox.Services.zip


if "%skipNuget%" == "1" goto :finalize
rem :skipCopy

rem create Cpp.UWP nuget package
rmdir /s /q %TFS_DropLocation%\include\winrt
rmdir /s /q %TFS_DropLocation%\include\cppwinrt
\\scratch2\scratch\jasonsa\tools\nuget pack %TFS_DropLocation%\Nuget\Microsoft.Xbox.Live.SDK.Cpp.UWP.nuspec -BasePath %TFS_DropLocation% -OutputDirectory %TFS_DropLocation% -Verbosity normal -version %NUGET_VERSION_NUMBER%
\\scratch2\scratch\jasonsa\tools\nuget pack %TFS_DropLocation%\Nuget\Microsoft.Xbox.Live.SDK.Cpp.UWP.v140.ARM.nuspec -BasePath %TFS_DropLocation% -OutputDirectory %TFS_DropLocation% -Verbosity normal -version %NUGET_VERSION_NUMBER%
\\scratch2\scratch\jasonsa\tools\nuget pack %TFS_DropLocation%\Nuget\Microsoft.Xbox.Live.SDK.Cpp.UWP.v140.x64.nuspec -BasePath %TFS_DropLocation% -OutputDirectory %TFS_DropLocation% -Verbosity normal -version %NUGET_VERSION_NUMBER% 
\\scratch2\scratch\jasonsa\tools\nuget pack %TFS_DropLocation%\Nuget\Microsoft.Xbox.Live.SDK.Cpp.UWP.v140.x86.nuspec -BasePath %TFS_DropLocation% -OutputDirectory %TFS_DropLocation% -Verbosity normal -version %NUGET_VERSION_NUMBER% 
\\scratch2\scratch\jasonsa\tools\nuget pack %TFS_DropLocation%\Nuget\Microsoft.Xbox.Live.SDK.Cpp.UWP.v141.ARM.nuspec -BasePath %TFS_DropLocation% -OutputDirectory %TFS_DropLocation% -Verbosity normal -version %NUGET_VERSION_NUMBER%
\\scratch2\scratch\jasonsa\tools\nuget pack %TFS_DropLocation%\Nuget\Microsoft.Xbox.Live.SDK.Cpp.UWP.v141.x64.nuspec -BasePath %TFS_DropLocation% -OutputDirectory %TFS_DropLocation% -Verbosity normal -version %NUGET_VERSION_NUMBER% 
\\scratch2\scratch\jasonsa\tools\nuget pack %TFS_DropLocation%\Nuget\Microsoft.Xbox.Live.SDK.Cpp.UWP.v141.x86.nuspec -BasePath %TFS_DropLocation% -OutputDirectory %TFS_DropLocation% -Verbosity normal -version %NUGET_VERSION_NUMBER% 

rem create WinRT.UWP nuget package
rmdir /s /q %TFS_DropLocation%\include\winrt
rmdir /s /q %TFS_DropLocation%\include\cppwinrt
set WINSDK_OUTPUT_SRC=%TFS_DropLocation%\CppWinRT\XSAPI_WinSDK_Headers\winrt
set WINSDK_OUTPUT_DEST=%TFS_DropLocation%\include\cppwinrt\winrt
robocopy /NJS /NJH /MT:16 /S /NP %WINSDK_OUTPUT_SRC% %WINSDK_OUTPUT_DEST%
\\scratch2\scratch\jasonsa\tools\nuget pack %TFS_DropLocation%\Nuget\Microsoft.Xbox.Live.SDK.WinRT.UWP.nuspec -BasePath %TFS_DropLocation% -OutputDirectory %TFS_DropLocation% -Verbosity normal -version %NUGET_VERSION_NUMBER%
\\scratch2\scratch\jasonsa\tools\nuget pack %TFS_DropLocation%\Nuget\Microsoft.Xbox.Live.SDK.WinRT.UWP.Native.Debug.nuspec -BasePath %TFS_DropLocation% -OutputDirectory %TFS_DropLocation% -Verbosity normal -version %NUGET_VERSION_NUMBER%
\\scratch2\scratch\jasonsa\tools\nuget pack %TFS_DropLocation%\Nuget\Microsoft.Xbox.Live.SDK.WinRT.UWP.Native.Release.nuspec -BasePath %TFS_DropLocation% -OutputDirectory %TFS_DropLocation% -Verbosity normal -version %NUGET_VERSION_NUMBER%
\\scratch2\scratch\jasonsa\tools\nuget pack %TFS_DropLocation%\Nuget\Microsoft.Xbox.Live.SDK.WinRT.UWP.Netcore.Debug.nuspec -BasePath %TFS_DropLocation% -OutputDirectory %TFS_DropLocation% -Verbosity normal -version %NUGET_VERSION_NUMBER%
\\scratch2\scratch\jasonsa\tools\nuget pack %TFS_DropLocation%\Nuget\Microsoft.Xbox.Live.SDK.WinRT.UWP.Netcore.Release.nuspec -BasePath %TFS_DropLocation% -OutputDirectory %TFS_DropLocation% -Verbosity normal -version %NUGET_VERSION_NUMBER%

rmdir /s /q %WINSDK_OUTPUT_DEST%

rem create Cpp.XDK nuget package
rmdir /s /q %TFS_DropLocation%\include\winrt
rmdir /s /q %TFS_DropLocation%\include\cppwinrt
\\scratch2\scratch\jasonsa\tools\nuget pack %TFS_DropLocation%\Nuget\Microsoft.Xbox.Live.SDK.Cpp.XboxOneXDK.nuspec -BasePath %TFS_DropLocation% -OutputDirectory %TFS_DropLocation% -Verbosity normal -version %NUGET_VERSION_NUMBER%

rem create WinRT.XDK nuget package
rmdir /s /q %TFS_DropLocation%\include\winrt
rmdir /s /q %TFS_DropLocation%\include\cppwinrt
robocopy /NJS /NJH /MT:16 /S /NP %TFS_DropLocation%\ABI\include %XDK_BINARIES_DROP%\include\winrt
set XDK_OUTPUT_SRC=%TFS_DropLocation%\CppWinRT\XSAPI_XDK_Headers\winrt
set XDK_OUTPUT_DEST=%TFS_DropLocation%\include\cppwinrt\winrt
robocopy /NJS /NJH /MT:16 /S /NP %XDK_OUTPUT_SRC% %XDK_OUTPUT_DEST%
\\scratch2\scratch\jasonsa\tools\nuget pack %TFS_DropLocation%\Nuget\Microsoft.Xbox.Live.SDK.WinRT.XboxOneXDK.nuspec -BasePath %TFS_DropLocation% -OutputDirectory %TFS_DropLocation% -Verbosity normal -version %NUGET_VERSION_NUMBER%
rmdir /s /q %XDK_OUTPUT_DEST%

mkdir %TFS_DropLocation%\NuGetBinaries
move %TFS_DropLocation%\*.nupkg %TFS_DropLocation%\NuGetBinaries

:finalize
if "%1" == "local" goto skipEmail
set MSGTITLE="BUILD: %BUILD_SOURCEVERSIONAUTHOR% %BUILD_DEFINITIONNAME% %BUILD_SOURCEBRANCH% = %agent.jobstatus%"
set MSGBODY="%TFS_DROPLOCATION%    https://microsoft.visualstudio.com/OS/_build/index?buildId=%BUILD_BUILDID%&_a=summary"
call \\scratch2\scratch\jasonsa\tools\send-build-email.cmd %MSGTITLE% %MSGBODY% 
:skipEmail

echo.
echo Done postBuildScript.cmd
echo.
endlocal

:done
