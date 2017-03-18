// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once 

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

/// <summary>
/// The property used to order results by for retrieving a list of teams.
/// </summary>
public enum class TeamOrderBy
{
    /// <summary>
    /// No order provided.
    /// </summary>
    None = xbox::services::tournaments::team_order_by::none,

    /// <summary>
    /// Order by name.
    /// </summary>
    Name = xbox::services::tournaments::team_order_by::name,

    /// <summary>
    /// Order by ranking.
    /// </summary>
    Ranking = xbox::services::tournaments::team_order_by::ranking
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END