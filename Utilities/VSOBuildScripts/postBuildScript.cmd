if "%1" == "local" goto testlocal
goto start

:testlocal
set TFS_DropLocation=c:\scratch\tfsdrop
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

echo BUILD_BUILDNUMBER = %BUILD_BUILDNUMBER%
FOR /F "TOKENS=4 eol=/ DELIMS=. " %%A IN ("%BUILD_BUILDNUMBER%") DO SET SDK_POINT_NAME_VER=%%A
set SDK_POINT_NAME_YEAR=%DATETIME_YEAR%
set SDK_POINT_NAME_MONTH=%DATETIME_MONTH%
set SDK_POINT_NAME_DAY=%DATETIME_DAY%
set SDK_RELEASE_NAME=%SDK_RELEASE_YEAR:~2,2%%SDK_RELEASE_MONTH%
set LONG_SDK_RELEASE_NAME=%SDK_RELEASE_NAME%-%SDK_POINT_NAME_YEAR%%SDK_POINT_NAME_MONTH%%SDK_POINT_NAME_DAY%-%SDK_RELEASE_QFE%
set NUGET_VERSION_NUMBER=%SDK_RELEASE_YEAR%.%SDK_RELEASE_MONTH%.%SDK_POINT_NAME_YEAR%%SDK_POINT_NAME_MONTH%%SDK_POINT_NAME_DAY%.%SDK_RELEASE_QFE%
set MINOR_VERSION_NUMBER=%SDK_POINT_NAME_YEAR%%SDK_POINT_NAME_MONTH%%SDK_POINT_NAME_DAY%%SDK_POINT_NAME_VER%
if "%NUGET_EXE%" == "" set NUGET_EXE=%AGENT_TOOLSDIRECTORY%\NuGet\4.6.2\x64\nuget.exe

set

xcopy %TFS_SourcesDirectory%\Utilities\Nuget\* %TFS_DropLocation%\Nuget\ /s /e /q /y
rem goto skipCopy
robocopy /NJS /NJH /MT:16 /S /NP %TFS_SourcesDirectory%\Include %TFS_DropLocation%\Include
robocopy /NJS /NJH /MT:16 /S /NP %TFS_SourcesDirectory%\External\cpprestsdk\Release\include %TFS_DropLocation%\include
robocopy /NJS /NJH /MT:16 /S /NP %TFS_SourcesDirectory%\External\xal\External\libHttpClient\Include %TFS_DropLocation%\include
robocopy /NJS /NJH /MT:16 /S /NP %TFS_SourcesDirectory%\External\xal\Source\Xal\Include %TFS_DropLocation%\include
robocopy /NJS /NJH /MT:16 /S /NP %TFS_SourcesDirectory%\External\xal\Source\XalExtra\Include %TFS_DropLocation%\include
rmdir /s /q %TFS_DropLocation%\include\json_cpp

rem setup folders for XDK drop
set XDK_BINARIES_DROP=%TFS_DropLocation%\xdkservicesapi
set UWP_BUILD_SHARE=%TFS_DropLocation%
set XDK_BUILD_SHARE=%TFS_DropLocation%

set XDK_BIN_BUILD_SHARE_RELEA=%XDK_BUILD_SHARE%\Release\x64
set XDK_BIN_BUILD_SHARE_DEBUG=%XDK_BUILD_SHARE%\Debug\x64

mkdir %XDK_BINARIES_DROP%

rem copy NuGetPackages to build output folder

rem copy includes to build output folder
robocopy /NJS /NJH /MT:16 /S /NP %XDK_BUILD_SHARE%\Include %XDK_BINARIES_DROP%\cpp\include

rem copy binaries to build output folder
mkdir %XDK_BINARIES_DROP%
mkdir %XDK_BINARIES_DROP%\SourceDist
mkdir %XDK_BINARIES_DROP%\cpp\binaries\release\v140
mkdir %XDK_BINARIES_DROP%\cpp\binaries\debug\v140

copy %TFS_SourcesDirectory%\Utilities\VSOBuildScripts\XDK\Readme.txt %XDK_BINARIES_DROP%\SourceDist\Readme.txt
%TFS_SourcesDirectory%\Utilities\VSOBuildScripts\FindAndReplace.exe %XDK_BINARIES_DROP%\SourceDist\Readme.txt GITCOMMIT %BUILD_SOURCEVERSION%
%TFS_SourcesDirectory%\Utilities\VSOBuildScripts\FindAndReplace.exe %XDK_BINARIES_DROP%\SourceDist\Readme.txt VERSION %NUGET_VERSION_NUMBER%

echo set MAJOR_VERSION=%SDK_RELEASE_NAME% > %XDK_BINARIES_DROP%\setver.cmd
echo set MINOR_VERSION=%MINOR_VERSION_NUMBER% >> %XDK_BINARIES_DROP%\setver.cmd


