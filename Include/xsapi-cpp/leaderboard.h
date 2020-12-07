// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "types.h"
#include <cstdint>
#include <vector>
#include "xbox_live_app_config.h"
#include "xsapi-c/leaderboard_c.h"

#define NO_SKIP_XUID (_T(""))
#define NO_XUID (_T(""))
#define NO_SOCIAL_GROUP (_T(""))
#define NO_SKIP_RANK (0)
#define NO_MAX_ITEMS (0)
#define NO_CONTINUATION (_T(""))
#define NO_SORT_ORDER (_T(""))

namespace xbox { namespace services {
    class xbox_live_context;

    /// <summary>
    /// Contains classes and enumerations that let you retrieve 
    /// leaderboard information from Xbox Live.
    /// </summary>
    namespace leaderboard {

/// <summary>Enumerates the data type of a leaderboard statistic.</summary>
enum class leaderboard_stat_type
{
    /// <summary>Unsigned 64 bit integer.</summary>
    stat_uint64,

    /// <summary>Boolean.</summary>
    stat_boolean,

    /// <summary>Double.</summary>
    stat_double,

    /// <summary>String.</summary>
    stat_string,

    /// <summary>Unknown.</summary>
    stat_other
};

#if !XSAPI_NO_PPL

/// <summary>
/// Represents a column in a collection of leaderboard items.
/// </summary>
class leaderboard_column
{
public:
    inline leaderboard_column(
        std::shared_ptr<char> buffer, 
        const XblLeaderboardColumn& leaderboardColumn
    );

    /// <summary>
    /// The name the statistic displayed in the column.
    /// </summary>
    inline string_t stat_name() const;

    /// <summary>
    /// The property type of the statistic in the column.
    /// </summary>
    inline leaderboard_stat_type stat_type() const;

private:
    std::shared_ptr<char> m_buffer;
    XblLeaderboardColumn m_leaderboardColumn;
};

/// <summary>
/// Represents a row in a collection of leaderboard items.
/// </summary>
class leaderboard_row
{
public:
    inline leaderboard_row(
        std::shared_ptr<char> buffer, 
        const XblLeaderboardRow& leaderboardRow
    );

    /// <summary>
    /// The Gamertag of the player.
    /// </summary>
    inline string_t gamertag() const;

    /// <summary>
    /// The Xbox user ID of the player.
    /// </summary>
    inline string_t xbox_user_id() const;

    /// <summary>
    /// The percentile rank of the player.
    /// </summary>
    inline double percentile() const;

    /// <summary>
    /// The rank of the player.
    /// </summary>
    inline uint32_t rank() const;

    /// <summary>
    /// Values for each column in the leaderboard row for the player.
    /// </summary>
    inline std::vector<string_t> column_values() const;

private:
    std::shared_ptr<char> m_buffer;
    XblLeaderboardRow m_leaderboardRow;
};

/// <summary>
/// Represents the results of a leaderboard request.
/// </summary>
class leaderboard_result
{
public:
    inline leaderboard_result() { }
    inline leaderboard_result(
        std::shared_ptr<char> buffer,
        const XblContextHandle& xblContext
    );
        
    /// <summary>
    /// The total number of rows in the leaderboard results.
    /// </summary>
    inline uint32_t total_row_count() const;

    /// <summary>
    /// The collection of columns in the leaderboard results.
    /// </summary>
    inline const std::vector<leaderboard_column>& columns() const;

    /// <summary>
    /// The collection of rows in the leaderboard results.
    /// </summary>
    inline const std::vector<leaderboard_row>& rows() const;

