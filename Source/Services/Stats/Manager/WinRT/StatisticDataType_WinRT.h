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

public enum class StatisticDataType
{
    Undefined = xbox::services::stats::manager::stat_data_type::undefined,
    Number = xbox::services::stats::manager::stat_data_type::number,
    String = xbox::services::stats::manager::stat_data_type::string
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_STATISTIC_MANAGER_END