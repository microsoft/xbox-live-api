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
#include "xsapi/multiplayer.h"
#include "multiplayer_internal.h"

using namespace xbox::services::real_time_activity;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

multiplayer_subscription::multiplayer_subscription(
    _In_ const std::function<void(const multiplayer_session_change_event_args&)>& multiplayerSessionChangeHandler,
    _In_ const std::function<void()>& multiplayerSubscriptionLostHandler,
    _In_ const std::function<void(const xbox::services::real_time_activity::real_time_activity_subscription_error_event_args&)>& subscriptionErrorHandler
    ) :
    real_time_activity_subscription(subscriptionErrorHandler),
    m_multiplayerSessionChangeHandler(multiplayerSessionChangeHandler),
    m_multiplayerSubscriptionLostHandler(multiplayerSubscriptionLostHandler)
{
    XSAPI_ASSERT(m_multiplayerSessionChangeHandler != nullptr);
    XSAPI_ASSERT(m_multiplayerSubscriptionLostHandler != nullptr);

    m_resourceUri = _T("https://sessiondirectory.xboxlive.com/connections/");
}

const string_t&
multiplayer_subscription::rta_connection_id() const
{
    return m_mpConnectionId;
}

void
multiplayer_subscription::on_subscription_created(
    _In_ uint32_t id, 
    _In_ const web::json::value& data
    )
{
    xbox_live_result<string_t> result;
    if (!data.is_null())
    {
        real_time_activity_subscription::on_subscription_created(id, data);
        std::error_code errc;

        bool connectionWasEmpty = m_mpConnectionId.empty();
        m_mpConnectionId = utils::extract_json_string(data, _T("ConnectionId"), errc, true);

        result = xbox_live_result<string_t>(
            m_mpConnectionId,
            errc
            );

        if (errc)
        {
            result._Set_err_message("JSON deserialization failed");
            return;
        }

        if (connectionWasEmpty)
        {
            m_resourceUri.append(m_mpConnectionId);
        }
    }
    else
    {
        result = xbox_live_result<string_t>(
            xbox_live_error_code::json_error,
            "Data not found on subscription"
            );
    }

    task.set(result);
}

void
multiplayer_subscription::on_event_received(
    _In_ const web::json::value& data
    )
{
    std::error_code errc;
    web::json::array shoulderTaps = utils::extract_json_as_array(utils::extract_json_field(data, _T("shoulderTaps"), errc, true), errc);
    if (errc && m_subscriptionErrorHandler)
    {
        m_subscriptionErrorHandler(
            xbox::services::real_time_activity::real_time_activity_subscription_error_event_args(
                *this,
                xbox_live_error_code::json_error,
                "JSON deserialization failure"
                )
            );
    }

    for (const auto& tapValue : shoulderTaps)
    {
        string_t resourceName = utils::extract_json_string(tapValue, _T("resource"), true);
        std::vector<string_t> nameComponents = utils::string_split(resourceName, '~');

        if (nameComponents.size() != 3)
        {
            if(m_subscriptionErrorHandler != nullptr)
            {
                m_subscriptionErrorHandler(
                    xbox::services::real_time_activity::real_time_activity_subscription_error_event_args(
                        *this,
                        xbox_live_error_code::json_error,
                        "Resource has too many values"
                        )
                    );
            }

            continue;
        }

        multiplayer_session_reference sessionRef(
            nameComponents[0],
            nameComponents[1],
            nameComponents[2]
            );

        uint64_t changeNumber = utils::extract_json_int(tapValue, _T("changeNumber"), false);
        string_t branch = utils::extract_json_string(tapValue, _T("branch"), false);

        multiplayer_session_change_event_args changeEventArgs(
            sessionRef,
            branch,
            changeNumber
            );

        if (m_multiplayerSessionChangeHandler != nullptr)
        {
            m_multiplayerSessionChangeHandler(changeEventArgs);
        }
    }
}

void
multiplayer_subscription::_Set_state(
    _In_ real_time_activity_subscription_state newState
    )
{
    // if state is change from pending subscribe to closed, subscription failed.
    if (m_state == real_time_activity_subscription_state::pending_subscribe && newState == real_time_activity_subscription_state::closed)
    {
        task.set(xbox_live_result<string_t>(xbox_live_error_code::runtime_error, "multiplayer subscription failure"));
    }

    if (m_state != real_time_activity_subscription_state::closed && newState == real_time_activity_subscription_state::closed)
    {
        // Because the title could disable subscriptions and destruct the XboxLiveContext right after, 
        // multiplayer_service may no longer exist.
        if (m_multiplayerSubscriptionLostHandler != nullptr)
        {
            auto subscriptionHandler = m_multiplayerSubscriptionLostHandler;
            pplx::create_task([subscriptionHandler]()
            {
                try
                {
                    subscriptionHandler();
                }
                catch (...)
                {
                }
            });
        }
    }

    real_time_activity_subscription::_Set_state(newState);
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END