// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once


typedef enum XblLeaderboardStatType
{
    /// <summary>Unsigned 64 bit integer.</summary>
    XblLeaderboardStatType_Uint64,

    /// <summary>Boolean.</summary>
    XblLeaderboardStatType_Boolean,

    /// <summary>Double.</summary>
    XblLeaderboardStatType_Double,

    /// <summary>String.</summary>
    XblLeaderboardStatType_String,

    /// <summary>Unknown.</summary>
    XblLeaderboardStatType_Other
} XblLeaderboardStatType;

/// <summary> 
/// The order to sort the leaderboard in
/// </summary>
typedef enum XblLeaderboardSortOrder
{
    /// <summary> 
    /// Sorting the leaderboard lowest to highest
    /// </summary>
    XblLeaderboardSortOrder_Ascending,

    /// <summary> 
    /// Sorting the leaderboard highest to lowest
    /// </summary>
    XblLeaderboardSortOrder_Descending
} XblLeaderboardSortOrder;

/// <summary> 
/// The order to sort the leaderboard in
/// </summary>
typedef enum XblSocialGroupType
{
    /// <summary> 
    /// No social group
    /// </summary>
    XblSocialGroupType_None,

    /// <summary> 
    /// Social group for the people followed
    /// </summary>
    XblSocialGroupType_People,

    /// <summary> 
    /// Social group for the people tagged as favorites
    /// </summary>
    XblSocialGroupType_Favorites
} XblSocialGroupType;

/// <summary>
/// Represents a row in a collection of leaderboard items.
/// </summary>
typedef struct XblLeaderboardRow
{
    /// <summary>
    /// The UTF-8 encoded gamertag of the player.
    /// </summary>
    _Field_z_ const char* gamertag;

    /// <summary>
    /// The Xbox user ID of the player.
    /// </summary>
    uint64_t xboxUserId;

    /// <summary>
    /// The percentile rank of the player.
    /// </summary>
    double percentile;

    /// <summary>
    /// The rank of the player.
    /// </summary>
    uint32_t rank;

    /// <summary>
    /// UTF-8 encoded values for each column in the leaderboard row for the player.
    /// </summary>
    _Field_z_ const char** columnValues;

    /// <summary>
    /// The count of string in socialNetworks array
    /// </summary>
    uint32_t columnValuesCount;

} XblLeaderboardRow;

/// <summary>
/// Represents a column in a collection of leaderboard items.
/// </summary>
typedef struct XblLeaderboardColumn
{
    /// <summary>
    /// The UTF-8 encoded name the statistic displayed in the column.
    /// </summary>
    _Field_z_ const char* statName;

    /// <summary>
    /// The property type of the statistic in the column.
    /// </summary>
    XblLeaderboardStatType statType;

} XblLeaderboardColumn;

/// <summary>
/// Represents the parameters for submitting a leaderboard query using stats 2017
/// </summary>
typedef struct XblLeaderboard2017Query
{
    /// <summary>
    /// The Xbox user ID of the requesting user
    /// </summary>
    uint64_t xboxUserId;

    /// <summary>
    /// The UTF-8 encoded service configuration ID (SCID) of the title
    /// </summary>
    _Field_z_ const char* scid;

    /// <summary>
    /// The UTF-8 encoded statistic name to get a leaderboard for.
    /// </summary>
    _Field_z_ const char* statName;

    /// <summary>
    /// Optional name of the group of users to get get leaderboard results for.
    /// Set to XblSocialGroupType_None to ignore
    /// </summary>
    XblSocialGroupType socialGroup;

    /// <summary>
    /// Set sort order for the resulting leaderboard
    /// </summary>
    XblLeaderboardSortOrder order;

    /// <summary>
    /// Set maximum items that the resulting leaderboard will contain.  
    /// Set to 0 to ignore
    /// </summary>
    uint32_t maxItems;

    /// <summary>
    /// The resulting leaderboard will start with the 
    /// user that requested the leaderboard.  
    /// Set to 0 to ignore.
    /// </summary>
    uint64_t skipToXboxUserId;

    /// <summary>
    /// Set which rank the resulting leaderboard will start at.
    /// Set 0 to ignore
    /// </summary>
    uint32_t skipResultToRank;

} XblLeaderboard2017Query;


