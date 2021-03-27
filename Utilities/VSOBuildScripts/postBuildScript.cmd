if "%1" == "local" goto testlocal
goto start

:testlocal
set BUILD_BINARIESDIRECTORY=%CD%\..\..\Bins
set BUILD_SOURCESDIRECTORY=%CD%\..\..

goto serializeForPostbuild

:start

rem if "%XES_SERIALPOSTBUILDREADY%" == "True" goto serializeForPostbuild
rem goto done
:serializeForPostbuild

echo Running postBuildScript.cmd
echo on

set

set SDK_DROP_LOCATION=%BUILD_BINARIESDIRECTORY%\SDK
rmdir /s /q %SDK_DROP_LOCATION%
mkdir %SDK_DROP_LOCATION%

if "%BUILD_DEFINITIONNAME%" == "XSAPI_Combined_Rolling_Build" goto finalize

setlocal
call %BUILD_SOURCESDIRECTORY%\Utilities\VSOBuildScripts\setBuildVersion.cmd

REM ------------------- VERSION SETUP BEGIN -------------------
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

REM goto skipCopy

xcopy %BUILD_SOURCESDIRECTORY%\Utilities\Nuget\* %SDK_DROP_LOCATION%\Nuget\ /s /e /q /y
robocopy /NJS /NJH /MT:16 /S /NP %BUILD_SOURCESDIRECTORY%\Include %SDK_DROP_LOCATION%\Include
robocopy /NJS /NJH /MT:16 /S /NP %BUILD_SOURCESDIRECTORY%\External\xal\External\libHttpClient\Include %SDK_DROP_LOCATION%\include
robocopy /NJS /NJH /MT:16 /S /NP %BUILD_SOURCESDIRECTORY%\External\xal\Source\Xal\Include %SDK_DROP_LOCATION%\include
robocopy /NJS /NJH /MT:16 /S /NP %BUILD_SOURCESDIRECTORY%\External\xal\Source\XalExtra\Include %SDK_DROP_LOCATION%\include
rmdir /s /q %SDK_DROP_LOCATION%\include\json_cpp

REM ------------------- XDK BEGIN -------------------
set SDK_DROP_LOCATION_XDK=%SDK_DROP_LOCATION%\XDK
set UWP_BINARIES=%BUILD_BINARIESDIRECTORY%
set XDK_BINARIES=%BUILD_BINARIESDIRECTORY%

set XDK_BIN_BUILD_SHARE_RELEA=%XDK_BINARIES%\Release\x64
set XDK_BIN_BUILD_SHARE_DEBUG=%XDK_BINARIES%\Debug\x64

mkdir %SDK_DROP_LOCATION_XDK%

mkdir %SDK_DROP_LOCATION_XDK%
mkdir %SDK_DROP_LOCATION_XDK%\SourceDist
mkdir %SDK_DROP_LOCATION_XDK%\cpp\binaries\release\v140
mkdir %SDK_DROP_LOCATION_XDK%\cpp\binaries\debug\v140

robocopy /NJS /NJH /MT:16 /S /NP %SDK_DROP_LOCATION%\Include %SDK_DROP_LOCATION_XDK%\cpp\include

copy %BUILD_SOURCESDIRECTORY%\Utilities\VSOBuildScripts\XDK\Readme.txt %SDK_DROP_LOCATION_XDK%\SourceDist\Readme.txt
%BUILD_SOURCESDIRECTORY%\Utilities\VSOBuildScripts\FindAndReplace.exe %SDK_DROP_LOCATION_XDK%\SourceDist\Readme.txt GITCOMMIT %BUILD_SOURCEVERSION%
%BUILD_SOURCESDIRECTORY%\Utilities\VSOBuildScripts\FindAndReplace.exe %SDK_DROP_LOCATION_XDK%\SourceDist\Readme.txt VERSION %NUGET_VERSION_NUMBER%

echo set MAJOR_VERSION=%SDK_RELEASE_NAME% > %SDK_DROP_LOCATION_XDK%\setver.cmd
echo set MINOR_VERSION=%MINOR_VERSION_NUMBER% >> %SDK_DROP_LOCATION_XDK%\setver.cmd

copy %XDK_BIN_BUILD_SHARE_RELEA%\Microsoft.Xbox.Services.140.XDK.Cpp\Microsoft.Xbox.Services.140.XDK.Cpp.lib %SDK_DROP_LOCATION_XDK%\cpp\binaries\release\v140\Microsoft.Xbox.Services.140.XDK.Cpp.lib.remove
copy %XDK_BIN_BUILD_SHARE_DEBUG%\Microsoft.Xbox.Services.140.XDK.Cpp\Microsoft.Xbox.Services.140.XDK.Cpp.lib %SDK_DROP_LOCATION_XDK%\cpp\binaries\debug\v140\Microsoft.Xbox.Services.140.XDK.Cpp.lib.remove
copy %XDK_BIN_BUILD_SHARE_RELEA%\Microsoft.Xbox.Services.140.XDK.Cpp\Microsoft.Xbox.Services.140.XDK.Cpp.pdb %SDK_DROP_LOCATION_XDK%\cpp\binaries\release\v140\Microsoft.Xbox.Services.140.XDK.Cpp.pdb
copy %XDK_BIN_BUILD_SHARE_DEBUG%\Microsoft.Xbox.Services.140.XDK.Cpp\Microsoft.Xbox.Services.140.XDK.Cpp.pdb %SDK_DROP_LOCATION_XDK%\cpp\binaries\debug\v140\Microsoft.Xbox.Services.140.XDK.Cpp.pdb

