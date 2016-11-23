@echo off
REM ====================================================================
REM Set path variables
REM ==========================================================================

IF /I "%1"=="VS2012" (
  CALL :SetVS2012
)ELSE IF /I "%1"=="" (
  REM Deprecated, but present for backward compatibility among scripts.
  CALL :SetVS2012
)ELSE IF /I "%1"=="VS2015" (
  CALL :SetVS2015
)ELSE IF /I "%1"=="XDKVS2012" (
  REM Deprecated, but present for backward compatibility among scripts.
  CALL :SetVS2012
)ELSE IF /I "%1"=="XDK" (
  REM Deprecated, but present for backward compatibility among scripts.
  CALL :SetVS2012
)ELSE IF /I "%1"=="XDKVS2015" (
  REM Deprecated, but present for backward compatibility among scripts.
  CALL :SetVS2015
)ELSE (
  ECHO You must specify VS2012, or VS2015 on the command line.
  EXIT /B 1
)

IF "%XDKEDITION%"=="" (
    IF NOT "%2"=="" (
       SET XDKEDITION=%2
    )ELSE (
        REM Get the most recent XDK edition installed.
        CALL :GetLatestXDK
    )
)

IF "%XDKEDITION%"=="" (
    REM If no XDK edition found then assume Durango.
    SET XDKEDITION=000000
)

Echo Setting environment for using Microsoft Xbox One XDK %DTARGETVS% Tools
TITLE Xbox One XDK %DTARGETVS% Command Prompt

IF "%DurangoXDK%"=="" (
    CALL :GetDurangoXDKInstallPath
)
IF "%DurangoXDK%"=="" (
    ECHO Microsoft Xbox One Development Kit directory is not found on this machine.
    EXIT /B 1
)

IF NOT "%XDKEDITION%"=="000000" (
    IF "%XboxOneXDKBuild%"=="" (
        CALL :GetXDKBuildInstallPath
    )

    IF "%XboxOneXDKEditionVersionFriendlyName%"=="" (
        CALL :GetXDKEditionVersionFriendlyName
    )

    IF "%XboxOneExtensionSdkBuild%"=="" (
        CALL :GetXboxOneExtensionSdkPath
    )
)
    
IF NOT "%XDKEDITION%"=="000000" (
    IF "%XboxOneXDKBuild%"=="" (
        ECHO Microsoft Xbox One Development Kit build for %XDKEDITION% is not found on this machine.
        EXIT /B 1
    )

    IF "%XboxOneExtensionSdkBuild%"=="" (
        ECHO Microsoft Xbox One Development Kit SDK Extensions for %XDKEDITION% are not found on this machine.
        EXIT /B 1
    )

    IF NOT "%XboxOneXDKEditionVersionFriendlyName%"=="" (
        TITLE Xbox One XDK %XboxOneXDKEditionVersionFriendlyName% %DTARGETVS% Command Prompt
    )
)ELSE (
    REM Present for backward compatibility with Durango XDKs.
    SET "XboxOneXDKBuild=%DurangoXDK%"
)

IF "%VSInstallDir%"=="" (
    CALL :GetVSInstallDir
)
IF "%VSInstallDir%"=="" (
    ECHO Warning: %DTARGETVS% is not found on this machine.
)

IF "%VCInstallDir%"=="" (
    CALL :GetVCInstallDir
)
IF "%VCInstallDir%"=="" (
    ECHO Warning: %DTARGETVS% VC is not found on this machine.
)

IF "%FrameworkDir%"=="" (
    CALL :GetFrameworkDir
)
IF "%FrameworkDir%"=="" (
    ECHO Warning: .Net Framework is not found on this machine.
)

IF "%FrameworkVersion%"=="" (
    CALL :GetFrameworkVer
)
IF "%FrameworkVersion%"=="" (
    IF EXIST "%FrameworkDir%v4.0.30319" (
        SET "FrameworkVersion=v4.0.30319"
    )
)

IF "%MSBuildInstallDir11%"=="" (
    CALL :GetMSBuildInstallDir11
)

IF "%MSBuildInstallDir14%"=="" (
    CALL :GetMSBuildInstallDir14
)

IF "%PIXPath%"=="" (
    CALL :GetPIXPath
)

REM ==========================================================================
REM Set path 
REM ==========================================================================

IF EXIST "%VSInstallDir%Team Tools\Performance Tools" (
    SET "PATH=%VSInstallDir%Team Tools\Performance Tools\x64;%VSInstallDir%Team Tools\Performance Tools;%PATH%"
)

