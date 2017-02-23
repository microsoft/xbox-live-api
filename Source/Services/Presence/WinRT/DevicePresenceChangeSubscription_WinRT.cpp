// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "DevicePresenceChangeSubscription_WinRT.h"

using namespace Microsoft::Xbox::Services::RealTimeActivity;
NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_BEGIN

DevicePresenceChangeSubscription::DevicePresenceChangeSubscription(
    _In_ std::shared_ptr<xbox::services::presence::device_presence_change_subscription> cppObj
    ) :
    m_cppObj(cppObj)
{
}

RealTimeActivitySubscriptionState
DevicePresenceChangeSubscription::State::get()
{
    return static_cast<RealTimeActivitySubscriptionState>(m_cppObj->state());
}

std::shared_ptr<xbox::services::presence::device_presence_change_subscription> 
DevicePresenceChangeSubscription::GetCppObj() const
{
    return m_cppObj;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_END