copy %XDK_BIN_BUILD_SHARE_RELEA%\libHttpClient.140.XDK.C\libHttpClient.140.XDK.C.lib %SDK_DROP_LOCATION_XDK%\cpp\binaries\release\v140\libHttpClient.140.XDK.C.lib.remove
copy %XDK_BIN_BUILD_SHARE_DEBUG%\libHttpClient.140.XDK.C\libHttpClient.140.XDK.C.lib %SDK_DROP_LOCATION_XDK%\cpp\binaries\debug\v140\libHttpClient.140.XDK.C.lib.remove
copy %XDK_BIN_BUILD_SHARE_RELEA%\libHttpClient.140.XDK.C\libHttpClient.140.XDK.C.pdb %SDK_DROP_LOCATION_XDK%\cpp\binaries\release\v140
copy %XDK_BIN_BUILD_SHARE_DEBUG%\libHttpClient.140.XDK.C\libHttpClient.140.XDK.C.pdb %SDK_DROP_LOCATION_XDK%\cpp\binaries\debug\v140

copy %XDK_BIN_BUILD_SHARE_RELEA%\casablanca140.Xbox\casablanca140.xbox.lib %SDK_DROP_LOCATION_XDK%\cpp\binaries\release\v140\casablanca140.xbox.lib.remove
copy %XDK_BIN_BUILD_SHARE_DEBUG%\casablanca140.Xbox\casablanca140.xbox.lib %SDK_DROP_LOCATION_XDK%\cpp\binaries\debug\v140\casablanca140.xbox.lib.remove
copy %XDK_BIN_BUILD_SHARE_RELEA%\casablanca140.Xbox\casablanca140.xbox.pdb %SDK_DROP_LOCATION_XDK%\cpp\binaries\release\v140\casablanca140.xbox.pdb
copy %XDK_BIN_BUILD_SHARE_DEBUG%\casablanca140.Xbox\casablanca140.xbox.pdb %SDK_DROP_LOCATION_XDK%\cpp\binaries\debug\v140\casablanca140.xbox.pdb

set PATH_XDK_DROP_BIN=%SDK_DROP_LOCATION_XDK%\cpp

mkdir %PATH_XDK_DROP_BIN%\binaries\release\v141
mkdir %PATH_XDK_DROP_BIN%\binaries\debug\v141

set TYPE=release
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\x64\Microsoft.Xbox.Services.141.XDK.Cpp\Microsoft.Xbox.Services.141.XDK.Cpp.pdb %PATH_XDK_DROP_BIN%\binaries\%TYPE%\v141
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\x64\Microsoft.Xbox.Services.141.XDK.Cpp\Microsoft.Xbox.Services.141.XDK.Cpp.lib %PATH_XDK_DROP_BIN%\binaries\%TYPE%\v141\Microsoft.Xbox.Services.141.XDK.Cpp.lib.remove
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\x64\casablanca141.Xbox\casablanca141.xbox.pdb %PATH_XDK_DROP_BIN%\binaries\%TYPE%\v141
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\x64\casablanca141.Xbox\casablanca141.xbox.lib %PATH_XDK_DROP_BIN%\binaries\%TYPE%\v141\casablanca141.xbox.lib.remove
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\x64\libHttpClient.141.XDK.C\libHttpClient.141.XDK.C.pdb %PATH_XDK_DROP_BIN%\binaries\%TYPE%\v141
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\x64\libHttpClient.141.XDK.C\libHttpClient.141.XDK.C.lib %PATH_XDK_DROP_BIN%\binaries\%TYPE%\v141\libHttpClient.141.XDK.C.lib.remove

set TYPE=debug
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\x64\Microsoft.Xbox.Services.141.XDK.Cpp\Microsoft.Xbox.Services.141.XDK.Cpp.pdb %PATH_XDK_DROP_BIN%\binaries\%TYPE%\v141
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\x64\Microsoft.Xbox.Services.141.XDK.Cpp\Microsoft.Xbox.Services.141.XDK.Cpp.lib %PATH_XDK_DROP_BIN%\binaries\%TYPE%\v141\Microsoft.Xbox.Services.141.XDK.Cpp.lib.remove
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\x64\casablanca141.Xbox\casablanca141.xbox.pdb %PATH_XDK_DROP_BIN%\binaries\%TYPE%\v141
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\x64\casablanca141.Xbox\casablanca141.xbox.lib %PATH_XDK_DROP_BIN%\binaries\%TYPE%\v141\casablanca141.xbox.lib.remove
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\x64\libHttpClient.141.XDK.C\libHttpClient.141.XDK.C.pdb %PATH_XDK_DROP_BIN%\binaries\%TYPE%\v141
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\x64\libHttpClient.141.XDK.C\libHttpClient.141.XDK.C.lib %PATH_XDK_DROP_BIN%\binaries\%TYPE%\v141\libHttpClient.141.XDK.C.lib.remove

