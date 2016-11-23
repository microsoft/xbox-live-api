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
/// Defines values that indicate how visible a session is.
/// </summary>
public enum class MultiplayerSessionVisibility
{
    /// <summary>
    /// Unknown.
    /// </summary>
    Unknown = xbox::services::multiplayer::multiplayer_session_visibility::unknown,

    /// <summary>
    /// Ignore the SessionVisibility filter. 
    /// </summary>
    Any = xbox::services::multiplayer::multiplayer_session_visibility::any,

    /// <summary>
    /// The session is private and it's not visible to other users who
    /// aren't in the session. Attempting to join a private session causes the server to return HTTP_E_STATUS_FORBIDDEN.
    /// </summary>
    Private = xbox::services::multiplayer::multiplayer_session_visibility::private_session,

    /// <summary>
    /// The session is visible to other users who aren't in the session, but the session is read-only to them and they can't join.
    /// Attempting to join a visible session causes the service to return HTTP_E_STATUS_FORBIDDEN.
    /// </summary>
    Visible = xbox::services::multiplayer::multiplayer_session_visibility::visible,

    /// <summary>
    /// The session is full and cannot be joined by anyone.  
    /// Joining an open but full session causes the service to return HTTP_E_STATUS_BAD_REQUEST (400).
    /// </summary>
    Full = xbox::services::multiplayer::multiplayer_session_visibility::full,

    /// <summary>
    /// The session is open and can be joined by anyone.
    /// </summary>
    Open = xbox::services::multiplayer::multiplayer_session_visibility::open
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END