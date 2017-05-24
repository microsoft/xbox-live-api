if "%1" EQU "" goto help
set ROOT_FOLDER=%1
call %ROOT_FOLDER%\Utilities\CMake\scripts\RunCMake.cmd 
REM call %ROOT_FOLDER%\Utilities\CMake\scripts\RunCMake.cmd -DWINRT=TRUE
REM call %ROOT_FOLDER%\Utilities\CMake\scripts\RunCMake.cmd -DXDK=TRUE
REM call %ROOT_FOLDER%\Utilities\CMake\scripts\RunCMake.cmd -DXDK=TRUE -DWINRT=TRUE
REM call %ROOT_FOLDER%\Utilities\CMake\scripts\RunCMake.cmd -DSHIP=TRUE
REM call %ROOT_FOLDER%\Utilities\CMake\scripts\RunCMake.cmd -DUNITTEST=TRUE -DTAEF=TRUE
REM call %ROOT_FOLDER%\Utilities\CMake\scripts\RunCMake.cmd -DUNITTEST=TRUE -DTE=TRUE
%ROOT_FOLDER%\Utilities\CMake\ProjectFileProcessor\bin\Debug\ProjectFileProcessor.exe %ROOT_FOLDER%
call %ROOT_FOLDER%\Utilities\CMake\scripts\DiffProjects.cmd %ROOT_FOLDER%
goto done



goto done
:help
@echo off
echo.
echo GenerateAndDiff.cmd rootFolder [skipCopy]
echo.
echo Example:
echo GenerateAndDiff.cmd C:\git\forks\xbox-live-api
echo.

:done