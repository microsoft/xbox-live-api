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
#include "xsapi/leaderboard.h"
#include "xsapi/social.h"
#include "xsapi/user_statistics.h"
#include "xsapi/achievements.h"

namespace xbox { namespace services { namespace experimental { namespace stats { namespace manager { 

#if TV_API | XBOX_UWP
    typedef  Windows::Xbox::System::User^ xbox_live_user_t;
#else
    typedef std::shared_ptr<xbox::services::system::xbox_live_user> xbox_live_user_t;
#endif

class stats_manager_impl;

/// <summary> 
/// Represents data for stat
/// internal representation
/// </summary>
union stat_data
{
    double numberType;
    char_t stringType[64];
};

/// <summary> 
/// Type for how to replace a numerical stat
/// </summary>
enum class stat_compare_type
{
    always,
    min,
    max
};

/// <summary> 
/// Type of data the stat is
/// </summary>
enum class stat_data_type
{
    undefined,
    number,
    string
};

/// <summary> 
/// Type of stat change event
/// </summary>
enum class stat_event_type
{
    local_user_added,
    local_user_removed,
    stat_update_complete,
    stat_sync_complete,
    leaderboard_load_complete,
    user_statistic_view_load_complete,
    leaderboard_updated,
    user_statistic_view_updated,
    achievement_update_complete,
    achievement_load_complete
};


/// <summary> 
/// The order to sort the leaderboard in
/// </summary>
enum class sort_order
{
    ascending,
    descending
};

/// <summary> 
/// The type of preconfigured leaderboard to display
/// </summary>
enum class preconfigured_leaderboard_type
{
    daily,
    monthly,
    yearly
};

/// <summary> 
/// Represents a simplified stat
/// </summary>
class stat_value
{
public:
    /// <summary> 
    /// Name of the statistic
    /// </summary>
    /// <returns>A stat container that has the stat name string</returns>
    const char_t* name() const;

    /// <summary> 
    /// Return data as numerical type
    /// </summary>
    /// <returns>Float data for statistic</returns>
    /// <remarks>Will debug assert if data is not type requested</returns>
    double as_number() const;

    /// <summary> 
    /// Return data as string type
    /// </summary>
    /// <returns>data as char_t*</returns>
    /// <remarks>Will debug assert if data is not type requested</returns>
    const char_t* as_string() const;

    /// <summary> 
    /// Return type of data the data object is
    /// </summary>
    /// <returns>Stat data type</returns>
    stat_data_type data_type() const;

    /// <summary> 
    /// Enum of logic of how to handle statistic
    /// </summary>
    /// <returns>The compare type</returns>
    stat_compare_type compare_type() const;

    /// <summary> 
    /// Local user owner
    /// </summary>
    /// <returns>The local user object</returns>
    const xbox_live_user_t& local_user_owner() const;

    /// Internal function
    static xbox_live_result<stat_value> _Deserialize(_In_ const web::json::value& data);

private:
    void set_stat(_In_ double value);
    void set_stat(_In_ const char_t* value);
    web::json::value serialize() const;

    stat_data_type m_dataType;
    stat_compare_type m_statCompareType;
    xbox_live_user_t m_localUserOwner;
    char_t m_name[64];
    stat_data m_statData;
    std::function<void(stat_value)> m_callbackFunc;
    friend class stats_value_document;
};

/// <summary> 
/// Represents a contextual key object used for filtering stats
/// </summary>
class stat_context
{
public:
    /// <summary> 
    /// Represents a stat_context object used for filtering stats
    /// </summary>
    /// <param name="name">Name for stat_context</param>
    /// <param name="value">Value for stat_context</param>
    stat_context(
        _In_ const char_t* name,
        _In_ const char_t* value
        );

    /// <summary> 
    /// Represents the name for the stat_context
    /// </summary>
    /// <return>Returns the name of the stat_context</return>
    const char_t* name() const;

