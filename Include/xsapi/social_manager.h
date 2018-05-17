// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/presence.h"

#if !XSAPI_CPP & !TV_API
#include "User_WinRT.h"
#endif
#include "xsapi/mem.h"
#include "xsapi/types.h"

namespace xbox {
    namespace services {

        namespace social {
            /// <summary>
            /// Contains classes and enumerations for more easily managing social
            /// scenarios.
            /// </summary>
            namespace manager {

class social_graph;
class social_manager_internal;
class xbox_social_user_group_internal;
class social_event_internal;
class social_user_group_loaded_event_args_internal;
struct xbox_social_user_context;
struct user_group_status_change;
enum class change_list_enum;

static const uint32_t GAMERSCORE_CHAR_SIZE = 16;
static const uint32_t GAMERTAG_CHAR_SIZE = 16;
static const uint32_t XBOX_USER_ID_CHAR_SIZE = 21;
static const uint32_t DISPLAY_NAME_CHAR_SIZE = 30;
static const uint32_t REAL_NAME_CHAR_SIZE = 255;
static const uint32_t DISPLAY_PIC_URL_RAW_CHAR_SIZE = 225;
static const uint32_t COLOR_CHAR_SIZE = 7;
static const uint32_t RICH_PRESENCE_CHAR_SIZE = 100;
static const uint32_t NUM_PRESENCE_RECORDS = 6;

static const uint32_t MAX_USERS_FROM_LIST = 100;
/// <summary>
/// Detail level controls how much information is exposed in each xbox_live_social_graph_user
/// Detail level can only be set on construction of social_manager
/// </summary>
enum class social_manager_extra_detail_level
{
    /// <summary>Only get default PeopleHub information (presence, profile)</summary>
    no_extra_detail,

    /// <summary>Add extra detail for the title history for the users</summary>
    title_history_level = 0x1,

