// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

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