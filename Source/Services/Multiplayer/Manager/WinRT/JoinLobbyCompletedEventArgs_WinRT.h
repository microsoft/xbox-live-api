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
#include "MultiplayerEventArgs_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_BEGIN

/// <summary>
/// Notifies the title when the client joins a lobby. Once join succeeds, the member is now 
/// part of the lobby session, and can use data in the session to connect to other lobby members.
/// To join a friend lobby, call join_lobby() operation using the handleId you back get from
/// multiplayer_service::get_activities_for_social_group. If the user accepts an invite or joined a friends game via a shell UI, 
/// the title will get a protocol activation. If the invited user is not added 
/// via local_member_manager::add_local_user(), then join_lobby() will fail and provide the invited_xbox_user_id()
/// that the invite was sent for as part of the join_lobby_completed_event_args()
/// </summary>
public ref class JoinLobbyCompletedEventArgs sealed : MultiplayerEventArgs
{
public:

    /// <summary>
    /// Invited Xbox User ID of the member that.
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