// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/contextual_search_service.h"
#include "ContextualSearchConfiguredStat_WinRT.h"
#include "ContextualSearchFilterOperator_WinRT.h"
#include "ContextualSearchStatDisplayType_WinRT.h"
#include "ContextualSearchStatVisibility_WinRT.h"
#include "ContextualSearchGameClip_WinRT.h"
#include "ContextualSearchGameClipsResult_WinRT.h"
#include "ContextualSearchGameClipStat_WinRT.h"
#include "ContextualSearchGameClipUriInfo_WinRT.h"
#include "ContextualSearchGameClipThumbnail_WinRT.h"
#include "ContextualSearchBroadcast_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_BEGIN

/// <summary>
/// Provides methods to perform Xbox Live contextual search.
/// </summary>
public ref class ContextualSearchService sealed
{
public:
    /// <summary>
    /// Get information to hydrate a UI for the end user to choose what data from a game they want to use 
    /// to filter or sort the broadcast
    /// </summary>
    /// <param name="titleId">The title ID.</param>
    /// <returns>
    /// The result contains information to hydrate a UI.
    /// </returns>
    /// <remarks>Calls V1 GET /titles/{titleid}/configuration</remarks>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<ContextualSearchConfiguredStat^>^>^ GetConfigurationAsync(
        _In_ uint32 titleId
        );

    /// <summary>
    /// Get information to hydrate a UI for the end user to choose what data from a game they want to use 
    /// to filter or sort the broadcast.  Defaults to the top 100 broadcasts ordered by viewers descending with no filter
    /// </summary>
    /// <param name="titleId">The title ID.</param>
    /// <returns>
    /// The result is a contextual_search_broadcasts_result object which contains information about broadcasts.
    /// </returns>
    /// <remarks>Calls V1 GET /titles/{titleId}/broadcasts</remarks>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<ContextualSearchBroadcast^>^>^ GetBroadcastsAsync(
        _In_ uint32 titleId
        );

    /// <summary>
    /// Get information to hydrate a UI for the end user to choose what data from a game they want to use 
    /// to filter or sort the broadcast
    /// </summary>
    /// <param name="titleId">The title ID.</param>
    /// <param name="skipItems">The number of broadcasts to skip.</param>
    /// <param name="maxItems">The maximum number of broadcasts the result can contain.  Pass 0 to attempt to retrieve the top 100 items.</param>
    /// <param name="orderByStatName">This specifies a stat to sort the broadcasts by.  Pass empty string to default to ordering by viewers descending</param>
    /// <param name="orderAscending">Pass true to order ascending, false to order descending</param>
    /// <param name="searchQuery">This specifies a OData Filter Expressions string that specifies the stats to filter on.
    /// For example "stats:maptype eq 1" means filtering by the stat "maptype" to the value 1.</param>
    /// <returns>
    /// The result is a contextual_search_broadcasts_result object which contains information about broadcasts.
    /// </returns>
    /// <remarks>Calls V1 GET /titles/{titleId}/broadcasts</remarks>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<ContextualSearchBroadcast^>^>^ GetBroadcastsAsync(
        _In_ uint32 titleId,
        _In_ uint32 skipItems,
        _In_ uint32 maxItems,
        _In_ Platform::String^ orderByStatName,
        _In_ bool orderAscending,
        _In_ Platform::String^ searchQuery
        );

    /// <summary>
    /// Get information to hydrate a UI for the end user to choose what data from a game they want to use 
    /// to filter or sort the broadcast
    /// </summary>
    /// <param name="titleId">The title ID.</param>
    /// <param name="skipItems">The number of broadcasts to skip.</param>
    /// <param name="maxItems">The maximum number of broadcasts the result can contain.  Pass 0 to attempt to retrieve the top 100 items.</param>
    /// <param name="orderByStatName">This specifies a stat to sort the broadcasts by.  Pass empty string to default to ordering by viewers descending</param>
    /// <param name="orderAscending">Pass true to order ascending, false to order descending</param>
    /// <param name="filterStatName">This specifies the stat to filter on. Pass empty string for no filter</param>
    /// <param name="filterOperator">This specifies operator to apply for the filter.</param>
    /// <param name="filterStatValue">This specifies the value to filter on. Pass empty string for no filter</param>
    /// <returns>
    /// The result is a contextual_search_broadcasts_result object which contains information about broadcasts.
    /// </returns>
    /// <remarks>Calls V1 GET /titles/{titleId}/broadcasts</remarks>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<ContextualSearchBroadcast^>^>^ GetBroadcastsAsync(
        _In_ uint32 titleId,
        _In_ uint32 skipItems,
        _In_ uint32 maxItems,
        _In_ Platform::String^ orderByStatName,
        _In_ bool orderAscending,
        _In_ Platform::String^ filterStatName,
        _In_ ContextualSearchFilterOperator filterOperator,
        _In_ Platform::String^ filterStatValue
        );

    /// <summary>
    /// Get information to hydrate a UI that allows end users to filter or sort game clips
    /// </summary>
    /// <param name="titleId">The title ID.</param>
    /// <param name="skipItems">The number of game clips to skip.</param>
    /// <param name="maxItems">The maximum number of game clips the result can contain.  Pass 0 to attempt to retrieve the top 100 items.</param>
    /// <param name="orderByStatName">This specifies a stat to sort the game clips by.  Pass empty string to default to ordering by viewers descending</param>
    /// <param name="orderAscending">Pass true to order ascending, false to order descending</param>
    /// <param name="filterStatName">This specifies the stat to filter on. Pass empty string for no filter. For example, "gametype.value".
    /// The "value" property is only available for string stats and integer stats. 
    /// The "min", "max" and "delta" properties are only available for double stats and integer stats.
    /// </param>
    /// <param name="filterOperator">This specifies operator to apply for the filter.</param>
    /// <param name="filterStatValue">This specifies the value to filter on. The stat properties that can be used to search with are 'value', 'min', 'max', and 'delta'. Pass empty string for no filter</param>
    /// <returns>
    /// The result is an object which contains information about game clips.
    /// </returns>
    /// <remarks>Calls V1 GET /titles/{titleId}/gameclips</remarks>
    Windows::Foundation::IAsyncOperation<ContextualSearchGameClipsResult^>^ GetGameClipsAsync(
        _In_ uint32 titleId,
        _In_ uint32 skipItems,
        _In_ uint32 maxItems,
        _In_ Platform::String^ orderByStatName,
        _In_ bool orderAscending,
        _In_ Platform::String^ filterStatName,
        _In_ ContextualSearchFilterOperator filterOperator,
        _In_ Platform::String^ filterStatValue
        );

    /// <summary>
    /// Get information to hydrate a UI that allows end users to filter or sort game clips
    /// </summary>
    /// <param name="titleId">The title ID.</param>
    /// <param name="skipItems">The number of game clips to skip.</param>
    /// <param name="maxItems">The maximum number of game clips the result can contain.  Pass 0 to attempt to retrieve the top 100 items.</param>
    /// <param name="orderByStatName">This specifies a stat to sort the game clips by.  Pass empty string to default to ordering by viewers descending</param>
    /// <param name="orderAscending">Pass true to order ascending, false to order descending</param>
    /// <param name="searchQuery">
    /// The search query.
    /// The query syntax is an OData like syntax with only the following operators supported EQ, NE, GE, GT, LE and LT along with the logical operators of AND and OR. You can also use grouping operator '()'. The stat properties that can be used to search with are "value", "min", "max", and "delta".
    ///
    /// Example 1:
    /// To search for game clips during which the "gametype" stat had a value of 1 use
    ///     "stats:gametype.value eq 1"
    ///
    /// Example 2:
    /// To search for game clips during which the "rank" stat jumped more than 2.
    ///     "stats:rank.value gt 2"
    ///
    /// Example 3:
    /// To search for game clips during which the "rank" stat jumped more than 2 and the "gametype" stat had a value of 1 use
    ///     "stats:rank.value gt 2 and stat:gametype.value eq 1"
    ///
    /// Note:
    /// - The "value" property is only available for string stats and integer stats that are used with a Set.
    /// - The "min", "max" and "delta" properties are only available for double stats and integer stats that are not used with a Set.
    /// </param>
    /// <returns>
    /// The result is an object which contains information about game clips.
    /// </returns>
    /// <remarks>Calls V1 GET /titles/{titleId}/gameclips</remarks>
    Windows::Foundation::IAsyncOperation<ContextualSearchGameClipsResult^>^ GetGameClipsAsync(
        _In_ uint32 titleId,
        _In_ uint32 skipItems,
        _In_ uint32 maxItems,
        _In_ Platform::String^ orderByStatName,
        _In_ bool orderAscending,
        _In_ Platform::String^ searchQuery
        );

internal:
    ContextualSearchService(
        _In_ xbox::services::contextual_search::contextual_search_service cppObj
        );

private:
    xbox::services::contextual_search::contextual_search_service m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_END
