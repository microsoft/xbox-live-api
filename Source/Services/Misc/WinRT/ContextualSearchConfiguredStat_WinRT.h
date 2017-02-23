// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "ContextualSearchStatVisibility_WinRT.h"
#include "xsapi/contextual_search_service.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_BEGIN

/// <summary>Contains information about a contextual search stat.</summary>
public ref class ContextualSearchConfiguredStat sealed
{
public:
    /// <summary>
    /// The actual name of the configured stat.  This is what you will use as a parameter in the search API.  
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Name, name);

    /// <summary>
    /// The data type of the stat.  It should align with the values you are passing for the stat into the Search API.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(DataType, data_type);

    /// <summary>
    /// The visibility of the stat
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(Visibility, visibility, ContextualSearchStatVisibility);

    /// <summary>
    /// This is the localizable string exposed to the end user. Depending on the language / culture you 
    /// pass up in the header, you will get a localized version of this string.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(DisplayName, display_name);

    /// <summary>
    /// Boolean on whether you can use this stat in a filter
    /// </summary>
    DEFINE_PROP_GET_OBJ(CanBeFiltered, can_be_filtered, bool);

    /// <summary>
    /// Boolean on whether you can make sort queries
    /// </summary>
    DEFINE_PROP_GET_OBJ(CanBeSorted, can_be_sorted, bool);

    /// <summary>
    /// How this Stat should be represented, as either a Set, Defined Range, or Undefined Range
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(DisplayType, display_type, ContextualSearchStatVisibility);

    /// <summary>
    /// If the representation type is set, this contains a map to convert values to display names
    /// </summary>
    property Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^>^ ValueToDisplayName
    {
        Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^>^ get();
    }

    /// <summary>
    /// If the representation type is defined range, this is the min range
    /// </summary>
    DEFINE_PROP_GET_OBJ(RangeMin, range_min, uint64);

    /// <summary>
    /// If the representation type is defined range, this is the max range
    /// </summary>
    DEFINE_PROP_GET_OBJ(RangeMax, range_max, uint64);

internal:
    ContextualSearchConfiguredStat(
        _In_ xbox::services::contextual_search::contextual_search_configured_stat cppObj
        );

private:
    xbox::services::contextual_search::contextual_search_configured_stat m_cppObj;
    Platform::Collections::Map<Platform::String^, Platform::String^>^ m_valueToDisplayName;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_END
