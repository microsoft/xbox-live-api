echo Running updateGdkScript.cmd

set GDK_LOCAL_ROOT=%GameDKLatest%GRDK\ExtensionLibraries\Xbox.Services.API.C\DesignTime\CommonConfiguration\Neutral

set SRC_INCLUDE_XSAPI=%TFS_SourcesDirectory%\Include\xsapi-c
set DEST_GDK_INCLUDE_PATH_XSAPI=%GDK_LOCAL_ROOT%\Include\xsapi-c
robocopy /NJS /NJH /MT:16 /S /NP "%SRC_INCLUDE_XSAPI%" "%DEST_GDK_INCLUDE_PATH_XSAPI%"

set SRC_INCLUDE_HTTP=%TFS_SourcesDirectory%\External\xal\External\libHttpClient\Include
set DEST_GDK_INCLUDE_PATH_HTTP=%GDK_LOCAL_ROOT%\Include\httpClient
robocopy /NJS /NJH /MT:16 /S /NP "%SRC_INCLUDE_HTTP%" "%DEST_GDK_INCLUDE_PATH_HTTP%"

set SRC_INCLUDE_XAL=%TFS_SourcesDirectory%\External\xal\Source\Xal\Include\Xal
set DEST_GDK_INCLUDE_PATH_XAL=%GDK_LOCAL_ROOT%\Include\Xal
robocopy /NJS /NJH /MT:16 /S /NP "%SRC_INCLUDE_XAL%" "%DEST_GDK_INCLUDE_PATH_XAL%"

set SRC_GDK_LIB_DEBUG_XSAPI=%TFS_DropLocation%\Debug\Gaming.Desktop.x64\Microsoft.Xbox.Services.141.GDK.C
set SRC_GDK_LIB_DEBUG_HTTP=%TFS_DropLocation%\%TYPE%\Debug\Gaming.Desktop.x64\libHttpClient.141.GDK.C
set DEST_GDK_LIB_DEBUG=%GDK_LOCAL_ROOT%\Lib\Debug\v141
robocopy /NJS /NJH /MT:16 /S /NP "%SRC_GDK_LIB_DEBUG_XSAPI%" "%DEST_GDK_LIB_DEBUG%"
robocopy /NJS /NJH /MT:16 /S /NP "%SRC_GDK_LIB_DEBUG_HTTP%" "%DEST_GDK_LIB_DEBUG%"

set SRC_GDK_LIB_RELEASE_XSAPI=%TFS_DropLocation%\Release\Gaming.Desktop.x64\Microsoft.Xbox.Services.141.GDK.C
set SRC_GDK_LIB_RELEASE_HTTP=%TFS_DropLocation%\%TYPE%\Release\Gaming.Desktop.x64\libHttpClient.141.GDK.C
set DEST_GDK_LIB_RELEASE=%GDK_LOCAL_ROOT%\Lib\Release\v141
robocopy /NJS /NJH /MT:16 /S /NP "%SRC_GDK_LIB_RELEASE_XSAPI%" "%DEST_GDK_LIB_RELEASE%"
robocopy /NJS /NJH /MT:16 /S /NP "%SRC_GDK_LIB_RELEASE_HTTP%" "%DEST_GDK_LIB_RELEASE%"

echo Done updateGdkScript.cmd
:done