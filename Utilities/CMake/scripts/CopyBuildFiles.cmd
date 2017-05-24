set ROOT_FOLDER=%1
if "%1" EQU "" goto help
set NEW_FOLDER=%ROOT_FOLDER%\Utilities\CMake\output
set OLD_FOLDER=%ROOT_FOLDER%\Build
 
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
@echo off
echo.
echo CopyBuildFiles.cmd rootFolder [skipCopy]
echo.
echo Example:
echo CopyBuildFiles.cmd C:\git\forks\xbox-live-api
echo.

:done