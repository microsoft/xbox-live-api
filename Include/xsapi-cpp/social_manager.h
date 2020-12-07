// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi-cpp/presence.h"
#include "xsapi-c/social_manager_c.h"

#include "xsapi-cpp/mem.h"
#include "xsapi-cpp/types.h"

namespace xbox {
    namespace services {

        namespace social {
            /// <summary>
            /// Contains classes and enumerations for more easily managing social
            /// scenarios.
            /// </summary>
            namespace manager {

/// <summary>
/// Detail level controls how much information is exposed in each xbox_live_social_graph_user
/// Detail level can only be set on construction of social_manager
/// </summary>
enum class social_manager_extra_detail_level
{
    /// <summary>Only get default PeopleHub information (presence, profile)</summary>
    no_extra_detail = static_cast<uint32_t>(XblSocialManagerExtraDetailLevel::NoExtraDetail),

    /// <summary>Add extra detail for the title history for the users</summary>
    title_history_level = static_cast<uint32_t>(XblSocialManagerExtraDetailLevel::TitleHistoryLevel),

    /// <summary>Add extra detail for the preferred color for the users</summary>
    preferred_color_level = static_cast<uint32_t>(XblSocialManagerExtraDetailLevel::PreferredColorLevel)
};

DEFINE_ENUM_FLAG_OPERATORS(social_manager_extra_detail_level);

/// <summary>
/// The filter level of information
/// Title will only show users associated with a particular title
/// </summary>
enum class presence_filter
{
    /// <summary>Unknown</summary>
    unknown = static_cast<uint32_t>(XblPresenceFilter::Unknown),

    /// <summary>Is currently playing current title and is online</summary>
    title_online = static_cast<uint32_t>(XblPresenceFilter::TitleOnline),

    /// <summary>Has played this title and is offline</summary>
    title_offline = static_cast<uint32_t>(XblPresenceFilter::TitleOffline),

    /// <summary>Has played this title, is online but not currently playing this title</summary>
    title_online_outside_title = static_cast<uint32_t>(XblPresenceFilter::TitleOnlineOutsideTitle),

    /// <summary>Everyone currently online</summary>
    all_online = static_cast<uint32_t>(XblPresenceFilter::AllOnline),

    /// <summary>Everyone currently offline</summary>
    all_offline = static_cast<uint32_t>(XblPresenceFilter::AllOffline),

    /// <summary>Everyone who has played or is playing the title</summary>
    all_title = static_cast<uint32_t>(XblPresenceFilter::AllTitle),

    /// <summary>Everyone</summary>
    all = static_cast<uint32_t>(XblPresenceFilter::All)
};

/// <summary>
/// The types of possible events
/// </summary>
enum class social_event_type
{
    /// <summary>Users added to social graph</summary>
    users_added_to_social_graph = static_cast<uint32_t>(XblSocialManagerEventType::UsersAddedToSocialGraph),

    /// <summary>Users removed from social graph</summary>
    users_removed_from_social_graph = static_cast<uint32_t>(XblSocialManagerEventType::UsersRemovedFromSocialGraph),

    /// <summary>Users presence record has changed</summary>
    presence_changed = static_cast<uint32_t>(XblSocialManagerEventType::PresenceChanged),

    /// <summary>Users profile information has changed</summary>
    profiles_changed = static_cast<uint32_t>(XblSocialManagerEventType::ProfilesChanged),

    /// <summary>Relationship to users has changed</summary>
    social_relationships_changed = static_cast<uint32_t>(XblSocialManagerEventType::SocialRelationshipsChanged),

    /// <summary>Social graph load complete from adding a local user</summary>
    local_user_added = static_cast<uint32_t>(XblSocialManagerEventType::LocalUserAdded),

