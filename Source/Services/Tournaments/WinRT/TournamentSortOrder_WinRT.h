// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once 

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

/// <summary>
/// The order in which to sort the results.
/// </summary>
public enum class TournamentSortOrder
{
    /// <summary>
    /// No sort order provided.
    /// </summary>
    None = xbox::services::tournaments::tournament_sort_order::none,

    /// <summary>
    /// Sorts smaller items first.
    /// </summary>
    Ascending = xbox::services::tournaments::tournament_sort_order::ascending,

    /// <summary>
    /// Sorts larger items first.
    /// </summary>
    Descending = xbox::services::tournaments::tournament_sort_order::descending
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END