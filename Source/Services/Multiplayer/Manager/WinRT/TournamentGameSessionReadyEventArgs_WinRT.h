#pragma once
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
#include "MultiplayerEventArgs_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_BEGIN

/// <summary>
/// Contains information for an event that indicates when the tournament registration state changes.
/// </summary>
public ref class TournamentGameSessionReadyEventArgs sealed : MultiplayerEventArgs
{
public:

    /// <summary>
    /// Game's start time for the tournament.
    /// </summary>
    DEFINE_PTR_PROP_GET_DATETIME_OBJ(StartTime, start_time);

internal:
    TournamentGameSessionReadyEventArgs(
        _In_ std::shared_ptr<xbox::services::multiplayer::manager::tournament_game_session_ready_event_args> cppObj
        );

    std::shared_ptr<xbox::services::multiplayer::manager::tournament_game_session_ready_event_args> GetCppObj() const;

private:
    std::shared_ptr<xbox::services::multiplayer::manager::tournament_game_session_ready_event_args> m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_END
