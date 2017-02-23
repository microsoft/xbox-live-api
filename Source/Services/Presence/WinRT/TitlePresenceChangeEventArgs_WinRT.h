// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once 
#include "xsapi/presence.h"
#include "PresenceRecord_WinRT.h"
#include "TitlePresenceState_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_BEGIN

/// <summary>
/// Contains information about a title presence change event.
/// </summary>
public ref class TitlePresenceChangeEventArgs sealed
{
public:
    /// <summary>
    /// The Xbox user ID for the user present on the device.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(XboxUserId, xbox_user_id);

    /// <summary>
    /// The ID for the title that uses the subscription.
    /// </summary>
    DEFINE_PROP_GET_OBJ(TitleId, title_id, uint32);

    /// <summary>
    /// The new title presence state.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(TitleState, title_state, TitlePresenceState);

internal:
    TitlePresenceChangeEventArgs(_In_ xbox::services::presence::title_presence_change_event_args cppObj);

private:
    xbox::services::presence::title_presence_change_event_args m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_END