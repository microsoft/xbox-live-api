// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "presence_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_BEGIN

DeviceRecord::~DeviceRecord()
{
    for (auto& titleRecord : m_titleRecords)
    {
        Delete(titleRecord.titleName);
        Delete(titleRecord.richPresenceString);

        if (titleRecord.broadcastRecord)
        {
            Delete(titleRecord.broadcastRecord->broadcastId);
            Delete(titleRecord.broadcastRecord);
        };
    }
}

DeviceRecord::DeviceRecord(const DeviceRecord& other)
    : m_deviceType{ other.m_deviceType },
    m_titleRecords{ other.m_titleRecords }
{
    for (auto& titleRecord : m_titleRecords)
    {
        titleRecord.titleName = Make(titleRecord.titleName);
        titleRecord.richPresenceString = Make(titleRecord.richPresenceString);
        titleRecord.broadcastRecord = Make<XblPresenceBroadcastRecord>(*titleRecord.broadcastRecord);
        titleRecord.broadcastRecord->broadcastId = Make(titleRecord.broadcastRecord->broadcastId);
    }
}

DeviceRecord& DeviceRecord::operator=(DeviceRecord other)
{
    std::swap(m_deviceType, other.m_deviceType);
    std::swap(m_titleRecords, other.m_titleRecords);
    return *this;
}

XblPresenceDeviceType DeviceRecord::DeviceType() const
{
    return m_deviceType;
}

const xsapi_internal_vector<XblPresenceTitleRecord>& DeviceRecord::TitleRecords() const
{
    return m_titleRecords;
}

Result<std::shared_ptr<DeviceRecord>> DeviceRecord::Deserialize(
    _In_ const JsonValue& inputJson
)
{
    if (inputJson.IsNull())
    {
        return Result<std::shared_ptr<DeviceRecord>>();
    }

    auto returnObject = MakeShared<DeviceRecord>();

    xsapi_internal_string type;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(inputJson, "type", type));
    returnObject->m_deviceType = DeviceTypeFromString(type);

    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonVector<XblPresenceTitleRecord>(
        DeserializeTitleRecord,
        inputJson, 
        "titles",
         returnObject->m_titleRecords,
        false
    ));

    return Result<std::shared_ptr<DeviceRecord>>(returnObject, S_OK);
}

Result<XblPresenceTitleRecord> DeviceRecord::DeserializeTitleRecord(_In_ const JsonValue& json)
{
    if (json.IsNull())
    {
        Result<XblPresenceTitleRecord>();
    }

    XblPresenceTitleRecord titleRecord{};

    HRESULT errc = S_OK;
    xsapi_internal_string id;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "id", id))
    titleRecord.titleId = utils::internal_string_to_uint32(id);
    xsapi_internal_string titleName;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "name", titleName));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonTimeT(json, "lastModified", titleRecord.lastModified));

    xsapi_internal_string state;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "state", state));
    titleRecord.titleActive = (!state.empty() && utils::str_icmp_internal(state, "active") == 0);
    xsapi_internal_string placement;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "placement", placement));
    titleRecord.viewState = TitleViewStateFromString(placement);

    xsapi_internal_string richPresenceString = "";
    if (json.IsObject() && json.HasMember("activity"))
    {
        const JsonValue& activityJson = json["activity"];
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(activityJson, "richPresence", richPresenceString));

        if (activityJson.IsObject() && activityJson.HasMember("broadcast"))
        {
            const JsonValue& broadcastRecordJson = activityJson["broadcast"];
            if (!broadcastRecordJson.IsNull())
            {
                auto broadcastRecordResult = DeserializeBroadcastRecord(broadcastRecordJson);
                if (SUCCEEDED(broadcastRecordResult.Hresult()))
                {
                    titleRecord.broadcastRecord = Make<XblPresenceBroadcastRecord>(broadcastRecordResult.Payload());
                }
                else
                {
                    return Result<XblPresenceTitleRecord>(broadcastRecordResult.Hresult());
                }
            }
        }
    }

    if (SUCCEEDED(errc))
    {
        titleRecord.titleName = Make(titleName);
        titleRecord.richPresenceString = Make(richPresenceString);
    }

    return Result<XblPresenceTitleRecord>(titleRecord, errc);
}

Result<XblPresenceBroadcastRecord> DeviceRecord::DeserializeBroadcastRecord(_In_ const JsonValue& json)
{
    if (json.IsNull())
    {
        return Result<XblPresenceBroadcastRecord>();
    }

    XblPresenceBroadcastRecord broadcastRecord{};

    HRESULT errc = S_OK;
    
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonStringToCharArray(json, "session", broadcastRecord.session, sizeof(broadcastRecord.session)));
    xsapi_internal_string provider;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "provider", provider));
    broadcastRecord.provider = BroadcastProviderFromString(provider);
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonInt(json, "viewers", broadcastRecord.viewerCount));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonTimeT(json, "started", broadcastRecord.startTime));
    xsapi_internal_string broadcastId;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "id", broadcastId));

    if (SUCCEEDED(errc))
    {
        broadcastRecord.broadcastId = Make(broadcastId);
    }

    return Result<XblPresenceBroadcastRecord>(broadcastRecord, errc);
}

XblPresenceTitleViewState DeviceRecord::TitleViewStateFromString(
    _In_ const xsapi_internal_string &value
)
{
    if (utils::str_icmp_internal(value, "full") == 0)
    {
        return XblPresenceTitleViewState::FullScreen;
    }
    else if (utils::str_icmp_internal(value, "fill") == 0)
    {
        return XblPresenceTitleViewState::Filled;
    }
    else if (utils::str_icmp_internal(value, "snapped") == 0)
    {
        return XblPresenceTitleViewState::Snapped;
    }
    else if (utils::str_icmp_internal(value, "background") == 0)
    {
        return XblPresenceTitleViewState::Background;
    }

    return XblPresenceTitleViewState::Unknown;
}

XblPresenceBroadcastProvider DeviceRecord::BroadcastProviderFromString(
    _In_ const xsapi_internal_string& value
)
{
    if (utils::str_icmp_internal(value, "twitch") == 0)
    {
        return XblPresenceBroadcastProvider::Twitch;
    }
    return XblPresenceBroadcastProvider::Unknown;
}

XblPresenceDeviceType DeviceRecord::DeviceTypeFromString(
    _In_ const xsapi_internal_string& value
)
{
    if (utils::str_icmp_internal(value, "MoLive") == 0)
    {
        return XblPresenceDeviceType::Windows8;
    }
    else if (utils::str_icmp_internal(value, "MCapensis") == 0)
    {
        return XblPresenceDeviceType::XboxOne;
    }
    else
    {
        // The other enums line up with strings so EnumValue to automap
        return EnumValue<XblPresenceDeviceType>(value.data());
    }
}

xsapi_internal_string DeviceRecord::DeviceTypeAsString(
    _In_ XblPresenceDeviceType deviceType
)
{
    switch (deviceType)
    {
    case XblPresenceDeviceType::Windows8:
        return "MoLive";
        
    default:
        // The other enums line up with enums so EnumName to automap
        return EnumName(deviceType);
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_END
