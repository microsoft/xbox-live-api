// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "user_statistics_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_CPP_BEGIN

StatisticChangeSubscription::StatisticChangeSubscription(
    _In_ uint64_t xuid,
    _In_ String scid,
    _In_ String statisticName,
    _In_ std::shared_ptr<UserStatisticsService> statisticsService
) noexcept :
    m_xuid{ xuid },
    m_scid{ std::move(scid) },
    m_statisticName{ std::move(statisticName) },
    m_statisticsService{ statisticsService }
{
    Stringstream uriPath;
    uriPath << "https://userstats.xboxlive.com/users/xuid(" << m_xuid << ")/scids/" << m_scid << "/stats/" << m_statisticName;
    m_resourceUri = uriPath.str();
}

void StatisticChangeSubscription::OnSubscribe(
    _In_ const JsonValue& data
) noexcept
{
    // Payload format http://xboxwiki/wiki/RTA%3ASUBSCRIBE#Subscribing_to_Stats
    // [<API_ID>, <SUB_ID>, { "name": "Kills", "type" : "Integer", "value": 219 }]

    if (data.IsNull())
    {
        LOGS_ERROR << __FUNCTION__ << ": RTA payload unexpectedly null";
        return;
    }
    else if (auto statisticsService{ m_statisticsService.lock() })
    {
        String name;
        String value;
        JsonUtils::ExtractJsonString(data, "name", name);
        assert(name == m_statisticName);
        JsonUtils::ExtractJsonString(data, "type", m_statisticType);

        if (data.IsObject() && data.HasMember("value"))
        {
            const JsonValue& field = data["value"];
            if (field.IsInt())
            {
                Stringstream stream;
                stream << field.GetInt();
                value = stream.str();
            }
            else if (field.IsInt64())
            {
                Stringstream stream;
                stream << field.GetInt64();
                value = stream.str();
            }
            else if (field.IsUint())
            {
                Stringstream stream;
                stream << field.GetUint();
                value = stream.str();
            }
            else if (field.IsUint64())
            {
                Stringstream stream;
                stream << field.GetUint64();
                value = stream.str();
            }
            else if (field.IsDouble())
            {
                Stringstream stream;
                stream << field.GetDouble();
                value = stream.str();
            }
            else if (field.IsBool())
            {
                Stringstream stream;
                stream << field.GetBool();
                value = stream.str();
            }
            else if (field.IsString())
            {
                value = field.GetString();
            }
        }

        statisticsService->HandleStatisticChanged(StatisticChangeEventArgs
            {
                m_xuid,
                m_scid,
                m_statisticName,
                m_statisticType,
                std::move(value)
            });
    }
}

void StatisticChangeSubscription::OnEvent(
    _In_ const JsonValue& data
) noexcept
{
    // Payload format http://xboxwiki/wiki/RTA:EVENT#Stats
    // [<API_ID>, <SUB_ID>, "Value"]

    if (data.IsNull())
    {
        LOGS_ERROR << __FUNCTION__ << ": RTA payload unexpectedly null";
    }
    else if (auto statisticsService{ m_statisticsService.lock() })
    {
        statisticsService->HandleStatisticChanged(StatisticChangeEventArgs
            {
                m_xuid,
                m_scid,
                m_statisticName,
                m_statisticType,
                JsonUtils::SerializeJson(data)
            });
    }
}

StatisticChangeEventArgs::StatisticChangeEventArgs(
    _In_ uint64_t _xboxUserId,
    _In_ const String& _serviceConfigurationId,
    _In_ String _statisticName,
    _In_ String _statisticType,
    _In_ String _value
) noexcept :
    XblStatisticChangeEventArgs{ _xboxUserId },
    m_statisticName{ std::move(_statisticName) },
    m_statisticType{ std::move(_statisticType) },
    m_value{ std::move(_value) }
{
    utils::strcpy(serviceConfigurationId, sizeof(serviceConfigurationId), _serviceConfigurationId.data());
    latestStatistic = XblStatistic
    {
        m_statisticName.data(),
        m_statisticType.data(),
        m_value.data()
    };
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_CPP_END