    /// <summary>
    /// Legacy. Formerly raised when a user's graph was destroyed, but is no longer raised.
    /// C++ interface will continue to raise this event (always on the next do_work call after removed_local_user)
    /// to maintain backward compatability.
    /// </summary>
    local_user_removed = static_cast<uint32_t>(XblSocialManagerEventType::UnknownEvent) + 1,

    /// <summary>Xbox Social User Group load complete (will only trigger for views that take a list of users)</summary>
    social_user_group_loaded = static_cast<uint32_t>(XblSocialManagerEventType::SocialUserGroupLoaded),

    /// <summary>Social user group updated</summary>
    social_user_group_updated = static_cast<uint32_t>(XblSocialManagerEventType::SocialUserGroupUpdated),

    /// <summary>unknown.</summary>
    unknown = static_cast<uint32_t>(XblSocialManagerEventType::UnknownEvent)
};

/// <summary>
/// Possible relationship types to filter by
/// </summary>
enum class relationship_filter
{
    /// <summary>Friends of the user (user is following)</summary>
    friends = static_cast<uint32_t>(XblRelationshipFilter::Friends),

    /// <summary>Favorites of the user</summary>
    favorite = static_cast<uint32_t>(XblRelationshipFilter::Favorite)
};

/// <summary>
/// Identifies type of social user group created
/// </summary>
enum class social_user_group_type
{
    /// <summary>Social user group based off of filters</summary>
    filter_type = static_cast<uint32_t>(XblSocialUserGroupType::FilterType),

    /// <summary>Social user group based off of list of users</summary>
    user_list_type = static_cast<uint32_t>(XblSocialUserGroupType::UserListType)
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
    inline bool has_user_played() const;

    /// <summary>
    /// The last time the user had played
    /// </summary>
    inline utility::datetime last_time_user_played() const;

    /// <summary>
    /// Internal functions
    /// </summary>
    inline title_history(const XblTitleHistory& titleHistory);
    inline title_history(const title_history& other);
    inline title_history& operator=(title_history other);
    inline ~title_history() = default;

private:
    std::shared_ptr<const XblTitleHistory> m_owningPtr{ nullptr };
    const XblTitleHistory* m_titleHistory;
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
    inline const char_t* primary_color() const;

    /// <summary>
    /// Users secondary color
    /// </summary>
    inline const char_t* secondary_color() const;

    /// <summary>
    /// Users tertiary color
    /// </summary>
    inline const char_t* tertiary_color() const;

    /// <summary>
    /// Does a comparison on if preferred colors are equal
    /// </summary>
    inline bool operator!=(const preferred_color& rhs) const;

    /// <summary>
    /// Internal function
    /// </summary>
    inline preferred_color(const XblPreferredColor& preferredColor);

private:
    string_t m_primaryColor;
    string_t m_secondaryColor;
    string_t m_tertiaryColor;
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
    inline uint32_t title_id() const;

    /// <summary>
    /// The active state for the title.
    /// </summary>
    inline bool is_title_active() const;

    /// <summary>
    /// The formatted and localized presence string.
    /// </summary>
    inline const char_t* presence_text() const;

    /// <summary>
    /// The active state for the title.
    /// </summary>
    inline bool is_broadcasting() const;
    
    /// <summary>
    /// Device type
    /// </summary>
    inline xbox::services::presence::presence_device_type device_type() const;

    /// <summary>
    /// Whether or not this is the primary primary presence record
    /// </summary>
    inline bool is_primary() const;

    /// <summary>
    /// Internal functions
    /// </summary>
    inline social_manager_presence_title_record(const XblSocialManagerPresenceTitleRecord& titleRecord);
    inline social_manager_presence_title_record(const social_manager_presence_title_record& other);
    inline social_manager_presence_title_record& operator=(social_manager_presence_title_record other);
    inline ~social_manager_presence_title_record() = default;

private:
    std::shared_ptr<const XblSocialManagerPresenceTitleRecord> m_owningPtr{ nullptr };
    const XblSocialManagerPresenceTitleRecord* m_titleRecord;
    string_t m_presenceText;
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
    inline xbox::services::presence::user_presence_state user_state() const;