REM ------------------- GDK BEGIN -------------------
set SDK_DROP_LOCATION_GDK=%SDK_DROP_LOCATION%\GDK
set SDK_DROP_LOCATION_GDK_BIN=%SDK_DROP_LOCATION_GDK%\c
mkdir %SDK_DROP_LOCATION_GDK%
mkdir %SDK_DROP_LOCATION_GDK_BIN%
mkdir %SDK_DROP_LOCATION_GDK_BIN%\include
mkdir %SDK_DROP_LOCATION_GDK_BIN%\binaries\GSDKXboxExtensionSDKs\release\v141
mkdir %SDK_DROP_LOCATION_GDK_BIN%\binaries\GSDKXboxExtensionSDKs\debug\v141
mkdir %SDK_DROP_LOCATION_GDK_BIN%\binaries\GSDKXboxExtensionSDKs\release\v142
mkdir %SDK_DROP_LOCATION_GDK_BIN%\binaries\GSDKXboxExtensionSDKs\debug\v142
mkdir %SDK_DROP_LOCATION_GDK_BIN%\binaries\GSDKDesktopExtensionSDKs\release\v141
mkdir %SDK_DROP_LOCATION_GDK_BIN%\binaries\GSDKDesktopExtensionSDKs\debug\v141
mkdir %SDK_DROP_LOCATION_GDK_BIN%\binaries\GSDKDesktopExtensionSDKs\release\v142
mkdir %SDK_DROP_LOCATION_GDK_BIN%\binaries\GSDKDesktopExtensionSDKs\debug\v142

copy %BUILD_SOURCESDIRECTORY%\Utilities\GDK\Xbox.Services.API.C.props %SDK_DROP_LOCATION_GDK_BIN%

