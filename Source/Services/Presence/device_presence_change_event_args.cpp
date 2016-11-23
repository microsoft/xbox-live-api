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