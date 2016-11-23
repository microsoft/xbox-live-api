//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#pragma once
#include "xsapi/multiplayer.h"
#include "MultiplayerSessionReference_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN
/// <summary>
/// Indicates the session that triggered the event.
/// </summary> 
public ref class MultiplayerSessionChangeEventArgs sealed
{
public:
    /// <summary>
    /// The session that triggered this event.
    /// </summary>
    property MultiplayerSessionReference^ SessionReference { Multiplayer::MultiplayerSessionReference^ get(); }

    /// <summary>
    /// The branch of the session used to scope change numbers. 
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Branch, branch)

    /// <summary>
    /// The change number of the session.
    /// </summary>
    DEFINE_PROP_GET_OBJ(ChangeNumber, change_number, uint64 )

internal:
    MultiplayerSessionChangeEventArgs(
        _In_ xbox::services::multiplayer::multiplayer_session_change_event_args cppObj
        );

private:
    xbox::services::multiplayer::multiplayer_session_change_event_args m_cppObj;
    MultiplayerSessionReference^ m_sessionRef;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END