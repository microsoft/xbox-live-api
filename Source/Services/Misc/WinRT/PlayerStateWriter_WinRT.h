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
#include "xsapi/player_state_writer.h"
#include "PlayerStateValue_WinRT.h"
#include "user_context.h"

#if !TV_API
#include "User_WinRT.h"
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_PLAYER_STATE_BEGIN

public ref class PlayerStateWriter sealed
{
public:
    /// <summary> 
    /// Instantiates and returns an instance of the player state writer
    /// </summary>
    static property PlayerStateWriter^ SingletonInstance
    {
        PlayerStateWriter^ get();
    }

    /// <summary> 
    /// Sets player state. Immediately applies.
    /// </summary>
    /// <return>A list of events that have happened since previous DoWork</return>
    void SetPlayerState(
        _In_ XboxLiveUser_t user,
        _In_ Windows::Foundation::Collections::IMapView<Platform::String^, PlayerStateValue^>^ playerStateList
        );

    /// <summary> 
    /// Gets the player state list
    /// </summary>
    Windows::Foundation::Collections::IMapView<Platform::String^, PlayerStateValue^>^
    GetPlayerState(
        _In_ XboxLiveUser_t user
        );

internal:
    PlayerStateWriter();
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_PLAYER_STATE_END