set MYPATH=%~dp0
if NOT EXIST %MYPATH%CodeSnippets mkdir %MYPATH%CodeSnippets
%MYPATH%ExtractCodeSnippet\ExtractCodeSnip.exe %MYPATH%..\..\..\Tests %MYPATH%CodeSnippets