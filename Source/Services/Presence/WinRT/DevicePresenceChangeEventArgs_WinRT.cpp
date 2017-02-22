// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "DevicePresenceChangeEventArgs_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_BEGIN

DevicePresenceChangeEventArgs::DevicePresenceChangeEventArgs(
    _In_ xbox::services::presence::device_presence_change_event_args cppObj
    ) : 
    m_cppObj(std::move(cppObj))
{
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_END