copy %XDK_BIN_BUILD_SHARE_RELEA%\Microsoft.Xbox.Services.140.XDK.Cpp\Microsoft.Xbox.Services.140.XDK.Cpp.lib %XDK_BINARIES_DROP%\cpp\binaries\release\v140\Microsoft.Xbox.Services.140.XDK.Cpp.lib.remove
copy %XDK_BIN_BUILD_SHARE_DEBUG%\Microsoft.Xbox.Services.140.XDK.Cpp\Microsoft.Xbox.Services.140.XDK.Cpp.lib %XDK_BINARIES_DROP%\cpp\binaries\debug\v140\Microsoft.Xbox.Services.140.XDK.Cpp.lib.remove
copy %XDK_BIN_BUILD_SHARE_RELEA%\Microsoft.Xbox.Services.140.XDK.Cpp\Microsoft.Xbox.Services.140.XDK.Cpp.pdb %XDK_BINARIES_DROP%\cpp\binaries\release\v140\Microsoft.Xbox.Services.140.XDK.Cpp.pdb
copy %XDK_BIN_BUILD_SHARE_DEBUG%\Microsoft.Xbox.Services.140.XDK.Cpp\Microsoft.Xbox.Services.140.XDK.Cpp.pdb %XDK_BINARIES_DROP%\cpp\binaries\debug\v140\Microsoft.Xbox.Services.140.XDK.Cpp.pdb

copy %XDK_BIN_BUILD_SHARE_RELEA%\libHttpClient.140.XDK.C\libHttpClient.140.XDK.C.lib %XDK_BINARIES_DROP%\cpp\binaries\release\v140\libHttpClient.140.XDK.C.lib.remove
copy %XDK_BIN_BUILD_SHARE_DEBUG%\libHttpClient.140.XDK.C\libHttpClient.140.XDK.C.lib %XDK_BINARIES_DROP%\cpp\binaries\debug\v140\libHttpClient.140.XDK.C.lib.remove
copy %XDK_BIN_BUILD_SHARE_RELEA%\libHttpClient.140.XDK.C\libHttpClient.140.XDK.C.pdb %XDK_BINARIES_DROP%\cpp\binaries\release\v140
copy %XDK_BIN_BUILD_SHARE_DEBUG%\libHttpClient.140.XDK.C\libHttpClient.140.XDK.C.pdb %XDK_BINARIES_DROP%\cpp\binaries\debug\v140

copy %XDK_BIN_BUILD_SHARE_RELEA%\casablanca140.Xbox\casablanca140.xbox.lib %XDK_BINARIES_DROP%\cpp\binaries\release\v140\casablanca140.xbox.lib.remove
copy %XDK_BIN_BUILD_SHARE_DEBUG%\casablanca140.Xbox\casablanca140.xbox.lib %XDK_BINARIES_DROP%\cpp\binaries\debug\v140\casablanca140.xbox.lib.remove
copy %XDK_BIN_BUILD_SHARE_RELEA%\casablanca140.Xbox\casablanca140.xbox.pdb %XDK_BINARIES_DROP%\cpp\binaries\release\v140\casablanca140.xbox.pdb
copy %XDK_BIN_BUILD_SHARE_DEBUG%\casablanca140.Xbox\casablanca140.xbox.pdb %XDK_BINARIES_DROP%\cpp\binaries\debug\v140\casablanca140.xbox.pdb

rem laying out the 141 binaries
set PATH_XDK_DROP_BIN=%XDK_BINARIES_DROP%\cpp

mkdir %PATH_XDK_DROP_BIN%\binaries\release\v141
mkdir %PATH_XDK_DROP_BIN%\binaries\debug\v141

set TYPE=release
copy %TFS_DropLocation%\%TYPE%\x64\Microsoft.Xbox.Services.141.XDK.Cpp\Microsoft.Xbox.Services.141.XDK.Cpp.pdb %PATH_XDK_DROP_BIN%\binaries\%TYPE%\v141
copy %TFS_DropLocation%\%TYPE%\x64\Microsoft.Xbox.Services.141.XDK.Cpp\Microsoft.Xbox.Services.141.XDK.Cpp.lib %PATH_XDK_DROP_BIN%\binaries\%TYPE%\v141\Microsoft.Xbox.Services.141.XDK.Cpp.lib.remove
copy %TFS_DropLocation%\%TYPE%\x64\casablanca141.Xbox\casablanca141.xbox.pdb %PATH_XDK_DROP_BIN%\binaries\%TYPE%\v141
copy %TFS_DropLocation%\%TYPE%\x64\casablanca141.Xbox\casablanca141.xbox.lib %PATH_XDK_DROP_BIN%\binaries\%TYPE%\v141\casablanca141.xbox.lib.remove
copy %TFS_DropLocation%\%TYPE%\x64\libHttpClient.141.XDK.C\libHttpClient.141.XDK.C.pdb %PATH_XDK_DROP_BIN%\binaries\%TYPE%\v141
copy %TFS_DropLocation%\%TYPE%\x64\libHttpClient.141.XDK.C\libHttpClient.141.XDK.C.lib %PATH_XDK_DROP_BIN%\binaries\%TYPE%\v141\libHttpClient.141.XDK.C.lib.remove

