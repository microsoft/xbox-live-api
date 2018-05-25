// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "achievements/achievements_internal.h"

struct xbl_achievements_result
{
    xbl_achievements_result(std::shared_ptr<xbox::services::achievements::achievements_result_internal> _internalResult);
    xbl_achievements_result(std::shared_ptr<xbox::services::achievements::achievement_internal> _internalAchievement);
    virtual ~xbl_achievements_result();

    xsapi_internal_vector<XblAchievement> items;
    std::shared_ptr<xbox::services::achievements::achievements_result_internal> internalResult;
    std::shared_ptr<xbox::services::achievements::achievement_internal> internalAchievement;
    std::atomic<int> refCount;
};