robocopy /NJS /NJH /MT:16 /S /NP %SDK_DROP_LOCATION%\Include %SDK_DROP_LOCATION_GDK_BIN%\include
mkdir %SDK_DROP_LOCATION_GDK_BIN%\include\cpprest
mkdir %SDK_DROP_LOCATION_GDK_BIN%\include\cpprest\details
mkdir %SDK_DROP_LOCATION_GDK_BIN%\include\pplx
copy %BUILD_SOURCESDIRECTORY%\ThirdPartyNotices.txt %SDK_DROP_LOCATION_GDK_BIN%\include\xsapi-cpp
copy %BUILD_SOURCESDIRECTORY%\ThirdPartyNotices.txt %SDK_DROP_LOCATION_GDK_BIN%\include\xsapi-c
copy %BUILD_SOURCESDIRECTORY%\External\xal\External\libHttpClient\ThirdPartyNotices.txt %SDK_DROP_LOCATION_GDK_BIN%\include\httpClient
copy %BUILD_SOURCESDIRECTORY%\ThirdPartyNotices.txt %SDK_DROP_LOCATION_GDK_BIN%\include\cpprest
copy %BUILD_SOURCESDIRECTORY%\ThirdPartyNotices.txt %SDK_DROP_LOCATION_GDK_BIN%\include\pplx
copy %SDK_DROP_LOCATION%\include\cpprestinclude\cpprest\astreambuf.h %SDK_DROP_LOCATION_GDK_BIN%\include\cpprest\astreambuf.h
copy %SDK_DROP_LOCATION%\include\cpprestinclude\cpprest\asyncrt_utils.h %SDK_DROP_LOCATION_GDK_BIN%\include\cpprest\asyncrt_utils.h
copy %SDK_DROP_LOCATION%\include\cpprestinclude\cpprest\base_uri.h %SDK_DROP_LOCATION_GDK_BIN%\include\cpprest\base_uri.h
copy %SDK_DROP_LOCATION%\include\cpprestinclude\cpprest\containerstream.h %SDK_DROP_LOCATION_GDK_BIN%\include\cpprest\containerstream.h
copy %SDK_DROP_LOCATION%\include\cpprestinclude\cpprest\http_headers.h %SDK_DROP_LOCATION_GDK_BIN%\include\cpprest\http_headers.h
copy %SDK_DROP_LOCATION%\include\cpprestinclude\cpprest\http_msg.h %SDK_DROP_LOCATION_GDK_BIN%\include\cpprest\http_msg.h
copy %SDK_DROP_LOCATION%\include\cpprestinclude\cpprest\json.h %SDK_DROP_LOCATION_GDK_BIN%\include\cpprest\json.h
copy %SDK_DROP_LOCATION%\include\cpprestinclude\cpprest\streams.h %SDK_DROP_LOCATION_GDK_BIN%\include\cpprest\streams.h
copy %SDK_DROP_LOCATION%\include\cpprestinclude\cpprest\uri.h %SDK_DROP_LOCATION_GDK_BIN%\include\cpprest\uri.h
copy %SDK_DROP_LOCATION%\include\cpprestinclude\cpprest\uri_builder.h %SDK_DROP_LOCATION_GDK_BIN%\include\cpprest\uri_builder.h
copy %SDK_DROP_LOCATION%\include\cpprestinclude\cpprest\details\basic_types.h %SDK_DROP_LOCATION_GDK_BIN%\include\cpprest\details\basic_types.h
copy %SDK_DROP_LOCATION%\include\cpprestinclude\cpprest\details\cpprest_compat.h %SDK_DROP_LOCATION_GDK_BIN%\include\cpprest\details\cpprest_compat.h
copy %SDK_DROP_LOCATION%\include\cpprestinclude\cpprest\details\http_constants.dat %SDK_DROP_LOCATION_GDK_BIN%\include\cpprest\details\http_constants.dat
copy %SDK_DROP_LOCATION%\include\cpprestinclude\cpprest\details\SafeInt3.hpp %SDK_DROP_LOCATION_GDK_BIN%\include\cpprest\details\SafeInt3.hpp
copy %SDK_DROP_LOCATION%\include\cpprestinclude\cpprest\details\uri_parser.h %SDK_DROP_LOCATION_GDK_BIN%\include\cpprest\details\uri_parser.h
copy %SDK_DROP_LOCATION%\include\cpprestinclude\pplx\pplxtasks.140.h %SDK_DROP_LOCATION_GDK_BIN%\include\pplx\pplxtasks.140.h
copy %SDK_DROP_LOCATION%\include\cpprestinclude\pplx\pplxtasks.h %SDK_DROP_LOCATION_GDK_BIN%\include\pplx\pplxtasks.h
rmdir /s /q %SDK_DROP_LOCATION_GDK_BIN%\include\cpprestinclude
rmdir /s /q %SDK_DROP_LOCATION_GDK_BIN%\Include\XalExtra
del %SDK_DROP_LOCATION_GDK_BIN%\include\Xal\xal_android.h
del %SDK_DROP_LOCATION_GDK_BIN%\include\Xal\xal_internal_telemetry.h
del %SDK_DROP_LOCATION_GDK_BIN%\include\Xal\xal_internal_web_account.h
del %SDK_DROP_LOCATION_GDK_BIN%\include\Xal\xal_ios.h
del %SDK_DROP_LOCATION_GDK_BIN%\include\Xal\xal_apple.h
del %SDK_DROP_LOCATION_GDK_BIN%\include\Xal\xal_generic.h
del %SDK_DROP_LOCATION_GDK_BIN%\include\Xal\xal_platform.h
del %SDK_DROP_LOCATION_GDK_BIN%\include\Xal\xal_platform_types.h
del %SDK_DROP_LOCATION_GDK_BIN%\include\Xal\xal_uwp.h
del %SDK_DROP_LOCATION_GDK_BIN%\include\Xal\xal_uwp_user.h
del %SDK_DROP_LOCATION_GDK_BIN%\include\Xal\xal_version.h
del %SDK_DROP_LOCATION_GDK_BIN%\include\Xal\xal_win32.h
del %SDK_DROP_LOCATION_GDK_BIN%\include\Xal\xal_xdk.h
del %SDK_DROP_LOCATION_GDK_BIN%\include\Xal\xal_grts*.h
del %SDK_DROP_LOCATION_GDK_BIN%\include\async.h
del %SDK_DROP_LOCATION_GDK_BIN%\include\asyncProvider.h
del %SDK_DROP_LOCATION_GDK_BIN%\include\asyncQueue.h
del %SDK_DROP_LOCATION_GDK_BIN%\include\asyncQueueEx.h
del %SDK_DROP_LOCATION_GDK_BIN%\include\XAsync.h
del %SDK_DROP_LOCATION_GDK_BIN%\include\XAsyncProvider.h
del %SDK_DROP_LOCATION_GDK_BIN%\include\XTaskQueue.h
del %SDK_DROP_LOCATION_GDK_BIN%\include\Xal\grts_base\grts_account_manager.h
del %SDK_DROP_LOCATION_GDK_BIN%\include\Xal\grts_base\impersonation_ctx.h
del %SDK_DROP_LOCATION_GDK_BIN%\include\Xal\grts_base\nt_user_context.h
del %SDK_DROP_LOCATION_GDK_BIN%\include\Xal\xal_internal_types.h

set PLAT=Gaming.Xbox.x64
set PLAT_LAYOUT_NAME=GSDKXboxExtensionSDKs
set TYPE=release
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\%PLAT%\Microsoft.Xbox.Services.141.GDK.C\Microsoft.Xbox.Services.141.GDK.C.pdb %SDK_DROP_LOCATION_GDK_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v141
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\%PLAT%\Microsoft.Xbox.Services.141.GDK.C\Microsoft.Xbox.Services.141.GDK.C.lib %SDK_DROP_LOCATION_GDK_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v141\Microsoft.Xbox.Services.141.GDK.C.lib.remove
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\%PLAT%\Microsoft.Xbox.Services.142.GDK.C\Microsoft.Xbox.Services.142.GDK.C.pdb %SDK_DROP_LOCATION_GDK_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v142
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\%PLAT%\Microsoft.Xbox.Services.142.GDK.C\Microsoft.Xbox.Services.142.GDK.C.lib %SDK_DROP_LOCATION_GDK_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v142\Microsoft.Xbox.Services.142.GDK.C.lib.remove
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\%PLAT%\libHttpClient.141.GDK.C\libHttpClient.141.GDK.C.pdb %SDK_DROP_LOCATION_GDK_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v141
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\%PLAT%\libHttpClient.141.GDK.C\libHttpClient.141.GDK.C.lib %SDK_DROP_LOCATION_GDK_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v141\libHttpClient.141.GDK.C.lib.remove
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\%PLAT%\libHttpClient.142.GDK.C\libHttpClient.142.GDK.C.pdb %SDK_DROP_LOCATION_GDK_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v142
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\%PLAT%\libHttpClient.142.GDK.C\libHttpClient.142.GDK.C.lib %SDK_DROP_LOCATION_GDK_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v142\libHttpClient.141.GDK.C.lib.remove

