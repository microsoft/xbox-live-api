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

NAMESPACE_MICROSOFT_XBOX_SERVICES_MATCHMAKING_BEGIN

/// <summary>
/// Defines values that indicate if a match ticket is for a new
/// game session or an existing session.
/// </summary>
public enum class PreserveSessionMode
{
    /// <summary>
    /// The server returned an unrecognized response.
    /// </summary>
    Unknown = xbox::services::matchmaking::preserve_session_mode::unknown,

    /// <summary>
    /// Always use an existing game session. This is for matching more players
    /// for a game session that is already created or in progress.
    /// </summary>
    Always = xbox::services::matchmaking::preserve_session_mode::always,

    /// <summary>
    /// Never use an existing game session. This is for matching players
    /// for a new game session.
    /// </summary>
    Never = xbox::services::matchmaking::preserve_session_mode::never
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MATCHMAKING_END