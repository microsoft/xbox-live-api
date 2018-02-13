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
#include "xsapi/stats_manager.h"
#include "SortOrder_WinRT.h"
#include "StatisticEventArgs_WinRT.h"
#include "../../../Leaderboard/WinRT/LeaderboardResult.h"
#include "Macros_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_STATISTIC_MANAGER_BEGIN

/// <summary>
/// Represents the results of a leaderboard request.
/// </summary>
public ref class LeaderboardResultEventArgs sealed : StatisticEventArgs
{
public:
    /// <summary> 
    /// Gets the leaderboard result from a leaderboard request.
    /// </summary>
    property Leaderboard::LeaderboardResult^ Result
    {
        Leaderboard::LeaderboardResult^ get();
    };

internal:
    LeaderboardResultEventArgs(
        _In_ std::shared_ptr<xbox::services::stats::manager::leaderboard_result_event_args> cppObj
        );

private:
    std::shared_ptr<xbox::services::stats::manager::leaderboard_result_event_args> m_cppObj;
    Leaderboard::LeaderboardResult^ m_result;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_STATISTIC_MANAGER_END