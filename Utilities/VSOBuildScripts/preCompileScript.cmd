echo Running preCompileScript.cmd

rem For debugging what is installed on build machines
dir "C:\Program Files (x86)\Microsoft Visual Studio\2017\"
dir "C:\Program Files (x86)\Microsoft Visual Studio\2019\"
dir "C:\Program Files (x86)\Microsoft Durango XDK"
dir "C:\Program Files (x86)\Windows Kits\10\Include"
dir "C:\Program Files (x86)\Microsoft GDK"

@echo off
echo Vars for GDK script
echo GXDKEDITION=%GXDKEDITION%
echo GameDK=%GameDK%
echo GameDKLatest=%GameDKLatest%
echo WindowsSdkDir=%WindowsSdkDir%
echo GamingGRDKBuild=%GamingGRDKBuild%
echo GamingGXDKBuild=%GamingGXDKBuild%
echo GamingWindowsSDKVersion=%GamingWindowsSDKVersion%
echo VSInstallDir=%VSInstallDir%
echo GDKUseBWOI=%GDKUseBWOI%
echo GDKUseBWOIDebug=%GDKUseBWOIDebug%
@echo on

set BUILD_TOOLS=%BUILD_STAGINGDIRECTORY%\sdk.buildtools\buildMachine
dir %BUILD_TOOLS%\*.cmd
call %BUILD_SOURCESDIRECTORY%\Utilities\VSOBuildScripts\setBuildVersion.cmd

for /f "tokens=2 delims==" %%G in ('wmic os get localdatetime /value') do set datetime=%%G

set DATETIME_YEAR=%datetime:~0,4%
set DATETIME_MONTH=%datetime:~4,2%
set DATETIME_DAY=%datetime:~6,2%

rem format release numbers
rem BUILD_BUILDNUMBER will be something like 2019.11.20191115.19649458
echo BUILD_BUILDNUMBER = %BUILD_BUILDNUMBER%
FOR /F "TOKENS=4 eol=/ DELIMS=. " %%A IN ("%BUILD_BUILDNUMBER%") DO SET SDK_POINT_NAME_VER=%%A
set SDK_POINT_NAME_YEAR=%DATETIME_YEAR%
set SDK_POINT_NAME_MONTH=%DATETIME_MONTH%
set SDK_POINT_NAME_DAY=%DATETIME_DAY%
set SDK_POINT_NAME_BUILDID=%BUILD_BUILDID:~0,8%

set SDK_RELEASE_NAME=%SDK_RELEASE_YEAR:~2,2%%SDK_RELEASE_MONTH%
set LONG_SDK_RELEASE_NAME=%SDK_RELEASE_NAME%-%SDK_POINT_NAME_YEAR%%SDK_POINT_NAME_MONTH%%SDK_POINT_NAME_DAY%-%SDK_RELEASE_QFE%
set NUGET_VERSION_NUMBER=%SDK_RELEASE_YEAR%.%SDK_RELEASE_MONTH%.%SDK_POINT_NAME_YEAR%%SDK_POINT_NAME_MONTH%%SDK_POINT_NAME_DAY%.%SDK_RELEASE_QFE%

set BUILD_VERSION_FILE=%BUILD_SOURCESDIRECTORY%\Source\Shared\build_version.h

del %BUILD_VERSION_FILE%
echo //********************************************************* > %BUILD_VERSION_FILE%
echo //>> %BUILD_VERSION_FILE%
echo // Copyright (c) Microsoft. All rights reserved.>> %BUILD_VERSION_FILE%
echo // THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF>> %BUILD_VERSION_FILE%
echo // ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY>> %BUILD_VERSION_FILE%
echo // IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR>> %BUILD_VERSION_FILE%
echo // PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.>> %BUILD_VERSION_FILE%
echo //>> %BUILD_VERSION_FILE%
echo //*********************************************************>> %BUILD_VERSION_FILE%
echo #pragma once>> %BUILD_VERSION_FILE%
echo. >> %BUILD_VERSION_FILE%
echo #define XBOX_SERVICES_API_VERSION_STRING "%NUGET_VERSION_NUMBER%" >> %BUILD_VERSION_FILE%
type %BUILD_VERSION_FILE%

:done
echo Done preCompileScript.cmd
