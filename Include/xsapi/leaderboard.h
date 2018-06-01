// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "types.h"
#include <cstdint>
#include <vector>
#include "xbox_live_app_config.h"

#define NO_SKIP_XUID (_T(""))
#define NO_XUID (_T(""))
#define NO_SOCIAL_GROUP (_T(""))
#define NO_SKIP_RANK (0)
#define NO_MAX_ITEMS (0)
#define NO_CONTINUATION (_T(""))
#define NO_SORT_ORDER (_T(""))


namespace xbox { namespace services {
    class xbox_live_context_impl;
    namespace stats {
        namespace manager {
            class stats_manager_impl;
        }
    }
    /// <summary>
    /// Contains classes and enumerations that let you retrieve 
    /// leaderboard information from Xbox Live.
    /// </summary>
    namespace leaderboard {

struct leaderboard_global_query;
struct leaderboard_social_query;
class leaderboard_result;

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

    /// <summary>
    /// DEPRECATED
    /// DateTime.
    /// </summary>
    stat_datetime,

    /// <summary>Unknown.</summary>
    stat_other
};

/// <summary> 
/// The order to sort the leaderboard in
/// </summary>
enum class sort_order
{
    /// <summary> 
    /// Sorting the Leaderboard lowest to highest
    /// </summary>
    ascending,

    /// <summary> 
    /// Sorting the Leaderboard highest to lowest
    /// </summary>
    descending
};

/// <summary>
/// Represents a column in a collection of leaderboard items.
/// </summary>
class leaderboard_column
{
public:
    leaderboard_column(
        _In_ string_t display_name,
        _In_ string_t statName,
        _In_ leaderboard_stat_type stat_type);

    /// <summary>
    /// DEPRECATED
    /// The display name of the column.
    /// </summary>
    _XSAPIIMP const string_t& display_name() const;

    /// <summary>
    /// The name the statistic displayed in the column.
    /// </summary>
    _XSAPIIMP const string_t& stat_name() const;

    /// <summary>
    /// The property type of the statistic in the column.
    /// </summary>
    _XSAPIIMP leaderboard_stat_type stat_type() const;

private:
    string_t m_displayName;
    string_t m_statName;
    leaderboard_stat_type m_stat_type;
};

/// <summary>
/// Represents a row in a collection of leaderboard items.
/// </summary>
class leaderboard_row
{
public:
    /// <summary>
    /// Internal function
    /// </summary>
    leaderboard_row(
        _In_ string_t gamertag,
        _In_ string_t xboxUserId,
        _In_ double percentile,
        _In_ uint32_t rank,
        _In_ std::vector<string_t> columnValues,
        _In_ string_t metadata);

    /// <summary>
    /// The Gamertag of the player.
    /// </summary>
    _XSAPIIMP const string_t& gamertag() const;

    /// <summary>
    /// The Xbox user ID of the player.
    /// </summary>
    _XSAPIIMP const string_t& xbox_user_id() const;

    /// <summary>
    /// The percentile rank of the player.
    /// </summary>
    _XSAPIIMP double percentile() const;

    /// <summary>
    /// The rank of the player.
    /// </summary>
    _XSAPIIMP uint32_t rank() const;

    /// <summary>
    /// Values for each column in the leaderboard row for the player.
    /// </summary>
    _XSAPIIMP const std::vector<string_t>& column_values() const;

private:
    string_t m_gamertag;
    string_t m_xboxUserId;
    double m_percentile;
    uint32_t m_rank;
    std::vector<string_t> m_columnValues;
    web::json::value m_metadata;

    friend leaderboard_result;
};

class leaderboard_query
{
public:
    /// <summary>
    /// Constructing a leaderboard_query object
    /// </summary>
    leaderboard_query();

    /// <summary>
    /// Set whether or not the resulting leaderboard will start with the 
    /// user that requested the leaderboard.
    /// </summary>
    void set_skip_result_to_me(_In_ bool skipResultToMe);

    /// <summary>
    /// Set Which rank the resulting leaderboard will start at
    /// </summary>
    void set_skip_result_to_rank(_In_ uint32_t skipResultToRank);

    /// <summary>
    /// Set maximum items that the resulting leaderboard will contain
    /// </summary>
    void set_max_items(_In_ uint32_t maxItems);

    /// <summary>
    /// Set sort order for the resulting leaderboard
    /// </summary>
    void set_order(_In_ sort_order order);