    /// <summary> 
    /// Represents the key name for the stat_context
    /// </summary>
    /// <return>Returns the value of the stat_context</return>
    const char_t* value() const;

    /// internal function
    stat_context();

    /// internal function
    static xbox_live_result<stat_context> _Deserialize(
        _In_ const web::json::value& data
        );

private:
    char_t m_name[64];
    char_t m_value[64];
};

/// <summary> 
/// Represents a contextual key object used for filtering stats
/// </summary>
class stat_event
{
public:
    /// <summary> 
    /// Represents error code and error message
    /// </summary>
    /// <return>The returned xbox live result</return>
    const xbox_live_result<void>& error_info() const;

    /// <summary> 
    /// The type of event the statistic is
    /// </summary>
    /// <return>The event type</return>
    stat_event_type event_type() const;

    /// <summary> 
    /// Local user the event is for
    /// </summary>
    /// <return>The returned user</return>
    const xbox_live_user_t& local_user() const;

    /// Internal function
    stat_event(stat_event_type eventType);

private:
    stat_event_type m_eventType;
    xbox_live_user_t m_localUser;
    xbox_live_result<void> m_errorInfo;
};

/// <summary> 
/// Information for querying a leaderboard object
/// </summary>
class leaderboard_query
{
public:
	leaderboard_query() : m_shouldSkipToXuid(false), m_maxItems(0), m_skipToRank(0), m_sortOrder(sort_order::ascending) {}

private:
    bool m_shouldSkipToXuid;
    uint32_t m_maxItems;
    uint32_t m_skipToRank;
    sort_order m_sortOrder;
    string_t m_xuidToSkipTo;
    std::vector<string_t> m_additionalColumnNames;
};

/// <summary> 
/// Wrapper object for holding user statistic result
/// This object will update on do_work when async task is complete or when object is updated if real time
/// </summary>
class user_statistic_view
{
public:
    const std::vector<xbox::services::user_statistics::user_statistics_result>& user_statistic_results() const;

private:
    uint32_t m_id;
    std::vector<xbox::services::user_statistics::user_statistics_result> m_userStatisticResults;
};

/// <summary> 
/// Wrapper object for holding leaderboard result
/// This object will update on do_work when async task is complete or when object is updated if real time
/// </summary>
class leaderboard_view
{
public:
    const std::vector<xbox::services::user_statistics::user_statistics_result>& user_statistic_results() const;

private:
    uint32_t m_id;
    std::vector<xbox::services::user_statistics::user_statistics_result> m_userStatisticResults;
};

class achievements_view
{
public:
    const xbox::services::achievements::achievements_result& achievement_results() const;

private:
    uint32_t m_id;
    xbox::services::achievements::achievements_result m_achievementResults;
};

/// <summary> 
/// Simplified Stats does REST requests to update stats
/// </summary>
class stats_manager
{
public:
    /// <summary> 
    /// Instantiates and returns an instance of simplified stats
    /// </summary>
    static stats_manager& get_singleton_instance();

    /// <summary> 
    /// Adds a local user to the simplified stats manager
    /// Returns a local_user_added event from do_work
    /// </summary>
    /// <param name="user">The user to add to the statistic manager</param>
    xbox_live_result<void> add_local_user(
        _In_ const xbox_live_user_t& user
        );

    /// <summary> 
    /// Removes a local user from the simplified stats manager
    /// Returns a local_user_removed event from do_work
    /// </summary>
    /// <param name="user">The user to be removed from the statistic manager</param>
    xbox_live_result<void> remove_local_user(
        _In_ const xbox_live_user_t& user
        );

    /// <summary> 
    /// Force the current stat values to be uploaded to the service
    /// This will send immediately instead of automatically during a 30 second window
    /// </summary>
    /// <remarks>This can be throttled if called too often</remarks>
    xbox_live_result<void> request_flush_to_service(
        _In_ const xbox_live_user_t& user,
        _In_ bool isHighPriority = false
        );