/// <summary>
/// Represents the parameters for submitting a leaderboard query using stats 2013
/// </summary>
typedef struct XblLeaderboard2013Query
{
    /// <summary>
    /// The Xbox user ID of the requesting user
    /// </summary>
    uint64_t xboxUserId;

    /// <summary>
    /// The UTF-8 encoded service configuration ID (SCID) of the title
    /// </summary>
    _Field_z_ const char* scid;

    /// <summary>
    /// Optional UTF-8 encoded leaderboard name to get a leaderboard for.  
    /// If doing a social leaderboard, then set to nullptr.
    /// </summary>
    _Field_z_ const char* leaderboardName;

    /// <summary>
    /// Optional UTF-8 encoded statistic name to get a leaderboard for.
    /// </summary>
    _Field_z_ const char* statName;

    /// <summary>
    /// Optional name of the group of users to get get leaderboard results for.
    /// Set to XblSocialGroupType_None to ignore
    /// </summary>
    XblSocialGroupType socialGroup;

    /// <summary>
    /// Optional UTF-8 encoded array of names of stats for the additional columns.
    /// </summary>
    _Field_z_ const char** additionalColumnleaderboardNames;

    /// <summary>
    /// Optional count of additionalColumnleaderboardNames passed in.
    /// </summary>
    uint32_t additionalColumnleaderboardNameCount;

    /// <summary>
    /// Set sort order for the resulting leaderboard
    /// </summary>
    XblLeaderboardSortOrder order;

    /// <summary>
    /// Set maximum items that the resulting leaderboard will contain.  
    /// Set to 0 to ignore
    /// </summary>
    uint32_t maxItems;

    /// <summary>
    /// Set the resulting leaderboard will start with the 
    /// user that requested the leaderboard.  
    /// Set to 0 to ignore.
    /// </summary>
    uint64_t skipToXboxUserId;

    /// <summary>
    /// Set which rank the resulting leaderboard will start at.
    /// Set 0 to ignore
    /// </summary>
    uint32_t skipResultToRank;

} XblLeaderboard2013Query;


/// <summary>
/// A handle to a social relationship result. This handle is used by other APIs to get the social relationship objects
/// and to get the next page of results from the service if there is is one. The handle must be closed
/// using XblSocialRelationshipResultCloseHandle when the result is no longer needed.
/// </summary>
typedef struct xbl_leaderboard_result* xbl_leaderboard_result_handle;


/// <summary>
/// Get a leaderboard for a title using stats 2017
/// </summary>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="leaderboardQuery">The query parameters of the leaderboard request.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Calls V4 GET 
/// https://leaderboards.xboxlive.com/users/xuid({xuid})/scids/{scid}/stats/{statname}/people/{all|favorites}[?sort=descending|ascending]&amp;skipToUser={skipToUser}
/// </remarks>
STDAPI XblLeaderboardGetLeaderboard2017Async(
    _Inout_ AsyncBlock* async,
    _In_ xbl_context_handle xboxLiveContext,
    _In_ XblLeaderboard2017Query leaderboardQuery
    ) XBL_NOEXCEPT;

/// <summary>
/// Get the result for a completed XblLeaderboardGetLeaderboard2017Async operation.
/// </summary>
/// <param name="async">The same AsyncBlock that passed to XblLeaderboardGetLeaderboard2017Async.</param>
/// <param name="handle">Returns a xbl_leaderboard_result_handle.
/// Call XblLeaderboardResultGetData to get the data from the handle, and call 
/// XblLeaderboardResultCloseHandle when done with this handle.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblLeaderboardGetLeaderboard2017GetResult(
    _Inout_ AsyncBlock* async,
    _Out_ xbl_leaderboard_result_handle* handle
    ) XBL_NOEXCEPT;

/// <summary>
/// Get a leaderboard for a title using stats 2013
/// </summary>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="leaderboardQuery">The query parameters of the leaderboard request.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Calls V4 GET 
/// https://leaderboards.xboxlive.com/users/xuid({xuid})/scids/{scid}/stats/{statname}/people/{all|favorites}[?sort=descending|ascending]&amp;skipToUser={skipToUser}
/// </remarks>
STDAPI XblLeaderboardGetLeaderboard2013Async(
    _Inout_ AsyncBlock* async,
    _In_ xbl_context_handle xboxLiveContext,
    _In_ XblLeaderboard2013Query leaderboardQuery
    ) XBL_NOEXCEPT;