set TYPE=debug
copy %TFS_DropLocation%\%TYPE%\x64\Microsoft.Xbox.Services.141.XDK.Cpp\Microsoft.Xbox.Services.141.XDK.Cpp.pdb %PATH_XDK_DROP_BIN%\binaries\%TYPE%\v141
copy %TFS_DropLocation%\%TYPE%\x64\Microsoft.Xbox.Services.141.XDK.Cpp\Microsoft.Xbox.Services.141.XDK.Cpp.lib %PATH_XDK_DROP_BIN%\binaries\%TYPE%\v141\Microsoft.Xbox.Services.141.XDK.Cpp.lib.remove
copy %TFS_DropLocation%\%TYPE%\x64\casablanca141.Xbox\casablanca141.xbox.pdb %PATH_XDK_DROP_BIN%\binaries\%TYPE%\v141
copy %TFS_DropLocation%\%TYPE%\x64\casablanca141.Xbox\casablanca141.xbox.lib %PATH_XDK_DROP_BIN%\binaries\%TYPE%\v141\casablanca141.xbox.lib.remove
copy %TFS_DropLocation%\%TYPE%\x64\libHttpClient.141.XDK.C\libHttpClient.141.XDK.C.pdb %PATH_XDK_DROP_BIN%\binaries\%TYPE%\v141
copy %TFS_DropLocation%\%TYPE%\x64\libHttpClient.141.XDK.C\libHttpClient.141.XDK.C.lib %PATH_XDK_DROP_BIN%\binaries\%TYPE%\v141\libHttpClient.141.XDK.C.lib.remove



rem setup folders for GDK drop
set PATH_GDK_DROP=%TFS_DropLocation%\gdk-drop
set PATH_GDK_DROP_BIN=%PATH_GDK_DROP%\c
mkdir %PATH_GDK_DROP%
mkdir %PATH_GDK_DROP_BIN%
mkdir %PATH_GDK_DROP_BIN%\include
mkdir %PATH_GDK_DROP_BIN%\binaries\GSDKXboxExtensionSDKs\release\v141
mkdir %PATH_GDK_DROP_BIN%\binaries\GSDKXboxExtensionSDKs\debug\v141
mkdir %PATH_GDK_DROP_BIN%\binaries\GSDKXboxExtensionSDKs\release\v142
mkdir %PATH_GDK_DROP_BIN%\binaries\GSDKXboxExtensionSDKs\debug\v142
mkdir %PATH_GDK_DROP_BIN%\binaries\GSDKDesktopExtensionSDKs\release\v141
mkdir %PATH_GDK_DROP_BIN%\binaries\GSDKDesktopExtensionSDKs\debug\v141
mkdir %PATH_GDK_DROP_BIN%\binaries\GSDKDesktopExtensionSDKs\release\v142
mkdir %PATH_GDK_DROP_BIN%\binaries\GSDKDesktopExtensionSDKs\debug\v142

copy %TFS_SourcesDirectory%\Utilities\GDK\Xbox.Services.API.C.props %PATH_GDK_DROP_BIN%

