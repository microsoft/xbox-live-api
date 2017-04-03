// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/tournaments.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_CPP_BEGIN

tournament_change_subscription::tournament_change_subscription(
    _In_ const string_t& organizerId,
    _In_ const string_t& tournamentId,
    _In_ std::function<void(const tournament_change_event_args&)> handler,
    _In_ std::function<void(const xbox::services::real_time_activity::real_time_activity_subscription_error_event_args&)> subscriptionErrorHandler
    ) :
    real_time_activity_subscription(subscriptionErrorHandler),
    m_organizerId(std::move(organizerId)),
    m_tournamentId(std::move(tournamentId)),
    m_tournamentChangeHandler(handler)
{
    XSAPI_ASSERT(handler != nullptr);

    stringstream_t uriPath;
    uriPath << _T("https://tournamentshub.xboxlive.com/tournaments/") << m_organizerId << _T("/") << m_tournamentId;
    m_resourceUri = uriPath.str();
}

void
tournament_change_subscription::on_subscription_created(
    _In_ uint32_t id,
    _In_ const web::json::value& data
    )
{
    real_time_activity_subscription::on_subscription_created(
        id,
        data
    );

    if (m_tournamentChangeHandler)
    {
        m_tournamentChangeHandler(
            tournament_change_event_args(
                m_organizerId,
                m_tournamentId
            )
        );
    }
    else
    {
        if (m_subscriptionErrorHandler != nullptr)
        {
            m_subscriptionErrorHandler(
                xbox::services::real_time_activity::real_time_activity_subscription_error_event_args(
                    *this,
                    xbox_live_error_code::json_error,
                    "JSON not found on subscription"
                    )
                );
        }
    }
}

void
tournament_change_subscription::on_event_received(
    _In_ const web::json::value& data
    )
{
    // The team subscription has no payload. 
    // Clients should get the tournament or team from the Hub when they receive a tap.
    UNREFERENCED_PARAMETER(data);

    tournament_change_event_args tournamentChangeEventArgs;

    if (m_tournamentChangeHandler)
    {
        m_tournamentChangeHandler(
            tournament_change_event_args(
                m_organizerId,
                m_tournamentId
                )
            );
    }
    else
    {
        if (m_subscriptionErrorHandler != nullptr)
        {
            m_subscriptionErrorHandler(
                xbox::services::real_time_activity::real_time_activity_subscription_error_event_args(
                    *this,
                    xbox_live_error_code::json_error,
                    "JSON not found on change event"
                )
            );
        }
    }
}

const string_t&
tournament_change_subscription::organizer_id() const
{
    return m_organizerId;
}

const string_t&
tournament_change_subscription::tournament_id() const
{
    return m_tournamentId;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_CPP_END