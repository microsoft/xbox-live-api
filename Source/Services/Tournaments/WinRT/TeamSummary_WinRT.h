// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/tournaments.h"
#include "TeamState_WinRT.h"
#include "TeamCompletedReason_WinRT.h"
#include "CurrentMatchMetadata_WinRT.h"
#include "PreviousMatchMetadata_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

/// <summary>
/// Represents details of the team participating in the tournament.
/// </summary>
public ref class TeamSummary sealed
{
public:
    /// <summary>
    /// The ID of the team. It is an opaque string specified by the tournament organizer.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Id, id);

    /// <summary>
    /// The state of the team.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(TeamState, state, Microsoft::Xbox::Services::Tournaments::TeamState);

    /// <summary>
    /// The team's final rank within the tournament, if it is available. If missing, the rank is not available.
    /// </summary>
    DEFINE_PROP_GET_OBJ(Ranking, ranking, uint64);

internal:
    TeamSummary(
        _In_ xbox::services::tournaments::team_summary cppObj
        );

    const xbox::services::tournaments::team_summary& GetCppObj() const;

private:
    xbox::services::tournaments::team_summary m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END
