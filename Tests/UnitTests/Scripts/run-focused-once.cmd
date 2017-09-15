set OUTPUT_FOLDER=%1
set TAEF_EXE="C:\Program Files (x86)\Windows Kits\10\Testing\Runtimes\TAEF\x64\TE.exe"
set MYPATH=%~dp0
set TE_DLL=%MYPATH:~0,-1%\..\..\..\Binaries\Debug\x64\Microsoft.Xbox.Services.UnitTest.140.TAEF\Microsoft.Xbox.Services.UnitTest.140.TAEF.dll 
if "%1" EQU "" set OUTPUT_FOLDER=c:\test
mkdir %OUTPUT_FOLDER%

%TAEF_EXE% /inproc /select:"@Focus = 1" %TE_DLL%
goto done

:help
echo run-focused-once.cmd c:\test
:done