set TYPE=debug
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\%PLAT%\Microsoft.Xbox.Services.141.GDK.C\Microsoft.Xbox.Services.141.GDK.C.pdb %SDK_DROP_LOCATION_GDK_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v141
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\%PLAT%\Microsoft.Xbox.Services.141.GDK.C\Microsoft.Xbox.Services.141.GDK.C.lib %SDK_DROP_LOCATION_GDK_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v141\Microsoft.Xbox.Services.141.GDK.C.lib.remove
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\%PLAT%\Microsoft.Xbox.Services.142.GDK.C\Microsoft.Xbox.Services.142.GDK.C.pdb %SDK_DROP_LOCATION_GDK_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v142
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\%PLAT%\Microsoft.Xbox.Services.142.GDK.C\Microsoft.Xbox.Services.142.GDK.C.lib %SDK_DROP_LOCATION_GDK_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v142\Microsoft.Xbox.Services.142.GDK.C.lib.remove
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\%PLAT%\libHttpClient.141.GDK.C\libHttpClient.141.GDK.C.pdb %SDK_DROP_LOCATION_GDK_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v141
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\%PLAT%\libHttpClient.141.GDK.C\libHttpClient.141.GDK.C.lib %SDK_DROP_LOCATION_GDK_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v141\libHttpClient.141.GDK.C.lib.remove
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\%PLAT%\libHttpClient.142.GDK.C\libHttpClient.142.GDK.C.pdb %SDK_DROP_LOCATION_GDK_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v142
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\%PLAT%\libHttpClient.142.GDK.C\libHttpClient.142.GDK.C.lib %SDK_DROP_LOCATION_GDK_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v142\libHttpClient.142.GDK.C.lib.remove

set PLAT=Gaming.Desktop.x64
set PLAT_LAYOUT_NAME=GSDKDesktopExtensionSDKs
set TYPE=release
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\%PLAT%\Microsoft.Xbox.Services.141.GDK.C\Microsoft.Xbox.Services.141.GDK.C.pdb %SDK_DROP_LOCATION_GDK_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v141
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\%PLAT%\Microsoft.Xbox.Services.141.GDK.C\Microsoft.Xbox.Services.141.GDK.C.lib %SDK_DROP_LOCATION_GDK_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v141\Microsoft.Xbox.Services.141.GDK.C.lib.remove
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\%PLAT%\Microsoft.Xbox.Services.142.GDK.C\Microsoft.Xbox.Services.142.GDK.C.pdb %SDK_DROP_LOCATION_GDK_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v142
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\%PLAT%\Microsoft.Xbox.Services.142.GDK.C\Microsoft.Xbox.Services.142.GDK.C.lib %SDK_DROP_LOCATION_GDK_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v142\Microsoft.Xbox.Services.142.GDK.C.lib.remove
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\%PLAT%\libHttpClient.141.GDK.C\libHttpClient.141.GDK.C.pdb %SDK_DROP_LOCATION_GDK_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v141
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\%PLAT%\libHttpClient.141.GDK.C\libHttpClient.141.GDK.C.lib %SDK_DROP_LOCATION_GDK_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v141\libHttpClient.141.GDK.C.lib.remove
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\%PLAT%\libHttpClient.142.GDK.C\libHttpClient.142.GDK.C.pdb %SDK_DROP_LOCATION_GDK_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v142
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\%PLAT%\libHttpClient.142.GDK.C\libHttpClient.142.GDK.C.lib %SDK_DROP_LOCATION_GDK_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v142\libHttpClient.142.GDK.C.lib.remove

