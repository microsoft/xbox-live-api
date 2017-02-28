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
#include "xsapi/leaderboard.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_BEGIN

leaderboard_query::leaderboard_query() :
    m_skipResultToMe(false),
    m_skipResultToRank(0),
    m_maxItems(0),
    m_order(sort_order::ascending)
{
}

void leaderboard_query::set_skip_result_to_me(_In_ bool skipResultToMe)
{
    m_skipResultToMe = skipResultToMe;
}

void leaderboard_query::set_skip_result_to_rank(_In_ uint32_t skipResultToRank)
{
    m_skipResultToRank = skipResultToRank;
}

void leaderboard_query::set_max_items(_In_ uint32_t maxItems)
{
    m_maxItems = maxItems;
}

void leaderboard_query::set_order(_In_ sort_order order)
{
    m_order = order;
}

bool leaderboard_query::skip_result_to_me() const
{
    return m_skipResultToMe;
}

uint32_t leaderboard_query::skip_result_to_rank() const
{
    return m_skipResultToRank;
}

uint32_t leaderboard_query::max_items() const
{
    return m_maxItems;
}

sort_order leaderboard_query::order() const
{
    return m_order;
}

const string_t& leaderboard_query::continuation_token() const
{
    return m_continuationToken;
}

void leaderboard_query::_Set_continuation_token(_In_ const string_t& continuationToken)
{
    m_continuationToken = continuationToken;
}

void leaderboard_query::_Set_stat_name(_In_ const string_t& statName)
{
    m_statName = statName;
}

void leaderboard_query::_Set_social_group(_In_ const string_t& socialGroup)
{
    m_socialGroup = socialGroup;
}

const string_t& leaderboard_query::stat_name() const
{
    return m_statName;
}

const string_t& leaderboard_query::social_group() const
{
    return m_socialGroup;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_END