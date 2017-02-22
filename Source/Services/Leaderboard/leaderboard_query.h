// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "shared_macros.h"
#include "xsapi/types.h"
#include <cstdint>

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_BEGIN

struct leaderboard_global_query
{
    string_t scid;
    string_t name;
    string_t xuid;
    string_t socialGroup;
    std::vector<string_t> columns;
};

struct leaderboard_social_query
{
    string_t xuid;
    string_t scid;
    string_t statName;
    string_t socialGroup;
    string_t sortOrder;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_END