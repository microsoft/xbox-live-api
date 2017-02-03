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
#include "PlayerStateWriter_WinRT.h"
#include "user_context.h"
#include "xsapi/player_state_writer.h"

using namespace xbox::services::player_state;

NAMESPACE_MICROSOFT_XBOX_SERVICES_PLAYER_STATE_BEGIN

PlayerStateWriter^ PlayerStateWriter::SingletonInstance::get()
{
    static std::mutex s_singletonLock;
    std::lock_guard<std::mutex> guard(s_singletonLock);

    static PlayerStateWriter^ instance;
    if (instance == nullptr)
    {
        instance = ref new PlayerStateWriter();
    }
    
    return instance;
}

PlayerStateWriter::PlayerStateWriter()
{
}

void
PlayerStateWriter::SetPlayerState(
    _In_ XboxLiveUser_t user,
    _In_ Windows::Foundation::Collections::IMapView<Platform::String^, PlayerStateValue^>^ playerStateList
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(user);
    THROW_INVALIDARGUMENT_IF_NULL(playerStateList);

    std::unordered_map<string_t, player_state_value> playerStateListCpp;
    playerStateListCpp.reserve(playerStateList->Size);
    for (auto state : playerStateList)
    {
        playerStateListCpp[state->Key->Data()] = state->Value->GetCppObj();
    }

    player_state_writer::get_singleton_instance()->set_player_state(
        xbox::services::user_context::user_convert(user),
        playerStateListCpp
        );
}

Windows::Foundation::Collections::IMapView<Platform::String^, PlayerStateValue^>^
PlayerStateWriter::GetPlayerState(
    _In_ XboxLiveUser_t user
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(user);

    std::unordered_map<string_t, player_state_value> valueMapCpp;
    player_state_writer::get_singleton_instance()->get_player_state(
        xbox::services::user_context::user_convert(user),
        valueMapCpp
        );

    Platform::Collections::UnorderedMap<Platform::String^, PlayerStateValue^>^ valueMap;
    for (auto& state : valueMapCpp)
    {
        valueMap->Insert(ref new Platform::String(state.first.c_str()), ref new PlayerStateValue(state.second));
    }

    return valueMap->GetView();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PLAYER_STATE_END