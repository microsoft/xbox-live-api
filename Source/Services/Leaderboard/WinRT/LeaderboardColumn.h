// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "shared_macros.h"
#include "xsapi/leaderboard.h"
#include "Macros_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_BEGIN

/// <summary>
/// Represents a column in a collection of leaderboard items.
/// </summary>
public ref class LeaderboardColumn sealed
{
public:
    
    /// <summary>
    /// DEPRECATED
    /// The display name of the column.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(DisplayName, display_name)

    /// <summary>
    /// The name the statistic displayed in the column.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(StatisticName, stat_name)

    /// <summary>
    /// The property type of the statistic in the column.
    /// The type can be one of the following:
    /// PropertyType.Uint64, PropertyType.Double, 
    /// PropertyType.String, PropertyType.DateTime, 
    /// or PropertyType.OtherType if unknown.
    /// </summary>
    property Windows::Foundation::PropertyType StatisticType { Windows::Foundation::PropertyType get(); }

internal:
    LeaderboardColumn(
        _In_ xbox::services::leaderboard::leaderboard_column cppObj
        );

private:
    xbox::services::leaderboard::leaderboard_column m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_END