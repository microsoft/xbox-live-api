// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#if !defined(__cplusplus)
#error C++11 required
#endif

#include "real_time_activity_c.h"

#pragma once

extern "C"
{

/// <summary>
/// Contains information about a user statistic.
/// </summary>
/// <memof><see cref="XblServiceConfigurationStatistic"/></memof>
/// <memof><see cref="XblStatisticChangeEventArgs"/></memof>
typedef struct XblStatistic
{
    /// <summary>
    /// The name of the statistic.
    /// </summary>
    _Field_z_ const char* statisticName;

    /// <summary>
    /// The type of the statistic.
    /// </summary>
    _Field_z_ const char* statisticType;

    /// <summary>
    /// The value of the statistic.
    /// </summary>
    _Field_z_ const char* value;
} XblStatistic;

/// <summary>
/// Contains statistical information from a service configuration.
/// </summary>
/// <memof><see cref="XblUserStatisticsResult"/></memof>
typedef struct XblServiceConfigurationStatistic
{
    /// <summary>
    /// The service configuration ID (SCID) associated with the leaderboard.
    /// </summary>
    _Null_terminated_ char serviceConfigurationId[XBL_SCID_LENGTH];

    /// <summary>
    /// A collection of statistics used in leaderboards.
    /// </summary>
    XblStatistic* statistics;

    /// <summary>
    /// The size of **statistics**.
    /// </summary>
    uint32_t statisticsCount;
} XblServiceConfigurationStatistic;

/// <summary>
/// Represents the results of a user statistic query.
/// </summary>
/// <argof><see cref="XblUserStatisticsGetSingleUserStatisticResult"/></argof>
/// <argof><see cref="XblUserStatisticsGetSingleUserStatisticsResult"/></argof>
/// <argof><see cref="XblUserStatisticsGetMultipleUserStatisticsResult"/></argof>
/// <argof><see cref="XblUserStatisticsGetMultipleUserStatisticsForMultipleServiceConfigurationsResult"/></argof>
typedef struct XblUserStatisticsResult
{
    /// <summary>
    /// The Xbox User ID for the user in a statistic.
    /// </summary>
    uint64_t xboxUserId;

    /// <summary>
    /// A collection of statistics from a service configuration.
    /// </summary>
    XblServiceConfigurationStatistic* serviceConfigStatistics;

    /// <summary>
    /// The size of **serviceConfigStatistics**.
    /// </summary>
    uint32_t serviceConfigStatisticsCount;
} XblUserStatisticsResult;

/// <summary>
/// Represents the results of a user statistic query.
/// </summary>
/// <argof><see cref="XblUserStatisticsGetMultipleUserStatisticsForMultipleServiceConfigurationsAsync"/></argof>
typedef struct XblRequestedStatistics
{
    /// <summary>
    /// The service configuration ID in use.
    /// </summary>
    _Null_terminated_ char serviceConfigurationId[XBL_SCID_LENGTH];

    /// <summary>
    /// A collection of statistics.
    /// </summary>
    _Field_z_ const char** statistics;

    /// <summary>
    /// The size of **statistics**.
    /// </summary>
    uint32_t statisticsCount;
} XblRequestedStatistics;

/// <summary>
/// Contains information about a change to a subscribed statistic.
/// </summary>
/// <argof><see cref="XblStatisticChangedHandler"/></argof>
typedef struct XblStatisticChangeEventArgs
{
    /// <summary>
    /// The Xbox user ID used to create the subscription.
    /// </summary>
    uint64_t xboxUserId;

    /// <summary>
    /// The service configuration ID used to create the subscription.
    /// </summary>
    _Null_terminated_ char serviceConfigurationId[XBL_SCID_LENGTH];

    /// <summary>
    /// The statistic with an updated value.
    /// </summary>
    XblStatistic latestStatistic;
} XblStatisticChangeEventArgs;

/// <summary>
/// Get a specified statistic for a specified user.
/// </summary>
/// <param name="xblContextHandle">Xbox live context for the local user.</param>
/// <param name="xboxUserId">The Xbox User ID of the player to get statistics for.</param>
/// <param name="serviceConfigurationId">The Service Configuration ID (SCID) for the title. The SCID is considered case sensitive so paste it directly from the Partner Center.</param>
/// <param name="statisticName">The name of the statistic to return.</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// If multiple statistics are required for a single user, use this batch API instead: <see cref="XblUserStatisticsGetSingleUserStatisticsAsync"/>
/// If statistics are needed for multiple users, use this batch API: <see cref="XblUserStatisticsGetMultipleUserStatisticsAsync"/>
/// Call <see cref="XblUserStatisticsGetSingleUserStatisticResultSize"/> and <see cref="XblUserStatisticsGetSingleUserStatisticResult"/> 
/// upon completion to get the result.
/// </remarks>
/// <rest>V1 GET /users/xuid({xuid})/scids/{scid}/stats/{statname1}</rest>
STDAPI XblUserStatisticsGetSingleUserStatisticAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_ uint64_t xboxUserId,
    _In_z_ const char* serviceConfigurationId,
    _In_z_ const char* statisticName,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Get the result size for an XblUserStatisticsGetSingleUserStatisticsAsync call.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="resultSizeInBytes">Passes bakc the size in bytes required to store the user statistics result.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblUserStatisticsGetSingleUserStatisticResultSize(
    _In_ XAsyncBlock* async,
    _Out_ size_t* resultSizeInBytes
) XBL_NOEXCEPT;

/// <summary>
/// Get the result for an XblUserStatisticsGetSingleUserStatisticAsync call.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="bufferSize">The size of the provided buffer.  
/// Use <see cref="XblUserStatisticsGetSingleUserStatisticResultSize"/> to get the size required.</param>
/// <param name="buffer">A caller allocated byte buffer that passes back the single user statistic.</param>
/// <param name="ptrToBuffer">Passes back a strongly typed pointer that points into buffer.  
/// Do not free this as its lifecycle is tied to buffer.</param>
/// <param name="bufferUsed">Passes back the number of bytes written to the buffer.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblUserStatisticsGetSingleUserStatisticResult(
    _In_ XAsyncBlock* async,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_(bufferSize, *bufferUsed) void* buffer,
    _Outptr_ XblUserStatisticsResult** ptrToBuffer,
    _Out_opt_ size_t* bufferUsed
) XBL_NOEXCEPT;

