@echo on
echo.

call %TFS_SourcesDirectory%\Utilities\VSOBuildScripts\setBuildVersion.cmd
set SDKSRC=%1

if "%1" == "local" goto testlocal
goto start
:testlocal
set TFS_DropLocation=c:\test
mkdir %TFS_DropLocation%
set TFS_SourcesDirectory=%CD%\..\..\..
set SDKSRC=%CD%\..\..\..
goto serializeForPostbuild

:start
if "%XES_SERIALPOSTBUILDREADY%" == "True" goto serializeForPostbuild
goto done

:serializeForPostbuild
set REMOTE_COMPILER_FOLDER=\\scratch2\scratch\jasonsa\tools\cppwinrt
set OUTFOLDER=%2

if "%1" EQU "" set SDKSRC=%TFS_SourcesDirectory%
if "%2" EQU "" set OUTFOLDER=%TFS_DropLocation%\CppWinRT
set REMOTE_COMPILER=%REMOTE_COMPILER_FOLDER%
set LOCAL_COMPILER=%OUTFOLDER%\bin
set LOCAL_COMPILER_1=%LOCAL_COMPILER%\cppwinrt1.exe
set LOCAL_COMPILER_2=%LOCAL_COMPILER%\cppwinrt2.exe

set XDK_RSP=%SDKSRC%\Utilities\VSOBuildScripts\CppWinRTCompiler\ERA.rsp
set XDK_LOCAL_DB_BASE=%SDKSRC%\Utilities\VSOBuildScripts\CppWinRTCompiler\db\xdk_metadata_base.db
set XDK_LOCAL_DB=%SDKSRC%\Utilities\VSOBuildScripts\CppWinRTCompiler\db\xdk_metadata.db
set XDK_LOCAL_OUT_BASE=%SDKSRC%\Utilities\VSOBuildScripts\CppWinRTCompiler\XDK_Headers_Base
set XDK_LOCAL_OUT=%SDKSRC%\Utilities\VSOBuildScripts\CppWinRTCompiler\XDK_Headers
set XDK_LOCAL_OUT_XSAPI=%OUTFOLDER%\XSAPI_XDK_Headers
set XDK_XSAPI_WINRT="C:\Program Files (x86)\Microsoft SDKs\Durango.%XDKVER%\v8.0\ExtensionSDKs\Xbox Services API\8.0\References\CommonConfiguration\neutral\Microsoft.Xbox.Services.winmd"
if EXIST %TFS_DropLocation%\Release\x64\Microsoft.Xbox.Services.141.XDK.WinRT\Microsoft.Xbox.Services.winmd set XDK_XSAPI_WINRT=%TFS_DropLocation%\Release\x64\Microsoft.Xbox.Services.141.XDK.WinRT\Microsoft.Xbox.Services.winmd
echo %XDK_XSAPI_WINRT%

set WINSDK_RSP=%SDKSRC%\Utilities\VSOBuildScripts\CppWinRTCompiler\WinSDK.rsp
set WINSDK_LOCAL_DB_BASE=%SDKSRC%\Utilities\VSOBuildScripts\CppWinRTCompiler\db\sdk_metadata_base.db
set WINSDK_LOCAL_DB=%SDKSRC%\Utilities\VSOBuildScripts\CppWinRTCompiler\db\sdk_metadata.db
set WINSDK_LOCAL_OUT_BASE=%SDKSRC%\Utilities\VSOBuildScripts\CppWinRTCompiler\WinSDK_Headers_Base
set WINSDK_LOCAL_OUT=%SDKSRC%\Utilities\VSOBuildScripts\CppWinRTCompiler\WinSDK_Headers
set WINSDK_LOCAL_OUT_XSAPI=%OUTFOLDER%\XSAPI_WinSDK_Headers
set WINSDK_XSAPI_WINRT=
if EXIST %TFS_DropLocation%\Release\x64\Microsoft.Xbox.Services.140.UWP.WinRT\Microsoft.Xbox.Services.winmd set WINSDK_XSAPI_WINRT=%TFS_DropLocation%\Release\x64\Microsoft.Xbox.Services.140.UWP.WinRT\Microsoft.Xbox.Services.winmd

