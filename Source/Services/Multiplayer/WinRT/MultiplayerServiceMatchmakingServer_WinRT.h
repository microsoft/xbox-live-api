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
#include "shared_macros.h"
#include "xsapi/multiplayer.h"
#include "MatchmakingStatus_WinRT.h"
#include "MultiplayerSessionReference_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

/// <summary>
/// Represents the server that allows multiplayer session discovery to collaborate in a matchmaking session. 
/// </summary>
public ref class MultiplayerSessionMatchmakingServer sealed
{
public:
    /// <summary>
    /// The matchmaking status of the multiplayer session server.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(Status, status, MatchmakingStatus);

    /// <summary>
    /// The matchmaking status details of the multiplayer session server.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(StatusDetails, status_details);

    /// <summary>
    /// The typical wait time of the multiplayer session server.
    /// </summary>
    DEFINE_PROP_GET_TIMESPAN_OBJ(TypicalWait, typical_wait);

    /// <summary>
    /// The target session reference of the multiplayer session server.
    /// </summary>
    property MultiplayerSessionReference^ TargetSessionRef { MultiplayerSessionReference^ get(); }

internal:
    MultiplayerSessionMatchmakingServer(_In_ xbox::services::multiplayer::multiplayer_session_matchmaking_server cppObj);

private:
    xbox::services::multiplayer::multiplayer_session_matchmaking_server m_cppObj;
    MultiplayerSessionReference^ m_targetSessionReference;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END