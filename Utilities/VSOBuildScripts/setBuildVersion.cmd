%BUILD_SOURCESDIRECTORY%\Utilities\VSOBuildScripts\ReadSDKVerHeader.exe %BUILD_SOURCESDIRECTORY%\Source\Shared\build_version.h year > year.txt
%BUILD_SOURCESDIRECTORY%\Utilities\VSOBuildScripts\ReadSDKVerHeader.exe %BUILD_SOURCESDIRECTORY%\Source\Shared\build_version.h month > month.txt
%BUILD_SOURCESDIRECTORY%\Utilities\VSOBuildScripts\ReadSDKVerHeader.exe %BUILD_SOURCESDIRECTORY%\Source\Shared\build_version.h qfe > qfe.txt
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
set XDKVER=170300