// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "taskargs.h"
#include "xsapi/achievements_c.h"

using namespace xbox::services::achievements;

struct get_next_taskargs : public taskargs_with_payload<XSAPI_ACHIEVEMENTS_RESULT*>
{
    XSAPI_ACHIEVEMENTS_RESULT* achievementsResult;
    uint32_t maxItems;
};

struct achievements_taskargs
{
    XSAPI_XBOX_LIVE_CONTEXT* pXboxLiveContext;
};

struct update_achievement_taskargs : public achievements_taskargs, public taskargs
{
    string_t xboxUserId;
    uint32_t* titleId;
    string_t serviceConfigurationId;
    string_t achievementId;
    uint32_t percentComplete;
};

struct get_achievement_for_title_id_taskargs : public achievements_taskargs, public taskargs_with_payload<XSAPI_ACHIEVEMENTS_RESULT*>
{
    string_t xboxUserId;
    uint32_t titleId;
    achievement_type type;
    bool unlockedOnly;
    achievement_order_by orderBy;
    uint32_t skipItems;
    uint32_t maxItems;
};

struct get_achievement_taskargs : public achievements_taskargs, public taskargs_with_payload<XSAPI_ACHIEVEMENT*>
{
    string_t xboxUserId;
    string_t serviceConfigurationId;
    string_t achievementId;
};