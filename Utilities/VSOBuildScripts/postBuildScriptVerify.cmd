if "%NUGET_EXE%" == "" set NUGET_EXE=%AGENT_TOOLSDIRECTORY%\NuGet\4.5.0\x64\nuget.exe

for /r %TFS_DropLocation%\NuGetBinaries %%x in (*) do %NUGET_EXE% verify -signatures %%x >> %TFS_DropLocation%\NuGetBinaries\nuget-verify.txt