//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#include "xsapi/player_state_writer.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PLAYER_STATE_CPP_BEGIN

std::shared_ptr<player_state_writer> player_state_writer::get_singleton_instance()
{
    static std::shared_ptr<player_state_writer> playerStateWriter;
    if (playerStateWriter == nullptr)
    {
        playerStateWriter = std::make_shared<player_state_writer>();
    }

    return playerStateWriter;
}

xbox_live_result<void>
player_state_writer::set_player_state(
    _In_ xbox_live_user_t user,
    _In_ const std::unordered_map<string_t, player_state_value>& playerStateList
    )
{
    m_currentState[user->xbox_user_id()] = playerStateList;

    return xbox_live_result<void>();
}

xbox_live_result<void>
player_state_writer::get_player_state(
    _In_ xbox_live_user_t user,
    _Inout_ std::unordered_map<string_t, player_state_value>& playerStateList
    )
{
    auto currentStateIter = m_currentState.find(user->xbox_user_id());
    if (currentStateIter == m_currentState.end())
    {
        return xbox_live_result<void>(xbox_live_error_code::invalid_argument, "user not in map");
    }

    playerStateList = currentStateIter->second;

    return xbox_live_result<void>();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PLAYER_STATE_CPP_END