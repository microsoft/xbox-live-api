// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "MultiplayerEventArgs_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_BEGIN

/// <summary>
/// Triggered when a new game has been scheduled. Game session is where the match will actually be played.
/// </summary>
public ref class TournamentGameSessionReadyEventArgs sealed : MultiplayerEventArgs
{
public:

    /// <summary>
    /// Game's start time for the tournament.
    /// </summary>
    DEFINE_PTR_PROP_GET_DATETIME_OBJ(StartTime, start_time);

internal:
    /// <summary>
    /// Internal function.
    /// </summary>
    TournamentGameSessionReadyEventArgs(
        _In_ std::shared_ptr<xbox::services::multiplayer::manager::tournament_game_session_ready_event_args> cppObj
        );

    std::shared_ptr<xbox::services::multiplayer::manager::tournament_game_session_ready_event_args> GetCppObj() const;

private:
    std::shared_ptr<xbox::services::multiplayer::manager::tournament_game_session_ready_event_args> m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_END
