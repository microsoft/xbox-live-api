// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/presence.h"
#include "presence_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_BEGIN

using namespace xbox::services::real_time_activity;

device_presence_change_subscription::device_presence_change_subscription(
    _In_ std::shared_ptr<device_presence_change_subscription_internal> internalObj
    ) :
    m_internalObj(internalObj)
{
}

DEFINE_GET_STRING(device_presence_change_subscription, xbox_user_id);
DEFINE_GET_ENUM_TYPE(device_presence_change_subscription, real_time_activity_subscription_state, state);
DEFINE_GET_UINT32(device_presence_change_subscription, subscription_id);

const string_t& device_presence_change_subscription::resource_uri() const
{
    return m_internalObj->resource_uri();
}

device_presence_change_subscription_internal::device_presence_change_subscription_internal(
    _In_ xsapi_internal_string xboxUserId,
    _In_ xbox_live_callback<std::shared_ptr<device_presence_change_event_args_internal>> handler,
    _In_ std::function<void(const xbox::services::real_time_activity::real_time_activity_subscription_error_event_args&)> subscriptionErrorHandler
    ) :
    real_time_activity_subscription(subscriptionErrorHandler),
    m_xboxUserId(std::move(xboxUserId)),
    m_devicePresenceChangeHandler(handler)
{
    XSAPI_ASSERT(!m_xboxUserId.empty());
    XSAPI_ASSERT(handler != nullptr);

    stringstream_t uri;
    uri << _T("https://userpresence.xboxlive.com/users/xuid(") << utils::string_t_from_internal_string(m_xboxUserId) << _T(")/devices");

    m_resourceUri = uri.str();
}

void
device_presence_change_subscription_internal::on_subscription_created(
    _In_ uint32_t id, 
    _In_ const web::json::value& data
    )
{
    real_time_activity_subscription::on_subscription_created(id, data);
    xbox_live_result<device_presence_change_event_args> devicePresenceChangeArgs;

    if (!data.is_null())
    {
        auto initialPresenceRecord = presence_record_internal::deserialize(data);

        if (!initialPresenceRecord.err())
        {
            if (m_devicePresenceChangeHandler != nullptr)
            {
                for (const auto& deviceRecord : initialPresenceRecord.payload()->presence_device_records())
                {
                    auto deviceType = deviceRecord->device_type();

                    m_devicePresenceChangeHandler(
                        xsapi_allocate_shared<device_presence_change_event_args_internal>(
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
device_presence_change_subscription_internal::on_event_received(
    _In_ const web::json::value& data
    )
{
    std::error_code errc;
    auto dataAsString = utils::internal_string_from_string_t(utils::extract_json_as_string(data, errc));
    if (!errc)
    {
        auto devicePresenceValues = utils::string_split(dataAsString, ':');

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
                xsapi_allocate_shared<device_presence_change_event_args_internal>(
                    m_xboxUserId,
                    presence_device_record_internal::convert_string_to_presence_device_type(devicePresenceValues[0]),
                    utils::str_icmp(devicePresenceValues[1], "true") == 0
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

const xsapi_internal_string&
device_presence_change_subscription_internal::xbox_user_id() const
{
    return m_xboxUserId;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_END