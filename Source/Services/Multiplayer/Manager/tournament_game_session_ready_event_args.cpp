// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "xsapi/multiplayer_manager.h"

using namespace xbox::services::tournaments;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN

tournament_game_session_ready_event_args::tournament_game_session_ready_event_args(
    _In_ utility::datetime startTime
    ):
    m_startTime(std::move(startTime))
{
}

const utility::datetime&
tournament_game_session_ready_event_args::start_time() const
{
    return m_startTime;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END