robocopy /NJS /NJH /MT:16 /S /NP %TFS_DropLocation%\Include %PATH_GDK_DROP_BIN%\include
mkdir %PATH_GDK_DROP_BIN%\include\cpprest
mkdir %PATH_GDK_DROP_BIN%\include\cpprest\details
mkdir %PATH_GDK_DROP_BIN%\include\pplx
copy %TFS_SourcesDirectory%\ThirdPartyNotices.txt %PATH_GDK_DROP_BIN%\include\xsapi-cpp
copy %TFS_SourcesDirectory%\ThirdPartyNotices.txt %PATH_GDK_DROP_BIN%\include\xsapi-c
copy %TFS_SourcesDirectory%\External\xal\External\libHttpClient\ThirdPartyNotices.txt %PATH_GDK_DROP_BIN%\include\httpClient
copy %TFS_SourcesDirectory%\ThirdPartyNotices.txt %PATH_GDK_DROP_BIN%\include\cpprest
copy %TFS_SourcesDirectory%\ThirdPartyNotices.txt %PATH_GDK_DROP_BIN%\include\pplx
copy %TFS_DropLocation%\include\cpprestinclude\cpprest\astreambuf.h %PATH_GDK_DROP_BIN%\include\cpprest\astreambuf.h
copy %TFS_DropLocation%\include\cpprestinclude\cpprest\asyncrt_utils.h %PATH_GDK_DROP_BIN%\include\cpprest\asyncrt_utils.h
copy %TFS_DropLocation%\include\cpprestinclude\cpprest\base_uri.h %PATH_GDK_DROP_BIN%\include\cpprest\base_uri.h
copy %TFS_DropLocation%\include\cpprestinclude\cpprest\containerstream.h %PATH_GDK_DROP_BIN%\include\cpprest\containerstream.h
copy %TFS_DropLocation%\include\cpprestinclude\cpprest\http_headers.h %PATH_GDK_DROP_BIN%\include\cpprest\http_headers.h
copy %TFS_DropLocation%\include\cpprestinclude\cpprest\http_msg.h %PATH_GDK_DROP_BIN%\include\cpprest\http_msg.h
copy %TFS_DropLocation%\include\cpprestinclude\cpprest\json.h %PATH_GDK_DROP_BIN%\include\cpprest\json.h
copy %TFS_DropLocation%\include\cpprestinclude\cpprest\streams.h %PATH_GDK_DROP_BIN%\include\cpprest\streams.h
copy %TFS_DropLocation%\include\cpprestinclude\cpprest\uri.h %PATH_GDK_DROP_BIN%\include\cpprest\uri.h
copy %TFS_DropLocation%\include\cpprestinclude\cpprest\uri_builder.h %PATH_GDK_DROP_BIN%\include\cpprest\uri_builder.h
copy %TFS_DropLocation%\include\cpprestinclude\cpprest\details\basic_types.h %PATH_GDK_DROP_BIN%\include\cpprest\details\basic_types.h
copy %TFS_DropLocation%\include\cpprestinclude\cpprest\details\cpprest_compat.h %PATH_GDK_DROP_BIN%\include\cpprest\details\cpprest_compat.h
copy %TFS_DropLocation%\include\cpprestinclude\cpprest\details\http_constants.dat %PATH_GDK_DROP_BIN%\include\cpprest\details\http_constants.dat
copy %TFS_DropLocation%\include\cpprestinclude\cpprest\details\SafeInt3.hpp %PATH_GDK_DROP_BIN%\include\cpprest\details\SafeInt3.hpp
copy %TFS_DropLocation%\include\cpprestinclude\cpprest\details\uri_parser.h %PATH_GDK_DROP_BIN%\include\cpprest\details\uri_parser.h
copy %TFS_DropLocation%\include\cpprestinclude\pplx\pplxtasks.140.h %PATH_GDK_DROP_BIN%\include\pplx\pplxtasks.140.h
copy %TFS_DropLocation%\include\cpprestinclude\pplx\pplxtasks.h %PATH_GDK_DROP_BIN%\include\pplx\pplxtasks.h
rmdir /s /q %PATH_GDK_DROP_BIN%\include\cpprestinclude
rmdir /s /q %PATH_GDK_DROP_BIN%\Include\XalExtra
del %PATH_GDK_DROP_BIN%\include\Xal\xal_android.h
del %PATH_GDK_DROP_BIN%\include\Xal\xal_internal_telemetry.h
del %PATH_GDK_DROP_BIN%\include\Xal\xal_internal_web_account.h
del %PATH_GDK_DROP_BIN%\include\Xal\xal_ios.h
del %PATH_GDK_DROP_BIN%\include\Xal\xal_apple.h
del %PATH_GDK_DROP_BIN%\include\Xal\xal_generic.h
del %PATH_GDK_DROP_BIN%\include\Xal\xal_platform.h
del %PATH_GDK_DROP_BIN%\include\Xal\xal_platform_types.h
del %PATH_GDK_DROP_BIN%\include\Xal\xal_uwp.h
del %PATH_GDK_DROP_BIN%\include\Xal\xal_uwp_user.h
del %PATH_GDK_DROP_BIN%\include\Xal\xal_version.h
del %PATH_GDK_DROP_BIN%\include\Xal\xal_win32.h
del %PATH_GDK_DROP_BIN%\include\Xal\xal_xdk.h
del %PATH_GDK_DROP_BIN%\include\Xal\xal_grts*.h
del %PATH_GDK_DROP_BIN%\include\async.h
del %PATH_GDK_DROP_BIN%\include\asyncProvider.h
del %PATH_GDK_DROP_BIN%\include\asyncQueue.h
del %PATH_GDK_DROP_BIN%\include\asyncQueueEx.h

set PLAT=Gaming.Xbox.x64
set PLAT_LAYOUT_NAME=GSDKXboxExtensionSDKs
set TYPE=release
copy %TFS_DropLocation%\%TYPE%\%PLAT%\Microsoft.Xbox.Services.141.GDK.C\Microsoft.Xbox.Services.141.GDK.C.pdb %PATH_GDK_DROP_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v141
copy %TFS_DropLocation%\%TYPE%\%PLAT%\Microsoft.Xbox.Services.141.GDK.C\Microsoft.Xbox.Services.141.GDK.C.lib %PATH_GDK_DROP_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v141\Microsoft.Xbox.Services.141.GDK.C.lib.remove
copy %TFS_DropLocation%\%TYPE%\%PLAT%\Microsoft.Xbox.Services.142.GDK.C\Microsoft.Xbox.Services.142.GDK.C.pdb %PATH_GDK_DROP_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v142
copy %TFS_DropLocation%\%TYPE%\%PLAT%\Microsoft.Xbox.Services.142.GDK.C\Microsoft.Xbox.Services.142.GDK.C.lib %PATH_GDK_DROP_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v142\Microsoft.Xbox.Services.142.GDK.C.lib.remove
copy %TFS_DropLocation%\%TYPE%\%PLAT%\libHttpClient.141.GDK.C\libHttpClient.141.GDK.C.pdb %PATH_GDK_DROP_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v141
copy %TFS_DropLocation%\%TYPE%\%PLAT%\libHttpClient.141.GDK.C\libHttpClient.141.GDK.C.lib %PATH_GDK_DROP_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v141\libHttpClient.141.GDK.C.lib.remove
copy %TFS_DropLocation%\%TYPE%\%PLAT%\libHttpClient.142.GDK.C\libHttpClient.142.GDK.C.pdb %PATH_GDK_DROP_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v142
copy %TFS_DropLocation%\%TYPE%\%PLAT%\libHttpClient.142.GDK.C\libHttpClient.142.GDK.C.lib %PATH_GDK_DROP_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v142\libHttpClient.141.GDK.C.lib.remove

