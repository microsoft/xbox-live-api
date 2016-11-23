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

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_BEGIN

/// <summary>Defines values that indicate the title presence state for a user.</summary>
public enum class TitlePresenceState
{
    /// <summary>
    /// Indicates this is a Unknown state.
    /// </summary>
    Unknown = xbox::services::presence::title_presence_state::unknown,

    /// <summary>
    /// Indicates the user started playing the title.
    /// </summary>
    Started = xbox::services::presence::title_presence_state::started,

    /// <summary>
    /// Indicates the user ended playing the title.
    /// </summary>
    Ended = xbox::services::presence::title_presence_state::ended
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_END