// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "MultiplayerSessionChangeEventArgs_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

MultiplayerSessionChangeEventArgs::MultiplayerSessionChangeEventArgs(
    _In_ xbox::services::multiplayer::multiplayer_session_change_event_args cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
    m_sessionRef = ref new MultiplayerSessionReference(m_cppObj.session_reference());
}

MultiplayerSessionReference^ 
MultiplayerSessionChangeEventArgs::SessionReference::get()
{
    return m_sessionRef;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END