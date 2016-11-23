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

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN
/// <summary>
/// Defines values used to indicate status for member of a session. 
/// </summary>

public enum class MultiplayerSessionMemberStatus
{
    /// <summary>
    /// The member slot is reserved for a specific Xbox User ID.  
    /// This specific member must join the session to fill the reservation before
    /// the JoinTimeout or they will be removed.
    /// </summary>
    Reserved = xbox::services::multiplayer::multiplayer_session_member_status::reserved,

    /// <summary>
    /// The member is inactive in the current title.  
    /// The member may be active in another title as specified by ActiveTitleId,
    /// If a inactive member doesn't mark themselves as Active within the MemberInactiveTimeout they will be removed from the session.
    /// </summary>
    Inactive = xbox::services::multiplayer::multiplayer_session_member_status::inactive,

    /// <summary>
    /// When the shell launches the title to start a multiplayer game, the member is marked as ready.
    /// If a ready member doesn't mark themselves as Active within the MemberReadyTimeout they will be marked as inactive.
    /// </summary>
    Ready = xbox::services::multiplayer::multiplayer_session_member_status::ready,

    /// <summary>
    /// The member is active in the current title.
    /// </summary>
    Active = xbox::services::multiplayer::multiplayer_session_member_status::active
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END