    /// <summary>
    /// Indicates whether there is a next page of results.
    /// </summary>
    /// <returns>True if there is another page of results; otherwise false.</returns>
    inline bool has_next() const;

#if !defined(XBOX_LIVE_CREATORS_SDK)
    /// <summary>
    /// Get the next page of a previous leaderboard call using the same service config Id and leaderboard name.
    /// </summary>
    /// <param name="maxItems">The maximum number of items to return.</param>
    /// <returns>A leaderboard_results object that contains the next set of results.</returns>
    /// <remarks>
    /// This query is only to be used to retrieve a leaderboard in a pre stats 2017 system
    /// Returns a concurrency::task&lt;T&gt; object that represents the state of the asynchronous operation.
    ///
    /// Calls V1 GET /scids/{scid}/leaderboards/{leaderboardname}?
    ///  [maxItems={maxItems}]
    ///  [continuationToken={token}]
    /// </remarks>
    inline pplx::task<xbox_live_result<leaderboard_result>> get_next(_In_ uint32_t maxItems);
#endif

private:
    std::shared_ptr<char> m_buffer;
    XblLeaderboardResult m_leaderboardResult{};
    XblContextHandle m_xblContext{};
    std::vector<leaderboard_column> m_columns;
    std::vector<leaderboard_row> m_rows;
};

#if !defined(XBOX_LIVE_CREATORS_SDK)
/// <summary>
/// Represents the leaderboard service.
/// </summary>
class leaderboard_service
{
public:
    /// <summary>
    /// Get a leaderboard for a single leaderboard given a service configuration ID and a leaderboard name.
    /// </summary>
    /// <param name="scid">The service configuration ID (SCID) of the title</param>
    /// <param name="name">The name of the leaderboard.</param>
    /// <param name="additionalColumnNames">The name of the stats for the additionalColumns. (Optional)</param>
    /// <returns>
    /// A leaderboard_result object containing a collection of the leaderboard columns and rows,
    /// ordered by player rank descending.
    /// </returns>
    /// <remarks>
    /// Returns a concurrency::task&lt;T&gt; object that represents the state of the asynchronous operation.
    ///
    /// Calls V1 GET /scids/{scid}/leaderboards/{leaderboardname}
    /// </remarks>
    inline pplx::task<xbox_live_result<leaderboard_result>> get_leaderboard(
        _In_ const string_t& scid,
        _In_ const string_t& name,
        _In_ const std::vector<string_t>& additionalColumnNames = std::vector<string_t>()
        );

    /// <summary>
    /// Get a leaderboard for a single leaderboard given a service configuration ID and a leaderboard name.
    /// </summary>
    /// <param name="scid">The service configuration ID (SCID) of the title</param>
    /// <param name="name">The name of the leaderboard.</param>
    /// <param name="xuid">The Xbox user ID of the requesting user.</param>
    /// <param name="socialGroup">The name of the group of users to get get leaderboard results for.
    /// See Microsoft::Xbox::Services::Social::SocialGroupConstants for the latest options.</param>
    /// <param name="additionalColumnNames">The name of the stats for the additionalColumns. (Optional)</param>
    /// <returns>
    /// A leaderboard_result object containing a collection of the leaderboard columns and rows,
    /// ordered by player rank descending.
    /// </returns>
    /// <remarks>
    /// Returns a concurrency::task&lt;T&gt; object that represents the state of the asynchronous operation.
    ///
    /// Calls V3 GET /scids/{scid}/leaderboards/{leaderboardname}?include=valuemetadata&amp;xuid={xuid}&amp;viewTarget=people&amp;view=people
    /// </remarks>
    inline pplx::task<xbox_live_result<leaderboard_result>> get_leaderboard(
        _In_ const string_t& scid,
        _In_ const string_t& name,
        _In_ const string_t& xuid,
        _In_ const string_t& socialGroup,
        _In_ uint32_t maxItems = 0,
        _In_ const std::vector<string_t>& additionalColumnNames = std::vector<string_t>()
        );

    /// <summary>
    /// Get a page of leaderboard results for a single leaderboard given a service configuration ID
    /// and a leaderboard name.
    /// </summary>
    /// <param name="scid">The service configuration ID (SCID) of the title</param>
    /// <param name="name">The name of the leaderboard.</param>
    /// <param name="skipToRank">The number of ranks to skip before returning results.</param>
    /// <param name="maxItems">The maximum number of items to retrieve. If this value is 0, the server defaults to 10. (Optional)</param>
    /// <param name="additionalColumnNames">The name of the stats for the additionalColumns. (Optional)</param>
    /// <returns>A leaderboard_result object containing a collection of the leaderboard columns and rows.</returns>
    /// <remarks>
    /// Returns a concurrency::task&lt;T&gt; object that represents the state of the asynchronous operation.
    ///
    /// Calls V1 GET 
    /// /scids/{scid}/leaderboards/{leaderboardname}?[&amp;skipItems={skipItems}][&amp;maxItems={maxItems}]
    /// </remarks>
    inline pplx::task<xbox_live_result<leaderboard_result>> get_leaderboard(
        _In_ const string_t& scid,
        _In_ const string_t& name,
        _In_ uint32_t skipToRank,
        _In_ uint32_t maxItems = 0,
        _In_ const std::vector<string_t>& additionalColumnNames = std::vector<string_t>()
        );

