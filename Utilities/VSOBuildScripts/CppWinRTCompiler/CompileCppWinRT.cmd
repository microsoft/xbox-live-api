@echo on
echo.

if "%XES_SERIALPOSTBUILDREADY%" == "True" goto skipLocal
goto done

:skipLocal
set REMOTE_COMPILER_FOLDER=\\redmond\osg\Threshold\Tools\CORE\DEP\DART\CppForWinRT\1.0.161012.5
set XDKVER=160801
set WINSDKVER=160801

set SDKSRC=%1
set OUTFOLDER=%2
if "%1" EQU "" set SDKSRC=%TFS_SourcesDirectory%
if "%2" EQU "" set OUTFOLDER=%TFS_DropLocation%\CppWinRT
set REMOTE_COMPILER=%REMOTE_COMPILER_FOLDER%\x64\release\compiler
set LOCAL_COMPILER=%OUTFOLDER%\bin
set LOCAL_COMPILER_1=%LOCAL_COMPILER%\cppwinrt1.exe
set LOCAL_COMPILER_2=%LOCAL_COMPILER%\cppwinrt2.exe

set XDK_RSP=%SDKSRC%\Utilities\VSOBuildScripts\CppWinRTCompiler\ERA.rsp
set XDK_LOCAL_DB=%SDKSRC%\Utilities\VSOBuildScripts\CppWinRTCompiler\db\xdk_metadata.db
set XDK_LOCAL_OUT=%SDKSRC%\Utilities\VSOBuildScripts\CppWinRTCompiler\XDK_Headers
set XDK_LOCAL_OUT_XSAPI=%OUTFOLDER%\XSAPI_XDK_Headers
set XDK_XSAPI_WINRT="C:\Program Files (x86)\Microsoft SDKs\Durango.%XDKVER%\v8.0\ExtensionSDKs\Xbox Services API\8.0\References\CommonConfiguration\neutral\Microsoft.Xbox.Services.winmd"
if EXIST %TFS_DropLocation%\Release\x64\Microsoft.Xbox.Services.110.XDK.WinRT\Microsoft.Xbox.Services.winmd set XDK_XSAPI_WINRT=%TFS_DropLocation%\Release\x64\Microsoft.Xbox.Services.110.XDK.WinRT\Microsoft.Xbox.Services.winmd

set WINSDK_RSP=%SDKSRC%\Utilities\VSOBuildScripts\CppWinRTCompiler\WinSDK.rsp
set WINSDK_LOCAL_DB=%SDKSRC%\Utilities\VSOBuildScripts\CppWinRTCompiler\db\sdk_metadata.db
set WINSDK_LOCAL_OUT=%SDKSRC%\Utilities\VSOBuildScripts\CppWinRTCompiler\WinSDK_Headers
set WINSDK_LOCAL_OUT_XSAPI=%OUTFOLDER%\XSAPI_WinSDK_Headers
set WINSDK_XSAPI_WINRT="C:\Program Files (x86)\Microsoft SDKs\Durango.%XDKVER%\v8.0\ExtensionSDKs\Xbox Services API\8.0\References\CommonConfiguration\neutral\Microsoft.Xbox.Services.winmd"
if EXIST %TFS_DropLocation%\Release\x64\Microsoft.Xbox.Services.140.UWP.WinRT\Microsoft.Xbox.Services.winmd set WINSDK_XSAPI_WINRT=%TFS_DropLocation%\Release\x64\Microsoft.Xbox.Services.140.UWP.WinRT\Microsoft.Xbox.Services.winmd

robocopy /NJS /NJH /MT:16 /S /NP %REMOTE_COMPILER% %LOCAL_COMPILER%
echo ERRORLEVEL %ERRORLEVEL%

dir /b "C:\Program Files (x86)\Microsoft SDKs\Durango*"
%LOCAL_COMPILER_1% -db %XDK_LOCAL_DB% -winrt %XDK_XSAPI_WINRT% @%XDK_RSP%
echo ERRORLEVEL %ERRORLEVEL%
IF NOT ERRORLEVEL 0 GOTO done
dir /s /b %SDKSRC%\Utilities\VSOBuildScripts\CppWinRTCompiler
mkdir %XDK_LOCAL_OUT%
%LOCAL_COMPILER_2% %XDK_LOCAL_DB% -out %XDK_LOCAL_OUT%
echo ERRORLEVEL %ERRORLEVEL%
dir /s /b %SDKSRC%\Utilities\VSOBuildScripts\CppWinRTCompiler
mkdir %XDK_LOCAL_OUT_XSAPI%
robocopy /NJS /NJH /MT:16 /S /NP %XDK_LOCAL_OUT% %XDK_LOCAL_OUT_XSAPI%
echo ERRORLEVEL %ERRORLEVEL%

dir /b "C:\Program Files (x86)\Microsoft SDKs\Durango*"
%LOCAL_COMPILER_1% -db %WINSDK_LOCAL_DB% -winrt %WINSDK_XSAPI_WINRT% @%WINSDK_RSP%
echo ERRORLEVEL %ERRORLEVEL%
IF NOT ERRORLEVEL 0 GOTO done
mkdir %WINSDK_LOCAL_OUT%
%LOCAL_COMPILER_2% %WINSDK_LOCAL_DB% -out %WINSDK_LOCAL_OUT%
echo ERRORLEVEL %ERRORLEVEL%
mkdir %WINSDK_LOCAL_OUT_XSAPI%
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