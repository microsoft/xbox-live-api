// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#if !defined(__cplusplus)
#error C++11 required
#endif

#pragma once

extern "C"
{

/// <summary>
/// Enumeration values that indicate the Title Managed Stat type.
/// </summary>
/// <memof><see cref="XblTitleManagedStatistic"/></memof>
enum class XblTitleManagedStatType : uint32_t
{
    /// <summary>
    /// Sets the type of Title Managed Stat to a Number.
    /// </summary>
    Number,

    /// <summary>
    /// Sets the type of Title Managed Stat to a String.
    /// </summary>
    String
};

/// <summary>
/// Contains information about a Title Managed statistic.
/// </summary>
/// <argof><see cref="XblTitleManagedStatsWriteAsync"/></argof>
typedef struct XblTitleManagedStatistic
{
    /// <summary>
    /// The name of the statistic (case insensitive).
    /// </summary>
    _Field_z_ const char* statisticName;

    /// <summary>
    /// The type of the statistic.
    /// </summary>
    XblTitleManagedStatType statisticType;

    /// <summary>
    /// The value of the double statistic. Backed by a JSON number value, which can 
    /// lead to precision issues when storing 64-bit fixed point values.
    /// </summary>
    double numberValue;

    /// <summary>
    /// The value of the string statistic.
    /// </summary>
    _Field_z_ const char* stringValue;
} XblTitleManagedStatistic;

/// <summary>
/// Completely update the calling user's stats.  
/// This call wipes out all existing stats (any stats not referenced in the provided array will be removed).
/// </summary>
/// <param name="xblContextHandle">Xbox live context for the local user.</param>
/// <param name="xboxUserId">User whose stats are being updated (can only be the local user).</param>
/// <param name="statistics">A list of XblTitleManagedStatistic to submit.</param>
/// <param name="statisticsCount">Number of items in the statistics array.</param>
/// <param name="async">Used for async call.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// To get the result, call <see cref="XAsyncGetStatus"/> inside 
/// the AsyncBlock callback or after the AsyncBlock is complete.  
/// If the call fails for any reason, it is the responsibility of the game to re-submit the request.
/// </remarks>
STDAPI XblTitleManagedStatsWriteAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_ uint64_t xboxUserId,
    _In_ const XblTitleManagedStatistic* statistics,
    _In_ size_t statisticsCount,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Partially update the calling user's stats.  
/// Stats will only be overwritten if they already exist (any stats not referenced will remain unchanged).
/// </summary>
/// <param name="xblContextHandle">Xbox live context for the local user.</param>
/// <param name="statistics">A list of XblTitleManagedStatistic to submit.</param>
/// <param name="statisticsCount">Number of items in the statistics array.</param>
/// <param name="async">Used for async call.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Note that stat names are case insensitive.  
/// To get the result, call <see cref="XAsyncGetStatus"/> inside 
/// the AsyncBlock callback or after the AsyncBlock is complete.  
/// If the call fails for any reason, it is the responsibility of the game to re-submit the request.
/// </remarks>
STDAPI XblTitleManagedStatsUpdateStatsAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_ const XblTitleManagedStatistic* statistics,
    _In_ size_t statisticsCount,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Delete stats for the calling user (any stats not referenced will remain unchanged).
/// </summary>
/// <param name="xblContextHandle">Xbox live context for the local user.</param>
/// <param name="statisticNames">A list of names of statistics to delete.  
/// If there isn't an existing statistic matching a provided name, no changes will be made for that statistic.</param>
/// <param name="statisticNamesCount">Number of items in the statisticNames array.</param>
/// <param name="async">Used for async call.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Note that stat names are case insensitive.  
/// To get the result, call <see cref="XAsyncGetStatus"/> inside 
/// the AsyncBlock callback or after the AsyncBlock is complete.  
/// If the call fails for any reason, it is the responsibility of the game to re-submit the request.
/// </remarks>
STDAPI XblTitleManagedStatsDeleteStatsAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_ const char** statisticNames,
    _In_ size_t statisticNamesCount,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

}