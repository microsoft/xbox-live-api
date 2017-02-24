// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/leaderboard.h"
#include "xsapi/social.h"
#include "xsapi/user_statistics.h"
#include "xsapi/achievements.h"

namespace xbox { namespace services { namespace stats { namespace manager { 

#if TV_API | XBOX_UWP
    typedef  Windows::Xbox::System::User^ xbox_live_user_t;
#else
    typedef std::shared_ptr<xbox::services::system::xbox_live_user> xbox_live_user_t;
#endif

class stats_manager_impl;
static const uint8_t STAT_PRESENCE_CHARS_NUM = 64;

/// <summary> 
/// Represents data for stat
/// internal representation
/// </summary>
union stat_data
{
    double numberType;
    char_t stringType[STAT_PRESENCE_CHARS_NUM];
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
    stat_update_complete
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
/// Represents a simplified stat
/// </summary>
class stat_value
{
public:
    /// <summary> 
    /// Name of the statistic
    /// </summary>
    /// <returns>A stat container that has the stat name string</returns>
    const string_t name() const;

    /// <summary> 
    /// Return data as numerical type
    /// </summary>
    /// <returns>Float data for statistic</returns>
    /// <remarks>Will debug assert if data is not type requested</returns>
    double as_number() const;

    /// <summary> 
    /// Return data as integer type
    /// </summary>
    /// <returns>Float data for statistic</returns>
    /// <remarks>Will debug assert if data is not type requested</returns>
    int64_t as_integer() const;

    /// <summary> 
    /// Return data as string type
    /// </summary>
    /// <returns>data as char_t*</returns>
    /// <remarks>Will debug assert if data is not type requested</returns>
    const string_t as_string() const;

    /// <summary> 
    /// Return type of data the data object is
    /// </summary>
    /// <returns>Stat data type</returns>
    stat_data_type data_type() const;

    /// Internal function
    static xbox_live_result<stat_value> _Deserialize(_In_ const web::json::value& data);

    /// Internal function
    stat_value();

private:
    void set_stat(
        _In_ double value
        );

    void set_stat(
        _In_ const char_t* value
        );

    void set_name(
    _In_ const string_t& name
    );

    web::json::value serialize() const;

    stat_data_type m_dataType;
    xbox_live_user_t m_localUserOwner;
    char_t m_name[STAT_PRESENCE_CHARS_NUM];
    stat_data m_statData;
    std::function<void(stat_value)> m_callbackFunc;
    friend class stats_value_document;
};

class stat_event
{
public:
    /// <summary> 
    /// Represents error code and error message
    /// </summary>
    /// <return>The returned xbox live result</return>
    xbox_live_result<void>& error_info();

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
    stat_event(
        stat_event_type eventType,
        xbox_live_user_t user,
        xbox_live_result<void> errorInfo
        );

private:
    stat_event_type m_eventType;
    xbox_live_user_t m_localUser;
    xbox_live_result<void> m_errorInfo;
};

/// <summary> 
/// Stats Manager handles and writes to the service a local users stats
/// </summary>
class stats_manager
{
public:
    /// <summary> 
    /// Instantiates and returns an instance of stats manager
    /// </summary>
    static std::shared_ptr<stats_manager> get_singleton_instance();

    /// <summary> 
    /// Adds a local user to the stats manager
    /// Returns a local_user_added event from do_work
    /// </summary>
    /// <param name="user">The user to add to the statistic manager</param>
    xbox_live_result<void> add_local_user(
        _In_ const xbox_live_user_t& user
        );

    /// <summary> 
    /// Removes a local user from the stats manager
    /// Returns a local_user_removed event from do_work
    /// </summary>
    /// <param name="user">The user to be removed from the statistic manager</param>
    xbox_live_result<void> remove_local_user(
        _In_ const xbox_live_user_t& user
        );

    /// <summary> 
    /// Requests the current stat values to be uploaded to the service
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
    /// <param name="statisticReplaceCompareType">
    /// Will override the compare type. Stat will only be updated if follows the stat compares rule
    /// </param>
    /// <return>Whether or not the setting was successful. Can fail if stat is not of numerical type. Will return updated stat</return>
    xbox_live_result<void> set_stat_as_number(
        _In_ const xbox_live_user_t& user,
        _In_ const string_t& name,
        _In_ double value
        );

    /// <summary> 
    /// Replaces the numerical stat by the value. Can be positive or negative
    /// </summary>
    /// <param name="user">The local user whose stats to access</param>
    /// <param name="name">The name of the statistic to modify</param>
    /// <param name="value">Value to replace the stat by</param>
    /// <param name="statisticReplaceCompareType">
    /// Will override the compare type. Stat will only be updated if follows the stat compares rule
    /// *Note* This is not recommended to be modified after release of the title
    /// </param>
    /// <return>Whether or not the setting was successful. Can fail if stat is not of numerical type. Will return updated stat</return>
    xbox_live_result<void> set_stat_as_integer(
        _In_ const xbox_live_user_t& user,
        _In_ const string_t& name,
        _In_ int64_t value
        );

    /// <summary> 
    /// Replaces a string stat with the given value.
    /// </summary>
    /// <param name="user">The local user whose stats to access</param>
    /// <param name="name">The name of the statistic to modify</param>
    /// <param name="value">Value to replace the stat by</param>
    /// <return>Whether or not the setting was successful. Can fail if stat is not of string type. Will return updated stat</return>
    xbox_live_result<void> set_stat_as_string(
        _In_ const xbox_live_user_t& user,
        _In_ const string_t& name,
        _In_ const string_t& value
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
    /// Deletes a stat. Will clear stat from service and social leaderboard information
    /// </summary>
    /// <param name="user">The local user whose stats to access</param>
    /// <param name="name">The name of the statistic to delete</param>
    /// <return>Whether or not the stat deletion was successful</return>
    xbox_live_result<void> delete_stat(
        _In_ const xbox_live_user_t& user,
        _In_ const string_t& name
        );

    stats_manager();

private:
    std::shared_ptr<stats_manager_impl> m_statsManagerImpl;
};

} } } }