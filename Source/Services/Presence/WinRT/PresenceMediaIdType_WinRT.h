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
#include "xsapi/presence.h"
NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_BEGIN

/// <summary> Defines values that indicate the media id types for media presence data.</summary>
public enum class PresenceMediaIdType
{
    /// <summary>Unknown media Id.</summary>
    Unknown = xbox::services::presence::presence_media_id_type::unknown,

    /// <summary>Bing media Id.</summary>
    Bing = xbox::services::presence::presence_media_id_type::bing,

    /// <summary>MediaProvider media Id.</summary>
    MediaProvider = xbox::services::presence::presence_media_id_type::media_provider
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_END