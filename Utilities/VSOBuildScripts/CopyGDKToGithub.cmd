@echo off

set startingDir="%cd%"

if "%1"=="/?" goto usage
if "%1"=="help" goto usage
if "%1"=="" goto usage
if "%2"=="" goto usage

set adoPath=%1\
set githubPath=%2\

echo adoPath=%adoPath%
echo githubPath=%githubPath%

:initgithub
if "%3"=="/init" (
mkdir %githubPath%
cd /d %githubPath%
git init

:: Add libHttpClient submodule
mkdir External\Xal\External
cd /d External\Xal\External
git submodule add https://github.com/microsoft/libHttpClient

:: Add rapidjson submodule
cd /d %githubPath%External
git submodule add https://github.com/jasonsandlin/rapidjson

cd /d %githubPath%
git submodule init
)

:getsubmodules
echo Extracting ADO submodule hashes

cd /d %adoPath%
git submodule update --recursive

git submodule status External\rapidjson > rapidjsonHash.tmp
for /f "tokens=1" %%a in (rapidjsonHash.tmp) do set rapidjsonHash=%%a
del rapidjsonHash.tmp

if "%rapidjsonHash%"=="" (
echo Unable to extract rapidjson commit hash from %adoPath%External\rapidjson
goto end
)

cd /d External\Xal\
git submodule status External\libHttpClient > lhcHash.tmp
for /f "tokens=1" %%a in (lhcHash.tmp) do set lhcHash=%%a
del lhcHash.tmp

if "%lhcHash%"=="" (
echo "Unable to extract libHttpClient commit hash from %adoPath%\External\Xal\External\libHttpClient"
goto end
)

:cleangithub
echo Cleaning old files from github repository

cd /d %githubPath%
:: Keep the .git folder and .gitmodules which is distinct for public github and maintained seperately. See :submoduleupdate tag below.
for /f %%a in ('dir /b') do (if not "%%a"==".gitmodules" and not "%%a"==".git" rmdir "%%a" /s /q || del "%%a" /s /q)

:copysource
echo Copy files from ADO repository to github repository

robocopy %adoPath% %githubPath% /xf ".gitmodules"
del "%githubPath%Microsoft.Xbox.Services.Android.*"
del "%githubPath%Microsoft.Xbox.Services.Win32.*"
del "%githubPath%Microsoft.Xbox.Services.XDK.*"
del "%githubPath%Microsoft.Xbox.Services.GDK.Bin.*"
del "%githubPath%tcui.*.props"

:: Copy build directory and selectively delete non-GDK and UnitTests projects
robocopy "%adoPath%Build" "%githubPath%Build" 
del "%githubPath%Build\tcui.*"
del "%githubPath%Build\xsapi.android.props"
robocopy "%adoPath%Build\Microsoft.Xbox.Services.141.GDK.C" "%githubPath%Build\Microsoft.Xbox.Services.141.GDK.C" /s
robocopy "%adoPath%Build\Microsoft.Xbox.Services.141.GDK.C.Thunks" "%githubPath%Build\Microsoft.Xbox.Services.141.GDK.C.Thunks" /s
robocopy "%adoPath%Build\Microsoft.Xbox.Services.141.GDK.Cpp" "%githubPath%Build\Microsoft.Xbox.Services.141.GDK.Cpp" /s
robocopy "%adoPath%Build\Microsoft.Xbox.Services.142.GDK.C" "%githubPath%Build\Microsoft.Xbox.Services.142.GDK.C" /s
robocopy "%adoPath%Build\Microsoft.Xbox.Services.142.GDK.Cpp" "%githubPath%Build\Microsoft.Xbox.Services.142.GDK.Cpp" /s
robocopy "%adoPath%Build\Microsoft.Xbox.Services.UnitTest.141.TE" "%githubPath%Build\Microsoft.Xbox.Services.UnitTest.141.TE" /s
robocopy "%adoPath%Build\Microsoft.Xbox.Services.UnitTest.142.TE" "%githubPath%Build\Microsoft.Xbox.Services.UnitTest.142.TE" /s
robocopy "%adoPath%Build\Microsoft.Xbox.Services.UnitTest.141.TAEF" "%githubPath%Build\Microsoft.Xbox.Services.UnitTest.141.TAEF" /s
robocopy "%adoPath%Build\Microsoft.Xbox.Services.UnitTest.142.TAEF" "%githubPath%Build\Microsoft.Xbox.Services.UnitTest.142.TAEF" /s

:: Copy include folder
robocopy "%adoPath%Include" "%githubPath%Include" /s

:: Copy source folder
robocopy "%adoPath%Source" "%githubPath%Source" /s

:: Copy APIRunner GDK and UnitTest source files selectively
robocopy "%adoPath%Tests" "%githubPath%Tests"
robocopy "%adoPath%Tests\ApiExplorer" "%githubPath%Tests\ApiExplorer" /s
rmdir "%githubPath%Tests\ApiExplorer\iOS" /s /q
rmdir "%githubPath%Tests\ApiExplorer\UWP" /s /q
rmdir "%githubPath%Tests\ApiExplorer\Win" /s /q
rmdir "%githubPath%Tests\ApiExplorer\Win32" /s /q
rmdir "%githubPath%Tests\ApiExplorer\XDK" /s /q
robocopy "%adoPath%Tests\GDK" "%githubPath%Tests\GDK" /s
robocopy "%adoPath%Tests\UnitTests" "%githubPath%Tests\UnitTests" /s

:: Copy Utilities
robocopy "%adoPath%Utilities" "%githubPath%Utilities" /s
rmdir "%githubPath%Utilities\TCUIBackup" /s /q
rmdir "%githubPath%Utilities\SetupBuildScripts" /s /q

:: Setup Xal headers
robocopy "%adoPath%External\Xal\Source\Xal\Include" "%githubPath%External\Xal\Source\Xal\Include" /s

:: Create Xal readme
echo This repository only contains the public headers for the Xbox Authentication Library (XAL), the same headers shipped as part of the GDK. Full source for XAL is not publicly available at this time. > "%githubPath%External\Xal\README"

:submoduleupdate

echo Update github submodules to point to extracted ADO hashes

cd /d %githubPath%
git submodule sync
git submodule update --checkout --force --recursive

cd /d %githubPath%External\Xal\External\libHttpClient
git checkout %lhcHash%

cd /d %githubPath%External\rapidjson
git checkout %rapidjsonHash%

cd /d %startingDir%
goto end

:usage
echo Usage: "%0 <ADO repository path> <github repository path> [/init]"
echo 	"/init" option will create and initialize a new git repository in "<github repository path>".
echo 	Otherwise, the github repository should be a clone of https://github.com/microsoft/xbox-live-api

:end
cd /d %startingDir%
