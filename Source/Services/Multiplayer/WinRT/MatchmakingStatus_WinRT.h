// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN
/// <summary>
/// Defines values that indicate the current status of the matchmaking operation.
/// </summary>

public enum class MatchmakingStatus
{
    /// <summary>
    /// The server returned an unrecognized response.
    /// </summary>
    Unknown = xbox::services::multiplayer::matchmaking_status::unknown,

    /// <summary>
    /// Indicates that the matchmaking search is not specified.  This status is optional and requires the clientMatchmaking capability.
    /// </summary>
    None = xbox::services::multiplayer::matchmaking_status::none,

    /// <summary>
    /// Indicates that the matchmaking search is still searching.
    /// </summary>
    Searching = xbox::services::multiplayer::matchmaking_status::searching,

    /// <summary>
    /// Indicates that the matchmaking search has expired.
    /// </summary>
    Expired = xbox::services::multiplayer::matchmaking_status::expired,

    /// <summary>
    /// Indicates that the matchmaking search has found a session.
    /// </summary>
    Found = xbox::services::multiplayer::matchmaking_status::found,

    /// <summary>
    /// Indicates that the matchmaking search has been canceled.
    /// </summary>
    Canceled = xbox::services::multiplayer::matchmaking_status::canceled
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END