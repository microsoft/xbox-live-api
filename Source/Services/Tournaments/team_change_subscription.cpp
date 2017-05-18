// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/tournaments.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_CPP_BEGIN

team_change_subscription::team_change_subscription(
    _In_ const string_t& organizerId,
    _In_ const string_t& tournamentId,
    _In_ const string_t& teamId,
    _In_ std::function<void(const team_change_event_args&)> handler,
    _In_ std::function<void(const xbox::services::real_time_activity::real_time_activity_subscription_error_event_args&)> subscriptionErrorHandler
    ) :
    real_time_activity_subscription(subscriptionErrorHandler),
    m_organizerId(std::move(organizerId)),
    m_tournamentId(std::move(tournamentId)),
    m_teamId(std::move(teamId)),
    m_teamChangeHandler(handler)
{
    XSAPI_ASSERT(handler != nullptr);

    stringstream_t uriPath;
    uriPath << _T("https://tournamentshub.xboxlive.com/tournaments/") << m_organizerId << _T("/") << m_tournamentId << _T("/teams/") << m_teamId;
    m_resourceUri = uriPath.str();
}

void
team_change_subscription::on_subscription_created(
    _In_ uint32_t id,
    _In_ const web::json::value& data
)
{
    real_time_activity_subscription::on_subscription_created(
        id,
        data
        );

    if (m_teamChangeHandler)
    {
        m_teamChangeHandler(
            team_change_event_args(
                m_organizerId,
                m_tournamentId,
                m_teamId
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
team_change_subscription::on_event_received(
    _In_ const web::json::value& data
    )
{
    // The team subscription has no payload. 
    // Clients should get the tournament or team from the Hub when they receive a tap.
    UNREFERENCED_PARAMETER(data);

    team_change_event_args teamChangeEventArgs;

    if (m_teamChangeHandler)
    {
        m_teamChangeHandler(
            team_change_event_args(
                m_organizerId,
                m_tournamentId,
                m_teamId
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
team_change_subscription::organizer_id() const
{
    return m_organizerId;
}

const string_t&
team_change_subscription::tournament_id() const
{
    return m_tournamentId;
}

const string_t&
team_change_subscription::team_id() const
{
    return m_teamId;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_CPP_END