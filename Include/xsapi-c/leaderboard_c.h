// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#if !defined(__cplusplus)
#error C++11 required
#endif

#pragma once

extern "C"
{
    
/// <summary>
/// Enumerates the data type of a leaderboard statistic.
/// </summary>
/// <memof><see cref="XblLeaderboardColumn"/></memof>
enum class XblLeaderboardStatType : uint32_t
{
    /// <summary>
    /// Unsigned 64 bit integer.
    /// </summary>
    Uint64,

    /// <summary>
    /// Boolean.
    /// </summary>
    Boolean,

    /// <summary>
    /// Double.
    /// </summary>
    Double,

    /// <summary>
    /// String.
    /// </summary>
    String,

    /// <summary>
    /// Unknown.
    /// </summary>
    Other
};

/// <summary> 
/// The order to sort the leaderboard in.
/// </summary>
/// <memof><see cref="XblLeaderboardQuery"/></memof>
enum class XblLeaderboardSortOrder : uint32_t
{
    /// <summary> 
    /// Sorting the leaderboard highest to lowest.
    /// </summary>
    Descending,

    /// <summary> 
    /// Sorting the leaderboard lowest to highest.
    /// </summary>
    Ascending
};

/// <summary> 
/// Predefined Xbox Live social groups.
/// </summary>
/// <memof><see cref="XblLeaderboardQuery"/></memof>
enum class XblSocialGroupType : uint32_t
{
    /// <summary> 
    /// No social group.
    /// </summary>
    None,

    /// <summary> 
    /// Social group for the people followed.
    /// </summary>
    People,

    /// <summary> 
    /// Social group for the people tagged as favorites.
    /// </summary>
    Favorites
};

/// <summary>
/// Enum used to specify the type of leaderboard in a Leaderboard query.
/// </summary>
/// <memof><see cref="XblLeaderboardQuery"/></memof>
enum class XblLeaderboardQueryType : uint32_t
{
    /// <summary>
    /// A leaderboard based an event based user stat.
    /// </summary>
    UserStatBacked = 0,

    /// <summary>
    /// A global leaderboard backed by a title managed stat.
    /// </summary>
    TitleManagedStatBackedGlobal = 1,

    /// <summary>
    /// A social leaderboard backed by a title managed stat.
    /// </summary>
    TitleManagedStatBackedSocial = 2,
};


/// <summary>
/// Represents a column in a collection of leaderboard items.
/// </summary>
/// <memof><see cref="XblLeaderboardResult"/></memof>
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
/// Represents a row in a collection of leaderboard items.
/// </summary>
/// <memof><see cref="XblLeaderboardResult"/></memof>
typedef struct XblLeaderboardRow 
{
    /// <summary>
    /// The UTF-8 encoded gamertag of the player.
    /// </summary>
    _Field_z_ char gamertag[XBL_GAMERTAG_CHAR_SIZE];

    /// <summary>
    /// The UTF-8 encoded modern gamertag for the player.  
    /// Not guaranteed to be unique.
    /// </summary>
    char modernGamertag[XBL_MODERN_GAMERTAG_CHAR_SIZE];

    /// <summary>
    /// The UTF-8 encoded suffix appended to modern gamertag to ensure uniqueness.  
    /// May be empty in some cases.
    /// </summary>
    char modernGamertagSuffix[XBL_MODERN_GAMERTAG_SUFFIX_CHAR_SIZE];

    /// <summary>
    /// The UTF-8 encoded unique modern gamertag and suffix. Format will be "modernGamertag#suffix".  
    /// Guaranteed to be no more than 16 rendered characters.
    /// </summary>
    char uniqueModernGamertag[XBL_UNIQUE_MODERN_GAMERTAG_CHAR_SIZE];

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
    /// The global rank of the player. If globalrank is 0, then the user has no global rank.
    /// </summary>
    uint32_t globalRank;

    /// <summary>
    /// UTF-8 encoded JSON values for each column in the leaderboard row for the player.
    /// </summary>
    _Field_z_ const char** columnValues;

    /// <summary>
    /// The count of string in socialNetworks array.
    /// </summary>
    size_t columnValuesCount;

} XblLeaderboardRow;

/// <summary>
/// Represents the parameters for submitting a leaderboard query using event-based or title-based stats.
/// </summary>
/// <memof><see cref="XblLeaderboardResult"/></memof>
/// <argof><see cref="XblLeaderboardGetLeaderboardAsync"/></argof>
typedef struct XblLeaderboardQuery
{
    /// <summary>
    /// Optional Xbox user ID of the requesting user.  
    /// If doing a global leaderboard, then set to 0.
    /// </summary>
    uint64_t xboxUserId;

    /// <summary>
    /// The UTF-8 encoded service configuration ID (SCID) of the title.
    /// </summary>
    _Field_z_ char scid[XBL_SCID_LENGTH];

    /// <summary>
    /// Optional UTF-8 encoded leaderboard name to get a leaderboard for.
    /// Set to nullptr if querying a social leaderboard or a title managed stat backed leaderboard.
    /// </summary>
    _Field_z_ const char* leaderboardName;

    /// <summary>
    /// Optional UTF-8 encoded statistic name to get a leaderboard for.
    /// Used when querying a social leaderboard or title managed stat backed leaderboard.
    /// </summary>
    _Field_z_ const char* statName;

    /// <summary>
    /// Optional the social group of users to get leaderboard results.  
    /// For example, to get a "friends only" leaderboard.  
    /// Set to XblSocialGroupType_None to get a global leaderboard.
    /// </summary>
    XblSocialGroupType socialGroup;

    /// <summary>
    /// Optional UTF-8 encoded array of names of stats for the additional columns.
    /// </summary>
    _Field_z_ const char** additionalColumnleaderboardNames;

    /// <summary>
    /// Optional count of additionalColumnleaderboardNames passed in.
    /// </summary>
    size_t additionalColumnleaderboardNamesCount;

    /// <summary>
    /// Set sort order for the resulting leaderboard.
    /// </summary>
    XblLeaderboardSortOrder order;

    /// <summary>
    /// Set maximum items that the resulting leaderboard will contain.  
    /// Set to 0 to let the service return the default number of max items.
    /// </summary>
    uint32_t maxItems;

    /// <summary>
    /// Set the resulting leaderboard will start with the 
    /// user that requested the leaderboard.  
    /// Set to 0 to not skip to a user.
    /// </summary>
    uint64_t skipToXboxUserId;

    /// <summary>
    /// Set which rank the resulting leaderboard will start at.  
    /// Set 0 to not skip to a specific rank.
    /// </summary>
    uint32_t skipResultToRank;

    /// <summary>
    /// The UTF-8 encoded continuationToken used to get the next set of leaderboard data.
    /// </summary>
    _Field_z_ const char* continuationToken;

    /// <summary>
    /// The type of leaderboard to query for.
    /// </summary>
    XblLeaderboardQueryType queryType;
} XblLeaderboardQuery;

/// <summary>
/// Represents the results of a leaderboard request.
/// </summary>
/// <argof><see cref="XblLeaderboardGetLeaderboardResult"/></argof>
/// <argof><see cref="XblLeaderboardResultGetNextAsync"/></argof>
/// <argof><see cref="XblLeaderboardResultGetNextResult"/></argof>
typedef struct XblLeaderboardResult 
{
    /// <summary>
    /// The total number of rows in the leaderboard results.
    /// </summary>
    uint32_t totalRowCount;

    /// <summary>
    /// The collection of columns in the leaderboard results.
    /// </summary>
    XblLeaderboardColumn* columns;

    /// <summary>
    /// The number of **columns**.
    /// </summary>
    size_t columnsCount;

    /// <summary>
    /// The collection of rows in the leaderboard results.
    /// </summary>
    XblLeaderboardRow* rows;

    /// <summary>
    /// The number of **rows**.
    /// </summary>
    size_t rowsCount;

    /// <summary>
    /// Indicates whether there is a next page of results.
    /// </summary>
    bool hasNext;

    /// <summary>
    /// The next query.  
    /// Internal use only.
    /// </summary>
    XblLeaderboardQuery nextQuery;
} XblLeaderboardResult;

/// <summary>
/// Get a leaderboard for a title using event-based or title-based stats.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="leaderboardQuery">The query parameters of the leaderboard request.</param>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <rest>V4 GET https://leaderboards.xboxlive.com/users/xuid({xuid})/scids/{scid}/stats/{statname}/people/{all|favorites}[?sort=descending|ascending]&amp;skipToUser={skipToUser} </rest>
STDAPI XblLeaderboardGetLeaderboardAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_ XblLeaderboardQuery leaderboardQuery,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Get the result size for an XblLeaderboardGetLeaderboardAsync call.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="resultSizeInBytes">The size in bytes required to store the user statistics result.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblLeaderboardGetLeaderboardResultSize(
    _In_ XAsyncBlock* async,
    _Out_ size_t* resultSizeInBytes
) XBL_NOEXCEPT;

/// <summary>
/// Get the result for an XblLeaderboardGetLeaderboardAsync call.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="bufferSize">The size of the provided buffer.  
/// Use <see cref="XblLeaderboardGetLeaderboardResultSize"/> to get the size required.</param>
/// <param name="buffer">A caller allocated byte buffer to write result into.</param>
/// <param name="ptrToBuffer">Strongly typed pointer that points into buffer.  
/// Do not free this as its lifecycle is tied to buffer.</param>
/// <param name="bufferUsed">Number of bytes written to the buffer.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblLeaderboardGetLeaderboardResult(
    _In_ XAsyncBlock* async,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_(bufferSize, *bufferUsed) void* buffer,
    _Outptr_ XblLeaderboardResult** ptrToBuffer,
    _Out_opt_ size_t* bufferUsed
) XBL_NOEXCEPT;

