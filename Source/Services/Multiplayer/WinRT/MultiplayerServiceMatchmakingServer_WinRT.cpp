//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#include "MultiplayerServiceMatchmakingServer_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

MultiplayerSessionMatchmakingServer::MultiplayerSessionMatchmakingServer(
    _In_ xbox::services::multiplayer::multiplayer_session_matchmaking_server cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
    if (!m_cppObj.target_session_ref().is_null())
    {
        m_targetSessionReference = ref new MultiplayerSessionReference(m_cppObj.target_session_ref());
    }
}

MultiplayerSessionReference^
MultiplayerSessionMatchmakingServer::TargetSessionRef::get()
{
    return m_targetSessionReference;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END