// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "shared_macros.h"
#include "xsapi/leaderboard.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_BEGIN

leaderboard_row::leaderboard_row(
    _In_ string_t gamertag,
    _In_ string_t xboxUserId,
    _In_ double percentile,
    _In_ uint32_t rank,
    _In_ std::vector<string_t> columnValues,
    _In_ string_t metadata) :
    m_gamertag(std::move(gamertag)),
    m_xboxUserId(std::move(xboxUserId)),
    m_percentile(percentile),
    m_rank(rank),
    m_columnValues(std::move(columnValues))
{
    if(!metadata.empty())
    {
        m_metadata = web::json::value::parse(metadata);
    }
}

const string_t& leaderboard_row::gamertag() const
{
    return m_gamertag;
}

const string_t& leaderboard_row::xbox_user_id() const
{
    return m_xboxUserId;
}

double leaderboard_row::percentile() const
{
    return m_percentile;
}

uint32_t leaderboard_row::rank() const
{
    return m_rank;
}

const std::vector<string_t>& leaderboard_row::column_values() const
{
    return m_columnValues;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_END