if "%1"=="2019" goto build2019
if "%1"=="2017" goto build2017
echo testLocalGdkBuild 2017
echo testLocalGdkBuild 2019
goto :EOF

:build2019
"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\amd64\msbuild.exe" "%~dp0..\..\Microsoft.Xbox.Services.GDK.VS2019.sln" /flp:"logfile=%~dp0..\..\Microsoft.Xbox.Services.GDK.VS2019.sln.log;verbosity=diagnostic" /p:platform="Gaming.Desktop.x64" /p:configuration="Debug" /p:VisualStudioVersion="16.0" /pp:%~dp0..\..\Microsoft.Xbox.Services.GDK.VS2019.sln.out
echo see Microsoft.Xbox.Services.GDK.VS2019.sln.log
echo see Microsoft.Xbox.Services.GDK.VS2019.sln.out
goto :EOF

:build2017
"C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\amd64\msbuild.exe" "%~dp0..\..\Microsoft.Xbox.Services.GDK.VS2017.sln" /flp:"logfile=%~dp0..\..\Microsoft.Xbox.Services.GDK.VS2017.sln.log;verbosity=diagnostic" /p:platform="Gaming.Desktop.x64" /p:configuration="Debug" /p:VisualStudioVersion="15.0" /pp:%~dp0..\..\Microsoft.Xbox.Services.GDK.VS2017.sln.out
echo see Microsoft.Xbox.Services.GDK.VS2017.sln.log
echo see Microsoft.Xbox.Services.GDK.VS2017.sln.out
goto :EOF


