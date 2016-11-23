if "%XES_SERIALPOSTBUILDREADY%" == "True" goto serializeForPostbuild
goto done
:serializeForPostbuild

echo Running skipBuild.cmd
echo on

if "%1" EQU "" goto help

set SRC_PATH=%1
set DEST_PATH=%TFS_DropLocation%

rmdir /s /q %DEST_PATH%\Debug
rmdir /s /q %DEST_PATH%\Release
robocopy /NJS /NJH /MT:16 /S /NP %SRC_PATH%\Debug %DEST_PATH%\Debug
robocopy /NJS /NJH /MT:16 /S /NP %SRC_PATH%\Release %DEST_PATH%\Release
goto finalize

:help
echo.
echo skipBuild pathToSourceFolder
echo.

:finalize
echo.
echo Done skipBuild.cmd
echo.

:done
