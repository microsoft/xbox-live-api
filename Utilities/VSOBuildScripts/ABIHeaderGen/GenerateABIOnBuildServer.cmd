setlocal

if "%XES_SERIALPOSTBUILDREADY%" == "True" goto start
goto done
:start

echo on
set XDK=lnm
set TARGETFOLDER=%TFS_DropLocation%\ABI
set TARGETWINMD=%TFS_DropLocation%\debug\x64\Microsoft.Xbox.Services.140.XDK.WinRT\Microsoft.Xbox.Services.winmd
set LOCALXDK=Durango.160801
set SDK_ROOT_FOLDER="C:\Program Files (x86)\Windows Kits\8.1\bin\x64"
set SDK_METADATA_FOLDER="C:\Program Files (x86)\Microsoft SDKs\%LOCALXDK%\v8.0\References\CommonConfiguration\Neutral"
set SDK_INC_FOLDER="C:\Program Files (x86)\Microsoft Durango XDK\xdk\Include\shared"
set SDK_INC_ABI_FOLDER="C:\Program Files (x86)\Microsoft Durango XDK\xdk\Include\winrt"
set OUTPUT_SRC=%TFS_DropLocation%\ABI\ABI\include
set OUTPUT_DEST=%TFS_DropLocation%\Include\winrt

call %TFS_SourcesDirectory%\Utilities\ABIHeaderGen\DurangoVars.cmd
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
%SDK_ROOT_FOLDER%\winmdidl.exe /nosystemdeclares /nologo /outdir:.\idl "%TARGETWINMD%"
echo ERRORLEVEL=%ERRORLEVEL%
if NOT %ERRORLEVEL% EQU 0 goto errorarg

for /f %%A IN ('dir /b .\idl\*.idl') do call %SDK_ROOT_FOLDER%\midlrt /Zp8 /char unsigned /enum_class /ns_prefix /no_warn /no_settings_comment /nologo /winrt /metadata_dir %SDK_METADATA_FOLDER% /I %SDK_INC_FOLDER% /I %SDK_INC_FOLDER% /I %SDK_INC_ABI_FOLDER% /I .\idl /h .\include\%%~nA.h .\idl\%%~nA.idl
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