/// <summary>
/// Get specified statistics for a single user.
/// </summary>
/// <param name="xblContextHandle">Xbox live context for the local user.</param>
/// <param name="xboxUserId">The Xbox User ID of the player to get statistics for.</param>
/// <param name="serviceConfigurationId">The Service Configuration ID (SCID) for the title. The SCID is considered case sensitive so paste it directly from the Partner Center.</param>
/// <param name="statisticNames">A collection of statistic names to lookup.</param>
/// <param name="statisticNamesCount">The number of statistic names.</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Call <see cref="XblUserStatisticsGetSingleUserStatisticsResultSize"/> and <see cref="XblUserStatisticsGetSingleUserStatisticsResult"/>
/// upon completion to get the result. Only statistics with values are returned.  
/// For example, if you ask for 3 statistic names and only 2 have values, 
/// only 2 statistics are returned by the service.
/// </remarks>
/// <rest>V1 GET /users/xuid({xuid})/scids/{scid}/stats/{statname1},...,{statnameN}</rest>
STDAPI XblUserStatisticsGetSingleUserStatisticsAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_ uint64_t xboxUserId,
    _In_z_ const char* serviceConfigurationId,
    _In_ const char** statisticNames,
    _In_ size_t statisticNamesCount,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Get the result size for an XblUserStatisticsGetSingleUserStatisticsAsync call.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="resultSizeInBytes">Passes back the size in bytes required to store the user statistics result.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblUserStatisticsGetSingleUserStatisticsResultSize(
    _In_ XAsyncBlock* async,
    _Out_ size_t* resultSizeInBytes
) XBL_NOEXCEPT;

/// <summary>
/// Get the result for an XblUserStatisticsGetSingleUserStatisticsAsync call.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="bufferSize">The size of the provided buffer.  
/// Use <see cref="XblUserStatisticsGetSingleUserStatisticsResultSize"/> to get the size required.</param>
/// <param name="buffer">A caller allocated byte buffer that passes back the single user statistics.</param>
/// <param name="ptrToBuffer">Passes back a strongly typed pointer that points into buffer.  
/// Do not free this as its lifecycle is tied to buffer.</param>
/// <param name="bufferUsed">Passes back the number of bytes written to the buffer.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblUserStatisticsGetSingleUserStatisticsResult(
    _In_ XAsyncBlock* async,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_(bufferSize, *bufferUsed) void* buffer,
    _Outptr_ XblUserStatisticsResult** ptrToBuffer,
    _Out_opt_ size_t* bufferUsed
) XBL_NOEXCEPT;

