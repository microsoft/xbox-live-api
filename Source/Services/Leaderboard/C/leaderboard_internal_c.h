// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

//#include "leaderboard/leaderboard_internal.h"

struct xbl_leaderboard_result
{
    xbl_leaderboard_result(std::shared_ptr<xbox::services::leaderboard::leaderboard_result_internal> _internalResult);
    virtual ~xbl_leaderboard_result();

    std::shared_ptr<xbox::services::leaderboard::leaderboard_result_internal> internalResult;
    std::atomic<int> refCount;

    xsapi_internal_vector<XblLeaderboardColumn> columns;
    xsapi_internal_vector<XblLeaderboardRow> rows;
};