    /// <summary>
    /// Get a page of leaderboard results for a single leaderboard given a service configuration ID
    /// and a leaderboard name.
    /// </summary>
    /// <param name="scid">The service configuration ID (SCID) of the title</param>
    /// <param name="name">The name of the leaderboard.</param>
    /// <param name="skipToRank">The number of ranks to skip before returning results.</param>
    /// <param name="xuid">The Xbox user ID of the requesting user.</param>
    /// <param name="socialGroup">The name of the group of users to get get leaderboard results for.
    /// See Microsoft::Xbox::Services::Social::SocialGroupConstants for the latest options.</param>
    /// <param name="maxItems">The maximum number of items to retrieve. If this value is 0, the server defaults to 10. (Optional)</param>
    /// <param name="additionalColumnNames">The name of the stats for the additionalColumns. (Optional)</param>
    /// <returns>A leaderboard_result object containing a collection of the leaderboard columns and rows.</returns>
    /// <remarks>
    /// Returns a concurrency::task&lt;T&gt; object that represents the state of the asynchronous operation.
    ///
    /// Calls V3 GET 
    /// /scids/{scid}/leaderboards/{leaderboardname}?[&amp;skipItems={skipItems}][&amp;maxItems={maxItems}]&amp;include=valuemetadata&amp;xuid={xuid}&amp;viewTarget=people&amp;view=people
    /// </remarks>
    inline pplx::task<xbox_live_result<leaderboard_result>> get_leaderboard(
        _In_ const string_t& scid,
        _In_ const string_t& name,
        _In_ uint32_t skipToRank,
        _In_ const string_t& xuid,
        _In_ const string_t& socialGroup,
        _In_ uint32_t maxItems = 0,
        _In_ const std::vector<string_t>& additionalColumnNames = std::vector<string_t>()
        );

    /// <summary>
    /// Get a leaderboard starting at a specified player, regardless of the player's rank or score, ordered by
    /// the player's percentile rank.
    /// </summary>
    /// <param name="scid">The service configuration ID (SCID) of the title</param>
    /// <param name="name">The name of the leaderboard.</param>
    /// <param name="skipToXuid">The Xbox user ID of the player to skip to before retrieving results.</param>
    /// <param name="maxItems">The maximum number of items to retrieve. If this value is 0, the server defaults to 10. (Optional)</param>
    /// <param name="additionalColumnNames">The name of the stats for the additionalColumns. (Optional)</param>
    /// <returns>
    /// A leaderboard_result object containing a collection of the leaderboard columns and rows.
    /// The result page is ordered by percentile rank, with the specified player in the last position of
    /// the page for predefined views, or in the middle for stat leaderboard views.
    /// </returns>
    /// <remarks>
    /// Returns a concurrency::task&lt;T&gt; object that represents the state of the asynchronous operation.
    ///
    /// There is no continuation Token provided for this query.
    ///
    /// Calls V1 GET 
    /// /scids/{scid}/leaderboards/{leaderboardname}?[&amp;skipToUser={xuid}][&amp;maxItems={maxItems}]
    /// </remarks>
    inline pplx::task<xbox_live_result<leaderboard_result>> get_leaderboard_skip_to_xuid(
        _In_ const string_t& scid,
        _In_ const string_t& name,
        _In_ const string_t& skipToXuid = string_t(),
        _In_ uint32_t maxItems = 0,
        _In_ const std::vector<string_t>& additionalColumnNames = std::vector<string_t>()
        );