/// <summary>
/// Get statistics for multiple users.
/// </summary>
/// <param name="xblContextHandle">Xbox live context for the local user.</param>
/// <param name="xboxUserIds">A list of the user Xbox user IDs to get stats for.</param>
/// <param name="xboxUserIdsCount">The number of Xbox user IDs.</param>
/// <param name="serviceConfigurationId">The Service Configuration ID (SCID) for the title. The SCID is considered case sensitive so paste it directly from the Partner Center.</param>
/// <param name="statisticNames">A collection of statistic names to lookup.</param>
/// <param name="statisticNamesCount">The number of statistic names.</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Call <see cref="XblUserStatisticsGetMultipleUserStatisticsResultSize"/> and <see cref="XblUserStatisticsGetMultipleUserStatisticsResult"/>
/// upon completion to get the result. Only statistics with values are returned.  
/// For example, if you ask for 3 statistic names and only 2 have values, 
/// only 2 statistics are returned by the service.
/// </remarks>
/// <rest>V1 POST /batch</rest>
STDAPI XblUserStatisticsGetMultipleUserStatisticsAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_ uint64_t* xboxUserIds,
    _In_ size_t xboxUserIdsCount,
    _In_z_ const char* serviceConfigurationId,
    _In_ const char** statisticNames,
    _In_ size_t statisticNamesCount,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Get the result size for an XblUserStatisticsGetMultipleUserStatisticsAsync call.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="resultSizeInBytes">Passes back the size in bytes required to store the user statistics result.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks></remarks>
STDAPI XblUserStatisticsGetMultipleUserStatisticsResultSize(
    _In_ XAsyncBlock* async,
    _Out_ size_t* resultSizeInBytes
) XBL_NOEXCEPT;

/// <summary>
/// Get the result for an XblUserStatisticsGetMultipleUserStatisticsAsync call.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="bufferSize">The size of the provided buffer.  
/// Use <see cref="XblUserStatisticsGetMultipleUserStatisticsResultSize"/> to get the size required.</param>
/// <param name="buffer">A caller allocated byte buffer that passes back the multiple user statistics.</param>
/// <param name="ptrToBuffer">Passes back a strongly typed pointer that points into buffer.  
/// Do not free this as its lifecycle is tied to buffer.</param>
/// <param name="resultsCount">Passes back the number of user statistics results.</param>
/// <param name="bufferUsed">Passes back the number of bytes written to the buffer.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks></remarks>
STDAPI XblUserStatisticsGetMultipleUserStatisticsResult(
    _In_ XAsyncBlock* async,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_(bufferSize, *bufferUsed) void* buffer,
    _Outptr_ XblUserStatisticsResult** ptrToBuffer,
    _Out_ size_t* resultsCount,
    _Out_opt_ size_t* bufferUsed
) XBL_NOEXCEPT;

/// <summary>
/// Get statistics for users across different Service configurations.
/// </summary>
/// <param name="xblContextHandle">Xbox live context for the local user.</param>
/// <param name="xboxUserIds">A list of the user Xbox user ID to get stats for.</param>
/// <param name="xboxUserIdsCount">A list of the user Xbox user ID to get stats for.</param>
/// <param name="requestedServiceConfigurationStatisticsCollection">A list of the service config IDs and its associated array of statistics.</param>
/// <param name="requestedServiceConfigurationStatisticsCollectionCount">The number of the service config IDs and its associated array of statistics.</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Call <see cref="XblUserStatisticsGetMultipleUserStatisticsForMultipleServiceConfigurationsResultSize"/> 
/// and <see cref="XblUserStatisticsGetMultipleUserStatisticsForMultipleServiceConfigurationsResult"/> 
/// upon completion to get the result. Only statistics with values are returned.  
/// For example, if you ask for 3 statistic names and only 2 have values, 
/// only 2 statistics are returned by the service.
/// </remarks>
/// <rest>V1 POST /batch</rest>
STDAPI XblUserStatisticsGetMultipleUserStatisticsForMultipleServiceConfigurationsAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_ uint64_t* xboxUserIds,
    _In_ uint32_t xboxUserIdsCount,
    _In_ const XblRequestedStatistics* requestedServiceConfigurationStatisticsCollection,
    _In_ uint32_t requestedServiceConfigurationStatisticsCollectionCount,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Get the result size for an XblUserStatisticsGetMultipleUserStatisticsAsync call.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="resultSizeInBytes">Passes back the size in bytes required to store the user statistics result.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblUserStatisticsGetMultipleUserStatisticsForMultipleServiceConfigurationsResultSize(
    _In_ XAsyncBlock* async,
    _Out_ size_t* resultSizeInBytes
) XBL_NOEXCEPT;

