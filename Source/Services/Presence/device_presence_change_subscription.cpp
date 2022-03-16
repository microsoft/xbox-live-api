// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "presence_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_BEGIN

DevicePresenceChangeSubscription::DevicePresenceChangeSubscription(
    _In_ uint64_t xuid,
    _In_ std::shared_ptr<PresenceService> presenceService
) noexcept
    : m_xuid{ xuid },
    m_presenceService{ presenceService }
{
    Stringstream uri;
    uri << "https://userpresence.xboxlive.com/users/xuid(" << m_xuid << ")/devices";
    m_resourceUri = uri.str();
}

void DevicePresenceChangeSubscription::OnSubscribe(
    const JsonValue& data
) noexcept
{
    if (data.IsNull())
    {
        LOGS_ERROR << __FUNCTION__ << ": RTA payload unexpectedly null";
        return;
    }

    auto presenceService{ m_presenceService.lock() };
    if (presenceService)
    {
        auto deserializationResult = XblPresenceRecord::Deserialize(data);
        if (Succeeded(deserializationResult))
        {
            for (const auto& deviceRecord : deserializationResult.Payload()->DeviceRecords())
            {
                presenceService->HandleDevicePresenceChanged(m_xuid, deviceRecord.deviceType, true);
            }
        }
    }
}

void DevicePresenceChangeSubscription::OnEvent(
    _In_ const JsonValue& data
) noexcept
{
    auto presenceService{ m_presenceService.lock() };
    if (presenceService && data.IsString())
    {
        auto devicePresenceValues = utils::string_split_internal(String{ data.GetString() }, ':');
        if (devicePresenceValues.size() == 2)
        {
            presenceService->HandleDevicePresenceChanged(
                m_xuid,
                DeviceRecord::DeviceTypeFromString(devicePresenceValues[0]),
                utils::str_icmp_internal(devicePresenceValues[1], "true") == 0
            );
        }
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_END