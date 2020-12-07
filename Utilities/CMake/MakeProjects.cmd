set ROOT_FOLDER=%~dp0\..\..
rem force root folder to an absolute path
pushd %ROOT_FOLDER%
set ROOT_FOLDER=%CD%
set NEW_FOLDER=%ROOT_FOLDER%\Utilities\CMake\output
set OLD_FOLDER=%ROOT_FOLDER%\Build

rem protect ourselves from the scripts randomly changing cwd
pushd .

setlocal
set PATH=%SystemRoot%\system32;%SystemRoot%;%SystemRoot%\System32\Wbem;%SYSTEMROOT%\System32\WindowsPowerShell\v1.0\
if EXIST "C:\Program Files (x86)\Microsoft Durango XDK\xdk\DurangoVars.cmd" call "C:\Program Files (x86)\Microsoft Durango XDK\xdk\DurangoVars.cmd" VS2017
if EXIST "C:\Program Files (x86)\Microsoft GDK\Command Prompts\GamingDesktopVars.cmd"  call "C:\Program Files (x86)\Microsoft GDK\Command Prompts\GamingDesktopVars.cmd" GamingDesktopVS2017
echo on

rem reset cwd
popd

call %ROOT_FOLDER%\Utilities\CMake\scripts\RunCMake.cmd -DUWP=TRUE
if %ERRORLEVEL% NEQ 0 goto :EOF
call %ROOT_FOLDER%\Utilities\CMake\scripts\RunCMake.cmd -DPCWIN32=TRUE
if %ERRORLEVEL% NEQ 0 goto :EOF
call %ROOT_FOLDER%\Utilities\CMake\scripts\RunCMake.cmd -DANDROID=TRUE
if %ERRORLEVEL% NEQ 0 goto :EOF
call %ROOT_FOLDER%\Utilities\CMake\scripts\RunCMake.cmd -DGDK=TRUE
if %ERRORLEVEL% NEQ 0 goto :EOF
call %ROOT_FOLDER%\Utilities\CMake\scripts\RunCMake.cmd -DXDK=TRUE
if %ERRORLEVEL% NEQ 0 goto :EOF
call %ROOT_FOLDER%\Utilities\CMake\scripts\RunCMake.cmd -DUNITTEST=TRUE -DTAEF=TRUE
if %ERRORLEVEL% NEQ 0 goto :EOF
call %ROOT_FOLDER%\Utilities\CMake\scripts\RunCMake.cmd -DUNITTEST=TRUE -DTE=TRUE
if %ERRORLEVEL% NEQ 0 goto :EOF

%ROOT_FOLDER%\Utilities\CMake\ProjectFileProcessor\bin\Debug\ProjectFileProcessor.exe %ROOT_FOLDER%
endlocal

if "%1" EQU "skipCopy" goto skipCopy
copy %NEW_FOLDER%\Microsoft.Xbox.Services.140.Android.vcxproj* %OLD_FOLDER%\Microsoft.Xbox.Services.140.Android
copy %NEW_FOLDER%\Microsoft.Xbox.Services.140.UWP.Cpp.vcxproj* %OLD_FOLDER%\Microsoft.Xbox.Services.140.UWP.Cpp
copy %NEW_FOLDER%\Microsoft.Xbox.Services.140.XDK.Cpp.vcxproj* %OLD_FOLDER%\Microsoft.Xbox.Services.140.XDK.Cpp
copy %NEW_FOLDER%\Microsoft.Xbox.Services.140.Win32.Cpp.vcxproj* %OLD_FOLDER%\Microsoft.Xbox.Services.140.Win32.Cpp
copy %NEW_FOLDER%\Microsoft.Xbox.Services.141.UWP.Cpp.vcxproj* %OLD_FOLDER%\Microsoft.Xbox.Services.141.UWP.Cpp
copy %NEW_FOLDER%\Microsoft.Xbox.Services.141.GDK.C.vcxproj* %OLD_FOLDER%\Microsoft.Xbox.Services.141.GDK.C
copy %NEW_FOLDER%\Microsoft.Xbox.Services.141.Win32.Cpp.vcxproj* %OLD_FOLDER%\Microsoft.Xbox.Services.141.Win32.Cpp
copy %NEW_FOLDER%\Microsoft.Xbox.Services.141.Android.vcxproj* %OLD_FOLDER%\Microsoft.Xbox.Services.141.Android
copy %NEW_FOLDER%\Microsoft.Xbox.Services.141.XDK.Cpp.vcxproj* %OLD_FOLDER%\Microsoft.Xbox.Services.141.XDK.Cpp
copy %NEW_FOLDER%\Microsoft.Xbox.Services.141.XDK.Cpp.vcxproj* %OLD_FOLDER%\Microsoft.Xbox.Services.141.XDK.Cpp
copy %NEW_FOLDER%\Microsoft.Xbox.Services.142.Win32.Cpp.vcxproj* %OLD_FOLDER%\Microsoft.Xbox.Services.142.Win32.Cpp
copy %NEW_FOLDER%\Microsoft.Xbox.Services.142.UWP.Cpp.vcxproj* %OLD_FOLDER%\Microsoft.Xbox.Services.142.UWP.Cpp
copy %NEW_FOLDER%\Microsoft.Xbox.Services.142.GDK.C.vcxproj* %OLD_FOLDER%\Microsoft.Xbox.Services.142.GDK.C
copy %NEW_FOLDER%\Microsoft.Xbox.Services.142.XDK.Cpp.vcxproj* %OLD_FOLDER%\Microsoft.Xbox.Services.142.XDK.Cpp
copy %NEW_FOLDER%\Microsoft.Xbox.Services.UnitTest.141.TAEF.vcxproj* %OLD_FOLDER%\Microsoft.Xbox.Services.UnitTest.141.TAEF
copy %NEW_FOLDER%\Microsoft.Xbox.Services.UnitTest.141.TE.vcxproj* %OLD_FOLDER%\Microsoft.Xbox.Services.UnitTest.141.TE
copy %NEW_FOLDER%\Microsoft.Xbox.Services.UnitTest.142.TAEF.vcxproj* %OLD_FOLDER%\Microsoft.Xbox.Services.UnitTest.142.TAEF
copy %NEW_FOLDER%\Microsoft.Xbox.Services.UnitTest.142.TE.vcxproj* %OLD_FOLDER%\Microsoft.Xbox.Services.UnitTest.142.TE
:skipCopy

goto done
:help
echo.
echo MakeProjects.cmd [skipCopy]
echo.
echo Example:
echo MakeProjects.cmd
echo.

:done