set TYPE=debug
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\%PLAT%\Microsoft.Xbox.Services.141.GDK.C\Microsoft.Xbox.Services.141.GDK.C.pdb %SDK_DROP_LOCATION_GDK_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v141
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\%PLAT%\Microsoft.Xbox.Services.141.GDK.C\Microsoft.Xbox.Services.141.GDK.C.lib %SDK_DROP_LOCATION_GDK_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v141\Microsoft.Xbox.Services.141.GDK.C.lib.remove
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\%PLAT%\Microsoft.Xbox.Services.142.GDK.C\Microsoft.Xbox.Services.142.GDK.C.pdb %SDK_DROP_LOCATION_GDK_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v142
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\%PLAT%\Microsoft.Xbox.Services.142.GDK.C\Microsoft.Xbox.Services.142.GDK.C.lib %SDK_DROP_LOCATION_GDK_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v142\Microsoft.Xbox.Services.142.GDK.C.lib.remove
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\%PLAT%\libHttpClient.141.GDK.C\libHttpClient.141.GDK.C.pdb %SDK_DROP_LOCATION_GDK_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v141
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\%PLAT%\libHttpClient.141.GDK.C\libHttpClient.141.GDK.C.lib %SDK_DROP_LOCATION_GDK_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v141\libHttpClient.141.GDK.C.lib.remove
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\%PLAT%\libHttpClient.142.GDK.C\libHttpClient.142.GDK.C.pdb %SDK_DROP_LOCATION_GDK_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v142
copy %BUILD_BINARIESDIRECTORY%\%TYPE%\%PLAT%\libHttpClient.142.GDK.C\libHttpClient.142.GDK.C.lib %SDK_DROP_LOCATION_GDK_BIN%\binaries\%PLAT_LAYOUT_NAME%\%TYPE%\v142\libHttpClient.142.GDK.C.lib.remove

set BUILD_TOOLS=%BUILD_STAGINGDIRECTORY%\sdk.buildtools\buildMachine

copy %BUILD_TOOLS%\GdkCopyHeaders.cmd %SDK_DROP_LOCATION_GDK%
copy %BUILD_TOOLS%\VPackLogReader.exe %SDK_DROP_LOCATION_GDK%
copy %BUILD_TOOLS%\GdkCreateVPack.cmd %SDK_DROP_LOCATION_GDK%
copy %BUILD_TOOLS%\GdkFormatKML.cmd %SDK_DROP_LOCATION_GDK%
copy %BUILD_TOOLS%\GdkCopyOutputToLocal.cmd %SDK_DROP_LOCATION_GDK%
copy %BUILD_SOURCESDIRECTORY%\Utilities\VSOBuildScripts\setBuildVersion.cmd %SDK_DROP_LOCATION_GDK%
copy %BUILD_SOURCESDIRECTORY%\Utilities\VSOBuildScripts\ReadSDKVerHeader.exe %SDK_DROP_LOCATION_GDK%
copy %BUILD_SOURCESDIRECTORY%\Source\Shared\build_version.h %SDK_DROP_LOCATION_GDK%

set BUILD_INFO_PATH=%SDK_DROP_LOCATION_GDK%\build_info.txt
set BUILD_INFO_BODY="%SDK_DROP_LOCATION%    https://microsoft.visualstudio.com/Xbox.Services/_build/index?buildId=%BUILD_BUILDID%&_a=summary"
del %BUILD_INFO_PATH%
echo %BUILD_INFO_BODY% > %BUILD_INFO_PATH%

REM ------------------- GDK DOCS BEGIN-------------------
set PATH_GDK_DOCS=%SDK_DROP_LOCATION%\GDK-Docs
set PATH_GDK_DOCS_DROP=%PATH_GDK_DOCS%\docs
mkdir %PATH_GDK_DOCS%
rmdir /s /q %PATH_GDK_DOCS%
mkdir %PATH_GDK_DOCS_DROP%
call %BUILD_TOOLS%\Noggin\GenerateReferenceDocs.cmd %SDK_DROP_LOCATION%\include %PATH_GDK_DOCS_DROP%
copy %BUILD_TOOLS%\Noggin\CopyReferenceDocsToDocsRepo.cmd %PATH_GDK_DOCS%


REM ------------------- UNITY BEGIN-------------------
REM set SDK_DROP_LOCATION_UNITY=%SDK_DROP_LOCATION%\unity
REM set UNITY_ASSET_SRC=%BUILD_SOURCESDIRECTORY%\Utilities\IDXboxUnityAssetLayout

REM set TYPE=x64
REM copy %UWP_BINARIES%\Release\%TYPE%\Microsoft.Xbox.Services.141.UWP.WinRT\Microsoft.Xbox.Services.dll %UNITY_ASSET_SRC%\Assets\XboxServicesAPI\Binaries\%TYPE%
REM copy %UWP_BINARIES%\Release\%TYPE%\Microsoft.Xbox.Services.141.UWP.WinRT\Microsoft.Xbox.Services.winmd %UNITY_ASSET_SRC%\Assets\XboxServicesAPI\Binaries\WinMD
REM set TYPE=ARM
REM copy %UWP_BINARIES%\Release\%TYPE%\Microsoft.Xbox.Services.141.UWP.WinRT\Microsoft.Xbox.Services.dll %UNITY_ASSET_SRC%\Assets\XboxServicesAPI\Binaries\%TYPE%
REM set TYPE=x86
REM copy %UWP_BINARIES%\Release\%TYPE%\Microsoft.Xbox.Services.141.UWP.WinRT\Microsoft.Xbox.Services.dll %UNITY_ASSET_SRC%\Assets\XboxServicesAPI\Binaries\%TYPE%

REM set UNITY_PACKAGE_NAME=%UNITY_ASSET_SRC%\XboxServicesAPI-%SDK_RELEASE_NAME%-%MINOR_VERSION_NUMBER%.unitypackage
REM "C:\Program Files\Unity\Editor\Unity.exe" -ea SilentlyContinue -batchmode -logFile "%UNITY_ASSET_SRC%\unity.log" -projectPath "%UNITY_ASSET_SRC%" -exportPackage "Assets\XboxServicesAPI" "%UNITY_PACKAGE_NAME%" -quit

