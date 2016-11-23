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