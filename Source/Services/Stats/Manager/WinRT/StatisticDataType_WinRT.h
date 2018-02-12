// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/stats_manager.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_STATISTIC_MANAGER_BEGIN

/// <summary> 
/// Indicates the type of data of the statistic.
/// </summary>
public enum class StatisticDataType
{
	/// <summary> 
	/// The stat data type is undefined.
	/// </summary>
    Undefined = xbox::services::stats::manager::stat_data_type::undefined,

	/// <summary> 
	/// The stat data type is a number.
	/// </summary>
    Number = xbox::services::stats::manager::stat_data_type::number,

	/// <summary> 
	/// The stat data type is a string.
	/// </summary>
    String = xbox::services::stats::manager::stat_data_type::string
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_STATISTIC_MANAGER_END