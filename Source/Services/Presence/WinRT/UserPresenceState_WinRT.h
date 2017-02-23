// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/presence.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_BEGIN

/// <summary>Defines values that indicate the user presence state on Xbox Live.</summary>
public enum class UserPresenceState
{
    /// <summary>The state is unknown.</summary>
    Unknown = xbox::services::presence::user_presence_state::unknown,

    /// <summary>User is signed in to Xbox LIVE and active in a title.</summary>
    Online = xbox::services::presence::user_presence_state::online,

    /// <summary>User is signed-in to Xbox LIVE, but inactive in all titles.</summary>
    Away = xbox::services::presence::user_presence_state::away,

    /// <summary>User is not signed in to Xbox LIVE.</summary>
    Offline = xbox::services::presence::user_presence_state::offline
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_END