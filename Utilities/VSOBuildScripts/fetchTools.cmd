echo Running fetchTools.cmd

set patArg=%1
set sdkArg=%2
set externalBranchArg=%3
set buildToolsBranchArg=%4
cd /D %BUILD_STAGINGDIRECTORY%
call git clone https://anything:%patArg%@microsoft.visualstudio.com/DefaultCollection/Xbox.Services/_git/sdk.buildtools
cd sdk.buildtools
if "%buildToolsBranchArg%" NEQ "" call git checkout %buildToolsBranchArg%
cd /D %BUILD_STAGINGDIRECTORY%
dir "%BUILD_STAGINGDIRECTORY%\sdk.buildtools\buildMachine

if "%sdkArg%" == "EXT" goto setupExt
goto skipExt
:setupExt
cd /D %BUILD_STAGINGDIRECTORY%
call git clone https://anything:%patArg%@microsoft.visualstudio.com/Xbox.Services/_git/sdk.external
cd sdk.external
if "%externalBranchArg%" NEQ "" call git checkout %externalBranchArg%
cd /D %BUILD_STAGINGDIRECTORY%
dir "%BUILD_STAGINGDIRECTORY%\sdk.external\ExtractedGDK\"
:skipExt