    /// <summary>
    /// Set the callback ID when this is used by stats_manager
    /// </summary>
    void set_callback_id(_In_ uint64_t callbackId);

    /// <summary>
    /// Set the callback ID when this is used by stats_manager
    /// </summary>
    uint64_t callback_id() const;

    /// <summary>
    /// Gets whether or not the resulting leaderboard will start with the 
    /// user that requested the leaderboard.
    /// </summary>
    bool skip_result_to_me() const;

    /// <summary>
    /// Gets Which rank the resulting leaderboard will start at
    /// </summary>
    uint32_t skip_result_to_rank() const;

    /// <summary>
    /// Gets maximum items that the resulting leaderboard will contain
    /// </summary>
    uint32_t max_items() const;

    /// <summary>
    /// Gets sort order for the resulting leaderboard
    /// </summary>
    sort_order order() const;

    /// <summary>
    /// Gets the stat name of the previous query. This property will only be set if its a query 
    /// gotten from get_next_query
    /// </summary>
    const string_t& stat_name() const;

    /// <summary>
    /// Gets the social group of the previous query. This property will only be set if its a query 
    /// gotten from get_next_query and the previous query was a social query
    /// </summary>
    const string_t& social_group() const;

    /// <summary>
    /// True if there is more data in the leaderboard
    /// If this is true then you can pass this object into the get_leaderboard call
    /// </summary>
    bool has_next() const;

    /// <summary>
    /// Internal Function
    /// </summary>
    const string_t& _Continuation_token() const;

    /// <summary>
    /// Internal Function
    /// </summary>
    void _Set_continuation_token(_In_ const string_t& continuationToken);

    /// <summary>
    /// Internal Function
    /// </summary>
    void _Set_stat_name(_In_ const string_t& statName);

    /// <summary>
    /// Internal Function
    /// </summary>
    void _Set_social_group(_In_ const string_t& socialGroup);

private:
    bool m_skipResultToMe;
    uint32_t m_skipResultToRank;
    uint32_t m_maxItems;
    sort_order m_order;
    string_t m_continuationToken;
    string_t m_statName;
    string_t m_socialGroup;
    uint64_t m_callbackId;
};

/// <summary>
/// Represents the results of a leaderboard request.
/// </summary>
class leaderboard_result
{
public:
    /// <summary>
    /// Creates a new leaderboard_result object.
    /// </summary>
    leaderboard_result();

    /// <summary>
    /// Internal function
    /// </summary>
    leaderboard_result(
        _In_ string_t displayName,
        _In_ uint32_t totalRowCount,
        _In_ string_t continuationToken,
        _In_ std::vector<leaderboard_column> columns,
        _In_ std::vector<leaderboard_row> rows,
        _In_ std::shared_ptr<xbox::services::user_context> userContext,
        _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
        _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
        );

    /// <summary>
    /// DEPRECATED
    /// The display name of the leaderboard results.
    /// </summary>
    _XSAPIIMP const string_t& display_name() const;

    /// <summary>
    /// The total number of rows in the leaderboard results.
    /// </summary>
    _XSAPIIMP uint32_t total_row_count() const;

    /// <summary>
    /// The collection of columns in the leaderboard results.
    /// </summary>
    _XSAPIIMP const std::vector<leaderboard_column>& columns() const;

    /// <summary>
    /// The collection of rows in the leaderboard results.
    /// </summary>
    _XSAPIIMP const std::vector<leaderboard_row>& rows() const;

    /// <summary>
    /// Indicates whether there is a next page of results.
    /// </summary>
    /// <returns>True if there is another page of results; otherwise false.</returns>
    _XSAPIIMP bool has_next() const;

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
    _XSAPIIMP pplx::task<xbox_live_result<leaderboard_result>> get_next(_In_ uint32_t maxItems) const;
#endif

    /// <summary>
    /// Gets a query to be used to retrieve more data about a leaderboard. 
    /// This query is only to be used to retrieve a leaderboard with stats 2017.
    /// </summary>
    _XSAPIIMP xbox_live_result<leaderboard_query> get_next_query() const;

    /// <summary>
    /// Internal function
    /// </summary>
    void _Set_next_query(std::shared_ptr<leaderboard_global_query> query);

    /// <summary>
    /// Internal function
    /// </summary>
    void _Set_next_query(std::shared_ptr<leaderboard_social_query> query);

    /// <summary>
    /// Internal function
    /// </summary>
    void _Set_next_query(const leaderboard_query& query);