    /// <summary> 
    /// Returns any events that have been processed
    /// </summary>
    /// <return>A list of events that have happened since previous do_work</return>
    std::vector<stat_event> do_work();

    /// <summary> 
    /// Replaces the numerical stat by the value. Can be positive or negative
    /// </summary>
    /// <param name="user">The local user whose stats to access</param>
    /// <param name="name">The name of the statistic to modify</param>
    /// <param name="value">Value to replace the stat by</param>
    /// <param name="stat_compare_type">
    /// Will override the compare type. Stat will only be updated if follows the stat compares rule
    /// *Note* This is not recommended to be modified after release of the title
    /// </param>
    /// <return>Whether or not the setting was successful. Can fail if stat is not of numerical type. Will return updated stat</return>
    xbox_live_result<void> set_stat(
        _In_ const xbox_live_user_t& user,
        _In_ const string_t& name,
        _In_ double value,
        _In_ stat_compare_type statisticReplaceCompareType = stat_compare_type::always
        );

    /// <summary> 
    /// Replaces the string stat by the value.
    /// </summary>
    /// <param name="user">The local user whose stats to access</param>
    /// <param name="name">The name of the statistic to modify</param>
    /// <param name="value">Value to replace the stat by</param>
    /// <return>Whether or not the setting was successful. Can fail if stat is not of string type. Will return updated stat</return>
    xbox_live_result<void> set_stat(
        _In_ const xbox_live_user_t& user,
        _In_ const string_t& name,
        _In_ const char_t* value
        );

    /// <summary> 
    /// Gets all stat names in the stat document.
    /// </summary>
    /// <param name="user">The local user whose stats to access</param>
    /// <param name="statNameList">The list to fill with stat names</param>
    /// <return>Whether or not the setting was successful.</return>
    xbox_live_result<void> get_stat_names(
        _In_ const xbox_live_user_t& user,
        _Inout_ std::vector<string_t>& statNameList
        );

    /// <summary> 
    /// Gets a stat value
    /// </summary>
    /// <param name="user">The local user whose stats to access</param>
    /// <param name="name">The name of the statistic to modify</param>
    /// <return>Whether or not the setting was successful along with updated stat</return>
    xbox_live_result<stat_value> get_stat(
        _In_ const xbox_live_user_t& user,
        _In_ const string_t& name
        );

    /// <summary> 
    /// Gets the current set contextual keys
    /// </summary>
    /// <param name="user">The local user whose keys to modify</param>
    /// <param name="statisticContextList">The list of currently active contextual keys</param>
    /// <return>Whether or not getting the keys was successful</return>
    xbox_live_result<void> get_stat_contexts(
        _In_ const xbox_live_user_t& user,
        _Inout_ std::vector<stat_context>& statisticContextList
        );

    /// <summary> 
    /// Sets the contextual keys
    /// </summary>
    /// <param name="user">The local user whose keys to modify</param>
    /// <param name="statisticContextList">The list of contextual keys to set</param>
    /// <return>Whether or not getting the keys was successful</return>
    xbox_live_result<void> set_stat_contexts(
        _In_ const xbox_live_user_t& user,
        _In_ const std::vector<stat_context>& statContextList
        );

    /// <summary> 
    /// Clears the contextual keys for a user
    /// </summary>
    /// <param name="user">The local user whose keys to modify</param>
    /// <return>Whether or not clearing the keys was successful</return>
    xbox_live_result<void> clear_stat_contexts(
        _In_ const xbox_live_user_t& user
        );

