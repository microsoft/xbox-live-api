if "%1" == "local" goto testlocal
goto start

:testlocal
set TFS_DropLocation=c:\ba\_temp
mkdir %TFS_DropLocation%
set TFS_VersionNumber=1701.10000
set TFS_SourcesDirectory=%CD%\..\..
set XES_INTERMEDIATESDIRECTORY=%TFS_DropLocation%\temp
mkdir %XES_INTERMEDIATESDIRECTORY%
goto serializeForPostbuild

:start
if "%XES_SERIALPOSTBUILDREADY%" == "True" goto serializeForPostbuild
goto done
:serializeForPostbuild

:start

call %TFS_SourcesDirectory%\Utilities\VSOBuildScripts\setBuildVersion.cmd

rem format release numbers
for /f "tokens=2 delims==" %%G in ('wmic os get localdatetime /value') do set datetime=%%G
set DATETIME_YEAR=%datetime:~0,4%
set DATETIME_MONTH=%datetime:~4,2%
set DATETIME_DAY=%datetime:~6,2%

FOR /F "TOKENS=4 eol=/ DELIMS=. " %%A IN ("%BUILD_BUILDNUMBER%") DO SET SDK_POINT_NAME_VER=%%A
set SDK_POINT_NAME_YEAR=%DATETIME_YEAR%
set SDK_POINT_NAME_MONTH=%DATETIME_MONTH%
set SDK_POINT_NAME_DAY=%DATETIME_DAY%
set SDK_RELEASE_NAME=%SDK_RELEASE_YEAR:~2,2%%SDK_RELEASE_MONTH%
set LONG_SDK_RELEASE_NAME=%SDK_RELEASE_NAME%-%SDK_POINT_NAME_YEAR%%SDK_POINT_NAME_MONTH%%SDK_POINT_NAME_DAY%-%SDK_POINT_NAME_VER%
set NUGET_VERSION_NUMBER=%SDK_RELEASE_YEAR%.%SDK_RELEASE_MONTH%.%SDK_POINT_NAME_YEAR%%SDK_POINT_NAME_MONTH%%SDK_POINT_NAME_DAY%.%SDK_POINT_NAME_VER%
set MINOR_VERSION_NUMBER=%SDK_POINT_NAME_YEAR%%SDK_POINT_NAME_MONTH%%SDK_POINT_NAME_DAY%%SDK_POINT_NAME_VER%
if "%NUGET_EXE%" == "" set NUGET_EXE=%AGENT_TOOLSDIRECTORY%\NuGet\4.6.2\x64\nuget.exe

set XBL_SDK_FOLDER=%TFS_DropLocation%\SDK
set XBL_IOS_FOLDER="\\gameshare\XboxLiveSDK\iOS\Latest"
mkdir %XBL_SDK_FOLDER%
call :GetTarget "\\gameshare\XboxLiveSDK\docs\Latest.lnk" PATH_XBL_DOC_FOLDER_SRC
echo %PATH_XBL_DOC_FOLDER_SRC%
set PATH_XBL_DOC_FOLDER_DEST=%XBL_SDK_FOLDER%\Docs

:makeBinarySDK
for /R %TFS_DropLocation%\NuGetBinaries\ %%X in (*.nupkg) do robocopy %%~dpX %XBL_SDK_FOLDER% %%~nxX 

mkdir %XBL_SDK_FOLDER%\Win32
move %XBL_SDK_FOLDER%\*Win32*.nupkg %XBL_SDK_FOLDER%\Win32

mkdir %XBL_SDK_FOLDER%\UWP
move %XBL_SDK_FOLDER%\*UWP*.nupkg %XBL_SDK_FOLDER%\UWP

mkdir %XBL_SDK_FOLDER%\XDK
move %XBL_SDK_FOLDER%\*XboxOneXDK*.nupkg %XBL_SDK_FOLDER%\XDK

mkdir %XBL_SDK_FOLDER%\Android
move %TFS_DropLocation%\Maven %XBL_SDK_FOLDER%\Android

