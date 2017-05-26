// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "xsapi/multiplayer_manager.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN

perform_qos_measurements_event_args::perform_qos_measurements_event_args(
    _In_ std::map<string_t, string_t> addressDeviceTokenMap
    ):
    m_addressToDeviceTokenMap(std::move(addressDeviceTokenMap))
{
}

const std::map<string_t, string_t>&
perform_qos_measurements_event_args::connection_address_to_device_tokens() const
{
    return m_addressToDeviceTokenMap;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END