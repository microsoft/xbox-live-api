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