mkdir %XBL_SDK_FOLDER%\iOS
robocopy /NJS /NJH /MT:16 /S /NP %XBL_IOS_FOLDER% %XBL_SDK_FOLDER%\iOS
echo zipping %XBL_SDK_FOLDER%\iOS to %XBL_SDK_FOLDER%\iOS\XboxLiveSDK-Binary-Latest.zip

rem mkdir %XBL_SDK_FOLDER%\Docs
rem robocopy /NJS /NJH /MT:16 /S /NP %PATH_XBL_DOC_FOLDER_SRC% %XBL_SDK_FOLDER%\Docs

copy %TFS_SourcesDirectory%\ThirdPartyNotices.txt %XBL_SDK_FOLDER%
copy %TFS_SourcesDirectory%\LICENSE.md %XBL_SDK_FOLDER%

echo zipping %XBL_SDK_FOLDER% to %XBL_SDK_FOLDER%\XboxLiveSDK-Binary-%NUGET_VERSION_NUMBER%.zip
set BUILD_TOOLS=%BUILD_STAGINGDIRECTORY%\sdk.buildtools\buildMachine
%BUILD_TOOLS%\vZip.exe /FOLDER:%XBL_SDK_FOLDER% /OUTPUTNAME:%XBL_SDK_FOLDER%\XboxLiveSDK-%NUGET_VERSION_NUMBER%.zip /EXCLUDE:UWP,XDK,Win32
:skipBinarySDK


goto skipSrcSDK
:makeSrcSDK
git clean -x -f -d -x
git submodule foreach --recursive git clean -x -f -d -x
set PATH_XBL_SOURCE_DROP=%XES_INTERMEDIATESDIRECTORY%\source
mkdir %XES_INTERMEDIATESDIRECTORY%
mkdir %PATH_XBL_SOURCE_DROP%
rmdir /s /q %PATH_XBL_SOURCE_DROP%
rmdir /s /q %TFS_SourcesDirectory%\Java\com.microsoft.xboxlive\app\build
rmdir /s /q %TFS_SourcesDirectory%\Java\com.microsoft.xboxlive\.gradle
rmdir /s /q %TFS_SourcesDirectory%\Tests\AndroidTestApp\SocialAndroid\build
rmdir /s /q %TFS_SourcesDirectory%\Tests\AndroidTestApp\SocialAndroid\.gradle
rmdir /s /q %TFS_SourcesDirectory%\Tests\AndroidTestApp\SocialAndroid\app\src\main\jniLibs
rmdir /s /q %TFS_SourcesDirectory%\Tests\AndroidTestApp\SocialAndroid\app\build
rmdir /s /q %TFS_SourcesDirectory%\External\xal\External\libHttpClient\Obj
rmdir /s /q %TFS_SourcesDirectory%\External\xal\Built
rmdir /s /q %TFS_SourcesDirectory%\Tests\CppTestApp\Social\Android\SocialAndroid\app\build\intermediates\
robocopy /NJS /NJH /MT:16 /S /NP %TFS_SourcesDirectory% %PATH_XBL_SOURCE_DROP% /XD .git /XD Packages /XD Binaries /XD .vs /XD Bins /XD corpora /XD NuGetBinaries
rmdir /s /q %PATH_XBL_SOURCE_DROP%\External\cpprestsdk\Release\tests
rmdir /s /q %PATH_XBL_SOURCE_DROP%\External\cpprestsdk\Release\libs 
rmdir /s /q %PATH_XBL_SOURCE_DROP%\External\cpprestsdk\Intermediate 
rmdir /s /q %PATH_XBL_SOURCE_DROP%\External\cpprestsdk\Release\samples
rmdir /s /q %PATH_XBL_SOURCE_DROP%\.git
rmdir /s /q %PATH_XBL_SOURCE_DROP%\External\cpprestsdk\Intermediate
rmdir /s /q %PATH_XBL_SOURCE_DROP%\External\Packages
rmdir /s /q %PATH_XBL_SOURCE_DROP%\External\cpprestsdk\Release\samples
rmdir /s /q %PATH_XBL_SOURCE_DROP%\External\cpprestsdk\Release\tests
rmdir /s /q %PATH_XBL_SOURCE_DROP%\External\cpprestsdk\Release\libs
rmdir /s /q %PATH_XBL_SOURCE_DROP%\Binaries
rmdir /s /q %PATH_XBL_SOURCE_DROP%\Build\Microsoft.Xbox.Services.141.XDK.Cpp\Debug
rmdir /s /q %PATH_XBL_SOURCE_DROP%\Build\Microsoft.Xbox.Services.141.Win32.Cpp\Debug
rmdir /s /q %PATH_XBL_SOURCE_DROP%\Build\Microsoft.Xbox.Services.141.Win32.Cpp\x64
rmdir /s /q %PATH_XBL_SOURCE_DROP%\Java\com.microsoft.xboxlive\x86
rmdir /s /q %PATH_XBL_SOURCE_DROP%\Utilities
rmdir /s /q %PATH_XBL_SOURCE_DROP%\Tests\OldOS
rmdir /s /q %PATH_XBL_SOURCE_DROP%\Maven
rmdir /s /q %PATH_XBL_SOURCE_DROP%\PrecompiledBinaries
rmdir /s /q %PATH_XBL_SOURCE_DROP%\External\Obj
rmdir /s /q %PATH_XBL_SOURCE_DROP%\External\libHttpClient\Samples\Kits\DirectXTK\Src\Shaders\Compiled
rmdir /s /q %PATH_XBL_SOURCE_DROP%\External\Kits\DirectXTK\Bin
rmdir /s /q %PATH_XBL_SOURCE_DROP%\Tests\CTestApp\XDKC_LinkToSource\Debug
rmdir /s /q %PATH_XBL_SOURCE_DROP%\Tests\CTestApp\XDKC_LinkToBins\Debug
rmdir /s /q %PATH_XBL_SOURCE_DROP%\Tests\CTestApp\Social\Xbox\Durango
rmdir /s /q %PATH_XBL_SOURCE_DROP%\Tests\CTestApp\Achievements\Xbox\Durango
del %PATH_XBL_SOURCE_DROP%\Custom.props

