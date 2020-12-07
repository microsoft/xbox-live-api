if "%1" EQU "" goto help
if "%1" EQU "" goto help
set ROOT_FOLDER=%1
set NEW_FOLDER=%ROOT_FOLDER%\Utilities\CMake\output
set OLD_FOLDER=%ROOT_FOLDER%\Build

fc %OLD_FOLDER%\Microsoft.Xbox.Services.140.Android\Microsoft.Xbox.Services.140.Android.vcxproj %NEW_FOLDER%\Microsoft.Xbox.Services.140.Android.vcxproj
if %ERRORLEVEL% NEQ 0 goto email
fc %OLD_FOLDER%\Microsoft.Xbox.Services.140.UWP.Cpp\Microsoft.Xbox.Services.140.UWP.Cpp.vcxproj %NEW_FOLDER%\Microsoft.Xbox.Services.140.UWP.Cpp.vcxproj
if %ERRORLEVEL% NEQ 0 goto email
fc %OLD_FOLDER%\Microsoft.Xbox.Services.140.Win32.Cpp\Microsoft.Xbox.Services.140.Win32.Cpp.vcxproj %NEW_FOLDER%\Microsoft.Xbox.Services.140.Win32.Cpp.vcxproj
if %ERRORLEVEL% NEQ 0 goto email
fc %OLD_FOLDER%\Microsoft.Xbox.Services.140.XDK.Cpp\Microsoft.Xbox.Services.140.XDK.Cpp.vcxproj %NEW_FOLDER%\Microsoft.Xbox.Services.140.XDK.Cpp.vcxproj
if %ERRORLEVEL% NEQ 0 goto email
fc %OLD_FOLDER%\Microsoft.Xbox.Services.141.Android\Microsoft.Xbox.Services.141.Android.vcxproj %NEW_FOLDER%\Microsoft.Xbox.Services.141.Android.vcxproj
if %ERRORLEVEL% NEQ 0 goto email
fc %OLD_FOLDER%\Microsoft.Xbox.Services.141.GDK.C\Microsoft.Xbox.Services.141.GDK.C.vcxproj %NEW_FOLDER%\Microsoft.Xbox.Services.141.GDK.C.vcxproj
if %ERRORLEVEL% NEQ 0 goto email
fc %OLD_FOLDER%\Microsoft.Xbox.Services.141.GDK.Cpp\Microsoft.Xbox.Services.141.GDK.Cpp.vcxproj %NEW_FOLDER%\Microsoft.Xbox.Services.141.GDK.Cpp.vcxproj
if %ERRORLEVEL% NEQ 0 goto email
fc %OLD_FOLDER%\Microsoft.Xbox.Services.141.UWP.Cpp\Microsoft.Xbox.Services.141.UWP.Cpp.vcxproj %NEW_FOLDER%\Microsoft.Xbox.Services.141.UWP.Cpp.vcxproj
if %ERRORLEVEL% NEQ 0 goto email
fc %OLD_FOLDER%\Microsoft.Xbox.Services.141.Win32.Cpp\Microsoft.Xbox.Services.141.Win32.Cpp.vcxproj %NEW_FOLDER%\Microsoft.Xbox.Services.141.Win32.Cpp.vcxproj
if %ERRORLEVEL% NEQ 0 goto email
fc %OLD_FOLDER%\Microsoft.Xbox.Services.141.XDK.Cpp\Microsoft.Xbox.Services.141.XDK.Cpp.vcxproj %NEW_FOLDER%\Microsoft.Xbox.Services.141.XDK.Cpp.vcxproj
if %ERRORLEVEL% NEQ 0 goto email
fc %OLD_FOLDER%\Microsoft.Xbox.Services.142.GDK.C\Microsoft.Xbox.Services.142.GDK.C.vcxproj %NEW_FOLDER%\Microsoft.Xbox.Services.142.GDK.C.vcxproj
if %ERRORLEVEL% NEQ 0 goto email
fc %OLD_FOLDER%\Microsoft.Xbox.Services.142.GDK.Cpp\Microsoft.Xbox.Services.142.GDK.Cpp.vcxproj %NEW_FOLDER%\Microsoft.Xbox.Services.142.GDK.Cpp.vcxproj
if %ERRORLEVEL% NEQ 0 goto email
fc %OLD_FOLDER%\Microsoft.Xbox.Services.142.UWP.Cpp\Microsoft.Xbox.Services.142.UWP.Cpp.vcxproj %NEW_FOLDER%\Microsoft.Xbox.Services.142.UWP.Cpp.vcxproj
if %ERRORLEVEL% NEQ 0 goto email
fc %OLD_FOLDER%\Microsoft.Xbox.Services.142.Win32.Cpp\Microsoft.Xbox.Services.142.Win32.Cpp.vcxproj %NEW_FOLDER%\Microsoft.Xbox.Services.142.Win32.Cpp.vcxproj
if %ERRORLEVEL% NEQ 0 goto email
fc %OLD_FOLDER%\Microsoft.Xbox.Services.142.XDK.Cpp\Microsoft.Xbox.Services.142.XDK.Cpp.vcxproj %NEW_FOLDER%\Microsoft.Xbox.Services.142.XDK.Cpp.vcxproj
if %ERRORLEVEL% NEQ 0 goto email
fc %OLD_FOLDER%\Microsoft.Xbox.Services.UnitTest.141.TAEF\Microsoft.Xbox.Services.UnitTest.141.TAEF.vcxproj %NEW_FOLDER%\Microsoft.Xbox.Services.UnitTest.141.TAEF.vcxproj
if %ERRORLEVEL% NEQ 0 goto email
fc %OLD_FOLDER%\Microsoft.Xbox.Services.UnitTest.141.TE\Microsoft.Xbox.Services.UnitTest.141.TE.vcxproj %NEW_FOLDER%\Microsoft.Xbox.Services.UnitTest.141.TE.vcxproj
if %ERRORLEVEL% NEQ 0 goto email

goto done

:email
if "%2" NEQ "emailfailures" goto done
set MSGTITLE="Mismatch detected. Please run MakeProjects.cmd to regen build projects. %BUILD_SOURCEVERSIONAUTHOR% %BUILD_DEFINITIONNAME% %BUILD_SOURCEBRANCH%"
set MSGBODY="%TFS_DROPLOCATION%    https://microsoft.visualstudio.com/Xbox.Services/_build/index?buildId=%BUILD_BUILDID%&_a=summary"
set BUILD_TOOLS=%BUILD_STAGINGDIRECTORY%\sdk.buildtools\buildMachine
call %BUILD_TOOLS%\send-build-email.cmd %MSGTITLE% %MSGBODY% 

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
