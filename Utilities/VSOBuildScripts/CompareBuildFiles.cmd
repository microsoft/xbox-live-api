if "%1" EQU "" goto help
if "%1" EQU "" goto help
set ROOT_FOLDER=%1
set NEW_FOLDER=%ROOT_FOLDER%\Utilities\CMake\output
set OLD_FOLDER=%ROOT_FOLDER%\Build

fc %OLD_FOLDER%\Microsoft.Xbox.Services.141.XDK.WinRT\Microsoft.Xbox.Services.141.XDK.WinRT.vcxproj %NEW_FOLDER%\Microsoft.Xbox.Services.141.XDK.WinRT.vcxproj
if %ERRORLEVEL% NEQ 0 goto email

fc %OLD_FOLDER%\Microsoft.Xbox.Services.140.UWP.Cpp\Microsoft.Xbox.Services.140.UWP.Cpp.vcxproj %NEW_FOLDER%\Microsoft.Xbox.Services.140.UWP.Cpp.vcxproj
if %ERRORLEVEL% NEQ 0 goto email

fc %OLD_FOLDER%\Microsoft.Xbox.Services.140.UWP.Cpp\Microsoft.Xbox.Services.140.UWP.Ship.Cpp.vcxproj %NEW_FOLDER%\Microsoft.Xbox.Services.140.UWP.Ship.Cpp.vcxproj
if %ERRORLEVEL% NEQ 0 goto email

fc %OLD_FOLDER%\Microsoft.Xbox.Services.140.UWP.WinRT\Microsoft.Xbox.Services.140.UWP.WinRT.vcxproj %NEW_FOLDER%\Microsoft.Xbox.Services.140.UWP.WinRT.vcxproj
if %ERRORLEVEL% NEQ 0 goto email

fc %OLD_FOLDER%\Microsoft.Xbox.Services.140.XDK.Cpp\Microsoft.Xbox.Services.140.XDK.Cpp.vcxproj %NEW_FOLDER%\Microsoft.Xbox.Services.140.XDK.Cpp.vcxproj
if %ERRORLEVEL% NEQ 0 goto email

fc %OLD_FOLDER%\Microsoft.Xbox.Services.140.XDK.Cpp\Microsoft.Xbox.Services.140.XDK.Ship.Cpp.vcxproj %NEW_FOLDER%\Microsoft.Xbox.Services.140.XDK.Ship.Cpp.vcxproj
if %ERRORLEVEL% NEQ 0 goto email

fc %OLD_FOLDER%\Microsoft.Xbox.Services.141.UWP.Cpp\Microsoft.Xbox.Services.141.UWP.Cpp.vcxproj %NEW_FOLDER%\Microsoft.Xbox.Services.141.UWP.Cpp.vcxproj
if %ERRORLEVEL% NEQ 0 goto email

fc %OLD_FOLDER%\Microsoft.Xbox.Services.141.UWP.Cpp\Microsoft.Xbox.Services.141.UWP.Ship.Cpp.vcxproj %NEW_FOLDER%\Microsoft.Xbox.Services.141.UWP.Ship.Cpp.vcxproj
if %ERRORLEVEL% NEQ 0 goto email

fc %OLD_FOLDER%\Microsoft.Xbox.Services.141.XDK.Cpp\Microsoft.Xbox.Services.141.XDK.Cpp.vcxproj %NEW_FOLDER%\Microsoft.Xbox.Services.141.XDK.Cpp.vcxproj
if %ERRORLEVEL% NEQ 0 goto email

fc %OLD_FOLDER%\Microsoft.Xbox.Services.141.XDK.Cpp\Microsoft.Xbox.Services.141.XDK.Ship.Cpp.vcxproj %NEW_FOLDER%\Microsoft.Xbox.Services.141.XDK.Ship.Cpp.vcxproj
if %ERRORLEVEL% NEQ 0 goto email

fc %OLD_FOLDER%\Microsoft.Xbox.Services.UnitTest.140.TAEF\Microsoft.Xbox.Services.UnitTest.140.TAEF.vcxproj %NEW_FOLDER%\Microsoft.Xbox.Services.UnitTest.140.TAEF.vcxproj
if %ERRORLEVEL% NEQ 0 goto email

fc %OLD_FOLDER%\Microsoft.Xbox.Services.UnitTest.140.TE\Microsoft.Xbox.Services.UnitTest.140.TE.vcxproj %NEW_FOLDER%\Microsoft.Xbox.Services.UnitTest.140.TE.vcxproj
if %ERRORLEVEL% NEQ 0 goto email

goto done

:email
if "%2" NEQ "emailfailures" goto done
set MSGTITLE="Mismatch detected. Please run MakeProjects.cmd to regen build projects. %BUILD_SOURCEVERSIONAUTHOR% %BUILD_DEFINITIONNAME% %BUILD_SOURCEBRANCH%"
set MSGBODY="%TFS_DROPLOCATION%    https://microsoft.visualstudio.com/OS/_build/index?buildId=%BUILD_BUILDID%&_a=summary"
call \\scratch2\scratch\jasonsa\tools\send-build-email.cmd %MSGTITLE% %MSGBODY% 

goto done
:help
@echo off
echo.
echo CompareBuildFiles.cmd rootFolder [emailfailures]
echo.
echo Example:
echo CompareBuildFiles.cmd C:\git\forks\xbox-live-api
echo.

:done