rem remove GDK projects
rmdir /s /q %PATH_XBL_SOURCE_DROP%\Build\libHttpClient.141.GDK.C
rmdir /s /q %PATH_XBL_SOURCE_DROP%\Build\Microsoft.Xbox.Services.141.GDK.C
del %PATH_XBL_SOURCE_DROP%\Microsoft.Xbox.Services.GDK.VS2017.sln
move %PATH_XBL_SOURCE_DROP%\Utilities\SetupBuildScripts %PATH_XBL_SOURCE_DROP%\SetupBuildScripts
rmdir /s /q %PATH_XBL_SOURCE_DROP%\Utilities

del /s %PATH_XBL_SOURCE_DROP%\*.log
del %PATH_XBL_SOURCE_DROP%\*.md
copy %TFS_SourcesDirectory%\ThirdPartyNotices.txt %PATH_XBL_SOURCE_DROP%
copy %TFS_SourcesDirectory%\LICENSE.md %PATH_XBL_SOURCE_DROP%
del %PATH_XBL_SOURCE_DROP%\.gitattributes
del %PATH_XBL_SOURCE_DROP%\.gitignore
del %PATH_XBL_SOURCE_DROP%\.gitmodules

echo zipping %PATH_XBL_SOURCE_DROP% to %TFS_DropLocation%\XboxLiveSDK-Src-%NUGET_VERSION_NUMBER%.zip
set BUILD_TOOLS=%BUILD_STAGINGDIRECTORY%\sdk.buildtools\buildMachine
%BUILD_TOOLS%\vZip.exe /FOLDER:%PATH_XBL_SOURCE_DROP% /OUTPUTNAME:%XBL_SDK_FOLDER%\Source\XboxLiveSDK-Src-%NUGET_VERSION_NUMBER%.zip
:skipSrcSDK

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