robocopy /NJS /NJH /MT:16 /S /NP %REMOTE_COMPILER% %LOCAL_COMPILER%
echo ERRORLEVEL %ERRORLEVEL%

dir /b "C:\Program Files (x86)\Microsoft SDKs\Durango*"
%LOCAL_COMPILER_1% -db %XDK_LOCAL_DB_BASE% @%XDK_RSP%
echo ERRORLEVEL %ERRORLEVEL%
IF NOT ERRORLEVEL 0 GOTO done
%LOCAL_COMPILER_1% -db %XDK_LOCAL_DB% -winmd %XDK_XSAPI_WINRT% @%XDK_RSP%
echo ERRORLEVEL %ERRORLEVEL%
IF NOT ERRORLEVEL 0 GOTO done
dir /s /b %SDKSRC%\Utilities\VSOBuildScripts\CppWinRTCompiler
mkdir %XDK_LOCAL_OUT_BASE%
mkdir %XDK_LOCAL_OUT%
%LOCAL_COMPILER_2% %XDK_LOCAL_DB_BASE% -out %XDK_LOCAL_OUT_BASE%
echo ERRORLEVEL %ERRORLEVEL%
%LOCAL_COMPILER_2% %XDK_LOCAL_DB% -out %XDK_LOCAL_OUT%
echo ERRORLEVEL %ERRORLEVEL%
dir /s /b %SDKSRC%\Utilities\VSOBuildScripts\CppWinRTCompiler
mkdir %XDK_LOCAL_OUT_XSAPI%
cd %XDK_LOCAL_OUT%\winrt
for %%F in (*.*) do if exist "%XDK_LOCAL_OUT_BASE%\winrt\%%~nxF" del "%%F"
cd %XDK_LOCAL_OUT%\winrt\internal
for %%F in (*.*) do if exist "%XDK_LOCAL_OUT_BASE%\winrt\internal\%%~nxF" del "%%F"
robocopy /NJS /NJH /MT:16 /S /NP %XDK_LOCAL_OUT% %XDK_LOCAL_OUT_XSAPI%
echo ERRORLEVEL %ERRORLEVEL%

dir /b "C:\Program Files (x86)\Microsoft SDKs\Durango*"
%LOCAL_COMPILER_1% -db %WINSDK_LOCAL_DB_BASE% @%WINSDK_RSP%
echo ERRORLEVEL %ERRORLEVEL%
IF NOT ERRORLEVEL 0 GOTO done
%LOCAL_COMPILER_1% -db %WINSDK_LOCAL_DB% -winmd %WINSDK_XSAPI_WINRT% @%WINSDK_RSP%
echo ERRORLEVEL %ERRORLEVEL%
IF NOT ERRORLEVEL 0 GOTO done
mkdir %WINSDK_LOCAL_OUT_BASE%
mkdir %WINSDK_LOCAL_OUT%
%LOCAL_COMPILER_2% %WINSDK_LOCAL_DB_BASE% -out %WINSDK_LOCAL_OUT_BASE%
echo ERRORLEVEL %ERRORLEVEL%
%LOCAL_COMPILER_2% %WINSDK_LOCAL_DB% -out %WINSDK_LOCAL_OUT%
echo ERRORLEVEL %ERRORLEVEL%
mkdir %WINSDK_LOCAL_OUT_XSAPI%
cd %WINSDK_LOCAL_OUT%\winrt
for %%F in (*.*) do if exist "%WINSDK_LOCAL_OUT_BASE%\winrt\%%~nxF" del "%%F"
cd %WINSDK_LOCAL_OUT%\winrt\internal
for %%F in (*.*) do if exist "%WINSDK_LOCAL_OUT_BASE%\winrt\internal\%%~nxF" del "%%F"
robocopy /NJS /NJH /MT:16 /S /NP %WINSDK_LOCAL_OUT% %WINSDK_LOCAL_OUT_XSAPI%
echo ERRORLEVEL %ERRORLEVEL%
set ERRORLEVEL=0

goto done

:help
echo.
echo Usage:
echo call CompileCppWinRT sourceDir outputFolder
echo.
echo Example:
echo CompileCppWinRT.cmd c:\git\XSAPI c:\temp
goto done

:done