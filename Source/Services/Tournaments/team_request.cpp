// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/tournaments.h"
#include "utils.h"

using namespace pplx;

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_CPP_BEGIN

team_request::team_request(
    _In_ string_t organizerId,
    _In_ string_t tournamentId,
    _In_ bool filterResultsForUser
    ) :
    m_organizerId(std::move(organizerId)),
    m_tournamentId(std::move(tournamentId)),
    m_filterResultsForUser(filterResultsForUser),
    m_maxItems(0),
    m_orderBy(team_order_by::none)
{
}

bool
team_request::filter_results_for_user() const
{
    return m_filterResultsForUser;
}

const string_t&
team_request::organizer_id() const
{
    return m_organizerId;
}

const string_t&
team_request::tournament_id() const
{
    return m_tournamentId;
}

uint32_t
team_request::max_items() const
{
    return m_maxItems;
}

void 
team_request::set_max_items(
    _In_ uint32_t maxItems
    )
{
    m_maxItems = maxItems;
}

const std::vector<team_state>&
team_request::state_filter() const
{
    return m_states;
}

void 
team_request::set_state_filter(
    _In_ std::vector<team_state> states
    )
{
    m_states = std::move(states);
}

team_order_by
team_request::order_by() const
{
    return m_orderBy;
}

void 
team_request::set_order_by(
    _In_ team_order_by orderBy
    )
{
    m_orderBy = orderBy;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_CPP_END