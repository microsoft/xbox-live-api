@echo off
if "%1" EQU "" goto help
echo on
set ROOT_FOLDER=%1

call %ROOT_FOLDER%\Utilities\CMake\scripts\RunCMake.cmd 
call %ROOT_FOLDER%\Utilities\CMake\scripts\RunCMake.cmd -DWINRT=TRUE
call %ROOT_FOLDER%\Utilities\CMake\scripts\RunCMake.cmd -DXDK=TRUE
call %ROOT_FOLDER%\Utilities\CMake\scripts\RunCMake.cmd -DXDK=TRUE -DWINRT=TRUE
call %ROOT_FOLDER%\Utilities\CMake\scripts\RunCMake.cmd -DSHIP=TRUE
call %ROOT_FOLDER%\Utilities\CMake\scripts\RunCMake.cmd -DUNITTEST=TRUE -DTAEF=TRUE
call %ROOT_FOLDER%\Utilities\CMake\scripts\RunCMake.cmd -DUNITTEST=TRUE -DTE=TRUE

ProjectFileProcessor\bin\Debug\ProjectFileProcessor.exe %ROOT_FOLDER%

set NEW_FOLDER=%ROOT_FOLDER%\Utilities\CMake\output
set OLD_FOLDER=%ROOT_FOLDER%\Build

if "%2" EQU "skipCopy" goto done
copy %NEW_FOLDER%\Microsoft.Xbox.Services.110.XDK.Cpp.vcxproj* %OLD_FOLDER%\Microsoft.Xbox.Services.110.XDK.Cpp
copy %NEW_FOLDER%\Microsoft.Xbox.Services.110.XDK.Ship.Cpp.vcxproj* %OLD_FOLDER%\Microsoft.Xbox.Services.110.XDK.Cpp
copy %NEW_FOLDER%\Microsoft.Xbox.Services.110.XDK.WinRT.vcxproj* %OLD_FOLDER%\Microsoft.Xbox.Services.110.XDK.WinRT
copy %NEW_FOLDER%\Microsoft.Xbox.Services.140.UWP.Cpp.vcxproj* %OLD_FOLDER%\Microsoft.Xbox.Services.140.UWP.Cpp
copy %NEW_FOLDER%\Microsoft.Xbox.Services.140.UWP.Ship.Cpp.vcxproj* %OLD_FOLDER%\Microsoft.Xbox.Services.140.UWP.Cpp
copy %NEW_FOLDER%\Microsoft.Xbox.Services.140.UWP.WinRT.vcxproj* %OLD_FOLDER%\Microsoft.Xbox.Services.140.UWP.WinRT
copy %NEW_FOLDER%\Microsoft.Xbox.Services.140.XDK.Cpp.vcxproj* %OLD_FOLDER%\Microsoft.Xbox.Services.140.XDK.Cpp
copy %NEW_FOLDER%\Microsoft.Xbox.Services.140.XDK.Ship.Cpp.vcxproj* %OLD_FOLDER%\Microsoft.Xbox.Services.140.XDK.Cpp
copy %NEW_FOLDER%\Microsoft.Xbox.Services.141.UWP.Cpp.vcxproj* %OLD_FOLDER%\Microsoft.Xbox.Services.141.UWP.Cpp
copy %NEW_FOLDER%\Microsoft.Xbox.Services.141.UWP.Ship.Cpp.vcxproj* %OLD_FOLDER%\Microsoft.Xbox.Services.141.UWP.Cpp
copy %NEW_FOLDER%\Microsoft.Xbox.Services.141.XDK.Cpp.vcxproj* %OLD_FOLDER%\Microsoft.Xbox.Services.141.XDK.Cpp
copy %NEW_FOLDER%\Microsoft.Xbox.Services.141.XDK.Ship.Cpp.vcxproj* %OLD_FOLDER%\Microsoft.Xbox.Services.141.XDK.Cpp
copy %NEW_FOLDER%\Microsoft.Xbox.Services.UnitTest.140.TAEF.vcxproj* %OLD_FOLDER%\Microsoft.Xbox.Services.UnitTest.140.TAEF
copy %NEW_FOLDER%\Microsoft.Xbox.Services.UnitTest.140.TE.vcxproj* %OLD_FOLDER%\Microsoft.Xbox.Services.UnitTest.140.TE

goto done
:help
echo.
echo MakeProjects.cmd rootFolder [skipCopy]
echo.
echo Example:
echo MakeProjects.cmd C:\git\forks\xbox-live-api
echo.

:done