set TYPE=debug
copy %TFS_DropLocation%\%TYPE%\%PLAT%\Microsoft.Xbox.Services.141.GDK.C\Microsoft.Xbox.Services.141.GDK.C.pdb %PATH_GDK_DROP_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v141
copy %TFS_DropLocation%\%TYPE%\%PLAT%\Microsoft.Xbox.Services.141.GDK.C\Microsoft.Xbox.Services.141.GDK.C.lib %PATH_GDK_DROP_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v141\Microsoft.Xbox.Services.141.GDK.C.lib.remove
copy %TFS_DropLocation%\%TYPE%\%PLAT%\Microsoft.Xbox.Services.142.GDK.C\Microsoft.Xbox.Services.142.GDK.C.pdb %PATH_GDK_DROP_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v142
copy %TFS_DropLocation%\%TYPE%\%PLAT%\Microsoft.Xbox.Services.142.GDK.C\Microsoft.Xbox.Services.142.GDK.C.lib %PATH_GDK_DROP_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v142\Microsoft.Xbox.Services.142.GDK.C.lib.remove
copy %TFS_DropLocation%\%TYPE%\%PLAT%\libHttpClient.141.GDK.C\libHttpClient.141.GDK.C.pdb %PATH_GDK_DROP_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v141
copy %TFS_DropLocation%\%TYPE%\%PLAT%\libHttpClient.141.GDK.C\libHttpClient.141.GDK.C.lib %PATH_GDK_DROP_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v141\libHttpClient.141.GDK.C.lib.remove
copy %TFS_DropLocation%\%TYPE%\%PLAT%\libHttpClient.142.GDK.C\libHttpClient.142.GDK.C.pdb %PATH_GDK_DROP_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v142
copy %TFS_DropLocation%\%TYPE%\%PLAT%\libHttpClient.142.GDK.C\libHttpClient.142.GDK.C.lib %PATH_GDK_DROP_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v142\libHttpClient.142.GDK.C.lib.remove

set PLAT=Gaming.Desktop.x64
set PLAT_LAYOUT_NAME=GSDKDesktopExtensionSDKs
set TYPE=release
copy %TFS_DropLocation%\%TYPE%\%PLAT%\Microsoft.Xbox.Services.141.GDK.C\Microsoft.Xbox.Services.141.GDK.C.pdb %PATH_GDK_DROP_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v141
copy %TFS_DropLocation%\%TYPE%\%PLAT%\Microsoft.Xbox.Services.141.GDK.C\Microsoft.Xbox.Services.141.GDK.C.lib %PATH_GDK_DROP_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v141\Microsoft.Xbox.Services.141.GDK.C.lib.remove
copy %TFS_DropLocation%\%TYPE%\%PLAT%\Microsoft.Xbox.Services.142.GDK.C\Microsoft.Xbox.Services.142.GDK.C.pdb %PATH_GDK_DROP_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v142
copy %TFS_DropLocation%\%TYPE%\%PLAT%\Microsoft.Xbox.Services.142.GDK.C\Microsoft.Xbox.Services.142.GDK.C.lib %PATH_GDK_DROP_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v142\Microsoft.Xbox.Services.142.GDK.C.lib.remove
copy %TFS_DropLocation%\%TYPE%\%PLAT%\libHttpClient.141.GDK.C\libHttpClient.141.GDK.C.pdb %PATH_GDK_DROP_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v141
copy %TFS_DropLocation%\%TYPE%\%PLAT%\libHttpClient.141.GDK.C\libHttpClient.141.GDK.C.lib %PATH_GDK_DROP_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v141\libHttpClient.141.GDK.C.lib.remove
copy %TFS_DropLocation%\%TYPE%\%PLAT%\libHttpClient.142.GDK.C\libHttpClient.142.GDK.C.pdb %PATH_GDK_DROP_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v142
copy %TFS_DropLocation%\%TYPE%\%PLAT%\libHttpClient.142.GDK.C\libHttpClient.142.GDK.C.lib %PATH_GDK_DROP_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v142\libHttpClient.142.GDK.C.lib.remove

