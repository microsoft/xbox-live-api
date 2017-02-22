// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/stats_manager.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_STATISTIC_MANAGER_BEGIN

public enum class StatisticEventType
{
    LocalUserAdded = xbox::services::stats::manager::stat_event_type::local_user_added,
    LocalUserRemoved = xbox::services::stats::manager::stat_event_type::local_user_removed,
    StatisticUpdateComplete = xbox::services::stats::manager::stat_event_type::stat_update_complete
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_STATISTIC_MANAGER_END