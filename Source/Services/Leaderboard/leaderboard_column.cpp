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
#include "shared_macros.h"
#include "xsapi/leaderboard.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_BEGIN

leaderboard_column::leaderboard_column(
    _In_ string_t display_name,
    _In_ string_t statName,
    _In_ leaderboard_stat_type stat_type
    ) :
    m_displayName(std::move(display_name)),
    m_statName(std::move(statName)),
    m_stat_type(std::move(stat_type))
{
}

const string_t& leaderboard_column::display_name() const
{
    return m_displayName;
}

const string_t& leaderboard_column::stat_name() const
{
    return m_statName;
}

leaderboard_stat_type leaderboard_column::stat_type() const
{
    return m_stat_type;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_END