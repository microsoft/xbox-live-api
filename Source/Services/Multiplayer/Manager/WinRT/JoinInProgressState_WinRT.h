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
#include "pch.h"
#include "xsapi/multiplayer_manager.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_BEGIN

/// <summary>
/// Defines values used to indicate join in progress state on who can join your game.
/// </summary>
public enum class Joinability
{
    /// <summary>
    /// Joinability not set or no lobby exists yet.
    /// </summary>
    None = xbox::services::multiplayer::manager::joinability::none,

    /// <summary>
    /// Default value. The lobby is joinable by users who are followed by an existing member of the session.
    /// </summary>
    JoinableByFriends = xbox::services::multiplayer::manager::joinability::joinable_by_friends,

    /// <summary>
    /// The lobby is joinable only via an invite.
    /// </summary>
    InviteOnly = xbox::services::multiplayer::manager::joinability::invite_only,

    /// <summary>
    /// This option will close the lobby only when a game is in progress. All other times,
    /// it will keep the lobby open for invite_only so invitees can join when no game is in progress.
    /// </summary>
    DisableWhileGameInProgress = xbox::services::multiplayer::manager::joinability::disable_while_game_in_progress,

    /// <summary>
    /// This option will close the lobby immediately.
    /// </summary>
    Closed = xbox::services::multiplayer::manager::joinability::closed
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_END