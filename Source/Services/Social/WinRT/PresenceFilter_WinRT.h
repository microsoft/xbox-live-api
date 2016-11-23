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
#include "xsapi/social_manager.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_BEGIN

/// <summary>
/// The filter level of information
/// Title will only show users associated with a particular title
///</summary>
public enum class PresenceFilter
{
    /// <summary>Unknown</summary>
    Unknown = xbox::services::social::manager::presence_filter::unknown,

    /// <summary>Is currently playing current title and is online</summary>
    TitleOnline = xbox::services::social::manager::presence_filter::title_online,

    /// <summary>Has played this title and is offline</summary>
    TitleOffline = xbox::services::social::manager::presence_filter::title_offline,

    /// <summary>Everyone currently online</summary>
    AllOnline = xbox::services::social::manager::presence_filter::all_online,

    /// <summary>Everyone currently offline</summary>
    AllOffline = xbox::services::social::manager::presence_filter::all_offline,

    /// <summary>Everyone who has played or is playing the title</summary>
    AllTitle = xbox::services::social::manager::presence_filter::all_title,

    /// <summary>Everyone</summary>
    All = xbox::services::social::manager::presence_filter::all
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_END