/// <summary>
/// Get the result for an XblUserStatisticsGetMultipleUserStatisticsAsync call.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="bufferSize">The size of the provided buffer.  
/// Use <see cref="XblUserStatisticsGetMultipleUserStatisticsForMultipleServiceConfigurationsResultSize"/> to get the size required.</param>
/// <param name="buffer">A caller allocated byte buffer that passes back the multiple user statistics.</param>
/// <param name="ptrToBufferResults">Passes back a strongly typed array of XblUserStatisticsResult that points into buffer.  
/// Do not free this as its lifecycle is tied to buffer.</param>
/// <param name="resultsCount">Passes back the number of results.</param>
/// <param name="bufferUsed">Passes back the number of bytes written to the buffer.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblUserStatisticsGetMultipleUserStatisticsForMultipleServiceConfigurationsResult(
    _In_ XAsyncBlock* async,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_(bufferSize, *bufferUsed) void* buffer,
    _Outptr_ XblUserStatisticsResult** ptrToBufferResults,
    _Out_ size_t* resultsCount,
    _Out_opt_ size_t* bufferUsed
) XBL_NOEXCEPT;

/// <summary>
/// Subscribes to statistic update notifications via the StatisticChanged event.
/// DEPRECATED. This continues to work, however it will be removed in a future release. 
/// Individual RTA subscription will be managed automatically by XSAPI as statistics are tracked with <see cref="XblUserStatisticsTrackStatistics"/>.
/// </summary>
/// <param name="xblContextHandle">Xbox live context for the local user.</param>
/// <param name="xboxUserId">The Xbox User ID of the player requesting the subscription.</param>
/// <param name="serviceConfigurationId">The Service Configuration ID (SCID) for the title. The SCID is considered case sensitive so paste it directly from the Partner Center.</param>
/// <param name="statisticName">The name of the statistic to subscribe to.</param>
/// <param name="subscriptionHandle">Passes back an XblRealTimeActivitySubscriptionHandle object that contains the state of the subscription.  
/// You can register an event handler for statistic changes by calling XblUserStatisticsAddStatisticChangedHandler().</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI_XBL_DEPRECATED XblUserStatisticsSubscribeToStatisticChange(
    _In_ XblContextHandle xblContextHandle,
    _In_ uint64_t xboxUserId,
    _In_z_ const char* serviceConfigurationId,
    _In_z_ const char* statisticName,
    _Out_ XblRealTimeActivitySubscriptionHandle* subscriptionHandle
) XBL_NOEXCEPT;

/// <summary>
/// Unsubscribes a previously created statistic change subscription.
/// DEPRECATED. This continues to work, however it will be removed in a future release. 
/// Individual RTA subscription will be managed automatically by XSAPI as statistics are untracked with <see cref="XblUserStatisticsStopTrackingStatistics"/>
/// or <see cref="XblUserStatisticsStopTrackingUsers"/>
/// </summary>
/// <param name="xblContextHandle">Xbox live context for the local user.</param>
/// <param name="subscriptionHandle">The subscription object to unsubscribe.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI_XBL_DEPRECATED XblUserStatisticsUnsubscribeFromStatisticChange(
    _In_ XblContextHandle xblContextHandle,
    _In_ XblRealTimeActivitySubscriptionHandle subscriptionHandle
) XBL_NOEXCEPT;

/// <summary>
/// Event handler for statistic change notifications.
/// </summary>
/// <param name="statisticChangeEventArgs">Contains information about a change to a subscribed statistic.</param>
/// <param name="context">Caller context that will be passed back to the handler.</param>
/// <returns></returns>
/// <argof><see cref="XblUserStatisticsAddStatisticChangedHandler"/></argof>
typedef void CALLBACK XblStatisticChangedHandler(
    _In_ XblStatisticChangeEventArgs statisticChangeEventArgs,
    _In_ void* context
);

