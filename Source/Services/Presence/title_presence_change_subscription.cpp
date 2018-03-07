// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/presence.h"
#include "presence_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_BEGIN

using namespace xbox::services::real_time_activity;

title_presence_change_subscription::title_presence_change_subscription(
    _In_ std::shared_ptr<title_presence_change_subscription_internal> internalObj
    ) : m_internalObj(std::move(internalObj))
{
}

DEFINE_GET_STRING(title_presence_change_subscription, xbox_user_id);
DEFINE_GET_UINT32(title_presence_change_subscription, title_id);
DEFINE_GET_ENUM_TYPE(title_presence_change_subscription, real_time_activity_subscription_state, state);
DEFINE_GET_UINT32(title_presence_change_subscription, subscription_id);

const string_t& title_presence_change_subscription::resource_uri() const
{
    return m_internalObj->resource_uri();
}

title_presence_change_subscription_internal::title_presence_change_subscription_internal(
    _In_ xsapi_internal_string xboxUserId,
    _In_ uint32_t titleId,
    _In_ xbox_live_callback<std::shared_ptr<title_presence_change_event_args_internal>> handler,
    _In_ std::function<void(const xbox::services::real_time_activity::real_time_activity_subscription_error_event_args&)> subscriptionErrorHandler
    ) :
    real_time_activity_subscription(subscriptionErrorHandler),
    m_xboxUserId(std::move(xboxUserId)),
    m_titleId(titleId),
    m_handler(handler)
{
    XSAPI_ASSERT(!m_xboxUserId.empty());
    XSAPI_ASSERT(handler != nullptr);

    stringstream_t uri;
    uri << _T("https://userpresence.xboxlive.com/users/xuid(") << utils::string_t_from_internal_string(m_xboxUserId) << _T(")/titles/") << m_titleId;

    m_resourceUri = uri.str();
}

void
title_presence_change_subscription_internal::on_subscription_created(
    _In_ uint32_t id,
    _In_ const web::json::value& data
    )
{
    real_time_activity_subscription::on_subscription_created(id, data);
    std::shared_ptr<title_presence_change_event_args_internal> presenceEventArgs;

    if (!data.is_null())
    {
        auto initialPresenceRecord = presence_record_internal::deserialize(data);
        bool isPlaying = false;

        if (!initialPresenceRecord.err())
        {
            auto& presenceDeviceRecords = initialPresenceRecord.payload()->presence_device_records();
            for (const auto& deviceRecord : presenceDeviceRecords)
            {
                for (const auto& titleRecord : deviceRecord->presence_title_records())
                {
                    if (titleRecord->title_id() == m_titleId)
                    {
                        isPlaying = true;
                        break;
                    }
                }

                if (isPlaying)
                {
                    break;
                }
            }

            presenceEventArgs = xsapi_allocate_shared<title_presence_change_event_args_internal>(
                m_xboxUserId,
                m_titleId,
                isPlaying ? title_presence_state::started : title_presence_state::ended
                );
        }
        else
        {
            if(m_subscriptionErrorHandler != nullptr)
            {
                m_subscriptionErrorHandler(
                    xbox::services::real_time_activity::real_time_activity_subscription_error_event_args(
                        *this,
                        initialPresenceRecord.err(),
                        "JSON deserialization failure"
                        )
                    );
            }

            return;
        }
    }
    else
    {
        if(m_subscriptionErrorHandler != nullptr)
        {
            m_subscriptionErrorHandler(
                xbox::services::real_time_activity::real_time_activity_subscription_error_event_args(
                    *this,
                    xbox_live_error_code::json_error,
                    "JSON not found on subscription"
                    )
                );
        }

        return;
    }

    if (m_handler != nullptr)
    {
        m_handler(presenceEventArgs);
    }
}

void
title_presence_change_subscription_internal::on_event_received(
    _In_ const web::json::value& data
    )
{
    if (m_handler != nullptr && !data.is_null())
    {
        std::error_code errc;
        auto titlePresenceValue = utils::internal_string_from_string_t(utils::extract_json_as_string(data, errc));

        title_presence_state titlePresenceState = title_presence_state::unknown;
        if (!errc)
        {
            if (utils::str_icmp(titlePresenceValue, "started") == 0)
            {
                titlePresenceState = title_presence_state::started;
            }
            else if (utils::str_icmp(titlePresenceValue, "ended") == 0)
            {
                titlePresenceState = title_presence_state::ended;
            }
        }
        else
        {
            if(m_subscriptionErrorHandler != nullptr)
            {
                m_subscriptionErrorHandler(
                    xbox::services::real_time_activity::real_time_activity_subscription_error_event_args(
                        *this,
                        xbox_live_error_code::json_error,
                        "JSON deserialization failure"
                        )
                    );
            }

            return;
        }

        auto presenceEventArgs = xsapi_allocate_shared<title_presence_change_event_args_internal>(
            m_xboxUserId,
            m_titleId,
            std::move(titlePresenceState)
            );

        m_handler(presenceEventArgs);
    }
}

const xsapi_internal_string&
title_presence_change_subscription_internal::xbox_user_id() const
{
    return m_xboxUserId;
}

uint32_t
title_presence_change_subscription_internal::title_id() const
{
    return m_titleId;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_END