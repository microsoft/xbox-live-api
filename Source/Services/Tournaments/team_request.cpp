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
#include "xsapi/tournaments.h"
#include "utils.h"

using namespace pplx;

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_CPP_BEGIN

team_request::team_request(
    _In_ string_t organizerId,
    _In_ string_t tournamentId
    ) :
    m_organizerId(std::move(organizerId)),
    m_tournamentId(std::move(tournamentId)),
    m_maxItems(0),
    m_orderBy(team_order_by::none)
{
}

const string_t&
team_request::_Organizer_id() const
{
    return m_organizerId;
}

const string_t&
team_request::_Tournament_id() const
{
    return m_tournamentId;
}

uint32_t
team_request::_Max_items() const
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
team_request::_Team_states() const
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
team_request::_Order_by() const
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