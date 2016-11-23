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