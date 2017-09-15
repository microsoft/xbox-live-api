if "%1" EQU "" goto help
if "%2" EQU "" goto help
set OUTPUT_FOLDER=%1
set TAEF_EXE="C:\Program Files (x86)\Windows Kits\10\Testing\Runtimes\TAEF\x64\TE.exe"
set TEST_NAME=%2
set MYPATH=%~dp0
set TE_DLL=%MYPATH:~0,-1%\..\..\..\Binaries\Debug\x64\Microsoft.Xbox.Services.UnitTest.140.TAEF\Microsoft.Xbox.Services.UnitTest.140.TAEF.dll 
mkdir %OUTPUT_FOLDER%
set /A X=1

:loop
set /A X=X+1
"C:\Program Files (x86)\Windows Kits\10\Windows Performance Toolkit\wpr.exe" -start ETWProfile.wprp
%TAEF_EXE% /labMode /sessionTimeout:0:15 /testTimeout:0:5 %TE_DLL% /select:@name='*%TEST_NAME%' > %OUTPUT_FOLDER%\test%x%.txt
set TAEF_ERROR=%ERRORLEVEL%

"C:\Program Files (x86)\Windows Kits\10\Windows Performance Toolkit\wpr.exe" -stop %OUTPUT_FOLDER%\test%x%.etl "XSAPI UnitTest Diagnostic Trace"
if %TAEF_ERROR% EQU 0 del %OUTPUT_FOLDER%\test%x%.txt
if %TAEF_ERROR% EQU 0 del %OUTPUT_FOLDER%\test%x%.etl
type %OUTPUT_FOLDER%\test%x%.txt

goto loop

:help
echo repeat-single-with-etw.cmd c:\test TestGetLeaderboardForSocialGroupWithSkipToRankAsync
:done