    /// <summary>Add extra detail for the preferred color for the users</summary>
    preferred_color_level = 0x2
};

social_manager_extra_detail_level inline operator&(_In_ social_manager_extra_detail_level lhs, _In_ social_manager_extra_detail_level rhs)
{
    return static_cast<social_manager_extra_detail_level>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

social_manager_extra_detail_level inline operator|(_In_ social_manager_extra_detail_level lhs, _In_ social_manager_extra_detail_level rhs)
{
    return static_cast<social_manager_extra_detail_level>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

/// <summary>
/// The filter level of information
/// Title will only show users associated with a particular title
/// </summary>
enum class presence_filter
{
    /// <summary>Unknown</summary>
    unknown,

    /// <summary>Is currently playing current title and is online</summary>
    title_online,

    /// <summary>Has played this title and is offline</summary>
    title_offline,

    /// <summary>Everyone currently online</summary>
    all_online,

    /// <summary>Everyone currently offline</summary>
    all_offline,

    /// <summary>Everyone who has played or is playing the title</summary>
    all_title,

    /// <summary>Everyone</summary>
    all
};

/// <summary>
/// The types of possible events
/// </summary>
enum class social_event_type
{
    /// <summary>Users added to social graph</summary>
    users_added_to_social_graph,

    /// <summary>Users removed from social graph</summary>
    users_removed_from_social_graph,

    /// <summary>Users presence record has changed</summary>
    presence_changed,

    /// <summary>Users profile information has changed</summary>
    profiles_changed,

    /// <summary>Relationship to users has changed</summary>
    social_relationships_changed,

    /// <summary>Social graph load complete from adding a local user</summary>
    local_user_added,

    /// <summary>Social graph removal complete</summary>
    local_user_removed,

    /// <summary>Xbox Social User Group load complete (will only trigger for views that take a list of users)</summary>
    social_user_group_loaded,

    /// <summary>Social user group updated</summary>
    social_user_group_updated,

    /// <summary>unknown.</summary>
    unknown
};

/// <summary>
/// Possible relationship types to filter by
/// </summary>
enum class relationship_filter
{
    /// <summary>Friends of the user (user is following)</summary>
    friends,

    /// <summary>Favorites of the user</summary>
    favorite
};

/// <summary>
/// Identifies type of social user group created
/// </summary>
enum class social_user_group_type
{
    /// <summary>Social user group based off of filters</summary>
    filter_type,

    /// <summary>Social user group based off of list of users</summary>
    user_list_type
};

/// <summary>
/// Data about whether the user has played the title
/// </summary>
class title_history
{
public:
    /// <summary>
    /// Whether the user has played this title
    /// </summary>
    _XSAPIIMP bool has_user_played() const;

    /// <summary>
    /// The last time the user had played
    /// </summary>
    _XSAPIIMP const utility::datetime& last_time_user_played() const;

    /// <summary>
    /// Internal function
    /// </summary>
    title_history();

    /// <summary>
    /// Internal function
    /// </summary>
    bool operator!= (
        _In_ const title_history& previousTitleHistory
        ) const;

    /// <summary>
    /// Internal function
    /// </summary>
    static xbox::services::xbox_live_result<title_history> _Deserialize(
        _In_ const web::json::value& json,
        _In_ std::error_code& errcOut
        );

private:
    bool m_userHasPlayed;
    uint32_t m_titleId;
    utility::datetime m_lastTimeUserPlayed;
};

/// <summary>
/// Preferred color for the user. Set via the shell. 
/// </summary>
class preferred_color
{
public:
    /// <summary>
    /// Users primary color
    /// </summary>
    _XSAPIIMP const char_t* primary_color() const;

    /// <summary>
    /// Users secondary color
    /// </summary>
    _XSAPIIMP const char_t* secondary_color() const;

    /// <summary>
    /// Users tertiary color
    /// </summary>
    _XSAPIIMP const char_t* tertiary_color() const;

    /// <summary>
    /// Does a comparison on if preferred colors are equal
    /// </summary>
    _XSAPIIMP bool operator!=(const preferred_color& rhs) const;

    /// <summary>
    /// Internal function
    /// </summary>
    preferred_color();

    /// <summary>
    /// Internal function
    /// </summary>
    static xbox::services::xbox_live_result<preferred_color> _Deserialize(
        _In_ const web::json::value& json,
        _In_ std::error_code& errcOut
    );

private:
    char_t m_primaryColor[COLOR_CHAR_SIZE];
    char_t m_secondaryColor[COLOR_CHAR_SIZE];
    char_t m_tertiaryColor[COLOR_CHAR_SIZE];
};

/// <summary>
/// Social manager version of the presence title record
/// Gives information about different titles presence information
/// </summary>
class social_manager_presence_title_record
{
public:
    /// <summary>
    /// The title ID.
    /// </summary>
    _XSAPIIMP uint32_t title_id() const;

    /// <summary>
    /// The active state for the title.
    /// </summary>
    _XSAPIIMP bool is_title_active() const;

    /// <summary>
    /// The formatted and localized presence string.
    /// </summary>
    _XSAPIIMP const char_t* presence_text() const;

    /// <summary>
    /// The active state for the title.
    /// </summary>
    _XSAPIIMP bool is_broadcasting() const;
    
    /// <summary>
    /// Device type
    /// </summary>
    _XSAPIIMP xbox::services::presence::presence_device_type device_type() const;

    /// <summary>
    /// Internal function
    /// </summary>
    social_manager_presence_title_record();

    /// <summary>
    /// Internal function
    /// </summary>
    social_manager_presence_title_record(_In_ const xbox::services::presence::presence_title_record& presenceTitleRecord, _In_ xbox::services::presence::presence_device_type deviceType);

    /// <summary>
    /// internal function
    /// </summary>
    bool _Is_null() const;

    /// <summary>
    /// Internal function
    /// </summary>
    static xbox_live_result<social_manager_presence_title_record> _Deserialize(_In_ const web::json::value& json);

private:
    bool m_isTitleActive;
    bool m_isBroadcasting;
    bool m_isNull;
    xbox::services::presence::presence_device_type m_deviceType;
    uint32_t m_titleId;
    char_t m_presenceText[RICH_PRESENCE_CHAR_SIZE];
};

/// <summary>
/// Social manager presence record. Shows information on users current presence status and stores title records
/// </summary>
class social_manager_presence_record
{
public:
    /// <summary>
    /// The user's presence state.
    /// </summary>
    _XSAPIIMP xbox::services::presence::user_presence_state user_state() const;

    /// <summary>
    /// Collection of presence title record objects returned by a request.
    /// </summary>
    _XSAPIIMP const std::vector<social_manager_presence_title_record> presence_title_records() const;

    /// <summary>
    /// Returns whether the user is playing this title id
    /// </summary>
    _XSAPIIMP bool is_user_playing_title(_In_ uint32_t titleId) const;

    /// <summary>
    /// Internal function
    /// </summary>
    uint64_t _Xbox_user_id() const;

    /// <summary>
    /// Internal function
    /// </summary>
    social_manager_presence_record();

    /// <summary>
    /// Internal function
    /// </summary>
    social_manager_presence_record(_In_ const xbox::services::presence::presence_record& presenceRecord);

    /// <summary>
    /// Internal function
    /// </summary>
    void _Update_device(
        _In_ xbox::services::presence::presence_device_type deviceType,
        _In_ bool isUserLoggedIn
        );

    /// <summary>
    /// Internal function
    /// </summary>
    void _Remove_title(
        _In_ uint32_t titleId
        );

    /// <summary>
    /// Internal function
    /// </summary>
    bool _Compare(_In_ const social_manager_presence_record& presenceRecord) const;

    /// <summary>
    /// Internal function
    /// </summary>
    void _Set_xbox_user_id(_In_ uint64_t xboxUserId);

    /// <summary>
    /// Internal function
    /// </summary>
    static xbox_live_result<social_manager_presence_record> _Deserialize(
        _In_ const web::json::value& json,
        _Inout_ std::error_code& errc
        );

private:
    xbox::services::presence::user_presence_state m_userState;
    uint64_t m_xboxUserId;
    social_manager_presence_title_record m_presenceVec[NUM_PRESENCE_RECORDS];

    friend class user_buffers_holder;
};

/// <summary>
/// Xbox Social User that contains profile, presence, preferred color, and title history data
/// </summary>
class xbox_social_user
{
public:
    /// <summary>
    /// The xbox user id
    /// </summary>
    _XSAPIIMP const char_t* xbox_user_id() const;

    /// <summary>
    /// Whether they are a favorite
    /// </summary>
    _XSAPIIMP bool is_favorite() const;

    /// <summary>
    /// Whether the calling user is following them
    /// </summary>
    _XSAPIIMP bool is_following_user() const;

    /// <summary>
    /// Whether they calling user is followed by this person
    /// </summary>
    _XSAPIIMP bool is_followed_by_caller() const;

    /// <summary>
    /// The display name
    /// </summary>
    _XSAPIIMP const char_t* display_name() const;

    /// <summary>
    /// The real name
    /// </summary>
    _XSAPIIMP const char_t* real_name() const;

    /// <summary>
    /// The display pic uri
    /// </summary>
    _XSAPIIMP const char_t* display_pic_url_raw() const;

    /// <summary>
    /// Whether to use the players avatar
    /// </summary>
    _XSAPIIMP bool use_avatar() const;

    /// <summary>
    /// Players gamerscore
    /// </summary>
    _XSAPIIMP const char_t* gamerscore() const;

    /// <summary>
    /// Players gamertag
    /// </summary>
    _XSAPIIMP const char_t* gamertag() const;

    /// <summary>
    /// Users presence record
    /// </summary>
    _XSAPIIMP const xbox::services::social::manager::social_manager_presence_record& presence_record() const;

    /// <summary>
    /// Title history for the user
    /// </summary>
    _XSAPIIMP const xbox::services::social::manager::title_history& title_history() const;

    /// <summary>
    /// Preferred color for the user
    /// </summary>
    _XSAPIIMP const preferred_color& preferred_color() const;

    xbox_social_user();

    /// <summary>
    /// Internal function
    /// </summary>
    uint64_t _Xbox_user_id_as_integer() const;

    /// <summary>
    /// Internal function
    /// </summary>
    void _Set_presence_record(_In_ const xbox::services::social::manager::social_manager_presence_record& presenceRecord);

    /// <summary>
    /// Internal function
    /// </summary>
    void _Set_is_followed_by_caller(_In_ bool isFollowed);

    /// <summary>
    /// Internal function
    /// </summary>
    static change_list_enum _Compare(_In_ const xbox_social_user& previous, _In_ const xbox_social_user& current);

    /// <summary>
    /// Internal function
    /// </summary>
    static xbox_live_result<xbox_social_user> _Deserialize(_In_ const web::json::value& json);

private:
    bool m_isFavorite;
    bool m_isFollowingCaller;
    bool m_isFollowedByCaller;
    bool m_useAvatar;
    uint64_t m_xboxUserIdAsInt;
    char_t m_gamerscore[GAMERSCORE_CHAR_SIZE];
    char_t m_gamertag[GAMERTAG_CHAR_SIZE];
    char_t m_xboxUserId[XBOX_USER_ID_CHAR_SIZE];
    char_t m_displayName[DISPLAY_NAME_CHAR_SIZE];
    char_t m_realName[REAL_NAME_CHAR_SIZE];
    char_t m_displayPicUrlRaw[DISPLAY_PIC_URL_RAW_CHAR_SIZE];
    xbox::services::social::manager::title_history m_titleHistory;
    xbox::services::social::manager::preferred_color m_preferredColor;
    xbox::services::social::manager::social_manager_presence_record m_presenceRecord;

    friend class social_graph;
    friend class user_buffers_holder;
};

/// <summary>
/// Base class for social event args
/// </summary>
class social_event_args
{
public:
    social_event_args() {}
    virtual ~social_event_args() {}
};

/// <summary>
/// Contains an xbox user id for purposes of storing in stl data types
/// </summary>
class xbox_user_id_container
{
public:
    /// <summary>
    /// A users xbox user id
    /// </summary>
    const char_t* xbox_user_id() const;

    /// <summary>
    /// Internal function
    /// </summary>
    xbox_user_id_container();

    /// <summary>
    /// Internal function
    /// </summary>
    xbox_user_id_container(_In_ const char_t* xboxUserId);

private:
    char_t m_xboxUserId[XBOX_USER_ID_CHAR_SIZE];
};

/// <summary>
/// An event that something in the social graph has changed
/// </summary>
class social_event
{
public:
    /// <summary>
    /// The user whose graph got changed
    /// </summary>
    _XSAPIIMP xbox_live_user_t user() const;

    /// <summary>
    /// The type of event this is 
    /// Tells the caller what to cast the event_args to
    /// </summary>
    _XSAPIIMP social_event_type event_type() const;

    /// <summary>
    /// List of users this event affects
    /// </summary>
    _XSAPIIMP std::vector<xbox_user_id_container> users_affected() const;

    /// <summary>
    /// The social event args
    /// </summary>
    _XSAPIIMP std::shared_ptr<social_event_args> event_args() const;

    /// <summary>
    /// Error that occurred
    /// </summary>
    _XSAPIIMP const std::error_code& err() const;

    /// <summary>
    /// Error message
    /// </summary>
    _XSAPIIMP std::string err_message() const;

#if defined(XSAPI_CPPWINRT)
#if TV_API
    _XSAPIIMP const winrt::Windows::Xbox::System::User& user_cppwinrt()
    {
        return convert_user_to_cppwinrt(user());
    }
#endif
#endif

    /// <summary>
    /// Internal function
    /// </summary>
    social_event(
        _In_ std::shared_ptr<social_event_internal> internalObj
        );

private:
    std::shared_ptr<social_event_internal> m_internalObj;
};

/// <summary>
/// A subset snapshot of the users social graph
/// </summary>
class xbox_social_user_group
{
public:
    /// <summary>
    /// Gets an up to date list of users from the social graph
    /// The returned value remains valid until the next call to do_work
    /// </summary>
    _XSAPIIMP std::vector<xbox_social_user*> users() const;

    /// <summary>
    /// Returns copied group of users from social user group
    /// </summary>
    /// <param name="socialUserVector">Vector of social users to populate</param>
    /// <returns>An xbox_live_result representing the success of copying the users</returns>
    _XSAPIIMP xbox_live_result<void> get_copy_of_users(
        _Inout_ std::vector<xbox_social_user>& socialUserVector
        );

    /// <summary>
    /// Type of social user group
    /// </summary>
    _XSAPIIMP social_user_group_type social_user_group_type() const;

    /// <summary>
    /// Users who are contained in this user group currently
    /// For list this is static, for filter this is dynamic and will change on do_work
    /// </summary>
    _XSAPIIMP std::vector<xbox_user_id_container> users_tracked_by_social_user_group() const;

    /// <summary>
    /// The local user who the user group is related to
    /// </summary>
    _XSAPIIMP const xbox_live_user_t& local_user() const;

    /// <summary>
    /// Returns the presence filter used if group type is filter type
    /// </summary>
    _XSAPIIMP presence_filter presence_filter_of_group() const;

    /// <summary>
    /// Returns the relationship filter used if group type is filter type
    /// </summary>
    _XSAPIIMP relationship_filter relationship_filter_of_group() const;

    /// <summary>
    /// Returns users from xuids. Pointers become invalidated by next do_work
    /// </summary>
    _XSAPIIMP std::vector<xbox_social_user*> get_users_from_xbox_user_ids(_In_ const std::vector<xbox_user_id_container>& xboxUserIds);

#if defined(XSAPI_CPPWINRT)
#if TV_API
    _XSAPIIMP const winrt::Windows::Xbox::System::User& local_user_cppwinrt()
    {
        return convert_user_to_cppwinrt(local_user());
    }
#endif
#endif

    /// <summary>
    /// Internal function
    /// </summary>
    xbox_social_user_group(
        _In_ std::shared_ptr<xbox_social_user_group_internal> internalObj
        );

private:
    std::shared_ptr<xbox_social_user_group_internal> m_internalObj;

    friend class social_manager;
};

/// <summary>
/// Social user group args for when a social user group loads
/// </summary>
class social_user_group_loaded_event_args : public social_event_args
{
public:
    /// <summary>
    /// The loaded social user group
    /// </summary>
    _XSAPIIMP std::shared_ptr<xbox_social_user_group> social_user_group() const;

    /// <summary>
    /// internal function
    /// </summary>
    social_user_group_loaded_event_args(_In_ std::shared_ptr<social_user_group_loaded_event_args_internal> internalObj);

private:
    std::shared_ptr<social_user_group_loaded_event_args_internal> m_internalObj;
};

/// <summary>
/// Social Manager that handles core logic
/// </summary>
class social_manager : public std::enable_shared_from_this<social_manager>
{
public:
    /// <summary>
    /// Gets the social_manager singleton instance
    /// </summary>
    _XSAPIIMP static std::shared_ptr<social_manager> get_singleton_instance();

    /// <summary>
    /// Create a social graph for the specified local user
    /// The result of a local user being added will be triggered through the local_user_added event in do_work
    /// </summary>
    /// <param name="user">Xbox Live User</param>
    /// <param name="extraDetailLevel">The level of verbosity that should be in the xbox_social_user</param>
    /// <returns>An xbox_live_result to report any potential error</returns>
    _XSAPIIMP virtual xbox_live_result<void> add_local_user(
        _In_ xbox_live_user_t user,
        _In_ social_manager_extra_detail_level extraDetailLevel
        );

    /// <summary>
    /// Removes a social graph for the specified local user
    /// The result of a local user being added will be triggered through the local_user_removed event in do_work
    /// </summary>
    /// <param name="user">Xbox Live User</param>
    /// <returns>An xbox_live_result to report any potential error</returns>
    _XSAPIIMP virtual xbox_live_result<void> remove_local_user(
        _In_ xbox_live_user_t user
        );

    /// <summary>
    /// Called whenever the title wants to update the social graph and get list of change events
    /// Must be called every frame for data to be up to date
    /// </summary>
    /// <returns> The list of what has changed in between social graph updates</returns>
    _XSAPIIMP std::vector<social_event> do_work();

    /// <summary>
    /// Constructs a social Xbox Social User Group, which is a collection of users with social information
    /// The result of a user group being loaded will be triggered through the social_user_group_loaded event in do_work
    /// </summary>
    /// <param name="user">Xbox Live User</param>
    /// <param name="presenceDetailLevel">The restriction of users based on their presence and title activity</param>
    /// <param name="relationshipFilter">The restriction of users based on their relationship to the calling user</param>
    /// <returns>An xbox_live_result of the created Xbox Social User Group</returns>
    _XSAPIIMP xbox_live_result<std::shared_ptr<xbox_social_user_group>> create_social_user_group_from_filters(
        _In_ xbox_live_user_t user,
        _In_ presence_filter presenceDetailLevel,
        _In_ relationship_filter filter
        );

    /// <summary>
    /// Constructs a social Xbox Social User Group, which is a collection of users with social information
    /// The result of a user group being loaded will be triggered through the social_user_group_loaded event in do_work
    /// </summary>
    /// <param name="user">Xbox Live User</param>
    /// <param name="xboxUserIdList">List of users to populate the Xbox Social User Group with. This is currently capped at 100 users total.</param>
    /// <returns>An xbox_live_result of the created Xbox Social User Group</returns>
    _XSAPIIMP xbox_live_result<std::shared_ptr<xbox_social_user_group>> create_social_user_group_from_list(
        _In_ xbox_live_user_t user,
        _In_ std::vector<string_t> xboxUserIdList
        );

    /// <summary>
    /// Destroys a created social Xbox Social User Group
    /// This will stop updating the Xbox Social User Group and remove tracking for any users the Xbox Social User Group holds
    /// </summary>
    /// <param name="socialUserGroup">The social Xbox Social User Group to destroy and stop tracking</param>
    /// <returns>An xbox_live_result to report any potential error</returns>
    _XSAPIIMP xbox_live_result<void> destroy_social_user_group(
        _In_ std::shared_ptr<xbox_social_user_group> socialUserGroup
        );

    /// <summary>
    /// Returns all local users who have been added to the social manager
    /// </summary>
    _XSAPIIMP std::vector<xbox_live_user_t> local_users() const;

    /// <summary>
    /// Updates specified social user group to new group of users
    /// Does a diff to see which users have been added or removed from 
    /// The result of a user group being updated will be triggered through the social_user_group_updated event in do_work
    /// </summary>
    /// <param name="group">The xbox social user group to add users to</param>
    /// <param name="users">List of users to add to the xbox social user group. Total number of users not in social graph is limited at 100.</param>
    /// <returns>An xbox_live_result representing the success of adding the users to the group</returns>
    _XSAPIIMP xbox_live_result<void> update_social_user_group(
        _In_ const std::shared_ptr<xbox_social_user_group>& group,
        _In_ const std::vector<string_t>& users
        );
    
    /// <summary>
    /// Whether to enable social manager to poll every 30 seconds from the presence service 
    /// </summary>
    /// <param name="user">Xbox Live User</param>
    /// <param name="shouldEnablePolling">Whether or not polling should enabled</param>
    /// <returns>An xbox_live_result representing the success enabling polling</returns>
    _XSAPIIMP xbox_live_result<void> set_rich_presence_polling_status(
        _In_ xbox_live_user_t user,
        _In_ bool shouldEnablePolling
        );

    /// <summary>
    /// Sets the level of debug messages to send to the debugger's Output window.
    /// </summary>
    /// <param name="traceLevel">The level of detail</param>
    _XSAPIIMP void set_diagnostics_trace_level(
        _In_ xbox_services_diagnostics_trace_level traceLevel
    );

    /// <summary>
    /// Internal function
    /// </summary>
    void _Log_state();

#if defined(XSAPI_CPPWINRT)
#if TV_API
    _XSAPIIMP virtual xbox_live_result<void> add_local_user(
        _In_ const winrt::Windows::Xbox::System::User& user,
        _In_ social_manager_extra_detail_level extraDetailLevel
        )
    {
        return add_local_user(convert_user_to_cppcx(user), extraDetailLevel);
    }

    _XSAPIIMP virtual xbox_live_result<void> remove_local_user(
        _In_ const winrt::Windows::Xbox::System::User& user
        )
    {
        return remove_local_user(convert_user_to_cppcx(user));
    }

    _XSAPIIMP xbox_live_result<std::shared_ptr<xbox_social_user_group>> create_social_user_group_from_filters(
        _In_ const winrt::Windows::Xbox::System::User& user,
        _In_ presence_filter presenceDetailLevel,
        _In_ relationship_filter filter
        )
    {
        return create_social_user_group_from_filters(convert_user_to_cppcx(user), presenceDetailLevel, filter);
    }

    _XSAPIIMP xbox_live_result<std::shared_ptr<xbox_social_user_group>> create_social_user_group_from_list(
        _In_ const winrt::Windows::Xbox::System::User& user,
        _In_ std::vector<string_t> xboxUserIdList
        )
    {
        return create_social_user_group_from_list(convert_user_to_cppcx(user), xboxUserIdList);
    }

    _XSAPIIMP const std::vector<winrt::Windows::Xbox::System::User>& local_users_cppwinrt() const
    {
        return convert_user_vector_to_cppwinrt(local_users());
    }
#endif
#endif

protected:
    social_manager();
    std::shared_ptr<social_manager_internal> m_internalObj;

    friend class xbox_social_user_group;
};

}}}}
