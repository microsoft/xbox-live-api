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

tournament_request::tournament_request(
    _In_ bool filterResultsForUser
    ) :
    m_filterResultsForUser(filterResultsForUser),
    m_maxItems(0),
    m_sortOrder(tournament_sort_order::none),
    m_orderBy(tournament_order_by::none)
{
}

bool 
tournament_request::_Filter_results_for_user() const
{
    return m_filterResultsForUser;
}

uint32_t
tournament_request::_Max_items() const
{
    return m_maxItems;
}

void 
tournament_request::set_max_items(
    _In_ uint32_t maxItems
    )
{
    m_maxItems = maxItems;
}

const std::vector<tournament_state>&
tournament_request::_Tournament_states() const
{
    return m_states;
}

void 
tournament_request::set_state_filter(
    _In_ std::vector<tournament_state> states
    )
{
    m_states = std::move(states);
}

tournament_order_by 
tournament_request::_Order_by() const
{
    return m_orderBy;
}

void 
tournament_request::set_order_by(
    _In_ tournament_order_by orderBy
    )
{
    m_orderBy = orderBy;
}

tournament_sort_order 
tournament_request::_Sort_order() const
{
    return m_sortOrder;
}

void 
tournament_request::set_sort_order(
    _In_ tournament_sort_order order
    )
{
    m_sortOrder = order;
}

const string_t& 
tournament_request::_Organizer_id() const
{
    return m_organizerId;
}

void 
tournament_request::set_organizer_id(
    _In_ const string_t& organizerId
    )
{
    m_organizerId = std::move(organizerId);
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_CPP_END