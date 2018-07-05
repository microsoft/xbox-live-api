// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "shared_macros.h"
#include "xsapi/types.h"
#include "xsapi/leaderboard.h"
#include "xsapi-c/leaderboard_c.h"
#include <cstdint>

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_BEGIN

struct leaderboard_global_query
{
    xsapi_internal_string scid;
    xsapi_internal_string name;
    uint64_t xuid;
    XblSocialGroupType socialGroup;
    xsapi_internal_vector<xsapi_internal_string> columns;
};

struct leaderboard_social_query
{
    uint64_t xuid;
    xsapi_internal_string scid;
    xsapi_internal_string statName;
    XblSocialGroupType socialGroup;
    uint32_t skipToRank;
    sort_order sortOrder;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_END