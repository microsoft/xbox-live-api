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