    /// <summary>
    /// Collection of presence title record objects returned by a request.
    /// </summary>
    inline const std::vector<social_manager_presence_title_record>& presence_title_records() const;

    /// <summary>
    /// Returns whether the user is playing this title id
    /// </summary>
    inline bool is_user_playing_title(_In_ uint32_t titleId) const;

    /// <summary>
    /// Internal functions
    /// </summary>
    inline social_manager_presence_record(const XblSocialManagerPresenceRecord& presenceRecord);
    inline social_manager_presence_record(const social_manager_presence_record& other);
    inline social_manager_presence_record& operator=(social_manager_presence_record other);
    inline ~social_manager_presence_record() = default;

private:
    std::shared_ptr<const XblSocialManagerPresenceRecord> m_owningPtr{ nullptr };
    const XblSocialManagerPresenceRecord* m_presenceRecord;
    std::vector<social_manager_presence_title_record> m_titleRecords;
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
    inline const char_t* xbox_user_id() const;

    /// <summary>
    /// Whether they are a favorite
    /// </summary>
    inline bool is_favorite() const;

    /// <summary>
    /// Whether the calling user is following them
    /// </summary>
    inline bool is_following_user() const;

    /// <summary>
    /// Whether they calling user is followed by this person
    /// </summary>
    inline bool is_followed_by_caller() const;

    /// <summary>
    /// The display name
    /// </summary>
    inline const char_t* display_name() const;

    /// <summary>
    /// The real name
    /// </summary>
    inline const char_t* real_name() const;

    /// <summary>
    /// The display pic uri
    /// </summary>
    inline const char_t* display_pic_url_raw() const;

    /// <summary>
    /// Whether to use the players avatar
    /// </summary>
    inline bool use_avatar() const;

    /// <summary>
    /// Players gamerscore
    /// </summary>
    inline const char_t* gamerscore() const;

    /// <summary>
    /// Players gamertag
    /// </summary>
    inline const char_t* gamertag() const;

    /// <summary>
    /// Modern gamertag for the player. Not guaranteed to be unique.
    /// </summary>
    inline const char_t* modern_gamertag() const;

    /// <summary>
    /// Suffix appended to modern gamertag to ensure uniqueness. May be empty in some cases.
    /// </summary>
    inline const char_t* modern_gamertag_suffix() const;

    /// <summary>
    /// Combined modern gamertag and suffix. Format will be "MGT#suffix".
    /// Guaranteed to be no more than 16 rendered characters.
    /// </summary>
    inline const char_t* unique_modern_gamertag() const;

    /// <summary>
    /// Users presence record
    /// </summary>
    inline const xbox::services::social::manager::social_manager_presence_record& presence_record() const;

    /// <summary>
    /// Title history for the user
    /// </summary>
    inline const xbox::services::social::manager::title_history& title_history() const;

    /// <summary>
    /// Preferred color for the user
    /// </summary>
    inline const preferred_color& preferred_color() const;

    /// <summary>
    /// Internal functions
    /// </summary>
    inline xbox_social_user(const XblSocialManagerUser& user);
    inline xbox_social_user(const xbox_social_user& other);
    inline xbox_social_user& operator=(xbox_social_user other);
    inline ~xbox_social_user() = default;

private:
    std::shared_ptr<const XblSocialManagerUser> m_owningPtr{ nullptr };
    const XblSocialManagerUser* m_user;

    string_t m_gamerscore;
    string_t m_gamertag;
    string_t m_modernGamertag;
    string_t m_modernGamertagSuffix;
    string_t m_uniqueModernGamertag;
    string_t m_xboxUserId;
    string_t m_displayName;
    string_t m_realName;
    string_t m_displayPicUrlRaw;
    xbox::services::social::manager::title_history m_titleHistory;
    xbox::services::social::manager::preferred_color m_preferredColor;
    xbox::services::social::manager::social_manager_presence_record m_presenceRecord;
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
    inline const char_t* xbox_user_id() const;

