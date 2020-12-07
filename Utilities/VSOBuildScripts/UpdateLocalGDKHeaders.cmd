set GDK_VERSION=190200
if "%TFS_SourcesDirectory%" == "" goto setLocalInstall
goto skipSetLocalInstall
:setLocalInstall
set TFS_SourcesDirectory=%CD%\..\..
:skipSetLocalInstall
set XSAPIC_INCLUDE_PATH=%TFS_SourcesDirectory%\Include\xsapi-c
set XAL_INCLUDE_PATH=%TFS_SourcesDirectory%\External\xal\Source\Xal\Include\Xal
set HTTPCLIENT_INCLUDE_PATH=%TFS_SourcesDirectory%\External\xal\External\libHttpClient\Include\httpClient

if "%1" == "rev" goto reverse

echo Source:
echo %XSAPIC_INCLUDE_PATH%
echo %XAL_INCLUDE_PATH%
echo %HTTPCLIENT_INCLUDE_PATH%

set DEST_PATH="C:\Program Files (x86)\Microsoft SDKs\Gaming.Xbox.x64.%GDK_VERSION%\v10.0\ExtensionSDKs\Xbox.Services.API.C\10.0\DesignTime\CommonConfiguration\Neutral\Include"
echo Updating:
echo %DEST_PATH%
robocopy /IS /IT /NJS /NJH /MT:16 /S /MIR /NP %HTTPCLIENT_INCLUDE_PATH% %DEST_PATH%\httpClient
robocopy /IS /IT /NJS /NJH /MT:16 /S /MIR /NP %XAL_INCLUDE_PATH% %DEST_PATH%\Xal
robocopy /IS /IT /NJS /NJH /MT:16 /S /MIR /NP %XSAPIC_INCLUDE_PATH% %DEST_PATH%\xsapi-c

set DEST_PATH="C:\Program Files (x86)\Microsoft SDKs\Gaming.Desktop.x64.%GDK_VERSION%\v10.0\ExtensionSDKs\Xbox.Services.API.C\10.0\DesignTime\CommonConfiguration\Neutral\Include"
echo Updating:
echo %DEST_PATH%

robocopy /IS /IT /NJS /NJH /MT:16 /S /MIR /NP %HTTPCLIENT_INCLUDE_PATH% %DEST_PATH%\httpClient
robocopy /IS /IT /NJS /NJH /MT:16 /S /MIR /NP %XAL_INCLUDE_PATH% %DEST_PATH%\Xal
robocopy /IS /IT /NJS /NJH /MT:16 /S /MIR /NP %XSAPIC_INCLUDE_PATH% %DEST_PATH%\xsapi-c
goto done

:reverse
@echo off
set FROM_PATH="C:\Program Files (x86)\Microsoft SDKs\Gaming.Xbox.x64.%GDK_VERSION%\v10.0\ExtensionSDKs\Xbox.Services.API.C\10.0\DesignTime\CommonConfiguration\Neutral\Include"
echo copying from %FROM_PATH%\httpClient
echo copying from %FROM_PATH%\Xal
echo copying from %FROM_PATH%\xsapi-c
echo copying from %FROM_PATH%\..\Xbox.Services.API.C.props 
@echo on
pause
robocopy /IS /IT /NJS /NJH /MT:16 /S /MIR /NP %FROM_PATH%\httpClient %HTTPCLIENT_INCLUDE_PATH%
robocopy /IS /IT /NJS /NJH /MT:16 /S /MIR /NP %FROM_PATH%\Xal %XAL_INCLUDE_PATH%
robocopy /IS /IT /NJS /NJH /MT:16 /S /MIR /NP %FROM_PATH%\xsapi-c %XSAPIC_INCLUDE_PATH%
goto done

:done
type nul>nul
