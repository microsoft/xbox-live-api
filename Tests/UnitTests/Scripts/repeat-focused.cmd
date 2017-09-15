if "%1" EQU "" goto help
set OUTPUT_FOLDER=%1
set TAEF_EXE="C:\Program Files (x86)\Windows Kits\10\Testing\Runtimes\TAEF\x64\TE.exe"
set MYPATH=%~dp0
set TE_DLL=%MYPATH:~0,-1%\..\..\..\Binaries\Debug\x64\Microsoft.Xbox.Services.UnitTest.140.TAEF\Microsoft.Xbox.Services.UnitTest.140.TAEF.dll 
mkdir %OUTPUT_FOLDER%
set /A X=1

:loop
set /A X=X+1
%TAEF_EXE% /labMode /sessionTimeout:0:15 /testTimeout:0:5 %TE_DLL% /select:"@Focus = 1" > %OUTPUT_FOLDER%\test%x%.txt
if %ERRORLEVEL% EQU 0 copy %OUTPUT_FOLDER%\test%x%.txt %OUTPUT_FOLDER%\test.txt && del %OUTPUT_FOLDER%\test%x%.txt
type %OUTPUT_FOLDER%\test.txt
del %OUTPUT_FOLDER%\test.txt

goto loop

:help
echo repeat-focused.cmd c:\test
:done