    /// <summary>
    /// Internal function
    /// </summary>
    inline xbox_user_id_container(_In_ uint64_t xuid);

private:
    string_t m_xboxUserId;
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
    inline xbox_live_user_t user() const;

    /// <summary>
    /// The type of event this is 
    /// Tells the caller what to cast the event_args to
    /// </summary>
    inline social_event_type event_type() const;

    /// <summary>
    /// List of users this event affects
    /// </summary>
    inline std::vector<xbox_user_id_container> users_affected() const;

    /// <summary>
    /// The social event args
    /// </summary>
    inline std::shared_ptr<social_event_args> event_args() const;

    /// <summary>
    /// Error that occurred
    /// </summary>
    inline std::error_code err() const;

    /// <summary>
    /// Error message
    /// </summary>
    inline std::string err_message() const;

    /// <summary>
    /// Internal functions
    /// </summary>
    inline social_event(const XblSocialManagerEvent& event);
    inline social_event(const xbox_live_user_t& removedUser);

private:
    XblSocialManagerEvent m_event{};
    social_event_type m_eventType{ social_event_type::unknown };
    std::shared_ptr<social_event_args> m_args;
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
    inline std::vector<xbox_social_user*> users() const;

    /// <summary>
    /// Returns copied group of users from social user group
    /// </summary>
    /// <param name="socialUserVector">Vector of social users to populate</param>
    /// <returns>An xbox_live_result representing the success of copying the users</returns>
    inline xbox_live_result<void> get_copy_of_users(
        _Inout_ std::vector<xbox_social_user>& socialUserVector
    );

    /// <summary>
    /// Type of social user group
    /// </summary>
    inline social_user_group_type social_user_group_type() const;

    /// <summary>
    /// Users who are contained in this user group currently
    /// For list this is static, for filter this is dynamic and will change on do_work
    /// </summary>
    inline std::vector<xbox_user_id_container> users_tracked_by_social_user_group() const;

    /// <summary>
    /// The local user who the user group is related to
    /// </summary>
    inline xbox_live_user_t local_user() const;

    /// <summary>
    /// Returns the presence filter used if group type is filter type
    /// </summary>
    inline presence_filter presence_filter_of_group() const;

    /// <summary>
    /// Returns the relationship filter used if group type is filter type
    /// </summary>
    inline relationship_filter relationship_filter_of_group() const;

    /// <summary>
    /// Returns users from xuids. Pointers become invalidated by next do_work
    /// </summary>
    inline std::vector<xbox_social_user*> get_users_from_xbox_user_ids(_In_ const std::vector<xbox_user_id_container>& xboxUserIds);

    /// <summary>
    /// Internal function
    /// </summary>
    inline xbox_social_user_group(XblSocialManagerUserGroupHandle group);

private:
    inline void PopulateUsers() const;

    XblSocialManagerUserGroupHandle m_group;
    mutable std::vector<xbox_social_user> m_users;

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
    inline std::shared_ptr<xbox_social_user_group> social_user_group() const;

    /// <summary>
    /// internal function
    /// </summary>
    inline social_user_group_loaded_event_args(std::shared_ptr<xbox_social_user_group> group);

private:
    std::shared_ptr<xbox_social_user_group> m_group;
};

/// <summary>
/// Social Manager that handles core logic
/// </summary>
class social_manager
{
public:
    /// <summary>
    /// Gets the social_manager singleton instance
    /// </summary>
    inline static std::shared_ptr<social_manager> get_singleton_instance();

