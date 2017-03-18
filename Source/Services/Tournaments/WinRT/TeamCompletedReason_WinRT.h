// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once 

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

/// <summary>
/// 
/// </summary>
public enum class TeamCompletedReason
{
    /// <summary>
    /// The team was rejected.
    /// </summary>
    Rejected = xbox::services::tournaments::team_completed_reason::rejected,

    /// <summary>
    /// The team was eliminated.
    /// </summary>
    Eliminated = xbox::services::tournaments::team_completed_reason::eliminated,

    /// <summary>
    /// The team was evicted.
    /// </summary>
    Evicted = xbox::services::tournaments::team_completed_reason::evicted,

    /// <summary>
    /// The team has successfully completed the tournament.
    /// </summary>
    Completed = xbox::services::tournaments::team_completed_reason::completed

};

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END