/// <summary>
/// Registers an event handler for statistic change notifications.
/// Event handlers receive a XblStatisticChangeEventArgs object.
/// </summary>
/// <param name="xblContextHandle">Xbox live context for the local user.</param>
/// <param name="handler">The callback function that receives notifications.</param>
/// <param name="handlerContext">Client context pointer to be passed back to the handler.</param>
/// <returns>A XblFunctionContext object that can be used to unregister the event handler.</returns>
STDAPI_(XblFunctionContext) XblUserStatisticsAddStatisticChangedHandler(
    _In_ XblContextHandle xblContextHandle,
    _In_ XblStatisticChangedHandler handler,
    _In_opt_ void* handlerContext
) XBL_NOEXCEPT;

/// <summary>
/// Unregisters an event handler for statistic change notifications.
/// </summary>
/// <param name="xblContextHandle">Xbox live context for the local user.</param>
/// <param name="context">The function_context object that was returned when the event handler was registered.</param>
/// <returns></returns>
STDAPI_(void) XblUserStatisticsRemoveStatisticChangedHandler(
    _In_ XblContextHandle xblContextHandle,
    _In_ XblFunctionContext context
) XBL_NOEXCEPT;

/// <summary>
/// Configures the set of stats that will be tracked real-time. This call will have no affect on stats
/// that were already being tracked.
/// </summary>
/// <param name="xblContextHandle">Xbox live context for the local user.</param>
/// <param name="xboxUserIds">Array of XboxUserIDs for whom to track the provided stats.</param>
/// <param name="xboxUserIdsCount">Length of xboxUserIds array.</param>
/// <param name="serviceConfigurationId">The Service Configuration ID (SCID) for the title. The SCID is considered case sensitive so paste it directly from the Partner Center.</param>
/// <param name="statisticNames">Array of statistic names for which real-time updates will be received.</param>
/// <param name="statisticNamesCount">Length of statisticNames array.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Updates will be delivered via XblStatisticChangedHandlers.
/// Note that the set of tracked stats can be updated independent from the handlers.
/// </remarks>
STDAPI XblUserStatisticsTrackStatistics(
    _In_ XblContextHandle xblContextHandle,
    _In_ const uint64_t* xboxUserIds,
    _In_ size_t xboxUserIdsCount,
    _In_z_ const char* serviceConfigurationId,
    _In_ const char** statisticNames,
    _In_ size_t statisticNamesCount
) XBL_NOEXCEPT;

/// <summary>
/// Configures the set of stats that will be tracked real-time. Updates will no longer be received
/// for the provided stats and users.
/// </summary>
/// <param name="xblContextHandle">Xbox live context for the local user.</param>
/// <param name="xboxUserIds">Array of XboxUserIDs for whom to stop tracking the provided stats.</param>
/// <param name="xboxUserIdsCount">Length of xboxUserIds array.</param>
/// <param name="serviceConfigurationId">The Service Configuration ID (SCID) for the title. The SCID is considered case sensitive so paste it directly from the Partner Center.</param>
/// <param name="statisticNames">Array of statistic names for which real-time updates are no longer needed.</param>
/// <param name="statisticNamesCount">Length of statisticNames array.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblUserStatisticsStopTrackingStatistics(
    _In_ XblContextHandle xblContextHandle,
    _In_ const uint64_t* xboxUserIds,
    _In_ size_t xboxUserIdsCount,
    _In_z_ const char* serviceConfigurationId,
    _In_ const char** statisticNames,
    _In_ size_t statisticNamesCount
) XBL_NOEXCEPT;

/// <summary>
/// Configures the set of stats that will be tracked real-time. The API will cancel all real-time
/// stat updates for the provided users.
/// </summary>
/// <param name="xblContextHandle">Xbox live context for the local user.</param>
/// <param name="xboxUserIds">Array of XboxUserIDs for whom to stop tracking the all stats.</param>
/// <param name="xboxUserIdsCount">Length of xboxUserIds array.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblUserStatisticsStopTrackingUsers(
    _In_ XblContextHandle xblContextHandle,
    _In_ const uint64_t* xboxUserIds,
    _In_ size_t xboxUserIdsCount
) XBL_NOEXCEPT;

} // end extern c
