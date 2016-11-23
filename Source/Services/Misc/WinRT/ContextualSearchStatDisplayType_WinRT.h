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
