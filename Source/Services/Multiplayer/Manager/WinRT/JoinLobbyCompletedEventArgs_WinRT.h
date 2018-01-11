// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "MultiplayerEventArgs_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_BEGIN

/// <summary>
/// Notifies the title when the client joins a lobby. Once join succeeds, the member becomes
/// part of the lobby session, and can use data in the session to connect to other lobby members.
/// </summary>
/// <remarks>
/// To join a friend's lobby, call the the JoinLobby(handleId) operation using the handleId you back get from
/// MultiplayerService::GetActivitiesForSocialGroupAsync(). If the user accepts an invite or joined a friend's game via the shell,
/// the title will get a protocol activated, in which case you should call JoinLobby(IProtocolActivatedEventArgs^).
///
/// For scenarios where the local user has not been added, you can pass the local user object part of the join_lobby API.
/// If the invited user is not added via MultiplayerLobbySession::AddLocalUser() or through JoinLobby(), then JoinLobby() will fail
/// and provide the InvitedXboxUserId that the invite was sent for as part of the JoinLobbyCompletedEventArgs.
/// </remarks>
public ref class JoinLobbyCompletedEventArgs sealed : MultiplayerEventArgs
{
public:

    /// <summary>
    /// Invited Xbox User ID of the member that the invite was sent for.
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_OBJ(InvitedXboxUserId, invited_xbox_user_id);

internal:
    JoinLobbyCompletedEventArgs(
        _In_ std::shared_ptr<xbox::services::multiplayer::manager::join_lobby_completed_event_args> cppObj
        );

    std::shared_ptr<xbox::services::multiplayer::manager::join_lobby_completed_event_args> GetCppObj() const;

private:
    std::shared_ptr<xbox::services::multiplayer::manager::join_lobby_completed_event_args> m_cppObj;

};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_END