    /// <summary> 
    /// Gets a social leaderboard for a user. 
    /// leaderboard_load_complete event triggers when leaderboard load is complete
    /// </summary>
    /// <param name="user">The local user to get the leaderboard for</param>
    /// <param name="leaderboardName">Either the name of the leaderboard to query by or the leaderboard to generate if a statName is passed in</param>
    /// <param name="statContext">Stat context that the stat is for</param>
    /// <param name="socialGroup">Social group parameter to filter list by</param>
    /// <param name="leaderboardQuery">Object that defines the leaderboard query information</param>
    /// <param name="isRealTime">Whether the leaderboard should update based on RTA shoulder taps</param>
    /// <return>Object that contains leaderboard_result that will update when complete</return>
    xbox_live_result<std::shared_ptr<xbox::services::leaderboard::leaderboard_result>> get_social_leaderboard(
        _In_ const xbox_live_user_t& user,
        _In_ const string_t& leaderboardOrStatName,
        _In_ const stat_context& statContext = stat_context(),
        _In_ const string_t& socialGroup = xbox::services::social::social_group_constants::people(),
        _In_ const leaderboard_query& leaderboardQuery = leaderboard_query(),
        _In_ bool isRealTime = false
        );

    /// <summary> 
    /// Gets a global leaderboard for a user.
    /// </summary>
    /// <param name="user">The local user to get the leaderboard for</param>
    /// <param name="leaderboardName">The leaderboard name to query by</param>
    /// <param name="statContext">The stat context to view the leaderboard with. This must be preconfigured in XDP</param>
    /// <param name="leaderboardQuery">Object that defines the leaderboard query information</param>
    /// <param name="shouldAutoRefresh">Whether the leaderboard should auto refresh at various intervals</param>
    /// <return>Object that contains leaderboard_result that will update when complete</return>
    xbox_live_result<std::shared_ptr<xbox::services::leaderboard::leaderboard_result>> get_global_leaderboard(
        _In_ const xbox_live_user_t& user,
        _In_ const string_t& leaderboardName,
        _In_ const stat_context& statContext = stat_context(),
        _In_ const leaderboard_query& leaderboardQuery = leaderboard_query(),
        _In_ bool shouldAutoRefresh = false
        );

    /// <summary> 
    /// Gets a statistic table for users
    /// </summary>
    /// <param name="user">The local user to get the statistic table for</param>
    /// <param name="statName">The name of the stat to get</param>
    /// <param name="xboxUserIds">The list of users to get the stats for</param>
    /// <param name="statContext">The stat context for the users</param>
    /// <return>Object that contains user_statistic_view that will update when complete</return>
    xbox_live_result<std::shared_ptr<user_statistic_view>> get_statistics_table(
        _In_ const xbox_live_user_t& user,
        _In_ const string_t& statName,
        _In_ const std::vector<string_t>& xboxUserIds,
        _In_ const stat_context& statContext = stat_context()
        );

    /// <summary> 
    /// Gets a multiple statistics table for users
    /// </summary>
    /// <param name="user">The local user to get the statistic table for</param>
    /// <param name="statName">The name of the stat to get</param>
    /// <param name="xboxUserIds">The list of users to get the stats for</param>
    /// <param name="statContext">The stat context for the users</param>
    /// <return>Object that contains user_statistic_view that will update when complete</return>
    xbox_live_result<std::shared_ptr<user_statistic_view>> get_multiple_statistics_table(
        _In_ const xbox_live_user_t& user,
        _In_ const std::vector<string_t>& statName,
        _In_ const std::vector<string_t>& xboxUserIds,
        _In_ const stat_context& statContext = stat_context()
        );

    /// <summary>
    /// Allow achievement progress to be updated and achievements to be unlocked.
    /// This API will work even when offline. On PC and Xbox One, updates will be 
    /// posted by the system when connection is re-established even if the title isn't running.
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID of the player.</param>
    /// <param name="achievementId">The achievement ID as defined by XDP or Dev Center.</param>
    /// <param name="percentComplete">The completion percentage of the achievement to indicate progress.
    /// Valid values are from 1 to 100. Set to 100 to unlock the achievement.
    /// Progress will be set by the server to the highest value sent</param>
    /// <remarks>
    /// Event achievement_update_complete shows up through do_work when complete
    ///
    /// This method calls V2 POST /users/xuid({xuid})/achievements/{scid}/update
    /// </remarks>
    xbox::services::xbox_live_result<void> update_achievement(
        _In_ const string_t& xboxUserId,
        _In_ const string_t& achievementId,
        _In_ uint32_t percentComplete
        );

