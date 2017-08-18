@echo off
if "%1" EQU "" goto help
echo on
set ROOT_FOLDER=%1
set NEW_FOLDER=%ROOT_FOLDER%\Utilities\CMake\output
set OLD_FOLDER=%ROOT_FOLDER%\Build

call %ROOT_FOLDER%\Utilities\CMake\scripts\RunCMake.cmd -DSHIP=TRUE

%ROOT_FOLDER%\Utilities\CMake\ProjectFileProcessor\bin\Debug\ProjectFileProcessor.exe %ROOT_FOLDER%

copy %NEW_FOLDER%\Microsoft.Xbox.Services.140.UWP.Ship.Cpp.vcxproj* %OLD_FOLDER%\Microsoft.Xbox.Services.140.UWP.Cpp
copy %NEW_FOLDER%\Microsoft.Xbox.Services.140.XDK.Ship.Cpp.vcxproj* %OLD_FOLDER%\Microsoft.Xbox.Services.140.XDK.Cpp
copy %NEW_FOLDER%\Microsoft.Xbox.Services.141.UWP.Ship.Cpp.vcxproj* %OLD_FOLDER%\Microsoft.Xbox.Services.141.UWP.Cpp
copy %NEW_FOLDER%\Microsoft.Xbox.Services.141.XDK.Ship.Cpp.vcxproj* %OLD_FOLDER%\Microsoft.Xbox.Services.141.XDK.Cpp

goto done
:help
echo.
echo MakeShipProjects.cmd rootFolder
echo.
echo Example:
echo MakeShipProjects.cmd C:\git\forks\xbox-live-api
echo.

:done