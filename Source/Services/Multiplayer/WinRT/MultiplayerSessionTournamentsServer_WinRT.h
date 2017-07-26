// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "shared_macros.h"
#include "xsapi/multiplayer.h"
#include "MultiplayerSessionReference_WinRT.h"
#include "TournamentRegistrationState_WinRT.h"
#include "TournamentRegistrationReason_WinRT.h"
#include "TournamentGameResultState_WinRT.h"
#include "TournamentGameResultSource_WinRT.h"
#include "TournamentReference_WinRT.h"
#include "TournamentTeamResult_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

/// <summary>
/// Represents the server that allows multiplayer session discovery to collaborate in a matchmaking session. 
/// </summary>
public ref class MultiplayerSessionTournamentsServer sealed
{
public:
    /// <summary>
    /// The tournament reference.
    /// </summary>
    property Microsoft::Xbox::Services::Tournaments::TournamentReference^ TournamentReference
    { 
        Microsoft::Xbox::Services::Tournaments::TournamentReference^ get();
    }

    /// <summary>
    /// The teams in the tournament
    /// </summary>
    property Windows::Foundation::Collections::IMapView<Platform::String^, MultiplayerSessionReference^>^ Teams
    { 
        Windows::Foundation::Collections::IMapView<Platform::String^, MultiplayerSessionReference^>^ get();
    }

    /// <summary>
    /// The tournament registration state of the team.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(RegistrationState, registration_state, Microsoft::Xbox::Services::Tournaments::TournamentRegistrationState);

    /// <summary>
    /// The tournament registration reason for the certain state.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(RegistrationReason, registration_reason, Microsoft::Xbox::Services::Tournaments::TournamentRegistrationReason);

    /// <summary>
    /// The next tournament game start time.
    /// </summary>
    DEFINE_PROP_GET_DATETIME_OBJ(NextGameStartTime, next_game_start_time);

    /// <summary>
    /// The next tournament game session reference.
    /// </summary>
    property MultiplayerSessionReference^ NextGameSessionRef { MultiplayerSessionReference^ get(); }

    /// <summary>
    /// The last tournament game end time.
    /// </summary>
    DEFINE_PROP_GET_DATETIME_OBJ(LastGameEndTime, last_game_end_time);

    /// <summary>
    /// The last game's state for the tournament.
    /// </summary>
    property Microsoft::Xbox::Services::Tournaments::TournamentTeamResult^ LastTeamResult 
    { 
        Microsoft::Xbox::Services::Tournaments::TournamentTeamResult^ get(); 
    }

    /// <summary>
    /// The source for the last game's state of the tournament.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(LastGameResultSource, last_game_result_source, Microsoft::Xbox::Services::Tournaments::TournamentGameResultSource);

internal:
    MultiplayerSessionTournamentsServer(_In_ xbox::services::multiplayer::multiplayer_session_tournaments_server cppObj);

private:
    xbox::services::multiplayer::multiplayer_session_tournaments_server m_cppObj;
    MultiplayerSessionReference^ m_nextGameSessionRef;
    Microsoft::Xbox::Services::Tournaments::TournamentReference^ m_tournamentRef;
    Microsoft::Xbox::Services::Tournaments::TournamentTeamResult^ m_lastTeamResult;
    Platform::Collections::Map<Platform::String^, MultiplayerSessionReference^>^ m_teams;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END