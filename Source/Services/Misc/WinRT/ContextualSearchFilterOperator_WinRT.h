// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/contextual_search_service.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_BEGIN

/// <summary>Enumerates the operators of a contextual search filter.</summary>
public enum class ContextualSearchFilterOperator
{
    /// <summary>Matches if filter name is equals to filter value.</summary>
    Equal = xbox::services::contextual_search::contextual_search_filter_operator::equal,

    /// <summary>Matches if filter name is not equal to filter value.</summary>
    NotEqual = xbox::services::contextual_search::contextual_search_filter_operator::not_equal,

    /// <summary>Matches if filter name is greater than filter value.</summary>
    GreaterThan = xbox::services::contextual_search::contextual_search_filter_operator::greater_than,

    /// <summary>Matches if filter name is greater than or equal to filter value.</summary>
    GreaterThanOrEqual = xbox::services::contextual_search::contextual_search_filter_operator::greater_than_or_equal,

    /// <summary>Matches if filter name is less than filter value.</summary>
    LessThan = xbox::services::contextual_search::contextual_search_filter_operator::less_than,

    /// <summary>Matches if filter name is less than or equal to filter value.</summary>
    LessThanOrEqual = xbox::services::contextual_search::contextual_search_filter_operator::less_than_or_equal
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_END