    /// <summary>
    /// Create a social graph for the specified local user
    /// The result of a local user being added will be triggered through the local_user_added event in do_work
    /// </summary>
    /// <param name="user">Xbox Live User</param>
    /// <param name="extraDetailLevel">The level of verbosity that should be in the xbox_social_user</param>
    /// <returns>An xbox_live_result to report any potential error</returns>
    inline xbox_live_result<void> add_local_user(
        _In_ xbox_live_user_t user,
        _In_ social_manager_extra_detail_level extraDetailLevel
    );

    /// <summary>
    /// Removes a social graph for the specified local user
    /// The result of a local user being added will be triggered through the local_user_removed event in do_work
    /// </summary>
    /// <param name="user">Xbox Live User</param>
    /// <returns>An xbox_live_result to report any potential error</returns>
    inline xbox_live_result<void> remove_local_user(
        _In_ xbox_live_user_t user
    );

    /// <summary>
    /// Called whenever the title wants to update the social graph and get list of change events
    /// Must be called every frame for data to be up to date
    /// </summary>
    /// <returns> The list of what has changed in between social graph updates</returns>
    inline std::vector<social_event> do_work();

    /// <summary>
    /// Constructs a social Xbox Social User Group, which is a collection of users with social information
    /// The result of a user group being loaded will be triggered through the social_user_group_loaded event in do_work
    /// </summary>
    /// <param name="user">Xbox Live User</param>
    /// <param name="presenceDetailLevel">The restriction of users based on their presence and title activity</param>
    /// <param name="relationshipFilter">The restriction of users based on their relationship to the calling user</param>
    /// <returns>An xbox_live_result of the created Xbox Social User Group</returns>
    inline xbox_live_result<std::shared_ptr<xbox_social_user_group>> create_social_user_group_from_filters(
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
    inline xbox_live_result<std::shared_ptr<xbox_social_user_group>> create_social_user_group_from_list(
        _In_ xbox_live_user_t user,
        _In_ std::vector<string_t> xboxUserIdList
    );

    /// <summary>
    /// Destroys a created social Xbox Social User Group
    /// This will stop updating the Xbox Social User Group and remove tracking for any users the Xbox Social User Group holds
    /// </summary>
    /// <param name="socialUserGroup">The social Xbox Social User Group to destroy and stop tracking</param>
    /// <returns>An xbox_live_result to report any potential error</returns>
    inline xbox_live_result<void> destroy_social_user_group(
        _In_ std::shared_ptr<xbox_social_user_group> socialUserGroup
    );

    /// <summary>
    /// Returns all local users who have been added to the social manager
    /// </summary>
    inline std::vector<xbox_live_user_t> local_users() const;

    /// <summary>
    /// Updates specified social user group to new group of users
    /// Does a diff to see which users have been added or removed from 
    /// The result of a user group being updated will be triggered through the social_user_group_updated event in do_work
    /// </summary>
    /// <param name="group">The xbox social user group to add users to</param>
    /// <param name="users">List of users to add to the xbox social user group. Total number of users not in social graph is limited at 100.</param>
    /// <returns>An xbox_live_result representing the success of adding the users to the group</returns>
    inline xbox_live_result<void> update_social_user_group(
        _In_ const std::shared_ptr<xbox_social_user_group>& group,
        _In_ const std::vector<string_t>& users
    );
    
    /// <summary>
    /// Whether to enable social manager to poll every 30 seconds from the presence service 
    /// </summary>
    /// <param name="user">Xbox Live User</param>
    /// <param name="shouldEnablePolling">Whether or not polling should enabled</param>
    /// <returns>An xbox_live_result representing the success enabling polling</returns>
    inline xbox_live_result<void> set_rich_presence_polling_status(
        _In_ xbox_live_user_t user,
        _In_ bool shouldEnablePolling
    );

private:
    social_manager() = default;
    std::vector<xbox_live_user_t> m_removedUsers;
    std::unordered_map<XblSocialManagerUserGroupHandle, std::shared_ptr<xbox_social_user_group>> m_groups;

    friend class social_event;
};

}}}}

#include "impl/social_manager.hpp"
