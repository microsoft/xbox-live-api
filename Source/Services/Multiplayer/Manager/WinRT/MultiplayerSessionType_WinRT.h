// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_BEGIN

/// <summary>
/// Defines values used to indicate types for multiplayer sessions.
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
