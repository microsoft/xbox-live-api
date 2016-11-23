//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#include "xsapi/presence.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_BEGIN

device_presence_change_subscription::device_presence_change_subscription(
    _In_ string_t xboxUserId,
    _In_ std::function<void(const device_presence_change_event_args&)> handler,
    _In_ std::function<void(const xbox::services::real_time_activity::real_time_activity_subscription_error_event_args&)> subscriptionErrorHandler
    ) :
    real_time_activity_subscription(subscriptionErrorHandler),
    m_xboxUserId(std::move(xboxUserId)),
    m_devicePresenceChangeHandler(handler)
{
    XSAPI_ASSERT(!m_xboxUserId.empty());
    XSAPI_ASSERT(handler != nullptr);

    stringstream_t uri;
    uri << _T("https://userpresence.xboxlive.com/users/xuid(") << m_xboxUserId << _T(")/devices");

    m_resourceUri = uri.str();
}

void
device_presence_change_subscription::on_subscription_created(
    _In_ uint32_t id, 
    _In_ const web::json::value& data
    )
{
    real_time_activity_subscription::on_subscription_created(id, data);
    xbox_live_result<device_presence_change_event_args> devicePresenceChangeArgs;

    if (!data.is_null())
    {
        auto initialPresenceRecord = presence_record::_Deserialize(data);

        if (!initialPresenceRecord.err())
        {
            if (m_devicePresenceChangeHandler != nullptr)
            {
                for (const auto& deviceRecord : initialPresenceRecord.payload().presence_device_records())
                {
                    auto deviceType = deviceRecord.device_type();

                    m_devicePresenceChangeHandler(
                        device_presence_change_event_args(
                            m_xboxUserId,
                            deviceType,
                            true
                            )
                        );
                }
            }
        }
        else
        {
            if (m_subscriptionErrorHandler != nullptr)
            {
                m_subscriptionErrorHandler(
                    xbox::services::real_time_activity::real_time_activity_subscription_error_event_args(
                        *this,
                        xbox_live_error_code::json_error,
                        "JSON Deserialization Failure"
                        )
                    );
            }
        }
    }
    else
    {
        if (m_subscriptionErrorHandler != nullptr)
        {
            m_subscriptionErrorHandler(
                xbox::services::real_time_activity::real_time_activity_subscription_error_event_args(
                    *this,
                    xbox_live_error_code::json_error,
                    "JSON Not Found"
                    )
                );
        }
    }
}

void
device_presence_change_subscription::on_event_received(
    _In_ const web::json::value& data
    )
{
    std::error_code errc;
    auto dataAsString = utils::extract_json_as_string(data, errc);
    xbox_live_result<device_presence_change_event_args> eventArgs;
    if (!errc)
    {
        std::vector<string_t> devicePresenceValues = utils::string_split(dataAsString, ':');

        if (devicePresenceValues.size() != 2)
        {
            if (m_subscriptionErrorHandler != nullptr)
            {
                m_subscriptionErrorHandler(
                    xbox::services::real_time_activity::real_time_activity_subscription_error_event_args(
                        *this,
                        xbox_live_error_code::json_error,
                        "JSON deserialization failed"
                        )
                    );
            }
            
            return;
        }

        if (m_devicePresenceChangeHandler != nullptr)
        {
            m_devicePresenceChangeHandler(
                device_presence_change_event_args(
                    m_xboxUserId,
                    presence_device_record::_Convert_string_to_presence_device_type(devicePresenceValues[0]),
                    utils::str_icmp(devicePresenceValues[1], _T("true")) == 0
                    )
                );
        }
    }
    else
    {
        if (m_subscriptionErrorHandler != nullptr)
        {
            LOG_DEBUG("device_presence_change_subscription error happened");
            m_subscriptionErrorHandler(
                xbox::services::real_time_activity::real_time_activity_subscription_error_event_args(
                    *this,
                    xbox_live_error_code::json_error,
                    "JSON deserialization failed"
                    )
                );
        }
    }
}

const string_t&
device_presence_change_subscription::xbox_user_id() const
{
    return m_xboxUserId;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_END