set TYPE=debug
copy %TFS_DropLocation%\%TYPE%\%PLAT%\Microsoft.Xbox.Services.141.GDK.C\Microsoft.Xbox.Services.141.GDK.C.pdb %PATH_GDK_DROP_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v141
copy %TFS_DropLocation%\%TYPE%\%PLAT%\Microsoft.Xbox.Services.141.GDK.C\Microsoft.Xbox.Services.141.GDK.C.lib %PATH_GDK_DROP_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v141\Microsoft.Xbox.Services.141.GDK.C.lib.remove
copy %TFS_DropLocation%\%TYPE%\%PLAT%\Microsoft.Xbox.Services.142.GDK.C\Microsoft.Xbox.Services.142.GDK.C.pdb %PATH_GDK_DROP_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v142
copy %TFS_DropLocation%\%TYPE%\%PLAT%\Microsoft.Xbox.Services.142.GDK.C\Microsoft.Xbox.Services.142.GDK.C.lib %PATH_GDK_DROP_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v142\Microsoft.Xbox.Services.142.GDK.C.lib.remove
copy %TFS_DropLocation%\%TYPE%\%PLAT%\libHttpClient.141.GDK.C\libHttpClient.141.GDK.C.pdb %PATH_GDK_DROP_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v141
copy %TFS_DropLocation%\%TYPE%\%PLAT%\libHttpClient.141.GDK.C\libHttpClient.141.GDK.C.lib %PATH_GDK_DROP_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v141\libHttpClient.141.GDK.C.lib.remove
copy %TFS_DropLocation%\%TYPE%\%PLAT%\libHttpClient.142.GDK.C\libHttpClient.142.GDK.C.pdb %PATH_GDK_DROP_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v142
copy %TFS_DropLocation%\%TYPE%\%PLAT%\libHttpClient.142.GDK.C\libHttpClient.142.GDK.C.lib %PATH_GDK_DROP_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v142\libHttpClient.142.GDK.C.lib.remove

set BUILD_TOOLS=%BUILD_STAGINGDIRECTORY%\sdk.buildtools\buildMachine

copy %BUILD_TOOLS%\GdkCopyHeaders.cmd %PATH_GDK_DROP%
copy %BUILD_TOOLS%\VPackLogReader.exe %PATH_GDK_DROP%
copy %BUILD_TOOLS%\GdkCreateVPack.cmd %PATH_GDK_DROP%
copy %BUILD_TOOLS%\GdkFormatKML.cmd %PATH_GDK_DROP%
copy %BUILD_TOOLS%\GdkCopyOutputToLocal.cmd %PATH_GDK_DROP%
copy %TFS_SourcesDirectory%\Utilities\VSOBuildScripts\setBuildVersion.cmd %PATH_GDK_DROP%
copy %TFS_SourcesDirectory%\Utilities\VSOBuildScripts\ReadSDKVerHeader.exe %PATH_GDK_DROP%
copy %TFS_SourcesDirectory%\Source\Shared\build_version.h %PATH_GDK_DROP%

set BUILD_INFO_PATH=%PATH_GDK_DROP%\build_info.txt
set BUILD_INFO_BODY="%TFS_DROPLOCATION%    https://microsoft.visualstudio.com/Xbox.Services/_build/index?buildId=%BUILD_BUILDID%&_a=summary"
del %BUILD_INFO_PATH%
echo %BUILD_INFO_BODY% > %BUILD_INFO_PATH%

rem setup folders for GDK docs
set PATH_GDK_DOCS=%TFS_DropLocation%\gdk-docs
set PATH_GDK_DOCS_DROP=%PATH_GDK_DOCS%\docs
mkdir %PATH_GDK_DOCS%
mkdir %PATH_GDK_DOCS_DROP%
call %BUILD_TOOLS%\Noggin\GenerateReferenceDocs.cmd %TFS_DropLocation%\include %PATH_GDK_DOCS_DROP%
copy %BUILD_TOOLS%\Noggin\CopyReferenceDocsToDocsRepo.cmd %PATH_GDK_DOCS%


rem create unity package
REM set UNITY_ASSET_DEST=%TFS_DropLocation%\unity
REM set UNITY_ASSET_SRC=%TFS_SourcesDirectory%\Utilities\IDXboxUnityAssetLayout

REM set TYPE=x64
REM copy %UWP_BUILD_SHARE%\Release\%TYPE%\Microsoft.Xbox.Services.141.UWP.WinRT\Microsoft.Xbox.Services.dll %UNITY_ASSET_SRC%\Assets\XboxServicesAPI\Binaries\%TYPE%
REM copy %UWP_BUILD_SHARE%\Release\%TYPE%\Microsoft.Xbox.Services.141.UWP.WinRT\Microsoft.Xbox.Services.winmd %UNITY_ASSET_SRC%\Assets\XboxServicesAPI\Binaries\WinMD
REM set TYPE=ARM
REM copy %UWP_BUILD_SHARE%\Release\%TYPE%\Microsoft.Xbox.Services.141.UWP.WinRT\Microsoft.Xbox.Services.dll %UNITY_ASSET_SRC%\Assets\XboxServicesAPI\Binaries\%TYPE%
REM set TYPE=x86
REM copy %UWP_BUILD_SHARE%\Release\%TYPE%\Microsoft.Xbox.Services.141.UWP.WinRT\Microsoft.Xbox.Services.dll %UNITY_ASSET_SRC%\Assets\XboxServicesAPI\Binaries\%TYPE%

