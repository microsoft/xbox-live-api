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
#include "xsapi/stats_manager.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_STATISTIC_MANAGER_BEGIN

public enum class StatisticEventType
{
    LocalUserAdded = xbox::services::stats::manager::stat_event_type::local_user_added,
    LocalUserRemoved = xbox::services::stats::manager::stat_event_type::local_user_removed,
    StatisticUpdateComplete = xbox::services::stats::manager::stat_event_type::stat_update_complete
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_STATISTIC_MANAGER_END