    /// <summary>
    /// Get a leaderboard starting at a specified player, regardless of the player's rank or score, ordered by
    /// the player's percentile rank.
    /// </summary>
    /// <param name="scid">The service configuration ID (SCID) of the title</param>
    /// <param name="name">The name of the leaderboard.</param>
    /// <param name="xuid">The Xbox user ID of the requesting user.</param>
    /// <param name="socialGroup">The name of the group of users to get get leaderboard results for.
    /// See Microsoft::Xbox::Services::Social::SocialGroupConstants for the latest options.</param>
    /// <param name="skipToXuid">The Xbox user ID of the player to skip to before retrieving results.</param>
    /// <param name="sortOrder">A value indicating the sort order for the returned leaderboard result.
    /// The possible values are 'ascending' or 'descending', without quotes.</param>
    /// <param name="maxItems">The maximum number of items to retrieve. If this value is 0, the server defaults to 10. (Optional)</param>
    /// <param name="additionalColumnNames">The name of the stats for the additionalColumns. (Optional)</param>
    /// <returns>
    /// A leaderboard_result object containing a collection of the leaderboard columns and rows.
    /// The result page is ordered by percentile rank, with the specified player in the last position of
    /// the page for predefined views, or in the middle for stat leaderboard views.
    /// </returns>
    /// <remarks>
    /// Returns a concurrency::task&lt;T&gt; object that represents the state of the asynchronous operation.
    ///
    /// There is no continuation Token provided for this query.
    ///
    /// Calls V1 GET 
    ///  /scids/{scid}/leaderboards/{leaderboardname}?[&skipToUser={xuid}][&maxItems={maxItems}]
    /// </remarks>
    inline pplx::task<xbox_live_result<leaderboard_result>> get_leaderboard_skip_to_xuid(
        _In_ const string_t& scid,
        _In_ const string_t& name,
        _In_ const string_t& xuid,
        _In_ const string_t& socialGroup,
        _In_ const string_t& skipToXuid,
        _In_ uint32_t maxItems = 0,
        _In_ const std::vector<string_t>& additionalColumnNames = std::vector<string_t>()
        );

    /// <summary>
    /// Get an unsorted leaderboard that shows members of a specified social group.
    /// </summary>
    /// <param name="xuid">The Xbox user ID of the requesting user.</param>
    /// <param name="scid">The service configuration ID (SCID) of the title</param>
    /// <param name="statName">The name of the statistic to get a leaderboard for.</param>
    /// <param name="socialGroup">The name of the group of users to get get leaderboard results for.  
    /// You can pass either "Favorites" or "People"</param>
    /// <param name="maxItems">The maximum number of items to retrieve. If this value is 0, the server defaults to 10. (Optional)</param>
    /// <returns>
    /// A LeaderboardResult object containing a collection of the leaderboard columns and rows.
    /// </returns>
    /// <remarks>
    /// Returns a concurrency::task&lt;T&gt; object that represents the state of the asynchronous operation.
    ///
    /// Calls V1 GET 
    /// https://leaderboards.xboxlive.com/users/xuid({xuid})/scids/{scid}/stats/{statname}/people/{all|favorites}
    /// </remarks>
    inline pplx::task<xbox_live_result<leaderboard_result>> get_leaderboard_for_social_group(
        _In_ const string_t& xuid,
        _In_ const string_t& scid,
        _In_ const string_t& statName,
        _In_ const string_t& socialGroup,
        _In_ uint32_t maxItems = 0
        );

    /// <summary>
    /// Get a sorted leaderboard that shows members of a specified social group.
    /// </summary>
    /// <param name="xuid">The Xbox user ID of the requesting user.</param>
    /// <param name="scid">The service configuration ID (SCID) of the title</param>
    /// <param name="statName">The name of the statistic to get a leaderboard for.</param>
    /// <param name="socialGroup">The name of the group of users to get get leaderboard results for.  
    /// You can pass either "Favorites" or "People"</param>
    /// <param name="sortOrder">A value indicating the sort order for the returned leaderboard result.
    /// The possible values are 'ascending' or 'descending', without quotes.</param>
    /// <param name="maxItems">The maximum number of items to retrieve. If this value is 0, the server defaults to 10. (Optional)</param>
    /// <returns>An object containing a collection of the leaderboard columns and rows</returns>
    /// <remarks>
    /// Returns a concurrency::task&lt;T&gt; object that represents the state of the asynchronous operation.
    ///
    /// Calls V1 GET 
    /// https://leaderboards.xboxlive.com/users/xuid({xuid})/scids/{scid}/stats/{statname}/people/{all|favorites}[?sort=descending|ascending]
    /// </remarks>
    inline pplx::task<xbox_live_result<leaderboard_result>> get_leaderboard_for_social_group(
        _In_ const string_t& xuid,
        _In_ const string_t& scid,
        _In_ const string_t& statName,
        _In_ const string_t& socialGroup,
        _In_ const string_t& sortOrder,
        _In_ uint32_t maxItems = 0
        );

