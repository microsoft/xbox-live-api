// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/stats_manager.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_STATISTIC_MANAGER_BEGIN

/// <summary> 
/// Indicates the type of statistic change event.
/// </summary>
public enum class StatisticEventType
{
    /// <summary> 
    /// A local user has been added
    /// </summary>
    LocalUserAdded = xbox::services::stats::manager::stat_event_type::local_user_added,

    /// <summary> 
    /// A local user has been removed
    /// </summary>
    LocalUserRemoved = xbox::services::stats::manager::stat_event_type::local_user_removed,

    /// <summary> 
    /// stat has been updated
    /// </summary>
    StatisticUpdateComplete = xbox::services::stats::manager::stat_event_type::stat_update_complete,

    /// <summary> 
    /// cast event args to leaderboard_result_event_args
    /// </summary>
    GetLeaderboardComplete = xbox::services::stats::manager::stat_event_type::get_leaderboard_complete
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_STATISTIC_MANAGER_END