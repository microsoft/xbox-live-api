// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/clubs.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_CPP_BEGIN

const std::vector<string_t>& clubs_owned_result::club_ids() const
{
    return m_clubIds;
}

uint32_t clubs_owned_result::remaining_clubs() const
{
    return m_remainingClubs;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_CPP_END