    /// <summary>
    /// Get a sorted leaderboard, starting at a specified rank, that shows members of a specified social group.
    /// </summary>
    /// <param name="xuid">The Xbox user ID of the requesting user.</param>
    /// <param name="scid">The service configuration ID (SCID) of the title</param>
    /// <param name="statName">The name of the statistic to get a leaderboard for.</param>
    /// <param name="socialGroup">The name of the group of users to get get leaderboard results for.
    /// See Microsoft::Xbox::Services::Social::SocialGroupConstants for the latest options.</param>
    /// <param name="skipToRank">The number of ranks to skip before retrieving results.</param>
    /// <param name="sortOrder">A value indicating the sort order for the returned leaderboard result.
    /// The possible values are 'ascending' or 'descending', without quotes.</param>
    /// <param name="maxItems">The maximum number of items to retrieve. If this value is 0, the server defaults to 10. (Optional)</param>
    /// <param name="additionalColumnNames">The name of the stats for the additionalColumns. (Optional)</param>
    /// <returns>
    /// A leaderboard_result object containing a collection of the leaderboard columns and rows.
    /// </returns>
    /// <remarks>
    /// Returns a concurrency::task&lt;T&gt; object that represents the state of the asynchronous operation.
    ///
    /// Calls V1 GET 
    /// https://leaderboards.xboxlive.com/users/xuid({xuid})/scids/{scid}/stats/{statname}/people/{all|favorites}[?sort=descending|ascending]&amp;skipToRank={skipToUser}
    /// </remarks>
    inline pplx::task<xbox_live_result<leaderboard_result>> get_leaderboard_for_social_group_skip_to_rank(
        _In_ const string_t& xuid,
        _In_ const string_t& scid,
        _In_ const string_t& statName,
        _In_ const string_t& socialGroup,
        _In_ uint32_t skipToRank,
        _In_ const string_t& sortOrder,
        _In_ uint32_t maxItems = 0
        );

    /// <summary>
    /// Get a sorted leaderboard, starting at a specified player, that shows members of a specified social group.
    /// </summary>
    /// <param name="xuid">The Xbox user ID of the requesting user.</param>
    /// <param name="scid">The service configuration ID (SCID) of the title</param>
    /// <param name="statName">The name of the statistic to get a leaderboard for.</param>
    /// <param name="socialGroup">The name of the group of users to get get leaderboard results for.
    /// See Microsoft::Xbox::Services::Social::SocialGroupConstants for the latest options.</param>
    /// <param name="skipToXuid">The Xbox user ID of the player to skip to before retrieving results.</param>
    /// <param name="sortOrder">A value indicating the sort order for the returned leaderboard result.
    /// The possible values are 'ascending' or 'descending', without quotes.</param>
    /// <param name="maxItems">The maximum number of items to retrieve. If this value is 0, the server defaults to 10. (Optional)</param>
    /// <returns>
    /// A leaderboard_result object that contains a page of leaderboard results around the specified player regardless
    /// of that player's rank or score.
    /// The result page is ordered by percentile rank, with the specified player in the last position of
    /// the page for predefined views, or in the middle for stat leaderboard views.
    /// </returns>
    /// <remarks>
    /// Returns a concurrency::task&lt;T&gt; object that represents the state of the asynchronous operation.
    ///
    /// Calls V1 GET 
    /// https://leaderboards.xboxlive.com/users/xuid({xuid})/scids/{scid}/stats/{statname}/people/{all|favorites}[?sort=descending|ascending]&amp;skipToUser={skipToUser}
    /// </remarks>
    inline pplx::task<xbox_live_result<leaderboard_result>> get_leaderboard_for_social_group_skip_to_xuid(
        _In_ const string_t& xuid,
        _In_ const string_t& scid,
        _In_ const string_t& statName,
        _In_ const string_t& socialGroup,
        _In_ const string_t& skipToXuid,
        _In_ const string_t& sortOrder,
        _In_ uint32_t maxItems = 0
        );

    inline leaderboard_service(const leaderboard_service& other);
    inline leaderboard_service& operator=(leaderboard_service other);
    inline ~leaderboard_service();

private:
    inline leaderboard_service(_In_ XblContextHandle contextHandle);
    XblContextHandle m_xblContext;
       
    inline XblSocialGroupType XblSocialGroupTypeFromString(string_t socialGroup);
    inline XblLeaderboardSortOrder XblLeaderboardSortOrderFromString(string_t socialGroup);

    friend xbox_live_context;
};
#endif

#endif
}}}

#if !XSAPI_NO_PPL
#include "impl/leaderboard.hpp"
#endif