rem mkdir %SDK_DROP_LOCATION_UNITY%
rem robocopy /NJS /NJH /MT:16 /S /NP %UNITY_ASSET_SRC% %SDK_DROP_LOCATION_UNITY%

if "%1" == "local" goto skipMavenPublish

REM -------------------ANDROID BEGIN-------------------
set JAVA_HOME="C:\Program Files\Android\jdk\microsoft_dist_openjdk_1.8.0.25"
if "%ANDROID_HOME%" == "" set ANDROID_HOME=C:\Program Files (x86)\Android\android-sdk
rem If we eventually want to include native libs/headers in the maven artifact we can re-enable this
rem call %BUILD_SOURCESDIRECTORY%\Java\com.microsoft.xboxlive\gradlew.bat --project-dir=%BUILD_SOURCESDIRECTORY%\Java\com.microsoft.xboxlive copyNdkLibsToAAR -PInputAar=%TFS_BinariesDirectory%\Release\x86\com.microsoft.xboxlive\com.microsoft.xboxlive.aar -POutDir=%TFS_BinariesDirectory%\Release\x86\com.microsoft.xboxlive\ndklibs --info
call %BUILD_SOURCESDIRECTORY%\Java\com.microsoft.xboxlive\gradlew.bat  --project-dir=%BUILD_SOURCESDIRECTORY%\Java\com.microsoft.xboxlive publish -PInputAar=%SDK_DROP_LOCATION%\Release\x86\com.microsoft.xboxlive\com.microsoft.xboxlive.aar --info
call %BUILD_SOURCESDIRECTORY%\external\xal\external\libHttpClient\Build\libHttpClient.141.Android.Java\gradlew.bat  --project-dir=%BUILD_SOURCESDIRECTORY%\external\xal\external\libHttpClient\Build\libHttpClient.141.Android.Java publish -PInputAar=%SDK_DROP_LOCATION%\Release\x86\libHttpClient.141.Android.Java\libHttpClient.aar --info
call %BUILD_SOURCESDIRECTORY%\external\xal\source\XalAndroidJava\gradlew.bat  --project-dir=%BUILD_SOURCESDIRECTORY%\external\xal\source\XalAndroidJava publish -PaarPath=%SDK_DROP_LOCATION%\Release\x86\XalAndroidJava\XalAndroidJava.aar --info

rem Copy Maven output
xcopy %BUILD_SOURCESDIRECTORY%\Java\com.microsoft.xboxlive\build\Maven\ %SDK_DROP_LOCATION%\Maven\ /s /e /q /y
xcopy %BUILD_SOURCESDIRECTORY%\external\xal\external\libHttpClient\Build\libHttpClient.141.Android.Java\build\Maven\ %SDK_DROP_LOCATION%\Maven\ /s /e /q /y
xcopy %BUILD_SOURCESDIRECTORY%\external\xal\source\XalAndroidJava\build\Maven\ %SDK_DROP_LOCATION%\Maven\ /s /e /q /y

rem Copy native android libs and headers
xcopy %SDK_DROP_LOCATION%\Include\* %SDK_DROP_LOCATION%\Maven\ndk\include\ /s /e /q /y
rmdir /s /q %SDK_DROP_LOCATION%\Maven\ndk\include\cpprest
rmdir /s /q %SDK_DROP_LOCATION%\Maven\ndk\include\pplx
rmdir /s /q %SDK_DROP_LOCATION%\Maven\ndk\include\XalExtra\Win32
for /R %SDK_DROP_LOCATION%\Release\x86\ %%f in (*.a) do xcopy %%f %SDK_DROP_LOCATION%\Maven\ndk\libs\Release\x86\
for /R %SDK_DROP_LOCATION%\Release\x64\ %%f in (*.a) do xcopy %%f %SDK_DROP_LOCATION%\Maven\ndk\libs\Release\x64\
for /R %SDK_DROP_LOCATION%\Release\arm\ %%f in (*.a) do xcopy %%f %SDK_DROP_LOCATION%\Maven\ndk\libs\Release\arm\
for /R %SDK_DROP_LOCATION%\Release\arm64\ %%f in (*.a) do xcopy %%f %SDK_DROP_LOCATION%\Maven\ndk\libs\Release\arm64\
for /R %SDK_DROP_LOCATION%\Debug\x86\ %%f in (*.a) do xcopy %%f %SDK_DROP_LOCATION%\Maven\ndk\libs\Debug\x86\
for /R %SDK_DROP_LOCATION%\Debug\x64\ %%f in (*.a) do xcopy %%f %SDK_DROP_LOCATION%\Maven\ndk\libs\Debug\x64\
for /R %SDK_DROP_LOCATION%\Debug\arm\ %%f in (*.a) do xcopy %%f %SDK_DROP_LOCATION%\Maven\ndk\libs\Debug\arm\
for /R %SDK_DROP_LOCATION%\Debug\arm64\ %%f in (*.a) do xcopy %%f %SDK_DROP_LOCATION%\Maven\ndk\libs\Debug\arm64\
xcopy %BUILD_SOURCESDIRECTORY%\Build\xsapi.android.props %SDK_DROP_LOCATION%\Maven\ndk\

