@echo off
set HOMEPATH=%~dp0
set ROOTPATH=%HOMEPATH%..\..\..\..\
echo Checking if XBL Docs repo is present as a sibling...
if EXIST %ROOTPATH%xbox-live-docs-pr\.git (GOTO repoExists) else (GOTO repoDoesNotExist)

:repoDoesNotExist
echo XBL Docs repo doesn't exist!
echo Auto-cloning XBL Docs repo as sibling...
cd ROOTPATH
git clone https://dev.azure.com/gamingcx/gaming/_git/xbox-live-docs-pr
if EXIST %HOMEPATH%xbox-live-docs-pr\.git (GOTO repoExists) else (GOTO cloneFailed)

:cloneFailed
echo Copying Code Snippets to XBL Docs repo requires that you have access to the repo.
echo Try again once you are granted access.
pause
EXIT

:repoExists
echo Switching to XBL Docs repo and pulling master...
cd %ROOTPATH%xbox-live-docs-pr
git checkout master
git pull
GOTO checkLocal

:checkLocal
echo Checking if local XSAPI-Automation branch exists...
for /f %%i in ('git branch') do if "%%i"=="XSAPI-Automation" (GOTO deleteLocal)
GOTO checkRemote

:deleteLocal
echo Resetting local branch...
git branch -D XSAPI-Automation
GOTO checkRemote

:checkRemote
echo Checking if remote XSAPI-Automation branch exists...
for /f %%i in ('git branch -r') do if "%%i"=="origin/XSAPI-Automation" (GOTO branchExists)
GOTO createRemote

:createRemote
echo Auto-creating remote branch...
git branch XSAPI-Automation origin/master
git push --set-upstream origin XSAPI-Automation
GOTO branchExists

:branchExists
echo Switching to XSAPI-Automation branch...
git checkout XSAPI-Automation
git pull
echo Merging with master...
git merge master
git push
echo Copying code snippets over...
robocopy %HOMEPATH%CodeSnippets %ROOTPATH%xbox-live-docs-pr\xbox-live-docs-pr\code-snippets
git add xbox-live-docs-pr/code-snippets/*
git commit -m "Updated code snippets from XSAPI Internal"
git push
echo Copying successfully completed into XSAPI-Automation branch of XBL Docs repo.
echo Make sure to complete the PR to merge XSAPI-Automation branch into master in XBL Docs repo!
echo Switching back to XSAPI repo...
cd %HOMEPATH%