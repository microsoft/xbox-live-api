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

#include "xsapi/multiplayer.h"
#include "TournamentGameResultState_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

/// <summary>
/// Represents a teams result in a tournament
/// </summary>
public ref class TournamentTeamResult sealed
{
public:
    TournamentTeamResult();
    /// <summary>
    /// The current tournament game's state for a team.
    /// </summary>
    DEFINE_PROP_GETSET_ENUM_OBJ(State, state, Microsoft::Xbox::Services::Tournaments::TournamentGameResultState, xbox::services::tournaments::tournament_game_result_state);

    /// <summary>
    /// The ranking for the current tournament game's state for a team.
    /// </summary>
    DEFINE_PROP_GETSET_OBJ(Ranking, ranking, uint64);

internal:
    TournamentTeamResult(
        _In_ xbox::services::tournaments::tournament_team_result cppObj
        );

    xbox::services::tournaments::tournament_team_result GetCppObj() const;
private:

    xbox::services::tournaments::tournament_team_result m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END