/// <summary>
/// Get the result for a completed XblLeaderboardGetLeaderboard2013Async operation.
/// </summary>
/// <param name="async">The same AsyncBlock that passed to XblLeaderboardGetLeaderboard2013Async.</param>
/// <param name="handle">Returns a xbl_leaderboard_result_handle.
/// Call XblLeaderboardResultGetData to get the data from the handle, and call 
/// XblLeaderboardResultCloseHandle when done with this handle.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblLeaderboardGetLeaderboard2013GetResult(
    _Inout_ AsyncBlock* async,
    _Out_ xbl_leaderboard_result_handle* handle
    ) XBL_NOEXCEPT;

/// <summary>
/// Checks if there are more pages of the leaderboard to retrieve from the service.
/// </summary>
/// <param name="resultHandle">Leaderboard result handle.</param>
/// <param name="hasNext">Return value. True if there are more results to retrieve, false otherwise.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblLeaderboardResultHasNext(
    _In_ xbl_leaderboard_result_handle resultHandle,
    _Out_ bool* hasNext
    ) XBL_NOEXCEPT;

/// <summary>
/// Gets the result of next page of the leaderboard for a player of the specified title.
/// To get the result, call XblLeaderboardResultGetNextResult inside the AsyncBlock callback
/// or after the AsyncBlock is complete.
/// </summary>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="resultHandle">Handle to the leaderboard result.</param>
/// <param name="maxItems">The maximum number of items that the result can contain. Pass 0 to attempt
/// to retrieve all items.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblLeaderboardResultGetNextAsync(
    _Inout_ AsyncBlock* async,
    _In_ xbl_context_handle xboxLiveContext,
    _In_ xbl_leaderboard_result_handle resultHandle,
    _In_ uint32_t maxItems
    ) XBL_NOEXCEPT;

/// <summary>
/// Get a list of leaderboard row objects.
/// The memory to this list is freed when the xbl_leaderboard_result_handle is closed 
/// with XblLeaderboardResultCloseHandle
/// </summary>
/// <param name="resultHandle">Leaderboard result handle.</param>
/// <param name="achievements">Pointer to an array of XblLeaderboardRow objects.</param>
/// <param name="achievementsCount">The count of objects in the returned array.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblLeaderboardResultGetRows(
    _In_ xbl_leaderboard_result_handle resultHandle,
    _Out_ XblLeaderboardRow** rows,
    _Out_ uint32_t* rowCount
    ) XBL_NOEXCEPT;

/// <summary>
/// Get a list of leaderboard column objects.
/// The memory to this list is freed when the xbl_leaderboard_result_handle is closed 
/// with XblLeaderboardResultCloseHandle
/// </summary>
/// <param name="resultHandle">Leaderboard result handle.</param>
/// <param name="achievements">Pointer to an array of XblLeaderboardColumn objects.</param>
/// <param name="achievementsCount">The count of objects in the returned array.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblLeaderboardResultGetColumns(
    _In_ xbl_leaderboard_result_handle resultHandle,
    _Out_ XblLeaderboardColumn** columns,
    _Out_ uint32_t* columnCount
    ) XBL_NOEXCEPT;

/// <summary>
/// Duplicates a xbl_leaderboard_result_handle
/// </summary>
/// <param name="handle">The xbl_leaderboard_result_handle to duplicate.</param>
/// <returns>Returns the duplicated handle.</returns>
STDAPI_(xbl_leaderboard_result_handle) XblLeaderboardResultDuplicateHandle(
    _In_ xbl_leaderboard_result_handle handle
    ) XBL_NOEXCEPT;

/// <summary>
/// Closes the xbl_leaderboard_result_handle.
/// When all outstanding handles have been closed, the memory associated with the leaderboard result will be freed.
/// </summary>
/// <param name="handle">The xbl_leaderboard_result_handle to close.</param>
STDAPI_(void) XblLeaderboardResultCloseHandle(
    _In_ xbl_leaderboard_result_handle handle
    ) XBL_NOEXCEPT;


