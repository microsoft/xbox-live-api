// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "LeaderboardColumn.h"
#include "xsapi/leaderboard.h"

using namespace pplx;
using namespace Platform;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_BEGIN

LeaderboardColumn::LeaderboardColumn(
    _In_ xbox::services::leaderboard::leaderboard_column cppObj) :
    m_cppObj(std::move(cppObj))
{
}

Windows::Foundation::PropertyType
LeaderboardColumn::StatisticType::get()
{
    using namespace xbox::services::leaderboard;

    switch (m_cppObj.stat_type())
    {
    case leaderboard_stat_type::stat_uint64:
        return PropertyType::UInt64;
    case leaderboard_stat_type::stat_double:
        return PropertyType::Double;
    case leaderboard_stat_type::stat_string:
        return PropertyType::String;
    case leaderboard_stat_type::stat_datetime:
        return PropertyType::DateTime;
    case leaderboard_stat_type::stat_other:
    default:
        return PropertyType::OtherType;
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_END