IF EXIST "%VSInstallDir%" (
    SET "PATH=%VSInstallDir%Common7\Tools;%VSInstallDir%Common7\IDE;%PATH%"
)

IF EXIST "%VCInstallDir%" (
    SET "PATH=%VCInstallDir%VCPackages;%VCInstallDir%BIN;%PATH%"
)

IF EXIST "%FrameworkDir%" (
    SET "PATH=%FrameworkDir%%FrameworkVersion%;%PATH%"
)
SET "PATH=%DurangoXDK%bin;%PATH%"

IF NOT "%XDKEDITION%"=="000000" (
    SET "PATH=%XboxOneXDKBuild%bin;%PATH%"
)

IF EXIST "%PIXPath%" (
    SET "PATH=%PIXPath%;%PATH%"
)

REM Custom compiler for VS2012/ERA.
IF "%DPLATFORM%"=="VS2012" (
    IF NOT "%XDKEDITION%"=="000000" (
        SET "PATH=%XboxOneXDKBuild%Compilers\dev11.1\vc\bin\amd64;%PATH%"
    )ELSE (
        REM Present for backward compatibility with Durango XDKs.
        SET "PATH=%DurangoXDK%xdk\vc\bin\amd64;%PATH%"
    )
)ELSE (
    REM Normal compiler for VS2015/ERA.
    SET "PATH=%VSInstallDir%\vc\bin\amd64;%PATH%"
)

REM Custom shader compiler.
SET "PATH=%XboxOneXDKBuild%xdk\fxc\amd64;%PATH%"


REM ==========================================================================
REM Set Include
REM ==========================================================================

IF EXIST "%VCInstallDir%" (
    SET "INCLUDE=%VCInstallDir%INCLUDE;%INCLUDE%"
)
SET "INCLUDE=%XboxOneXDKBuild%xdk\include\shared;%XboxOneXDKBuild%xdk\include\um;%XboxOneXDKBuild%xdk\include\winrt;%INCLUDE%"

IF "%DPLATFORM%"=="VS2015" (
  SET "INCLUDE=%XboxOneXDKBuild%xdk\ucrt\inc;%XboxOneXDKBuild%xdk\include\cppwinrt;%XboxOneXDKBuild%xdk\VS2015\vc\include;%XboxOneXDKBuild%xdk\VS2015\vc\platform\amd64;%INCLUDE%"
)

REM ==========================================================================
REM Set Lib
REM ==========================================================================
IF EXIST "%VCInstallDir%" (
    SET "LIB=%VCInstallDir%LIB\amd64;%LIB%"
)
SET LIB=%XboxOneXDKBuild%xdk\lib\amd64;%LIB%

IF "%DPLATFORM%"=="VS2012" (
    IF NOT "%XDKEDITION%"=="000000" (
        SET "LIB=%XboxOneXDKBuild%Compilers\dev11.1\crt\lib\amd64;%LIB%"
    ) ELSE (
        REM Present for backward compatibility with Durango XDKs.
        SET "LIB=%XboxOneXDKBuild%xdk\crt\lib\amd64;%LIB%"
    )
) ELSE (
    SET "LIB=%XboxOneXDKBuild%xdk\ucrt\lib\amd64;%XboxOneXDKBuild%xdk\VS2015\vc\lib\amd64;%LIB%"
)

REM ==========================================================================
REM Set LibPath
REM ==========================================================================
IF EXIST "%VCInstallDir%" (
    SET "LIBPATH=%VCInstallDir%LIB\amd64;%LIBPATH%"
)
IF EXIST "%FrameworkDir%" (
    SET "LIBPATH=%FrameworkDir%%FrameworkVersion%;%LIBPATH%"
)

IF NOT "%XDKEDITION%"=="000000" (
    SET "LIBPATH=%XboxOneExtensionSdkBuild%references\CommonConfiguration\Neutral;%LIBPATH%"
)ELSE (
    REM Present for backward compatibility with Durango XDKs.
    SET "LIBPATH=%DurangoXDK%xdk\references\CommonConfiguration\Neutral;%LIBPATH%"
)

SET Platform=Durango

REM ==========================================================================
REM Set the MSBuild path after everything else so that it appears first in the
REM path. VS2015 must not accidentally target msbuild.exe in the .NET
REM Framework since it stopped shipping there in dev12 and later.
REM ==========================================================================
IF /I "%DPLATFORM%"=="VS2012" (
  SET "PATH=%MSBuildInstallDir11%;%PATH%"
)ELSE IF /I "%DPLATFORM%"=="VS2015" (
  SET "PATH=%MSBuildInstallDir14%;%PATH%"
)

