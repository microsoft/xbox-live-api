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
#include "xsapi/simple_stats.h"
#include "StatisticDataType_WinRT.h"
#include "StatisticCompareType_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_STATISTIC_MANAGER_BEGIN

public ref class StatisticValue sealed
{
public:
    /// <summary> 
    /// Name of the statistic
    /// </summary>
    /// <returns>A stat container that has the stat name string</returns>
    DEFINE_PTR_PROP_GET_STR_OBJ_FROM_WCHAR_PTR(Name, name);

    /// <summary> 
    /// Return data as numerical type
    /// </summary>
    /// <returns>Float data for statistic</returns>
    /// <remarks>Will debug assert if data is not type requested</returns>
    DEFINE_PTR_PROP_GET_OBJ(AsNumber, as_number, double);

    /// <summary> 
    /// Return data as string type
    /// </summary>
    /// <returns>data as char_t*</returns>
    /// <remarks>Will debug assert if data is not type requested</returns>
    DEFINE_PTR_PROP_GET_STR_OBJ_FROM_WCHAR_PTR(AsString, as_string);

    /// <summary> 
    /// Return type of data the data object is
    /// </summary>
    /// <returns>Stat data type</returns>
    DEFINE_PTR_PROP_GET_ENUM_OBJ(DataType, data_type, StatisticDataType);

    /// <summary> 
    /// Enum of logic of how to handle statistic
    /// </summary>
    /// <returns>The compare type</returns>
    DEFINE_PTR_PROP_GET_ENUM_OBJ(CompareType, compare_type, StatisticCompareType);

internal:
    StatisticValue(
        _In_ std::shared_ptr<xbox::services::experimental::stats::manager::stat_value> cppObj
        );

private:
    std::shared_ptr<xbox::services::experimental::stats::manager::stat_value> m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_STATISTIC_MANAGER_END