set OUTPUT_FOLDER=%1
if "%1" EQU "" set OUTPUT_FOLDER=c:\test
set TAEF_EXE="C:\Program Files (x86)\Windows Kits\10\Testing\Runtimes\TAEF\x64\TE.exe"
set MYPATH=%~dp0
set TE_DLL=%MYPATH:~0,-1%\..\..\..\Binaries\Debug\x64\Microsoft.Xbox.Services.UnitTest.140.TAEF\Microsoft.Xbox.Services.UnitTest.140.TAEF.dll 
mkdir %OUTPUT_FOLDER%
set /A X=1

%TAEF_EXE% /inproc %TE_DLL%
goto done

:help
echo run-all-tests-once.cmd c:\test
:done