REM set UNITY_PACKAGE_NAME=%UNITY_ASSET_SRC%\XboxServicesAPI-%SDK_RELEASE_NAME%-%MINOR_VERSION_NUMBER%.unitypackage
REM "C:\Program Files\Unity\Editor\Unity.exe" -ea SilentlyContinue -batchmode -logFile "%UNITY_ASSET_SRC%\unity.log" -projectPath "%UNITY_ASSET_SRC%" -exportPackage "Assets\XboxServicesAPI" "%UNITY_PACKAGE_NAME%" -quit

mkdir %UNITY_ASSET_DEST%
robocopy /NJS /NJH /MT:16 /S /NP %UNITY_ASSET_SRC% %UNITY_ASSET_DEST%

if "%1" == "local" goto skipMavenPublish

REM -------------------ANDROID BEGIN-------------------
set JAVA_HOME="C:\Program Files\Android\jdk\microsoft_dist_openjdk_1.8.0.25"
if "%ANDROID_HOME%" == "" set ANDROID_HOME=C:\Program Files (x86)\Android\android-sdk
rem If we eventually want to include native libs/headers in the maven artifact we can re-enable this
rem call %TFS_SourcesDirectory%\Java\com.microsoft.xboxlive\gradlew.bat --project-dir=%TFS_SourcesDirectory%\Java\com.microsoft.xboxlive copyNdkLibsToAAR -PInputAar=%TFS_BinariesDirectory%\Release\x86\com.microsoft.xboxlive\com.microsoft.xboxlive.aar -POutDir=%TFS_BinariesDirectory%\Release\x86\com.microsoft.xboxlive\ndklibs --info
call %TFS_SourcesDirectory%\Java\com.microsoft.xboxlive\gradlew.bat  --project-dir=%TFS_SourcesDirectory%\Java\com.microsoft.xboxlive publish -PInputAar=%TFS_DropLocation%\Release\x86\com.microsoft.xboxlive\com.microsoft.xboxlive.aar --info
call %TFS_SourcesDirectory%\external\xal\external\libHttpClient\Build\libHttpClient.141.Android.Java\gradlew.bat  --project-dir=%TFS_SourcesDirectory%\external\xal\external\libHttpClient\Build\libHttpClient.141.Android.Java publish -PInputAar=%TFS_DropLocation%\Release\x86\libHttpClient.141.Android.Java\libHttpClient.aar --info
call %TFS_SourcesDirectory%\external\xal\source\XalAndroidJava\gradlew.bat  --project-dir=%TFS_SourcesDirectory%\external\xal\source\XalAndroidJava publish -PaarPath=%TFS_DropLocation%\Release\x86\XalAndroidJava\XalAndroidJava.aar --info

rem Copy Maven output
xcopy %TFS_SourcesDirectory%\Java\com.microsoft.xboxlive\build\Maven\ %TFS_DropLocation%\Maven\ /s /e /q /y
xcopy %TFS_SourcesDirectory%\external\xal\external\libHttpClient\Build\libHttpClient.141.Android.Java\build\Maven\ %TFS_DropLocation%\Maven\ /s /e /q /y
xcopy %TFS_SourcesDirectory%\external\xal\source\XalAndroidJava\build\Maven\ %TFS_DropLocation%\Maven\ /s /e /q /y

rem Copy native android libs and headers
xcopy %TFS_DropLocation%\Include\* %TFS_DropLocation%\Maven\ndk\include\ /s /e /q /y
rmdir /s /q %TFS_DropLocation%\Maven\ndk\include\cpprest
rmdir /s /q %TFS_DropLocation%\Maven\ndk\include\pplx
rmdir /s /q %TFS_DropLocation%\Maven\ndk\include\XalExtra\Win32
for /R %TFS_DropLocation%\Release\x86\ %%f in (*.a) do xcopy %%f %TFS_DropLocation%\Maven\ndk\libs\Release\x86\
for /R %TFS_DropLocation%\Release\x64\ %%f in (*.a) do xcopy %%f %TFS_DropLocation%\Maven\ndk\libs\Release\x64\
for /R %TFS_DropLocation%\Release\arm\ %%f in (*.a) do xcopy %%f %TFS_DropLocation%\Maven\ndk\libs\Release\arm\
for /R %TFS_DropLocation%\Release\arm64\ %%f in (*.a) do xcopy %%f %TFS_DropLocation%\Maven\ndk\libs\Release\arm64\
for /R %TFS_DropLocation%\Debug\x86\ %%f in (*.a) do xcopy %%f %TFS_DropLocation%\Maven\ndk\libs\Debug\x86\
for /R %TFS_DropLocation%\Debug\x64\ %%f in (*.a) do xcopy %%f %TFS_DropLocation%\Maven\ndk\libs\Debug\x64\
for /R %TFS_DropLocation%\Debug\arm\ %%f in (*.a) do xcopy %%f %TFS_DropLocation%\Maven\ndk\libs\Debug\arm\
for /R %TFS_DropLocation%\Debug\arm64\ %%f in (*.a) do xcopy %%f %TFS_DropLocation%\Maven\ndk\libs\Debug\arm64\
xcopy %TFS_SourcesDirectory%\Build\xsapi.android.props %TFS_DropLocation%\Maven\ndk\
REM -------------------ANDROID END-------------------

