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
#include "xsapi/tournaments.h"
#include "TournamentState_WinRT.h"
#include "TeamOrderBy_WinRT.h"
#include "TeamState_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

/// <summary>
/// Gets tournaments based on the configuration of this request.
/// </summary>
public ref class TeamRequest sealed
{
public:
    /// <summary>Creates a team_request object.</summary>
    /// <param name="organizerId">The ID of the tournament organizer.</param>
    /// <param name="tournamentId">The ID of the tournament.</param>
    TeamRequest(
        _In_ Platform::String^ organizerId,
        _In_ Platform::String^ tournamentId
        );

    /// <summary>
    /// The maximum number of items to return.
    /// </summary>
    /// <param name="maxItems">The maximum number of items to return.</param>
    DEFINE_PROP_SET_OBJ(MaxItems, max_items, uint32);

    /// <summary>
    /// Filter results based on the multiple states of the team.
    /// </summary>
    /// <param name="state">The team states to filter on.</param>
    property Windows::Foundation::Collections::IVectorView<TeamState>^ StateFilter
    {
        void set(_In_ Windows::Foundation::Collections::IVectorView<TeamState>^ states);
    }

    /// <summary>
    /// Filter results based on the order specified.
    /// </summary>
    /// <param name="orderBy">The field used to order results.</param>
    DEFINE_PROP_SET_ENUM_OBJ(OrderBy, order_by, TeamOrderBy, xbox::services::tournaments::team_order_by);

internal:
    TeamRequest(
        _In_ xbox::services::tournaments::team_request cppObj
        );

    const xbox::services::tournaments::team_request& GetCppObj() const;
private:
    xbox::services::tournaments::team_request m_cppObj;
    Windows::Foundation::Collections::IVectorView<TeamState>^ m_teamStates;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END
