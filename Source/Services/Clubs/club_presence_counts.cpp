// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/clubs.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_CPP_BEGIN

uint32_t club_presence_counts::total_count() const
{
    return m_totalCount;
}

uint32_t club_presence_counts::here_now() const
{
    return m_hereNow;
}

uint32_t club_presence_counts::here_today() const
{
    return m_hereToday;
}

uint32_t club_presence_counts::in_game_now() const
{
    return m_inGameNow;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_CPP_END
