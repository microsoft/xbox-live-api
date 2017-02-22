// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/stats_manager.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_STATISTIC_MANAGER_BEGIN

public enum class StatisticDataType
{
    Undefined = xbox::services::stats::manager::stat_data_type::undefined,
    Number = xbox::services::stats::manager::stat_data_type::number,
    String = xbox::services::stats::manager::stat_data_type::string
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_STATISTIC_MANAGER_END