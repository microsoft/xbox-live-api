//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
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