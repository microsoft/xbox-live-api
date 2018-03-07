// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/presence.h"
#include "presence_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_BEGIN

device_presence_change_event_args::device_presence_change_event_args(
    _In_ std::shared_ptr<device_presence_change_event_args_internal> internalObj
    ) :
    m_internalObj(std::move(internalObj))
{
}

DEFINE_GET_STRING(device_presence_change_event_args, xbox_user_id);
DEFINE_GET_ENUM_TYPE(device_presence_change_event_args, presence_device_type, device_type);
DEFINE_GET_BOOL(device_presence_change_event_args, is_user_logged_on_device);

device_presence_change_event_args_internal::device_presence_change_event_args_internal() :
    m_deviceType(presence_device_type::unknown),
    m_isUserLoggedOnDevice(false)
{
}

device_presence_change_event_args_internal::device_presence_change_event_args_internal(
    _In_ xsapi_internal_string xboxUserId,
    _In_ presence_device_type deviceType,
    _In_ bool isUserLoggedOnDevice
    ) :
    m_xboxUserId(std::move(xboxUserId)),
    m_deviceType(deviceType),
    m_isUserLoggedOnDevice(isUserLoggedOnDevice)
{
}

const xsapi_internal_string& 
device_presence_change_event_args_internal::xbox_user_id() const
{
    return m_xboxUserId;
}

presence_device_type 
device_presence_change_event_args_internal::device_type() const
{
    return m_deviceType;
}

bool 
device_presence_change_event_args_internal::is_user_logged_on_device() const
{
    return m_isUserLoggedOnDevice;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_END