:skipMavenPublish
@echo on

if "%skipNuget%" == "1" goto skipNuget
if "%1" == "local" goto skipNuget
rem :skipCopy

rem create Cpp.Win32 nuget package
rmdir /s /q %TFS_DropLocation%\include\winrt
rmdir /s /q %TFS_DropLocation%\include\cppwinrt
%NUGET_EXE% pack %TFS_DropLocation%\Nuget\Microsoft.Xbox.Live.SDK.Cpp.Win32.nuspec -BasePath %TFS_DropLocation% -OutputDirectory %TFS_DropLocation% -Verbosity normal -version %NUGET_VERSION_NUMBER%
%NUGET_EXE% pack %TFS_DropLocation%\Nuget\Microsoft.Xbox.Live.SDK.Cpp.Win32.v141.x64.nuspec -BasePath %TFS_DropLocation% -OutputDirectory %TFS_DropLocation% -Verbosity normal -version %NUGET_VERSION_NUMBER% 
%NUGET_EXE% pack %TFS_DropLocation%\Nuget\Microsoft.Xbox.Live.SDK.Cpp.Win32.v141.x86.nuspec -BasePath %TFS_DropLocation% -OutputDirectory %TFS_DropLocation% -Verbosity normal -version %NUGET_VERSION_NUMBER% 

rem create Cpp.UWP nuget package
%NUGET_EXE% pack %TFS_DropLocation%\Nuget\Microsoft.Xbox.Live.SDK.Cpp.UWP.nuspec -BasePath %TFS_DropLocation% -OutputDirectory %TFS_DropLocation% -Verbosity normal -version %NUGET_VERSION_NUMBER%
%NUGET_EXE% pack %TFS_DropLocation%\Nuget\Microsoft.Xbox.Live.SDK.Cpp.UWP.v140.ARM.nuspec -BasePath %TFS_DropLocation% -OutputDirectory %TFS_DropLocation% -Verbosity normal -version %NUGET_VERSION_NUMBER%
%NUGET_EXE% pack %TFS_DropLocation%\Nuget\Microsoft.Xbox.Live.SDK.Cpp.UWP.v140.x64.nuspec -BasePath %TFS_DropLocation% -OutputDirectory %TFS_DropLocation% -Verbosity normal -version %NUGET_VERSION_NUMBER% 
%NUGET_EXE% pack %TFS_DropLocation%\Nuget\Microsoft.Xbox.Live.SDK.Cpp.UWP.v140.x86.nuspec -BasePath %TFS_DropLocation% -OutputDirectory %TFS_DropLocation% -Verbosity normal -version %NUGET_VERSION_NUMBER% 
%NUGET_EXE% pack %TFS_DropLocation%\Nuget\Microsoft.Xbox.Live.SDK.Cpp.UWP.v141.ARM.nuspec -BasePath %TFS_DropLocation% -OutputDirectory %TFS_DropLocation% -Verbosity normal -version %NUGET_VERSION_NUMBER%
%NUGET_EXE% pack %TFS_DropLocation%\Nuget\Microsoft.Xbox.Live.SDK.Cpp.UWP.v141.x64.nuspec -BasePath %TFS_DropLocation% -OutputDirectory %TFS_DropLocation% -Verbosity normal -version %NUGET_VERSION_NUMBER% 
%NUGET_EXE% pack %TFS_DropLocation%\Nuget\Microsoft.Xbox.Live.SDK.Cpp.UWP.v141.x86.nuspec -BasePath %TFS_DropLocation% -OutputDirectory %TFS_DropLocation% -Verbosity normal -version %NUGET_VERSION_NUMBER% 


rmdir /s /q %WINSDK_OUTPUT_DEST%

rem create Cpp.XDK nuget package
%NUGET_EXE% pack %TFS_DropLocation%\Nuget\Microsoft.Xbox.Live.SDK.Cpp.XboxOneXDK.nuspec -BasePath %TFS_DropLocation% -OutputDirectory %TFS_DropLocation% -Verbosity normal -version %NUGET_VERSION_NUMBER%


mkdir %TFS_DropLocation%\NuGetBinaries\%BUILDCONFIGURATION%\%BUILDPLATFORM%
move %TFS_DropLocation%\*.nupkg %TFS_DropLocation%\NuGetBinaries\%BUILDCONFIGURATION%\%BUILDPLATFORM%
:skipNuget

:finalize

echo.
echo Done postBuildScript.cmd
echo.
endlocal

:done
exit /b

:GetTarget  
@echo off & setlocal
set gt=%temp%\_.vbs
echo set WshShell = WScript.CreateObject("WScript.Shell")>%gt%
echo set Lnk = WshShell.CreateShortcut(WScript.Arguments.Unnamed(0))>>%gt%
echo wscript.Echo Lnk.TargetPath>>%gt%
set script=cscript //nologo %gt%
For /f "delims=" %%a in ( '%script% "%~1"' ) do set target=%%a
del %gt%
endlocal & set %~2=%target%
exit /b