echo Running setBuildNumber.cmd

Utilities\VSOBuildScripts\ReadSDKVerHeader.exe Source\Shared\build_version.h year > year.txt
Utilities\VSOBuildScripts\ReadSDKVerHeader.exe Source\Shared\build_version.h month > month.txt
Utilities\VSOBuildScripts\ReadSDKVerHeader.exe Source\Shared\build_version.h qfe > qfe.txt
set /p SDK_RELEASE_YEAR=<year.txt
set /p SDK_RELEASE_MONTH=<month.txt
set /p SDK_RELEASE_QFE=<qfe.txt
del year.txt
del month.txt
del qfe.txt
if NOT "%UserAgentMonthOverride%" == "" set SDK_RELEASE_MONTH=%UserAgentMonthOverride%
if NOT "%UserAgentYearOverride%" == "" set SDK_RELEASE_YEAR=%UserAgentYearOverride%
if NOT "%UserAgentQFEOverride%" == "" set SDK_RELEASE_QFE=%UserAgentQFEOverride%
echo SDK_RELEASE_YEAR = '%SDK_RELEASE_YEAR%'
echo SDK_RELEASE_MONTH = '%SDK_RELEASE_MONTH%'
echo SDK_RELEASE_QFE = '%SDK_RELEASE_QFE%'

for /f "tokens=2 delims==" %%G in ('wmic os get localdatetime /value') do set datetime=%%G

set DATETIME_YEAR=%datetime:~0,4%
set DATETIME_MONTH=%datetime:~4,2%
set DATETIME_DAY=%datetime:~6,2%
set DATETIME_VER=%datetime:~8,6%

rem format release numbers
set SDK_POINT_NAME_YEAR=%DATETIME_YEAR%
set SDK_POINT_NAME_MONTH=%DATETIME_MONTH%
set SDK_POINT_NAME_DAY=%DATETIME_DAY%
set SDK_POINT_NAME_VER=%DATETIME_VER%
set SDK_POINT_NAME_BUILDID=%BUILD_BUILDID:~0,8%

set BUILD_VERSION_NUMBER=%SDK_RELEASE_YEAR%.%SDK_RELEASE_MONTH%.%SDK_POINT_NAME_YEAR%%SDK_POINT_NAME_MONTH%%SDK_POINT_NAME_DAY%.%SDK_RELEASE_QFE%
echo BUILD_VERSION_NUMBER='%BUILD_VERSION_NUMBER%'

rem set 

echo ##vso[build.updatebuildnumber]%BUILD_VERSION_NUMBER%