SET CommandPromptType=Native
SET VisualStudioVersion=%DTARGETVSVER%
CD /D "%DurangoXDK%bin"

GOTO :EOF

:GetVSInstallDir
FOR /f "tokens=1,2*" %%a in ('reg query "HKLM\Software\Microsoft\VisualStudio\SxS\VS7" /v "%DTARGETVSVER%" /reg:32 2^>NUL') DO SET VSInstallDir=%%c
GOTO :EOF

:GetVCInstallDir
FOR /f "tokens=1,2*" %%a in ('reg query "HKLM\Software\Microsoft\VisualStudio\SxS\VC7" /v "%DTARGETVSVER%" /reg:32 2^>NUL') DO SET VCInstallDir=%%c
GOTO :EOF

:GetLatestXDK
REM Sets XDKEDITION to latest XDK's edition number.
FOR /f "tokens=1,2*" %%a in ('reg query "HKLM\Software\Microsoft\Durango XDK" /v "Latest" /reg:32 2^>NUL') DO SET XDKEDITION=%%c
GOTO :EOF

:GetXDKBuildInstallPath
REM Sets XboxOneXDKBuild to location of Xbox One XDK build tools.
FOR /f "tokens=1,2*" %%a in ('reg query "HKLM\Software\Microsoft\Durango XDK\%XDKEDITION%" /v "InstallPath" /reg:32 2^>NUL') DO SET XboxOneXDKBuild=%%c
GOTO :EOF

:GetXboxOneExtensionSdkPath
REM Sets  XboxOneExtensionSdkBuild to the location of the current XDK edition's extension SDK root.
FOR /f "tokens=1,2*" %%a in ('reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Microsoft SDKs\Durango.%XDKEDITION%\v8.0" /v "InstallationFolder" /reg:32 2^>NUL') DO SET XboxOneExtensionSdkBuild=%%c
GOTO :EOF

:GetXDKEditionVersionFriendlyName
REM Sets XboxOneXDKEditionVersionFriendlyName to the friendly version name of the selected build tools.
FOR /f "tokens=1,2*" %%a in ('reg query "HKLM\Software\Microsoft\Durango XDK\%XDKEDITION%" /v "EditionVersionFriendlyName" /reg:32 2^>NUL') DO SET XboxOneXDKEditionVersionFriendlyName=%%c
GOTO :EOF

:GetDurangoXDKInstallPath
REM Sets DurangoXDK to location of Durango XDK
FOR /f "tokens=1,2*" %%a in ('reg query "HKLM\Software\Microsoft\Durango XDK" /v "InstallPath" /reg:32 2^>NUL') DO SET DurangoXDK=%%c
GOTO :EOF

:GetPixPath
FOR /f "tokens=1,2*" %%a in ('reg query "HKLM\Software\Microsoft\Durango XDK" /v "PIXPath" /reg:32 2^>NUL') DO SET PIXPath=%%c
GOTO :EOF

:GetMSBuildInstallDir11
REM Sets MSBuild11 directory.
FOR /f "tokens=1,2*" %%a in ('reg query "HKLM\Software\Microsoft\MSBuild\4.0" /v "MSBuildOverrideTasksPath" /reg:32 2^>NUL') DO SET MSBuildInstallDir11=%%c
GOTO :EOF

:GetMSBuildInstallDir14
REM Sets MSBuild14 directory.
FOR /f "tokens=1,2*" %%a in ('reg query "HKLM\Software\Microsoft\MSBuild\14.0" /v "MSBuildOverrideTasksPath" /reg:32 2^>NUL') DO SET MSBuildInstallDir14=%%c
GOTO :EOF

:GetFrameworkDir
FOR /f "tokens=1,2*" %%a in ('reg query "HKLM\Software\Microsoft\.NETFramework" /v "InstallRoot" /reg:32 2^>NUL') DO SET FrameworkDir=%%c
GOTO :EOF

:GetFrameworkVer
FOR /f "tokens=1,2*" %%a in ('reg query "HKLM\Software\Microsoft\VisualStudio\SxS\VC7" /v "FrameworkVer32" /reg:32 2^>NUL') DO SET FrameworkVersion=%%c
GOTO :EOF

:SetVS2012
SET DPLATFORM=VS2012
SET DTARGETVS=Visual Studio 2012
SET DTARGETVSVER=11.0
GOTO :EOF

:SetVS2015
SET DPLATFORM=VS2015
SET DTARGETVS=Visual Studio 2015
SET DTARGETVSVER=14.0
GOTO :EOF
:end
