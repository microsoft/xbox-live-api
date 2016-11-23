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
#include "PerformQosMeasurementsEventArgs_WinRT.h"

using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Microsoft::Xbox::Services::System;
using namespace xbox::services::multiplayer::manager;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_BEGIN

PerformQosMeasurementsEventArgs::PerformQosMeasurementsEventArgs(
    _In_ std::shared_ptr<perform_qos_measurements_event_args> cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
    XSAPI_ASSERT(m_cppObj != nullptr);

    m_addressToDeviceTokenMap = ref new Platform::Collections::Map<Platform::String^, Platform::String^>();
    for (const auto& address : m_cppObj->connection_address_to_device_tokens())
    {
        m_addressToDeviceTokenMap->Insert(ref new Platform::String(address.first.c_str()), ref new Platform::String(address.second.c_str()));
    }
}

std::shared_ptr<perform_qos_measurements_event_args>
PerformQosMeasurementsEventArgs::GetCppObj() const
{
    return m_cppObj;
}

Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^>^ 
PerformQosMeasurementsEventArgs::AddressToDeviceTokens::get()
{
    return m_addressToDeviceTokenMap->GetView();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_END