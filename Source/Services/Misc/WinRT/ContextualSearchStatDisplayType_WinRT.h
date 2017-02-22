// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "pch.h"
#include "xsapi/contextual_search_service.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_BEGIN

/// <summary>Enumerates the display type of a contextual search statistic.</summary>
public enum class ContextualSearchStatDisplayType
{
    /// <summary>Unknown type.</summary>
    Unknown = xbox::services::contextual_search::contextual_search_stat_display_type::unknown,

    /// <summary>This stat should be displayed as a undefined range.</summary>
    UndefinedRange = xbox::services::contextual_search::contextual_search_stat_display_type::undefined_range,

    /// <summary>This stat should be displayed as a defined range.</summary>
    DefinedRange = xbox::services::contextual_search::contextual_search_stat_display_type::defined_range,

    /// <summary>This stat should be displayed using the value_to_display_name_map.</summary>
    Set = xbox::services::contextual_search::contextual_search_stat_display_type::set
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_END
