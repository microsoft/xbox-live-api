if "%1" == "local" goto testlocal
goto start

:testlocal
set BUILD_BINARIESDIRECTORY=%CD%\..\..\Bins
set BUILD_SOURCESDIRECTORY=%CD%\..\..

goto serializeForPostbuild

:start

rem if "%XES_SERIALPOSTBUILDREADY%" == "True" goto serializeForPostbuild
rem goto done
:serializeForPostbuild

echo Running postBuildScriptVPack.cmd
echo on

set

set SDK_DROP_LOCATION=%BUILD_BINARIESDIRECTORY%\SDK
mkdir %SDK_DROP_LOCATION%

if "%BUILD_DEFINITIONNAME%" == "XSAPI_Combined_Rolling_Build" goto finalize

REM ------------------- GDK BEGIN -------------------
set SDK_DROP_LOCATION_GDK=%SDK_DROP_LOCATION%\GDK
set SDK_DROP_LOCATION_GDK_VPACK=%SDK_DROP_LOCATION%\GDK-VPack
mkdir %SDK_DROP_LOCATION_GDK_VPACK%
mkdir %SDK_DROP_LOCATION_GDK_VPACK%\include

robocopy /NJS /NJH /MT:16 /S /NP %SDK_DROP_LOCATION_GDK%\c\include %SDK_DROP_LOCATION_GDK_VPACK%\include
copy %XES_VPACKMANIFESTDIRECTORY%\%XES_VPACKMANIFESTNAME% %SDK_DROP_LOCATION_GDK_VPACK%

REM -------------------DONE-------------------
:finalize

echo.
echo Done postBuildScriptVPack.cmd
echo.
endlocal

:done
exit /b