:skipMavenPublish
@echo on

if "%skipNuget%" == "1" goto skipNuget
if "%1" == "local" goto skipNuget
rem :skipCopy

REM Skipping nuget packages creation until needed
goto skipNuget

REM -------------------NUGET PACKAGE BEGIN-------------------
rem create Cpp.Win32 nuget package
rmdir /s /q %SDK_DROP_LOCATION%\include\winrt
rmdir /s /q %SDK_DROP_LOCATION%\include\cppwinrt
%NUGET_EXE% pack %SDK_DROP_LOCATION%\Nuget\Microsoft.Xbox.Live.SDK.Cpp.Win32.nuspec -BasePath %SDK_DROP_LOCATION% -OutputDirectory %SDK_DROP_LOCATION% -Verbosity normal -version %NUGET_VERSION_NUMBER%
%NUGET_EXE% pack %SDK_DROP_LOCATION%\Nuget\Microsoft.Xbox.Live.SDK.Cpp.Win32.v141.x64.nuspec -BasePath %SDK_DROP_LOCATION% -OutputDirectory %SDK_DROP_LOCATION% -Verbosity normal -version %NUGET_VERSION_NUMBER% 
%NUGET_EXE% pack %SDK_DROP_LOCATION%\Nuget\Microsoft.Xbox.Live.SDK.Cpp.Win32.v141.x86.nuspec -BasePath %SDK_DROP_LOCATION% -OutputDirectory %SDK_DROP_LOCATION% -Verbosity normal -version %NUGET_VERSION_NUMBER% 

rem create Cpp.UWP nuget package
%NUGET_EXE% pack %SDK_DROP_LOCATION%\Nuget\Microsoft.Xbox.Live.SDK.Cpp.UWP.nuspec -BasePath %SDK_DROP_LOCATION% -OutputDirectory %SDK_DROP_LOCATION% -Verbosity normal -version %NUGET_VERSION_NUMBER%
%NUGET_EXE% pack %SDK_DROP_LOCATION%\Nuget\Microsoft.Xbox.Live.SDK.Cpp.UWP.v140.ARM.nuspec -BasePath %SDK_DROP_LOCATION% -OutputDirectory %SDK_DROP_LOCATION% -Verbosity normal -version %NUGET_VERSION_NUMBER%
%NUGET_EXE% pack %SDK_DROP_LOCATION%\Nuget\Microsoft.Xbox.Live.SDK.Cpp.UWP.v140.x64.nuspec -BasePath %SDK_DROP_LOCATION% -OutputDirectory %SDK_DROP_LOCATION% -Verbosity normal -version %NUGET_VERSION_NUMBER% 
%NUGET_EXE% pack %SDK_DROP_LOCATION%\Nuget\Microsoft.Xbox.Live.SDK.Cpp.UWP.v140.x86.nuspec -BasePath %SDK_DROP_LOCATION% -OutputDirectory %SDK_DROP_LOCATION% -Verbosity normal -version %NUGET_VERSION_NUMBER% 
%NUGET_EXE% pack %SDK_DROP_LOCATION%\Nuget\Microsoft.Xbox.Live.SDK.Cpp.UWP.v141.ARM.nuspec -BasePath %SDK_DROP_LOCATION% -OutputDirectory %SDK_DROP_LOCATION% -Verbosity normal -version %NUGET_VERSION_NUMBER%
%NUGET_EXE% pack %SDK_DROP_LOCATION%\Nuget\Microsoft.Xbox.Live.SDK.Cpp.UWP.v141.x64.nuspec -BasePath %SDK_DROP_LOCATION% -OutputDirectory %SDK_DROP_LOCATION% -Verbosity normal -version %NUGET_VERSION_NUMBER% 
%NUGET_EXE% pack %SDK_DROP_LOCATION%\Nuget\Microsoft.Xbox.Live.SDK.Cpp.UWP.v141.x86.nuspec -BasePath %SDK_DROP_LOCATION% -OutputDirectory %SDK_DROP_LOCATION% -Verbosity normal -version %NUGET_VERSION_NUMBER% 

rmdir /s /q %WINSDK_OUTPUT_DEST%

rem create Cpp.XDK nuget package
%NUGET_EXE% pack %SDK_DROP_LOCATION%\Nuget\Microsoft.Xbox.Live.SDK.Cpp.XboxOneXDK.nuspec -BasePath %SDK_DROP_LOCATION% -OutputDirectory %SDK_DROP_LOCATION% -Verbosity normal -version %NUGET_VERSION_NUMBER%

mkdir %SDK_DROP_LOCATION%\NuGetBinaries\%BUILDCONFIGURATION%\%BUILDPLATFORM%
move %SDK_DROP_LOCATION%\*.nupkg %SDK_DROP_LOCATION%\NuGetBinaries\%BUILDCONFIGURATION%\%BUILDPLATFORM%
:skipNuget

REM -------------------DONE-------------------
:finalize

echo.
echo Done postBuildScript.cmd
echo.
endlocal

:done
exit /b