/// <summary>
/// Gets the result of next page of the leaderboard for a player of the specified title.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="leaderboardResult">The leaderboard result from a previous call to XblLeaderboardGetLeaderboardResult.</param>
/// <param name="maxItems">The maximum number of items that the result can contain.  
/// Pass 0 to attempt to retrieve all items.</param>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// To get the result, call <see cref="XblLeaderboardResultGetNextResult"/> inside the AsyncBlock callback
/// or after the AsyncBlock is complete.
/// </remarks>
STDAPI XblLeaderboardResultGetNextAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_ XblLeaderboardResult* leaderboardResult,
    _In_ uint32_t maxItems,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Get the result size for an XblLeaderboardResultGetNextAsync call.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="resultSizeInBytes">The size in bytes required to store the user statistics result.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblLeaderboardResultGetNextResultSize(
    _In_ XAsyncBlock* async,
    _Out_ size_t* resultSizeInBytes
) XBL_NOEXCEPT;

/// <summary>
/// Get the result for an XblLeaderboardResultGetNextAsync call.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="bufferSize">The size of the provided buffer.  
/// Use <see cref="XblLeaderboardResultGetNextResultSize"/> to get the size required.</param>
/// <param name="buffer">A caller allocated byte buffer to write result into.</param>
/// <param name="ptrToBuffer">Strongly typed pointer that points into buffer.  
/// Do not free this as its lifecycle is tied to buffer.</param>
/// <param name="bufferUsed">Number of bytes written to the buffer.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblLeaderboardResultGetNextResult(
    _In_ XAsyncBlock* async,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_(bufferSize, *bufferUsed) void* buffer,
    _Outptr_ XblLeaderboardResult** ptrToBuffer,
    _Out_opt_ size_t* bufferUsed
) XBL_NOEXCEPT;

} // end extern c
