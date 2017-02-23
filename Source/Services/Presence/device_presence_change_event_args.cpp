// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/presence.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_BEGIN

device_presence_change_event_args::device_presence_change_event_args() : 
    m_deviceType(presence_device_type::unknown),
    m_isUserLoggedOnDevice(false)
{
}

device_presence_change_event_args::device_presence_change_event_args(
    _In_ string_t xboxUserId,
    _In_ presence_device_type deviceType,
    _In_ bool isUserLoggedOnDevice
    ) :
    m_xboxUserId(std::move(xboxUserId)),
    m_deviceType(deviceType),
    m_isUserLoggedOnDevice(isUserLoggedOnDevice)
{
}

const string_t& 
device_presence_change_event_args::xbox_user_id() const
{
    return m_xboxUserId;
}

presence_device_type 
device_presence_change_event_args::device_type() const
{
    return m_deviceType;
}

bool 
device_presence_change_event_args::is_user_logged_on_device() const
{
    return m_isUserLoggedOnDevice;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_END