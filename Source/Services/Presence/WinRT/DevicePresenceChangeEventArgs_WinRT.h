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
#include "PresenceDeviceType_WinRT.h"
#include "PresenceRecord_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_BEGIN

/// <summary>
/// Contains information about a device presence change event.
/// </summary>
public ref class DevicePresenceChangeEventArgs sealed
{
public:
    /// <summary>
    /// The Xbox user ID for the user present on the device.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(XboxUserId, xbox_user_id);

    /// <summary>
    /// The type of device related to the presence information.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(DeviceType, device_type, PresenceDeviceType);

    /// <summary>
    /// Indicates whether the user is logged on to the device.
    /// </summary>
    DEFINE_PROP_GET_OBJ(IsUserLoggedOnDevice, is_user_logged_on_device, bool);

internal:
    DevicePresenceChangeEventArgs(_In_ xbox::services::presence::device_presence_change_event_args cppObj);

private:
    _In_ xbox::services::presence::device_presence_change_event_args m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_END