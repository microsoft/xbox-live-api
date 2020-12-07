// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#if HC_PLATFORM == HC_PLATFORM_UWP || HC_PLATFORM == HC_PLATFORM_XDK || XSAPI_UNIT_TESTS
#include "social_manager_internal.h"
#endif
#include "presence_internal.h"

using namespace xbox::services;
using namespace xbox::services::presence;

uint64_t XblPresenceRecord::Xuid() const
{
    return m_xuid;
}

XblPresenceUserState XblPresenceRecord::UserState() const
{
    return m_userState;
}

const xsapi_internal_vector<XblPresenceDeviceRecord>& XblPresenceRecord::DeviceRecords() const
{
    return m_deviceRecords;
}

bool XblPresenceRecord::IsUserPlayingTitle(
    _In_ uint32_t titleId
) const
{
    if (m_userState == XblPresenceUserState::Offline || m_userState == XblPresenceUserState::Unknown)
    {
        return false;
    }

    for (const auto& deviceRecord : m_deviceRecordsInternal)
    {
        for (const auto& titleRecord : deviceRecord->TitleRecords())
        {
            if (titleRecord.titleId == titleId)
            {
                return titleRecord.titleActive;
            }
        }
    }

    return false;
}

Result<std::shared_ptr<XblPresenceRecord>> XblPresenceRecord::Deserialize(
    _In_ const JsonValue& json
)
{
    if (json.IsNull())
    {
        return Result<std::shared_ptr<XblPresenceRecord>>(nullptr);
    }

    auto presenceRecord = MakeShared<XblPresenceRecord>();

    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonXuid(json, "xuid", presenceRecord->m_xuid));
    xsapi_internal_string state;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "state", state));
    presenceRecord->m_userState = UserStateFromString(state);

    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonVector<std::shared_ptr<DeviceRecord>>(
        DeviceRecord::Deserialize,
        json,
        "devices",
         presenceRecord->m_deviceRecordsInternal,
        false
    ));

    for (const auto& deviceRecordInternal : presenceRecord->m_deviceRecordsInternal)
    {
        XblPresenceDeviceRecord deviceRecord
        {
            deviceRecordInternal->DeviceType(),
            deviceRecordInternal->TitleRecords().data(),
            deviceRecordInternal->TitleRecords().size()
        };
        presenceRecord->m_deviceRecords.push_back(std::move(deviceRecord));
    }

    return Result<std::shared_ptr<XblPresenceRecord>>(presenceRecord, S_OK);
}

XblPresenceUserState XblPresenceRecord::UserStateFromString(
    _In_ const xsapi_internal_string& value
    )
{
    if (utils::str_icmp_internal(value, "Online") == 0)
    {
        return XblPresenceUserState::Online;
    }
    else if (utils::str_icmp_internal(value, "Away") == 0)
    {
        return XblPresenceUserState::Away;
    }
    else if (utils::str_icmp_internal(value, "Offline") == 0)
    {
        return XblPresenceUserState::Offline;
    }

    return XblPresenceUserState::Unknown;
}

std::shared_ptr<RefCounter> XblPresenceRecord::GetSharedThis()
{
    return shared_from_this();
}