    /// <summary>
    /// Allow achievement progress to be updated and achievements to be unlocked.
    /// This API will work even when offline. On PC and Xbox One, updates will be 
    /// posted by the system when connection is re-established even if the title isn't running.
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID of the player.</param>
    /// <param name="titleId">The title ID.</param>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) for the title.</param>
    /// <param name="achievementId">The achievement ID as defined by XDP or Dev Center.</param>
    /// <param name="percentComplete">The completion percentage of the achievement to indicate progress.
    /// Valid values are from 1 to 100. Set to 100 to unlock the achievement.
    /// Progress will be set by the server to the highest value sent</param>
    /// <remarks>
    /// Event achievement_update_complete shows up through do_work when complete
    ///
    /// This method calls V2 POST /users/xuid({xuid})/achievements/{scid}/update
    /// </remarks>
    xbox::services::xbox_live_result<void> update_achievement(
        _In_ const string_t& xboxUserId,
        _In_ uint32_t titleId,
        _In_ const string_t& serviceConfigurationId,
        _In_ const string_t& achievementId,
        _In_ uint32_t percentComplete
        );

    /// <summary>
    /// Returns an achievements_result object containing the first page of achievements
    /// for a player of the specified title.
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID of the player.</param>
    /// <param name="titleId">The title ID.</param>
    /// <param name="type">The achievement type to retrieve.</param>
    /// <param name="unlockedOnly">Indicates whether to return unlocked achievements only.</param>
    /// <param name="orderby">Controls how the list of achievements is ordered.</param>
    /// <param name="skipItems">The number of achievements to skip.</param>
    /// <param name="maxItems">The maximum number of achievements the result can contain.  Pass 0 to attempt
    /// to retrieve all items.</param>
    /// <returns>An AchievementsResult object that contains a list of Achievement objects.</returns>
    /// <remarks>
    /// Returns a task&lt;T&gt; object that represents the state of the asynchronous operation.
    ///
    /// Event achievement_update_complete shows up through do_work when complete
    ///
    /// This method calls V2 GET /users/xuid({xuid})/achievements
    /// </remarks>
    xbox::services::xbox_live_result<std::shared_ptr<achievements_view>> get_achievements_for_title_id(
        _In_ const string_t& xboxUserId,
        _In_ uint32_t titleId,
        _In_ xbox::services::achievements::achievement_type type,
        _In_ bool unlockedOnly,
        _In_ xbox::services::achievements::achievement_order_by orderBy,
        _In_ uint32_t skipItems,
        _In_ uint32_t maxItems
        );

    /// <summary>
    /// Returns a specific achievement object for a specified player.
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID of the player.</param>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) for the title.</param>
    /// <param name="achievementId">The unique identifier of the Achievement as defined by XDP or Dev Center.</param>
    /// <returns>The requested achievement object if it exists.
    /// If the achievement does not exist, the method returns xbox_live_error_code::runtime_error .</returns>
    /// <remarks>
    /// Returns a task&lt;T&gt; object that represents the state of the asynchronous operation.
    ///
    /// This method calls V2 GET /users/xuid({xuid})/achievements/{scid}/{achievementId}.
    /// </remarks>
    xbox::services::xbox_live_result<std::shared_ptr<achievements_view>> get_achievement(
        _In_ const string_t& xboxUserId,
        _In_ const string_t& serviceConfigurationId,
        _In_ const string_t& achievementId
        );

    stats_manager();

private:
    std::shared_ptr<stats_manager_impl> m_statsManagerImpl;
};

} } } } }