// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "events_service_xsapi.h"
#include "xbox_live_app_config_internal.h"
#include "cll/BasicJsonWriter.h"
#include "cll/ConversionHelpers.h"

using namespace xbox::services;

NAMESPACE_MICROSOFT_XBOX_SERVICES_EVENTS_CPP_BEGIN

#define EVENT_NAME_PREFIX "Microsoft.XboxLive.T"

Event::Event(
    _In_ uint64_t xuid,
    _In_ const xsapi_internal_string& eventName,
    _In_ const JsonValue& dimensions,
    _In_ const JsonValue& measurements,
    _In_ xbox::services::datetime timestamp
) :
    m_xuid{ xuid },
    m_eventName{ eventName },
    m_fullEventName{ EVENT_NAME_PREFIX + std::to_string(AppConfig::Instance()->TitleId()) + "." + m_eventName.data() },
    m_timestamp{ std::move(timestamp) }
{

    JsonUtils::CopyFrom(m_dimensions, dimensions);
    JsonUtils::CopyFrom(m_measurements, measurements);

    bool state;

    cll::BasicJsonWriter::StartObject(m_data, state);
    cll::BasicJsonWriter::WriteField(m_data, state, "baseType", "Microsoft.XboxLive.InGame");
    cll::BasicJsonWriter::StartStruct(m_data, state, "baseData");

    cll::BasicJsonWriter::WriteField(m_data, state, "name", eventName.data());
    cll::BasicJsonWriter::WriteField(m_data, state, "serviceConfigId", AppConfig::Instance()->Scid().data());
    cll::BasicJsonWriter::WriteField(m_data, state, "titleId", std::to_string(AppConfig::Instance()->TitleId()));
    cll::BasicJsonWriter::WriteField(m_data, state, "userId", utils::uint64_to_internal_string(xuid).data());

    if (dimensions.IsObject())
    {
        cll::BasicJsonWriter::StartStruct(m_data, state, "properties");

        for (const auto& pair : dimensions.GetObject())
        {
#if _WIN32
            cll::BasicJsonWriter::WriteSerializedStruct(m_data, state, pair.name.GetString(), JsonUtils::SerializeJson(pair.value).c_str());
#else
            cll::BasicJsonWriter::WriteSerializedStruct(m_data, state, pair.name.GetString(), JsonUtils::SerializeJson(pair.value).c_str());
#endif
        }
        cll::BasicJsonWriter::EndStruct(m_data, state); // properties
    }

    if (measurements.IsObject())
    {
        cll::BasicJsonWriter::StartStruct(m_data, state, "measurements");
        for (const auto& pair : measurements.GetObject())
        {
#if _WIN32
            cll::BasicJsonWriter::WriteSerializedStruct(m_data, state, pair.name.GetString(), JsonUtils::SerializeJson(pair.value).c_str());
#else
            cll::BasicJsonWriter::WriteSerializedStruct(m_data, state, pair.name.GetString(), JsonUtils::SerializeJson(pair.value).c_str());
#endif
        }
        cll::BasicJsonWriter::EndStruct(m_data, state); // measurements
    }

    cll::BasicJsonWriter::EndStruct(m_data, state); // baseData
    cll::BasicJsonWriter::EndObject(m_data, state);
}

Event::Event(const Event& other)
{
    m_xuid = other.m_xuid;
    m_eventName = other.m_eventName;
    m_fullEventName = other.m_fullEventName;
    JsonUtils::CopyFrom(m_dimensions, other.m_dimensions);
    JsonUtils::CopyFrom(m_measurements, other.m_measurements);
    m_timestamp = other.m_timestamp;
    m_data = other.m_data;
}

Result<Event> Event::Deserialize(
    _In_ const xsapi_internal_string& inputData
)
{
    // tab-separated (0x0A)
    // xuid | event name | timestamp | dimensions json | measurements json

    auto parts = utils::string_split_internal(inputData, '\t');

    if (parts.size() < 5)
    {
        return Result<Event>{ Event{}, E_FAIL };
    }

    auto xuid = utils::internal_string_to_uint64(parts[0]);
    auto& eventName = parts[1];
    auto timestamp = xbox::services::datetime::from_string(parts[2], xbox::services::datetime::ISO_8601);

    JsonDocument dimensionsJson;
    dimensionsJson.Parse(parts[3].data());
    JsonDocument measurementsJson;
    measurementsJson.Parse(parts[4].data());

    if (dimensionsJson.IsNull() || measurementsJson.IsNull())
    {
        return Result<Event>{ Event{}, E_FAIL };
    }

    return Result<Event>(Event{ xuid, eventName, dimensionsJson, measurementsJson, timestamp });
}

const std::string& Event::Data() const
{
    return m_data;
}

const std::string& Event::FullEventName() const
{
    return m_fullEventName;
}

const xbox::services::datetime& Event::Timestamp() const
{
    return m_timestamp;
}

std::string Event::SerializeFieldValue(
    _In_ const JsonValue& value
)
{
    xsapi_internal_string valueStr;
    if (value.IsString())
    {
        valueStr = value.GetString();
    }
    else
    {
        valueStr = JsonUtils::SerializeJson(value);
    }
    return valueStr.data();
}

xsapi_internal_string Event::Serialize() const
{
    // serialization format
    // tab-separated (0x0A)
    // xuid | event name | timestamp | dimensions json | measurements json

    char seperator{ '\t' };

    xsapi_internal_stringstream ss;

    ss << m_xuid << seperator;
    ss << m_eventName << seperator;
    ss << m_timestamp.to_string(xbox::services::datetime::ISO_8601) << seperator;
    ss << JsonUtils::SerializeJson(m_dimensions) << seperator;
    ss << JsonUtils::SerializeJson(m_measurements);

    return ss.str();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_EVENTS_CPP_END
