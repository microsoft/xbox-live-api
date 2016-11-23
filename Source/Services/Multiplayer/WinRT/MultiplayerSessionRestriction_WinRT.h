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
/// Defines values used to indicate restrictions on the users who can join a session. 
/// </summary>

public enum class MultiplayerSessionRestriction
{
    /// <summary>
    /// The unrecognized restriction type. This value should never be used because it causes an exception to be thrown.
    /// </summary>
    Unknown = xbox::services::multiplayer::multiplayer_session_restriction::unknown,

    /// <summary>
    /// Default value, no restriction.
    /// </summary>
    None = xbox::services::multiplayer::multiplayer_session_restriction::none,

    /// <summary>
    /// Only the following users can read/join the session: 
    /// * members of the session
    /// * Users that have been invited to the session
    /// * Users that are signed in to the same console as a someone already in the session
    ///
    /// This means that only users whose token's DeviceId matches someone else already in the session and have "active" set to true
    /// can access the session.
    /// </summary>
    Local = xbox::services::multiplayer::multiplayer_session_restriction::local,

    /// <summary>
    /// If "followed", only local users (as defined above) and users who are followed by an existing (not reserved) member of the session can join without a reservation.
    /// </summary>
    Followed = xbox::services::multiplayer::multiplayer_session_restriction::followed,

};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END