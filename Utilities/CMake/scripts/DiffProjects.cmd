if "%1" EQU "" goto help
set ROOT_FOLDER=%1
set PROJECT_PROCESSOR=%ROOT_FOLDER%\Utilities\CMake\ProjectFileProcessor\bin\Debug\ProjectFileProcessor.exe

%PROJECT_PROCESSOR% %ROOT_FOLDER% diff %ROOT_FOLDER%\Build\Microsoft.Xbox.Services.UnitTest.140.TAEF\Microsoft.Xbox.Services.UnitTest.140.TAEF.vcxproj %ROOT_FOLDER%\Utilities\CMake\output\Microsoft.Xbox.Services.UnitTest.140.TAEF.vcxproj
%PROJECT_PROCESSOR% %ROOT_FOLDER% diff %ROOT_FOLDER%\Build\Microsoft.Xbox.Services.UnitTest.140.TE\Microsoft.Xbox.Services.UnitTest.140.TE.vcxproj %ROOT_FOLDER%\Utilities\CMake\output\Microsoft.Xbox.Services.UnitTest.140.TE.vcxproj
%PROJECT_PROCESSOR% %ROOT_FOLDER% diff %ROOT_FOLDER%\Build\Microsoft.Xbox.Services.140.UWP.Cpp\Microsoft.Xbox.Services.140.UWP.Cpp.vcxproj %ROOT_FOLDER%\Utilities\CMake\output\Microsoft.Xbox.Services.140.UWP.Cpp.vcxproj
%PROJECT_PROCESSOR% %ROOT_FOLDER% diff %ROOT_FOLDER%\Build\Microsoft.Xbox.Services.141.XDK.WinRT\Microsoft.Xbox.Services.141.XDK.WinRT.vcxproj %ROOT_FOLDER%\Utilities\CMake\output\Microsoft.Xbox.Services.141.XDK.WinRT.vcxproj
%PROJECT_PROCESSOR% %ROOT_FOLDER% diff %ROOT_FOLDER%\Build\Microsoft.Xbox.Services.140.UWP.Cpp\Microsoft.Xbox.Services.140.UWP.Ship.Cpp.vcxproj %ROOT_FOLDER%\Utilities\CMake\output\Microsoft.Xbox.Services.140.UWP.Ship.Cpp.vcxproj
%PROJECT_PROCESSOR% %ROOT_FOLDER% diff %ROOT_FOLDER%\Build\Microsoft.Xbox.Services.140.UWP.WinRT\Microsoft.Xbox.Services.140.UWP.WinRT.vcxproj %ROOT_FOLDER%\Utilities\CMake\output\Microsoft.Xbox.Services.140.UWP.WinRT.vcxproj
%PROJECT_PROCESSOR% %ROOT_FOLDER% diff %ROOT_FOLDER%\Build\Microsoft.Xbox.Services.140.XDK.Cpp\Microsoft.Xbox.Services.140.XDK.Cpp.vcxproj %ROOT_FOLDER%\Utilities\CMake\output\Microsoft.Xbox.Services.140.XDK.Cpp.vcxproj
%PROJECT_PROCESSOR% %ROOT_FOLDER% diff %ROOT_FOLDER%\Build\Microsoft.Xbox.Services.140.XDK.Cpp\Microsoft.Xbox.Services.140.XDK.Ship.Cpp.vcxproj %ROOT_FOLDER%\Utilities\CMake\output\Microsoft.Xbox.Services.140.XDK.Ship.Cpp.vcxproj
%PROJECT_PROCESSOR% %ROOT_FOLDER% diff %ROOT_FOLDER%\Build\Microsoft.Xbox.Services.141.UWP.Cpp\Microsoft.Xbox.Services.141.UWP.Cpp.vcxproj %ROOT_FOLDER%\Utilities\CMake\output\Microsoft.Xbox.Services.141.UWP.Cpp.vcxproj
%PROJECT_PROCESSOR% %ROOT_FOLDER% diff %ROOT_FOLDER%\Build\Microsoft.Xbox.Services.141.UWP.Cpp\Microsoft.Xbox.Services.141.UWP.Ship.Cpp.vcxproj %ROOT_FOLDER%\Utilities\CMake\output\Microsoft.Xbox.Services.141.UWP.Ship.Cpp.vcxproj
%PROJECT_PROCESSOR% %ROOT_FOLDER% diff %ROOT_FOLDER%\Build\Microsoft.Xbox.Services.141.XDK.Cpp\Microsoft.Xbox.Services.141.XDK.Cpp.vcxproj %ROOT_FOLDER%\Utilities\CMake\output\Microsoft.Xbox.Services.141.XDK.Cpp.vcxproj
%PROJECT_PROCESSOR% %ROOT_FOLDER% diff %ROOT_FOLDER%\Build\Microsoft.Xbox.Services.141.XDK.Cpp\Microsoft.Xbox.Services.141.XDK.Ship.Cpp.vcxproj %ROOT_FOLDER%\Utilities\CMake\output\Microsoft.Xbox.Services.141.XDK.Ship.Cpp.vcxproj

goto done
:help
@echo off
echo.
echo DiffProjects.cmd rootFolder [skipCopy]
echo.
echo Example:
echo DiffProjects.cmd C:\git\forks\xbox-live-api
echo.

:done