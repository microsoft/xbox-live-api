// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/stats_manager.h"
#include "StatisticDataType_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_STATISTIC_MANAGER_BEGIN

/// <summary> 
/// Represents a simplified stat.
/// </summary>
public ref class StatisticValue sealed
{
public:
    /// <summary> 
    /// Name of the statistic
    /// </summary>
    /// <returns>A stat container that has the stat name string</returns>
    DEFINE_PROP_GET_STR_OBJ(Name, name);

    /// <summary> 
    /// Return data as numerical type
    /// </summary>
    /// <returns>Float data for statistic</returns>
    /// <remarks>Will debug assert if data is not type requested</remarks>
    DEFINE_PROP_GET_OBJ(AsNumber, as_number, double);

    /// <summary> 
    /// Return data as integer type
    /// </summary>
    /// <returns>Integer data for statistic</returns>
    /// <remarks>Will debug assert if data is not type requested</remarks>
    DEFINE_PROP_GET_OBJ(AsInteger, as_integer, int64_t);

    /// <summary> 
    /// Return data as string type
    /// </summary>
    /// <returns>data as char_t*</returns>
    /// <remarks>Will debug assert if data is not type requested</remarks>
    DEFINE_PROP_GET_STR_OBJ(AsString, as_string);

    /// <summary> 
    /// Return type of data the data object is
    /// </summary>
    /// <returns>Stat data type</returns>
    DEFINE_PROP_GET_ENUM_OBJ(DataType, data_type, StatisticDataType);

internal:
    StatisticValue(
        _In_ xbox::services::stats::manager::stat_value cppObj
        );

private:
    xbox::services::stats::manager::stat_value m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_STATISTIC_MANAGER_END