    /// <summary>
    /// Internal function
    /// </summary>
    void _Parse_additional_columns(const std::vector<string_t>& additionalColumnNames);

private:
    string_t m_displayName;
    uint32_t m_totalRowCount;
    string_t m_continuationToken;
    std::vector<leaderboard_column> m_columns;
    std::vector<leaderboard_row> m_rows;

    std::shared_ptr<xbox::services::user_context> m_userContext;
    std::shared_ptr<xbox::services::xbox_live_context_settings> m_xboxLiveContextSettings;
    std::shared_ptr<xbox::services::xbox_live_app_config> m_appConfig;

    std::shared_ptr<leaderboard_global_query> m_globalQuery;
    std::shared_ptr<leaderboard_social_query> m_socialQuery;
    leaderboard_query m_nextQuery;
    string_t m_version;
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
    _XSAPIIMP pplx::task<xbox_live_result<leaderboard_result>> get_leaderboard(
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
    _XSAPIIMP pplx::task<xbox_live_result<leaderboard_result>> get_leaderboard(
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
    _XSAPIIMP pplx::task<xbox_live_result<leaderboard_result>> get_leaderboard(
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
    _XSAPIIMP pplx::task<xbox_live_result<leaderboard_result>> get_leaderboard(
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
    _XSAPIIMP pplx::task<xbox_live_result<leaderboard_result>> get_leaderboard_skip_to_xuid(
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
    //  /scids/{scid}/leaderboards/{leaderboardname}?[&skipToUser={xuid}][&maxItems={maxItems}]
    /// </remarks>
    _XSAPIIMP pplx::task<xbox_live_result<leaderboard_result>> get_leaderboard_skip_to_xuid(
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
    _XSAPIIMP pplx::task<xbox_live_result<leaderboard_result>> get_leaderboard_for_social_group(
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
    _XSAPIIMP pplx::task<xbox_live_result<leaderboard_result>> get_leaderboard_for_social_group(
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
    _XSAPIIMP pplx::task<xbox_live_result<leaderboard_result>> get_leaderboard_for_social_group_skip_to_rank(
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
    _XSAPIIMP pplx::task<xbox_live_result<leaderboard_result>> get_leaderboard_for_social_group_skip_to_xuid(
        _In_ const string_t& xuid,
        _In_ const string_t& scid,
        _In_ const string_t& statName,
        _In_ const string_t& socialGroup,
        _In_ const string_t& skipToXuid,
        _In_ const string_t& sortOrder,
        _In_ uint32_t maxItems = 0
        );

private:
    leaderboard_service() {}

    leaderboard_service(
        _In_ std::shared_ptr<xbox::services::user_context> userContext,
        _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
        _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
        );

    pplx::task<xbox_live_result<leaderboard_result>> get_leaderboard_for_social_group_internal(
        _In_ const string_t& xuid,
        _In_ const string_t& scid,
        _In_ const string_t& statName,
        _In_ const string_t& socialGroup,
        _In_ uint32_t skipToRank,
        _In_ const string_t& skipToXuid,
        _In_ const string_t& sortOrder,
        _In_ uint32_t maxItems,
        _In_ const string_t& continuationToken,
        _In_ const string_t& version = string_t(),
        _In_ leaderboard_query lbQuery = leaderboard_query()
        );

    pplx::task<xbox_live_result<leaderboard_result>> get_leaderboard_internal(
        _In_ const string_t& scid,
        _In_ const string_t& name,
        _In_ uint32_t skipToRank,
        _In_ const string_t& skipToXuid,
        _In_ const string_t& xuid,
        _In_ const string_t& socialGroup,
        _In_ uint32_t maxItems,
        _In_ const string_t& continuationToken,
        _In_ const std::vector<string_t>& additionalColumnNames = std::vector<string_t>(),
        _In_ const string_t& version = string_t(),
        _In_ leaderboard_query lbQuery = leaderboard_query()
        );

    pplx::task<xbox_live_result<leaderboard_result>> get_leaderboard_for_url(
        _In_ string_t url
        );

    std::shared_ptr<xbox::services::user_context> m_userContext;
    std::shared_ptr<xbox::services::xbox_live_context_settings> m_xboxLiveContextSettings;
    std::shared_ptr<xbox::services::xbox_live_app_config> m_appConfig;

    friend leaderboard_result;
    friend stats::manager::stats_manager_impl;
    friend xbox_live_context_impl;
};
#endif
}}}