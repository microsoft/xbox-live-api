// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/tournaments.h"
#include "TournamentState_WinRT.h"
#include "TournamentOrderBy_WinRT.h"
#include "TournamentSortOrder_WinRT.h"
#include "TournamentState_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

/// <summary>
/// Gets tournaments based on the configuration of this request.
/// </summary>
public ref class TournamentRequest sealed
{
public:
    /// <summary>Creates a tournament_request object.</summary>
    /// <param name="filterResultsForUser">Filter results to only tournaments where this user is participating.</param>
    TournamentRequest(
        _In_ bool filterResultsForUser
        );

    /// <summary>
    /// The maximum number of items to return.
    /// </summary>
    DEFINE_PROP_GETSET_OBJ(MaxItems, max_items, uint32);

    /// <summary>
    /// Filter results based on the multiple states of the tournament.
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<TournamentState>^ StateFilter
    {
        Windows::Foundation::Collections::IVectorView<TournamentState>^ get();
        void set(_In_ Windows::Foundation::Collections::IVectorView<TournamentState>^ states);
    }

    /// <summary>
    /// The property used to order results.
    /// </summary>
    DEFINE_PROP_GETSET_ENUM_OBJ(OrderBy, order_by, TournamentOrderBy, xbox::services::tournaments::tournament_order_by);

    /// <summary>
    ///The order in which to sort the results.
    /// </summary>
    DEFINE_PROP_GETSET_ENUM_OBJ(SortOrder, sort_order, TournamentSortOrder, xbox::services::tournaments::tournament_sort_order);

    /// <summary>
    /// The organizer of the tournament.
    /// </summary>
    DEFINE_PROP_GETSET_STR_OBJ(OrganizerId, organizer_id);

internal:
    TournamentRequest(
        _In_ xbox::services::tournaments::tournament_request cppObj
        );

    const xbox::services::tournaments::tournament_request& GetCppObj() const;
private:
    xbox::services::tournaments::tournament_request m_cppObj;
    Windows::Foundation::Collections::IVectorView<TournamentState>^ m_tournamentStates;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END
