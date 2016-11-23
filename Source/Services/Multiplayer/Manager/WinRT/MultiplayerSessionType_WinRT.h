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

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_BEGIN

/// <summary>
/// Defines values used to indicate change types for a multiplayer game.
/// </summary>
public enum class MultiplayerSessionType
{
    /// <summary>
    /// The session type is unknown.
    /// </summary>
    Unknown = xbox::services::multiplayer::manager::multiplayer_session_type::unknown,

    /// <summary>
    /// Indicates multiplayer lobby session.
    /// </summary>
    LobbySession = xbox::services::multiplayer::manager::multiplayer_session_type::lobby_session,

    /// <summary>
    /// Indicates multiplayer game session.
    /// </summary>
    GameSession = xbox::services::multiplayer::manager::multiplayer_session_type::game_session
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_END