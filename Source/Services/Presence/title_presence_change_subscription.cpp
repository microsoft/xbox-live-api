// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/presence.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_BEGIN

title_presence_change_subscription::title_presence_change_subscription(
    _In_ string_t xboxUserId,
    _In_ uint32_t titleId,
    _In_ std::function<void(const title_presence_change_event_args&)> handler,
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
    uri << _T("https://userpresence.xboxlive.com/users/xuid(") << m_xboxUserId << _T(")/titles/") << m_titleId;

    m_resourceUri = uri.str();
}

void
title_presence_change_subscription::on_subscription_created(
    _In_ uint32_t id,
    _In_ const web::json::value& data
    )
{
    real_time_activity_subscription::on_subscription_created(id, data);
    title_presence_change_event_args presenceEventArgs;

    if (!data.is_null())
    {
        auto initialPresenceRecord = presence_record::_Deserialize(data);
        bool isPlaying = false;

        if (!initialPresenceRecord.err())
        {
            auto& presenceDeviceRecords = initialPresenceRecord.payload().presence_device_records();
            for (const auto& deviceRecord : presenceDeviceRecords)
            {
                for (const auto& titleRecord : deviceRecord.presence_title_records())
                {
                    if (titleRecord.title_id() == m_titleId)
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

            presenceEventArgs = title_presence_change_event_args(
                title_presence_change_event_args(
                    m_xboxUserId,
                    m_titleId,
                    isPlaying ? title_presence_state::started : title_presence_state::ended
                    )
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
title_presence_change_subscription::on_event_received(
    _In_ const web::json::value& data
    )
{
    if (m_handler && !data.is_null())
    {
        std::error_code errc;
        auto titlePresenceValue = utils::extract_json_as_string(data, errc);

        title_presence_state titlePresenceState = title_presence_state::unknown;
        if (!errc)
        {
            if (utils::str_icmp(titlePresenceValue, _T("started")) == 0)
            {
                titlePresenceState = title_presence_state::started;
            }
            else if (utils::str_icmp(titlePresenceValue, _T("ended")) == 0)
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

        auto presenceEventArgs = title_presence_change_event_args(
            m_xboxUserId,
            m_titleId,
            std::move(titlePresenceState)
            );

        m_handler(presenceEventArgs);
    }
}

const string_t&
title_presence_change_subscription::xbox_user_id() const
{
    return m_xboxUserId;
}

uint32_t
title_presence_change_subscription::title_id() const
{
    return m_titleId;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_END