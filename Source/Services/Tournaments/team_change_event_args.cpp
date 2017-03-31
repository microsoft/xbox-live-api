// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/tournaments.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_CPP_BEGIN

team_change_event_args::team_change_event_args()
{
}

team_change_event_args::team_change_event_args(
    _In_ const string_t& organizerId,
    _In_ const string_t& tournamentId,
    _In_ const string_t& teamId
    ) :
    m_organizerId(std::move(organizerId)),
    m_tournamentId(std::move(tournamentId)),
    m_teamId(std::move(teamId))
{
}

const string_t&
team_change_event_args::organizer_id() const
{
    return m_organizerId;
}

const string_t&
team_change_event_args::tournament_id() const
{
    return m_tournamentId;
}

const string_t&
team_change_event_args::team_id() const
{
    return m_teamId;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_CPP_END