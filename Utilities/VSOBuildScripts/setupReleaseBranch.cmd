@echo off
set BUILD_SOURCESDIRECTORY=%CD%\..\..

if "%1" == "" goto help
if "%2" == "" goto help

set SDK_RELEASE_YEAR=%1
set SDK_RELEASE_MONTH=%2
set SDK_RELEASE_QFE=0
if NOT "%3" == "" set SDK_RELEASE_QFE=%3

set RElEASE_SHORTNAME=%SDK_RELEASE_YEAR:~2,2%%SDK_RELEASE_MONTH%
set RELEASE_BRANCH_NAME=preview/%RElEASE_SHORTNAME%

set /p RESPONSE="Create preview branch %RELEASE_BRANCH_NAME%? (y/n) "
if "%RESPONSE%" NEQ "y" goto done

git checkout -b %RELEASE_BRANCH_NAME%
git push --set-upstream origin %RELEASE_BRANCH_NAME%

set UserAgentYearOverride=%SDK_RELEASE_YEAR%
set UserAgentMonthOverride=%SDK_RELEASE_MONTH%
set UserAgentQFEOverride=%SDK_RELEASE_QFE%
call preCompileScript.cmd

goto done
:help
echo Missing parameters
echo setupReleaseBranch.cmd YEAR MONTH
echo setupReleaseBranch.cmd YEAR MONTH QFE

:done
