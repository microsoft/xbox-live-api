echo Running preCompileScript.cmd

call %TFS_SourcesDirectory%\Utilities\VSOBuildScripts\setBuildVersion.cmd

for /f "tokens=2 delims==" %%G in ('wmic os get localdatetime /value') do set datetime=%%G

set DATETIME_YEAR=%datetime:~0,4%
set DATETIME_MONTH=%datetime:~4,2%
set DATETIME_DAY=%datetime:~6,2%

rem format release numbers
FOR /F "TOKENS=1 eol=/ DELIMS=. " %%A IN ("%TFS_VersionNumber%") DO SET SDK_POINT_NAME_YEARMONTH=%%A
FOR /F "TOKENS=2 eol=/ DELIMS=. " %%A IN ("%TFS_VersionNumber%") DO SET SDK_POINT_NAME_DAYVER=%%A
set SDK_POINT_NAME_YEAR=%DATETIME_YEAR%
set SDK_POINT_NAME_MONTH=%DATETIME_MONTH%
set SDK_POINT_NAME_DAY=%DATETIME_DAY%
set SDK_POINT_NAME_VER=%SDK_POINT_NAME_DAYVER:~2,9%

set SDK_RELEASE_NAME=%SDK_RELEASE_YEAR:~2,2%%SDK_RELEASE_MONTH%
set LONG_SDK_RELEASE_NAME=%SDK_RELEASE_NAME%-%SDK_POINT_NAME_YEAR%%SDK_POINT_NAME_MONTH%%SDK_POINT_NAME_DAY%-%SDK_POINT_NAME_VER%
set NUGET_VERSION_NUMBER=%SDK_RELEASE_YEAR%.%SDK_RELEASE_MONTH%.%SDK_POINT_NAME_YEAR%%SDK_POINT_NAME_MONTH%%SDK_POINT_NAME_DAY%.%SDK_POINT_NAME_VER%

set BUILD_VERSION_FILE=%TFS_SourcesDirectory%\Source\Shared\build_version.h

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

rem generate the .Ship. vcxprojs
call %TFS_SourcesDirectory%\Utilities\VSOBuildScripts\MakeShipProjects.cmd %TFS_SourcesDirectory%

rem create the build.cpp files
del %TFS_SourcesDirectory%\Build\Microsoft.Xbox.Services.140.XDK.Cpp\build.cpp
del %TFS_SourcesDirectory%\Build\Microsoft.Xbox.Services.141.XDK.Cpp\build.cpp
del %TFS_SourcesDirectory%\Build\Microsoft.Xbox.Services.140.UWP.Cpp\build.cpp
del %TFS_SourcesDirectory%\Build\Microsoft.Xbox.Services.141.UWP.Cpp\build.cpp
%TFS_SourcesDirectory%\Utilities\VSOBuildScripts\GenSDKBuildCppFile.exe %TFS_SourcesDirectory% %TFS_SourcesDirectory%\Build\Microsoft.Xbox.Services.140.XDK.Cpp\build.cpp xbox
%TFS_SourcesDirectory%\Utilities\VSOBuildScripts\GenSDKBuildCppFile.exe %TFS_SourcesDirectory% %TFS_SourcesDirectory%\Build\Microsoft.Xbox.Services.141.XDK.Cpp\build.cpp xbox
%TFS_SourcesDirectory%\Utilities\VSOBuildScripts\GenSDKBuildCppFile.exe %TFS_SourcesDirectory% %TFS_SourcesDirectory%\Build\Microsoft.Xbox.Services.140.UWP.Cpp\build.cpp uwp
%TFS_SourcesDirectory%\Utilities\VSOBuildScripts\GenSDKBuildCppFile.exe %TFS_SourcesDirectory% %TFS_SourcesDirectory%\Build\Microsoft.Xbox.Services.141.UWP.Cpp\build.cpp uwp
dir %TFS_SourcesDirectory%\Build\Microsoft.Xbox.Services.140.XDK.Cpp\build.cpp
dir %TFS_SourcesDirectory%\Build\Microsoft.Xbox.Services.141.XDK.Cpp\build.cpp
dir %TFS_SourcesDirectory%\Build\Microsoft.Xbox.Services.140.UWP.Cpp\build.cpp
dir %TFS_SourcesDirectory%\Build\Microsoft.Xbox.Services.141.UWP.Cpp\build.cpp

rem generate and compare against what's in git to see if any vcxproj is stale and email team if it is
call %TFS_SourcesDirectory%\Utilities\CMake\MakeProjects.cmd %TFS_SourcesDirectory% skipCopy
call %TFS_SourcesDirectory%\Utilities\VSOBuildScripts\CompareBuildFiles.cmd %TFS_SourcesDirectory% emailfailures

echo Done preCompileScript.cmd
:done