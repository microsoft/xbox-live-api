call %TFS_SourcesDirectory%\Utilities\VSOBuildScripts\setBuildVersion.cmd

set LOCALXDK=Durango.%XDKVER%

if "%1" == "local" goto testlocal
goto start

:testlocal
set TFS_DropLocation=c:\test
mkdir %TFS_DropLocation%
set TFS_SourcesDirectory=%CD%\..\..\..
goto serializeForPostbuild

:start
if "%XES_SERIALPOSTBUILDREADY%" == "True" goto serializeForPostbuild
goto done

:serializeForPostbuild
echo on
set XDK=lnm
set TARGETFOLDER=%TFS_DropLocation%\ABI
set TARGETWINMD=%TFS_DropLocation%\Release\x64\Microsoft.Xbox.Services.141.XDK.WinRT\Microsoft.Xbox.Services.winmd
set SDK_ROOT_FOLDER=C:\Program Files (x86)\Windows Kits\8.1\bin\x64
set SDK_METADATA_FOLDER="C:\Program Files (x86)\Microsoft SDKs\%LOCALXDK%\v8.0\References\CommonConfiguration\Neutral"
set SDK_INC_FOLDER="C:\Program Files (x86)\Microsoft Durango XDK\xdk\Include\shared"
set SDK_INC_ABI_FOLDER="C:\Program Files (x86)\Microsoft Durango XDK\xdk\Include\winrt"
set OUTPUT_SRC=%TFS_DropLocation%\ABI\ABI\include
set OUTPUT_DEST=%TFS_DropLocation%\Include\winrt
dir /s "\Program Files (x86)\Windows Kits\inspect*.idl"
dir /s "\Program Files (x86)\Windows Kits\midlrt.exe"

SET DPLATFORM=VS2015
SET DTARGETVS=Visual Studio 2015

SET DTARGETVSVER=14.0
FOR /f "tokens=1,2*" %%a in ('reg query "HKLM\Software\Microsoft\VisualStudio\SxS\VS7" /v "%DTARGETVSVER%" /reg:32 2^>NUL') DO SET VSInstallDir=%%c
FOR /f "tokens=1,2*" %%a in ('reg query "HKLM\Software\Microsoft\VisualStudio\SxS\VC7" /v "%DTARGETVSVER%" /reg:32 2^>NUL') DO SET VCInstallDir=%%c
FOR /f "tokens=1,2*" %%a in ('reg query "HKLM\Software\Microsoft\Durango XDK" /v "Latest" /reg:32 2^>NUL') DO SET XDKEDITION=%%c
FOR /f "tokens=1,2*" %%a in ('reg query "HKLM\Software\Microsoft\Durango XDK\%XDKEDITION%" /v "InstallPath" /reg:32 2^>NUL') DO SET XboxOneXDKBuild=%%c
FOR /f "tokens=1,2*" %%a in ('reg query "HKLM\Software\Microsoft\Durango XDK" /v "InstallPath" /reg:32 2^>NUL') DO SET DurangoXDK=%%c
FOR /f "tokens=1,2*" %%a in ('reg query "HKLM\Software\Microsoft\.NETFramework" /v "InstallRoot" /reg:32 2^>NUL') DO SET FrameworkDir=%%c
FOR /f "tokens=1,2*" %%a in ('reg query "HKLM\Software\Microsoft\VisualStudio\SxS\VC7" /v "FrameworkVer32" /reg:32 2^>NUL') DO SET FrameworkVersion=%%c

SET "PATH=%VCInstallDir%VCPackages;%VCInstallDir%BIN;%PATH%"
SET "PATH=%VSInstallDir%Common7\Tools;%VSInstallDir%Common7\IDE;%PATH%"
SET "PATH=%VSInstallDir%Team Tools\Performance Tools\x64;%VSInstallDir%Team Tools\Performance Tools;%PATH%"
SET "PATH=%FrameworkDir%%FrameworkVersion%;%PATH%"
SET "INCLUDE=%VCInstallDir%INCLUDE;%INCLUDE%"
SET "INCLUDE=%XboxOneXDKBuild%xdk\include\shared;%XboxOneXDKBuild%xdk\include\um;%XboxOneXDKBuild%xdk\include\winrt;%INCLUDE%"
SET "INCLUDE=%XboxOneXDKBuild%xdk\ucrt\inc;%XboxOneXDKBuild%xdk\include\cppwinrt;%XboxOneXDKBuild%xdk\VS2015\vc\include;%XboxOneXDKBuild%xdk\VS2015\vc\platform\amd64;%INCLUDE%"

SET Platform=Durango

echo ERRORLEVEL=%ERRORLEVEL%
dir /b "C:\Program Files (x86)\Microsoft SDKs"

echo generating ABI header files ...
rd /s /Q "%TARGETFOLDER%\ABI"
md "%TARGETFOLDER%\ABI"
md "%TARGETFOLDER%\ABI\include"
md "%TARGETFOLDER%\ABI\idl"

md "%TARGETFOLDER%\ABI\temp"
set TEMP=%TARGETFOLDER%\ABI\temp
set TMP=%TARGETFOLDER%\ABI\temp

pushd "%TARGETFOLDER%\ABI"
xcopy /f /y "%TARGETWINMD%"

REM suppress warnings from winmdidl tool, only check exit code
"%SDK_ROOT_FOLDER%\winmdidl.exe" /nosystemdeclares /nologo /outdir:.\idl "%TARGETWINMD%"
echo ERRORLEVEL=%ERRORLEVEL%
if NOT %ERRORLEVEL% EQU 0 goto errorarg

cd idl
for /f %%A IN ('dir /b *.idl') do %TFS_SourcesDirectory%\Utilities\VSOBuildScripts\FindAndReplace.exe %%~fA "\[deprecated.*deprecate, 0x00000000\)\]" ""
cd ..

for /f %%A IN ('dir /b .\idl\*.idl') do "%SDK_ROOT_FOLDER%\midlrt" /Zp8 /char unsigned /enum_class /ns_prefix /no_warn /no_settings_comment /nologo /winrt /metadata_dir %SDK_METADATA_FOLDER% /I %SDK_INC_FOLDER% /I %SDK_INC_FOLDER% /I %SDK_INC_ABI_FOLDER% /I .\idl /h .\include\%%~nA.h .\idl\%%~nA.idl
echo ERRORLEVEL=%ERRORLEVEL%

robocopy /NJS /NJH /MT:16 /S /NP %OUTPUT_SRC% %OUTPUT_DEST%
echo ERRORLEVEL=%ERRORLEVEL%

echo generating ABI header files succeeded ...
goto done

:winmdidlerror
echo generating ABI header files failed ...
goto done

:done
set ERRORLEVEL=0
popd