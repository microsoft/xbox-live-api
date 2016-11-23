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
/// Defines values that indicate the current status of a session.
/// </summary>
public enum class MultiplayerSessionStatus
{
    /// <summary>
    /// The server returned an unrecognized response.
    /// </summary>
    Unknown = xbox::services::multiplayer::multiplayer_session_status::unknown,

    /// <summary>
    /// The session is active and there is at least one user.
    /// </summary>
    Active = xbox::services::multiplayer::multiplayer_session_status::active,

    /// <summary>
    /// The session is inactive. This means that no users in the session are
    /// active or all users left the session.
    /// </summary>
    Inactive = xbox::services::multiplayer::multiplayer_session_status::inactive,

    /// <summary>
    /// The session is reserved. This means one for more users have not
    /// accepted the session invite.
    /// </summary>
    Reserved = xbox::services::multiplayer::multiplayer_session_status::reserved
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END