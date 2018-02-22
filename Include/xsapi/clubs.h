// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "types.h"
#include "errors.h"

namespace xbox {
    namespace services {
        class xbox_live_context_impl;

        /// <summary>
        /// Contains classes and enumerations that let you create
        /// and configure xbox live clubs from within a game
        /// </summary>
        namespace clubs {


class clubs_serializers;
class clubs_service_impl;

/// <summary>The visibility of the club</summary>
enum class club_type
{
    /// <summary>Club type is unknown</summary>
    unknown,
    
    /// <summary>Public club</summary>
    public_club,

    /// <summary>Private club</summary>
    private_club,

    /// <summary>Hidden club</summary>
    hidden_club,
};

/// <summary>
/// A users role within a club. Users may have multiple roles at once, although some are mutually exclusive
/// </summary>
enum class club_role
{
    /// <summary>
    /// Not a member of the club. This will never be returned when a users roles are queried - if a user
    /// has no association with a club, an empty vector of would be returned. This enum value is used exclusively
    /// for permissions/settings.
    /// </summary>
    nonmember,

    /// <summary>Member of a club</summary>
    member,

    /// <summary>Moderator of a club</summary>
    moderator,

    /// <summary>Owner of a club</summary>
    owner,

    /// <summary>User has requested to join a club</summary>
    requested_to_join,

    /// <summary>User has been recommended for a club</summary>
    recommended,

    /// <summary>User has been invited to a club</summary>
    invited,

    /// <summary>
    /// User has been banned from all interaction with a club. A user cannot have any other role
    /// with a club if they are banned from it
    /// </summary>
    banned,

    /// <summary>Follower of a club</summary>
    follower
};

/// <summary>A users presence with respect to a club</summary>
enum class club_user_presence
{
    /// <summary>User is no longer on a club page.</summary>
    not_in_club,

    /// <summary>User is viewing the club, but not on any specific page.</summary>
    in_club,

    /// <summary>User is on the chat page.</summary>
    chat,

    /// <summary>User is viewing the club feed.</summary>
    feed,

    /// <summary>User is viewing the club roster/presence.</summary>
    roster,

    /// <summary>User is on the play tab in the club (not actually playing anything).</summary>
    play,

    /// <summary>User is playing the associated game.</summary>
    in_game
};

/// <summary>
/// Represents a configurable setting for a club that has a single active value
/// </summary>
template<typename T>
class club_setting
{
public:
    /// <summary>Current value of the setting</summary>
    _XSAPIIMP const T& value() const
    {
        return m_value;
    }

    /// <summary>
    /// List of possible values for the setting. If allowed values have not been configured an empty
    /// vector will be returned.
    /// </summary>
    _XSAPIIMP const std::vector<T>& allowed_values() const
    {
        return m_allowedValues;
    }

    /// <summary>Can the user viewing the setting change it</summary>
    _XSAPIIMP bool can_viewer_change_setting() const
    {
        return m_canViewerChangeSetting;
    }

private:
    T m_value;
    std::vector<T> m_allowedValues;
    bool m_canViewerChangeSetting;

    friend class clubs_serializers;
    friend class club;
};

/// <summary>
/// Represents a configurable setting for a club that can have multiple values at once
/// </summary>
template<typename T>
class club_multi_setting
{
public:
    /// <summary>Current values of the setting</summary>
    _XSAPIIMP const std::vector<T>& values() const
    {
        return m_values;
    }

    /// <summary>
    /// List of possible values for the setting. If allowed values have not been configured an empty
    /// vector will be returned.
    /// </summary>
    _XSAPIIMP const std::vector<T>& allowed_values() const
    {
        return m_allowedValues;
    }

    /// <summary>Can the user viewing the setting change it</summary>
    _XSAPIIMP bool can_viewer_change_setting() const
    {
        return m_canViewerChangeSetting;
    }

private:
    std::vector<T> m_values;
    std::vector<T> m_allowedValues;
    bool m_canViewerChangeSetting;

    friend class clubs_serializers;
};

/// <summary>
/// Configurable settings for a club.
/// </summary>
class club_profile
{
public:
    /// <summary>Name of the club</summary>
    _XSAPIIMP const club_setting<string_t>& name() const;

    /// <summary>Description of the club</summary>
    _XSAPIIMP const club_setting<string_t>& description() const;

    /// <summary>Is mature content enabled within the club</summary>
    _XSAPIIMP const club_setting<bool>& mature_content_enabled() const;

    /// <summary>
    /// If true, the club can only broadcast streams for titles with which the club is officially affiliated.
    /// </summary>
    _XSAPIIMP const club_setting<bool>& watch_club_titles_only() const;

    /// <summary>Should the club show up in search results</summary>
    _XSAPIIMP const club_setting<bool>& is_searchable() const;

    /// <summary>Should the club show up in recommendations</summary>
    _XSAPIIMP const club_setting<bool>& is_recommendable() const;

    /// <summary>Can users request to join the club</summary>
    _XSAPIIMP const club_setting<bool>& request_to_join_enabled() const;

    /// <summary>Can users leave the club</summary>
    _XSAPIIMP const club_setting<bool>& leave_enabled() const;

    /// <summary>Can ownership of the club be transferred</summary>
    _XSAPIIMP const club_setting<bool>& transfer_ownership_enabled() const;

    /// <summary>URL for display image</summary>
    _XSAPIIMP const club_setting<string_t>& display_image_url() const;

    /// <summary>URL for background image</summary>
    _XSAPIIMP const club_setting<string_t>& background_image_url() const;

    /// <summary>Tags associated with the club (ex. "Hate-Free", "Women only")</summary>
    _XSAPIIMP const club_multi_setting<string_t>& tags() const;

    /// <summary>The club's preferred locale</summary>
    _XSAPIIMP const club_setting<string_t>& preferred_locale() const;

    /// <summary>List of titles Ids associated with the club</summary>
    _XSAPIIMP const club_multi_setting<string_t>& associated_titles() const;

    /// <summary>Primary color of the club</summary>
    _XSAPIIMP const club_setting<string_t>& primary_color() const;

    /// <summary>Secondary color of the club</summary>
    _XSAPIIMP const club_setting<string_t>& secondary_color() const;

    /// <summary>Tertiary color of the club</summary>
    _XSAPIIMP const club_setting<string_t>& tertiary_color() const;

private:
    club_setting<string_t> m_nameSetting;
    club_setting<string_t> m_descriptionSetting;
    club_setting<bool> m_matureContentEnabledSetting;
    club_setting<bool> m_watchClubTitlesOnlySetting;
    club_setting<bool> m_isSearchableSetting;
    club_setting<bool> m_isRecommendableSetting;
    club_setting<bool> m_requestToJoinEnabledSetting;
    club_setting<bool> m_leaveEnabledSetting;
    club_setting<bool> m_transferOwnershipEnabledSetting;
    club_setting<string_t> m_displayImageUrlSetting;
    club_setting<string_t> m_backgroundImageUrlSetting;
    club_multi_setting<string_t> m_tagsSetting;
    club_setting<string_t> m_preferredLocaleSetting;
    club_multi_setting<string_t> m_associatedTitlesSetting;
    club_setting<string_t> m_primaryColorSetting;
    club_setting<string_t> m_secondaryColorSetting;
    club_setting<string_t> m_tertiaryColorSetting;

    friend class clubs_serializers;
    friend class club;
};

/// <summary>
/// Represents an action that users can take within a club
/// </summary>
class club_action_setting
{
public:
    /// <summary>
    /// Club role required to take the action. Will only every be one of the following:
    /// moderator, owner, member, nonmember
    /// </summary>
    _XSAPIIMP club_role required_role() const;

    /// <summary>Can the viewer take the action</summary>
    _XSAPIIMP bool can_viewer_act() const;

    /// <summary>
    /// List of possible values for the setting. If allowed values have not been configured an empty
    /// vector will be returned.
    /// </summary>
    _XSAPIIMP const std::vector<club_role>& allowed_values() const;

    /// <summary>Can the user viewing the setting change it</summary>
    _XSAPIIMP bool can_viewer_change_setting() const;

private:
    club_role m_requiredRole;
    bool m_canViewerAct;
    std::vector<club_role> m_allowedValues;
    bool m_canViewerChangeSetting;

    friend class clubs_serializers;
};

/// <summary>
/// Settings controlling who can take various actions in the club
/// </summary>
class club_action_settings
{
public:
    /// <summary>Setting controlling who can post to the club feed</summary>
    _XSAPIIMP const club_action_setting& post_to_feed() const;

    /// <summary>Setting controlling who can view the club feed</summary>
    _XSAPIIMP const club_action_setting& view_feed() const;

    /// <summary>Setting controlling who can write in chat</summary>
    _XSAPIIMP const club_action_setting& write_in_chat() const;

    /// <summary>Setting controlling who view chat</summary>
    _XSAPIIMP const club_action_setting& view_chat() const;

    /// <summary>Setting controlling who can set chat topic</summary>
    _XSAPIIMP const club_action_setting& set_chat_topic() const;

    /// <summary>Setting controlling who can join club looking for game requests</summary>
    _XSAPIIMP const club_action_setting& join_looking_for_game() const;

    /// <summary>Setting controlling who can create club looking for game requests</summary>
    _XSAPIIMP const club_action_setting& create_looking_for_game() const;

    /// <summary>Setting controlling who view club looking for game requests</summary>
    _XSAPIIMP const club_action_setting& view_looking_for_game() const;

    /// <summary>Setting controlling who invite people or accept requests to join the club</summary>
    _XSAPIIMP const club_action_setting& invite_or_accept_join_requests() const;

    /// <summary>Setting controlling who kick and ban club members</summary>
    _XSAPIIMP const club_action_setting& kick_or_ban() const;

    /// <summary>Setting controlling who can view the club roster</summary>
    _XSAPIIMP const club_action_setting& view_roster() const;

    /// <summary>Setting controlling who can update the club profile</summary>
    _XSAPIIMP const club_action_setting& update_profile() const;

    /// <summary>Setting controlling who can delete the club profile</summary>
    _XSAPIIMP const club_action_setting& delete_profile() const;

    /// <summary>Setting controlling who can view the club profile</summary>
    _XSAPIIMP const club_action_setting& view_profile() const;

private:
    club_action_setting m_postToFeedSetting;
    club_action_setting m_viewFeedSetting;
    club_action_setting m_writeInChatSetting;
    club_action_setting m_viewChatSetting;
    club_action_setting m_setChatTopicSetting;
    club_action_setting m_joinLFGSetting;
    club_action_setting m_createLFGSetting;
    club_action_setting m_viewLFGSetting;
    club_action_setting m_inviteOrAcceptJoinRequestsSetting;
    club_action_setting m_kickOrBanSetting;
    club_action_setting m_viewRosterSetting;
    club_action_setting m_updateProfileSetting;
    club_action_setting m_deleteProfileSetting;
    club_action_setting m_viewProfileSetting;

    friend class clubs_serializers;
};

/// <summary> 
/// The history of a users role within a club.
/// </summary>
class club_role_record
{
public:
    /// <summary>Xuid that belongs to the role.</summary>
    _XSAPIIMP const string_t& xuid() const;

    /// <summary>Role of the user .</summary>
    _XSAPIIMP club_role role() const;

    /// <summary>Actor Xuid that was responsible for user belonging to the role.</summary>
    _XSAPIIMP const string_t& actor_xuid() const;

    /// <summary>When the user was added to the role.</summary>
    _XSAPIIMP const utility::datetime& created_date() const;

private:
    club_role m_role;
    string_t m_xuid;
    string_t m_actorXuid;
    utility::datetime m_createdDate;

    friend class clubs_serializers;
};

/// <summary>
/// Represents a club roster, grouped by club_role. 
/// </summary>
class club_roster
{
public:
    /// <summary>Club moderators</summary>
    _XSAPIIMP const std::vector<club_role_record>& moderators() const;

    /// <summary>Users who've requested to join the club</summary>
    _XSAPIIMP const std::vector<club_role_record>& requested_to_join() const;

    /// <summary>Users who've been recommended for the club</summary>
    _XSAPIIMP const std::vector<club_role_record>& recommended() const;

    /// <summary>Users who've been banned from the club</summary>
    _XSAPIIMP const std::vector<club_role_record>& banned() const;

private:
    std::vector<club_role_record> m_moderators;
    std::vector<club_role_record> m_requestedToJoin;
    std::vector<club_role_record> m_recommended;
    std::vector<club_role_record> m_banned;

    friend class clubs_serializers;
};

/// <summary>
/// Details about users presence with respect to the club
/// </summary>
class club_user_presence_record
{
public:
    /// <summary>Xuid of the user who was present at the club.</summary>
    _XSAPIIMP const string_t& xuid() const;

    /// <summary>Time when the user was last present within the club.</summary>
    _XSAPIIMP const utility::datetime& last_seen() const;

    /// <summary>Users state when they were last seen.</summary>
    _XSAPIIMP club_user_presence last_seen_state() const;

private:
    string_t m_xuid;
    utility::datetime m_lastSeen;
    club_user_presence m_lastSeenState;

    friend class clubs_serializers;
};

/// <summary>
/// Represents a search facet result
/// </summary>
class club_search_facet_result
{
public:
    /// <summary>
    /// The particular value of the facet. For example, if the facet was "tags", this value might be 
    /// something like "cooperative".
    /// </summary>
    _XSAPIIMP const string_t& value() const;

    /// <summary>The number of times the value occurred in the returned search results. </summary>
    _XSAPIIMP uint32_t count() const;
    
private:
    uint32_t m_count;
    string_t m_value;

    friend class clubs_serializers;
};

/// <summary>
/// Represents info about a particular user's owned clubs
/// </summary>
class clubs_owned_result
{
public:
    /// <summary>Club Ids of all owned clubs</summary>
    _XSAPIIMP const std::vector<string_t>& club_ids() const;

    /// <summary>Number of clubs the user may create in addition to the ones they already own</summary>
    _XSAPIIMP uint32_t remaining_clubs() const;

private:
    std::vector<string_t> m_clubIds;
    uint32_t m_remainingClubs;

    friend class clubs_serializers;
};

/// <summary>
/// Represents info about the presence of club members within the club
/// </summary>
class club_presence_counts
{
public:
    /// <summary>The total number of club members that been tracked by club presence</summary>
    _XSAPIIMP uint32_t total_count() const;

    /// <summary>The number of members who are active within the club</summary>
    _XSAPIIMP uint32_t here_now() const;

    /// <summary>The number of members who have been active within the last 24 hours</summary>
    _XSAPIIMP uint32_t here_today() const;

    /// <summary>The number of members who are currently in game</summary>
    _XSAPIIMP uint32_t in_game_now() const;

private:
    uint32_t m_totalCount;
    uint32_t m_hereNow;
    uint32_t m_hereToday;
    uint32_t m_inGameNow;

    friend class clubs_serializers;
};

/// <summary>
/// Represents a search auto complete suggestion
/// </summary>
class club_search_auto_complete
{
public:
    /// <summary>Club Id of suggested club</summary>
    _XSAPIIMP const string_t& id() const;

    /// <summary>Name of suggested club</summary>
    _XSAPIIMP const string_t& name() const;

    /// <summary>Club description</summary>
    _XSAPIIMP const string_t& description() const;

    /// <summary>URL for the club display image</summary>
    _XSAPIIMP const string_t& display_image_url() const;

    /// <summary>Score of the suggestion result relative to others. Larger scores are more relevant results.</summary>
    _XSAPIIMP double score() const;

    /// <summary>Suggested query text for this club</summary>
    _XSAPIIMP const string_t& suggested_query_text() const;

    /// <summary>Tags associated with the club</summary>
    _XSAPIIMP const std::vector<string_t>& tags() const;

    /// <summary>Title Ids associated with the club</summary>
    _XSAPIIMP const std::vector<string_t>& associated_titles() const;

private:
    string_t m_id;
    string_t m_name;
    string_t m_description;
    string_t m_displayImageUrl;
    double m_score;
    string_t m_suggestedQueryText;
    std::vector<string_t> m_tags;
    std::vector<string_t> m_associatedTitles;

    friend class clubs_serializers;
};

/// <summary>
/// Information about a Club. Most fields are populated from clubhub endpoint.
/// </summary>
class club
{
public:
    club() {}

    /// <summary>ClubId</summary>
    _XSAPIIMP const string_t& id() const;

    /// <summary>Type (visibility) of club</summary>
    _XSAPIIMP club_type type() const;

    /// <summary>When the club was created.</summary>
    _XSAPIIMP const utility::datetime& created() const;

    /// <summary>Club owner's Xuid</summary>
    _XSAPIIMP const string_t& owner() const;

    /// <summary>Title family Id</summary>
    _XSAPIIMP const string_t& title_family_id() const;

    /// <summary>Localized Title Family Name</summary>
    _XSAPIIMP const string_t& title_family_name() const;

    /// <summary>Club short name</summary>
    _XSAPIIMP const string_t& short_name() const;

    /// <summary>Configurable club attributes</summary>
    _XSAPIIMP const club_profile& profile() const;

    /// <summary>Club's glyph image url</summary>
    _XSAPIIMP const string_t& glyph_image_url() const;

    /// <summary>Club's banner image url</summary>
    _XSAPIIMP const string_t& banner_image_url() const;

    /// <summary>Whether or not the club is currently suspended.</summary>
    _XSAPIIMP bool is_club_suspended() const;

    /// <summary>When the club remains suspended until.</summary>
    _XSAPIIMP const utility::datetime& suspended_until() const;

    /// <summary>Club founder's Xuid.</summary>
    _XSAPIIMP const string_t& founder_xuid() const;

    /// <summary>Number of followers of the club.</summary>
    _XSAPIIMP uint32_t followers_count() const;

    /// <summary>Count of members present in the club.</summary>
    _XSAPIIMP uint32_t presence_count() const;

    /// <summary>Count of members present in the club today.</summary>
    _XSAPIIMP uint32_t presence_today_count() const;

    /// <summary>Number of club members.</summary>
    _XSAPIIMP uint32_t members_count() const;

    /// <summary>Number of club moderators.</summary>
    _XSAPIIMP uint32_t moderators_count() const;

    /// <summary>Number of users recommended for the club.</summary>
    _XSAPIIMP uint32_t recommended_count() const;

    /// <summary>Number of users requesting to join the club.</summary>
    _XSAPIIMP uint32_t requested_to_join_count() const;

    /// <summary>Number of reports for the club.</summary>
    _XSAPIIMP uint32_t report_count() const;

    /// <summary>Number of reported items for the club.</summary>
    _XSAPIIMP uint32_t reported_items_count() const;

    /// <summary> 
    /// Settings dictating what actions users can take within the club depending on their role.
    /// </summary> 
    _XSAPIIMP const club_action_settings& action_settings() const;

    /// <summary>
    /// Roles the caller has in the club. If the caller has no association with the club an empty vector will be returned.
    /// </summary>
    _XSAPIIMP const std::vector<club_role>& viewer_roles() const;

    /// <summary>
    /// Retrieves a roster of club members that have a special relationship with the club (something other than just member).
    /// </summary>
    /// <returns>
    /// A club_roster object which contains the moderators, recommended users, banned users, and users 
    /// who have requested to join the club.
    /// </returns>
    /// <remarks>
    /// Calls V4 GET clubhub.xboxlive.com/clubs/Ids{clubId}/decoration/roster
    /// </remarks>
    _XSAPIIMP pplx::task<xbox::services::xbox_live_result<club_roster>> get_roster();

    /// <summary>Gets records of (at most) the last 1000 members active within a club.</summary>
    /// <param name="clubId">The club ID of the club.</param>
    /// <returns>A vector containing club_user_presence_records describing the presence of the club's members.</returns>
    /// <remarks>
    /// Calls V4 GET clubhub.xboxlive.com/clubs/Ids{clubId}/decoration/clubpresence
    /// </remarks>
    _XSAPIIMP pplx::task<xbox::services::xbox_live_result<std::vector<club_user_presence_record>>> get_club_user_presence_records();

    /// <summary>
    /// Renames the club. Can only be called by the club owner. Some restrictions apply when renaming and public or 
    /// private club; hidden clubs can be renamed freely. Note that the club object this is called from will not be 
    /// updated with the new name even if this method is successful, but if the club object is retrieved again the 
    /// new name will be reflected.
    ///</summary>
    /// <param name="newName">New name to give the club</param> 
    /// <remarks>
    /// Calls V1 POST clubaccounts.xboxlive.com/clubs/{clubId}
    /// </remarks>
    _XSAPIIMP pplx::task<xbox::services::xbox_live_result<void>> rename_club(
        _In_ const string_t& newName
        );

    /// <summary>Returns the roles a given user has within the club.</summary>
    /// <param name="xuid">Xuid of the user whose roles to query</param>
    /// <returns>
    /// A vector containing the club_roles the user has within the club. If the user has no association with 
    /// the club an empty vector will be returned.
    /// </returns>
    /// <remarks>
    /// Calls V4 GET clubhub.xboxlive.com/clubs/Xuid{xuid}
    /// </remarks>
    _XSAPIIMP pplx::task<xbox::services::xbox_live_result<std::vector<club_role>>> get_roles(
        _In_ const string_t& xuid
        );

    /// <summary>Returns records of the callers roles within the club.</summary>
    /// <returns>
    /// A vector containing the club_role_records which describe the history of a users roles in the club.
    /// If the user has no association with the club an empty vector will be returned.
    /// </returns>
    /// <remarks>
    /// Calls V4 GET clubhub.xboxlive.com/clubs/Xuid{xuid}
    /// </remarks>
    _XSAPIIMP pplx::task<xbox::services::xbox_live_result<std::vector<club_role_record>>> get_role_records();

    /// <summary>Returns records for a given users roles within the club.</summary>
    /// <param name="xuid">Xuid of the user whose roles to query</param>
    /// <returns>
    /// A vector containing the club_role_records which describe the history of a users roles in the club.
    /// If the user has no association with the club an empty vector will be returned.
    /// </returns>
    /// <remarks>
    /// Calls V4 GET clubhub.xboxlive.com/clubs/Xuid{xuid}
    /// </remarks>
    _XSAPIIMP pplx::task<xbox::services::xbox_live_result<std::vector<club_role_record>>> get_role_records(
        _In_ const string_t& xuid
        );

    /// <summary>Gets information about club member presence.</summary>
    /// <returns>A club_presence_counts object indicating the number of club members present currently/today</returns>
    /// <remarks>
    /// Calls V1 GET clubpresence.xboxlive.com/clubs/{clubId}/users/count
    /// </remarks>
    _XSAPIIMP pplx::task<xbox::services::xbox_live_result<club_presence_counts>> get_presence_counts();

    /// <summary>
    /// Sets the callers presence within the club. This keeps track of which club page, if any, 
    /// the user is currently viewing.
    /// </summary>
    /// <param name="xuid">Xuid of the user to set.</param>
    /// <param name="presence">The current presence state of the user</param>
    /// <remarks>
    /// Calls V1 POST clubpresence.xboxlive.com/clubs/{clubId}/users/{userId}
    /// </remarks>
    _XSAPIIMP pplx::task<xbox::services::xbox_live_result<void>> set_presence_within_club(
        _In_ club_user_presence presence
        );

    /// <summary> 
    /// Attempts to add the caller to the club. If the caller has not yet been invited, this call will send 
    /// a request to join the club which will need to be approved by a moderator before the caller becomes a member.
    /// If the caller has been invited already, they will be immediately added as a member.
    /// </summary>
    /// <returns>A vector containing the roles the calling user now has with the club.</returns>
    /// <remarks>
    /// Calls V1 PUT clubroster.xboxlive.com/clubs/{clubId}/users/{userId}
    /// </remarks>
    _XSAPIIMP pplx::task<xbox::services::xbox_live_result<std::vector<club_role>>> add_user_to_club();

    /// <summary> 
    /// Deletes all relationships the calling user has with a club including: membership, outstanding invitations, 
    /// recommendations to join, and following.
    /// </summary>
    /// <returns>
    /// A vector containing the roles the calling user now has with the club. If the call succeeds, it will
    /// not contain any role other than "banned" (which cannot be removed except by club owners and moderators)
    /// </returns>
    /// <remarks>
    /// Calls V1 DELETE clubroster.xboxlive.com/clubs/{clubId}/users/{userId}
    /// </remarks>
    _XSAPIIMP pplx::task<xbox::services::xbox_live_result<std::vector<club_role>>> remove_user_from_club();

    /// <summary> 
    /// Depending on the role of the caller, this function will have a different effect:
    /// 1) If the caller is a moderator of the club, the specified user will be either invited or added as
    /// a club member immediately. In order for the target user to be added immediately, they will have
    /// to have previously requested to join using the join_club method. Moderators can examine the roster() 
    /// to get a list of users who have requested to join.
    /// 2) If the caller is a member of the club, but not a moderator, the target user will be recommended
    /// for the club. For the user to be finally added as a member, add_user_to_club still must be called 
    /// by a moderator AND add_user_to_club() must be called by the target user. 
    /// Note: If the club doesn't support recommendations, an error will be returned.
    /// 3) If the target user is the caller, this method will have the same exact behavior as add_user_to_club()    
    /// </summary>
    /// <param name="xuid">The xuid of the user to add.</param>
    /// <returns>A vector containing the roles the target user now has with the club.</returns>
    /// <remarks>
    /// Calls V1 PUT clubroster.xboxlive.com/clubs/{clubId}/users/{userId}
    /// </remarks>
    _XSAPIIMP pplx::task<xbox::services::xbox_live_result<std::vector<club_role>>> add_user_to_club(
        _In_ const string_t& xuid
        );

    /// <summary> 
    /// Removes all relationships between the target user and the club including: membership, outstanding
    /// invitations/recommendations, and following. Can only be called by club moderator (or on yourself, 
    /// which has the same effect as calling remove_user_from_club())
    /// </summary>
    /// <param name="xuid">The xuid of the user to remove.</param>
    /// <returns>A vector containing the roles the target user now has with the club.</returns>
    /// <remarks>
    /// Calls V1 DELETE clubroster.xboxlive.com/clubs/{clubId}/users/{userId}
    /// </remarks>
    _XSAPIIMP pplx::task<xbox::services::xbox_live_result<std::vector<club_role>>> remove_user_from_club(
        _In_ const string_t& xuid
        );

    /// <summary>Adds the calling user as a follower of the club.</summary>
    /// <returns>A vector containing the roles the calling user now has with the club.</returns>
    /// <remarks>
    /// Calls V1 POST clubroster.xboxlive.com/clubs/{clubId}/users/{userId}/roles
    /// </remarks>
    _XSAPIIMP pplx::task<xbox::services::xbox_live_result<std::vector<club_role>>> follow_club();

    /// <summary>Removes the calling user as a follower of the club.</summary>
    /// <returns>A vector containing the roles the calling user now has with the club.</returns>
    /// <remarks>
    /// Calls V1 DELETE clubroster.xboxlive.com/clubs/{clubId}/users/{userId}/roles/Follower
    /// </remarks>
    _XSAPIIMP pplx::task<xbox::services::xbox_live_result<std::vector<club_role>>> unfollow_club();

    /// <summary>Bans the specified user from the club. Can only be called by club moderators or owner.</summary>
    /// <param name="xuid">The xuid of the user to ban.</param>
    /// <returns>A vector containing the roles the target user now has with the club.</returns>
    /// <remarks>
    /// Calls V1 POST clubroster.xboxlive.com/clubs/{clubId}/users/{userId}/roles
    /// </remarks>
    _XSAPIIMP pplx::task<xbox::services::xbox_live_result<std::vector<club_role>>> ban_user_from_club(
        _In_ const string_t& xuid
        );

    /// <summary> 
    /// Unbans the specified user from the club. Can only be called by club moderators or owner.
    /// Note: A similar affect can also be achieved by a moderator calling add_user_to_club. In that
    /// case however, banned users will immediately receive an invitation to the club in addition to
    /// being unbanned.
    /// </summary>
    /// <param name="xuid">The xuid of the user to ban.</param>
    /// <returns>A vector containing the roles the target user now has with the club.</returns>
    /// <remarks>
    /// Calls V1 DELETE clubroster.xboxlive.com/clubs/{clubId}/users/{userId}/roles/Banned
    /// </remarks>
    _XSAPIIMP pplx::task<xbox::services::xbox_live_result<std::vector<club_role>>> unban_user_from_club(
        _In_ const string_t& xuid
        );

    /// <summary> 
    /// Promotes the specified user to moderator of the club. The specified user must already be a
    /// member of the club. Can only be called by the club owner.
    /// </summary>
    /// <param name="xuid">The xuid of the user to promote.</param>
    /// <returns>A vector containing the roles the target user now has with the club.</returns>
    /// <remarks>
    /// Calls V1 POST clubroster.xboxlive.com/clubs/{clubId}/users/{userId}/roles
    /// </remarks>
    _XSAPIIMP pplx::task<xbox::services::xbox_live_result<std::vector<club_role>>> add_club_moderator(
        _In_ const string_t& xuid
        );

    /// <summary> 
    /// Removes the specified user as a moderator of the club. They will retain there other roles
    /// within the club. Can only be called by the club owner.
    /// </summary>
    /// <param name="xuid">The xuid of the user to demote.</param>
    /// <returns>A vector containing the roles the target user now has with the club.</returns>
    /// <remarks>
    /// Calls V1 DELETE clubroster.xboxlive.com/clubs/{clubId}/users/{userId}/roles/Moderator
    /// </remarks>
    _XSAPIIMP pplx::task<xbox::services::xbox_live_result<std::vector<club_role>>> remove_club_moderator(
        _In_ const string_t& xuid
        );

private:
    club(_In_ std::shared_ptr<clubs_service_impl> clubsService);

    /// Wraps PUT/DELETE clubroster.xboxlive.com/clubs/{clubId}/users/{userId}
    /// If advanceWithinClub is true PUT is called, otherwise DELETE is called
    pplx::task<xbox::services::xbox_live_result<std::vector<club_role>>> update_users_club_roles(
        _In_ const string_t& xuid,
        _In_ bool advanceWithinClub
        );

    /// Wraps POST/DELETE clubroster.xboxlive.com/clubs/{clubId}/users/{userId}/roles
    /// If addRole is true POST is called, otherwise DELETE is called. Only supported roles are
    /// moderator, follower, and banned
    pplx::task<xbox::services::xbox_live_result<std::vector<club_role>>> set_users_club_roles(
        _In_ const string_t& xuid,
        _In_ club_role role,
        _In_ bool addRole
        );
    
    string_t m_id;
    club_type m_type;
    utility::datetime m_created;
    string_t m_owner;
    string_t m_titleFamilyId;
    string_t m_titleFamilyName;
    string_t m_shortName;
    club_profile m_profile;
    string_t m_founderXuid;
    string_t m_glyphImageUrl;
    string_t m_bannerImageUrl;
    uint32_t m_followersCount;
    bool m_isClubSuspended;
    utility::datetime m_suspendedUntil;
    uint32_t m_presenceCount;
    uint32_t m_presenceTodayCount;
    uint32_t m_membersCount;
    uint32_t m_moderatorsCount;
    uint32_t m_recommendedCount;
    uint32_t m_requestedToJoinCount;
    uint32_t m_reportCount;
    uint32_t m_reportedItemsCount;
    club_action_settings m_actionSettings;
    std::vector<club_role> m_viewerRoles;
    std::map<string_t, std::vector<club_role_record>> m_roleRecords;

    std::shared_ptr<clubs_service_impl> m_service;
    
    friend class clubs_serializers;
    friend class clubs_service_impl;
};

/// <summary>
/// Represents the results a call to search_clubs
/// </summary>
class clubs_search_result
{
public:
    /// <summary>List of clubs that match the search query</summary>
    _XSAPIIMP const std::vector<club>& clubs() const;

    /// <summary>
    /// Facets can be used to further narrow down search results. The return map maps a facet (ie. tag or title) to
    /// to a collection of search search facet result objects.  A search facet result object describes how often a particular 
    /// value of that facet occurred.
    /// </summary>
    _XSAPIIMP const std::unordered_map<string_t, std::vector<club_search_facet_result>>& search_facet_results() const;

private:
    std::vector<club> m_clubs;
    std::unordered_map<string_t, std::vector<club_search_facet_result>> m_searchFacetResults;

    friend class clubs_serializers;
    friend class clubs_service_impl;
};

typedef std::pair<club, std::vector<string_t>> club_recommendation_t;

/// <summary>
/// Represents the Clubs service
/// </summary>
class clubs_service
{
public:
    /// <summary>Get information about a club.</summary>
    /// <param name="clubId">The club ID of the club.</param>
    /// <returns>A club object describing the club.</returns>
    /// <remarks>
    /// Calls V4 GET clubhub.xboxlive.com/clubs/Ids({clubId})/decoration/settings
    /// </remarks>
    _XSAPIIMP pplx::task<xbox::services::xbox_live_result<club>> get_club(
        _In_ const string_t& clubId
        );

    /// <summary>Get information about multiple clubs.</summary>
    /// <param name="clubIds">Vector of club Ids to get info about.</param>
    /// <returns>A vector of club objects describing the clubs.</returns>
    /// <remarks>
    /// Calls V4 POST clubhub.xboxlive.com/clubs/Batch
    /// </remarks>
    _XSAPIIMP pplx::task<xbox::services::xbox_live_result<std::vector<club>>> get_clubs(
        _In_ const std::vector<string_t>& clubIds
        );

    /// <summary>Get information about the clubs the caller owns.</summary>
    /// <returns>A clubs_owned_result containing info about the clubs owned</returns>
    /// <remarks>
    /// Calls V1 GET clubaccounts.xboxlive.com/users/{userId}/clubsowned
    /// </remarks>
    _XSAPIIMP pplx::task<xbox::services::xbox_live_result<clubs_owned_result>> get_clubs_owned();

    /// <summary>Creates a club.</summary>
    /// <param name="name">Name of the club to be created</param>
    /// <param name="type">Type of the club to be created</param>
    /// <param name="titleFamilyId">Information about the title the club is associated with</param>
    /// <returns>A club object describing the club.</returns>
    /// <remarks>
    /// Calls V1 POST clubaccounts.xboxlive.com/clubs/create
    /// </remarks>
    _XSAPIIMP pplx::task<xbox::services::xbox_live_result<club>> create_club(
        _In_ string_t name,
        _In_ club_type type,
        _In_ string_t titleFamilyId
        );

    /// <summary>Deletes a club.</summary>
    /// <param name="clubId">Club Id of the club to delete</param>
    /// <returns>A club object describing the deleted club.</returns>
    /// <remarks>
    /// Calls V1 DELETE clubaccounts.xboxlive.com/clubs/{clubId}
    /// </remarks>
    _XSAPIIMP pplx::task<xbox::services::xbox_live_result<void>> delete_club(
        _In_ const string_t& clubId
        );

    /// <summary>Returns details about all the clubs the caller is associated with.</summary>
    /// <returns>A vector of clubs that the user is associated with in any way.</returns>
    /// <remarks>
    /// Calls V4 GET clubhub.xboxlive.com/clubs/Xuid{xuid}
    /// </remarks>
    _XSAPIIMP pplx::task<xbox::services::xbox_live_result<std::vector<club>>> get_club_associations();
    
    /// <summary>Returns details about all the clubs a particular user is associated with.</summary>
    /// <param name="xuid">Xuid of the user</param>
    /// <returns>A vector of clubs that the user is associated with in any way.</returns>
    /// <remarks>
    /// Calls V4 GET clubhub.xboxlive.com/clubs/Xuid{xuid}
    /// </remarks>
    _XSAPIIMP pplx::task<xbox::services::xbox_live_result<std::vector<club>>> get_club_associations(
        _In_ const string_t& xuid
        );

    /// <summary>Get recommended clubs for the caller.</summary>
    /// <returns>
    /// A vector of club recommendations. A club recommendation is a tuple containing the recommended club
    /// and a vector of localized "reason" strings describing why the club was recommended
    /// </returns>
    /// <remarks>
    /// Calls V4 POST clubhub.xboxlive.com/clubs/recommendations
    /// </remarks>
    _XSAPIIMP pplx::task<xbox::services::xbox_live_result<std::vector<club_recommendation_t>>> get_club_recommendations();

    /// <summary>Search for clubs with a query string.</summary>
    /// <param name="queryString">The query string used to search.</param>
    /// <param name="tags">Tags used to filter the search results.</param>
    /// <param name="titleIds">Title Ids used to filter the search results.</param>
    /// <returns>A clubs_search_result containing the results of the search.</returns>
    /// <remarks>
    /// Calls V4 GET clubhub.xboxlive.com/clubs/search
    /// </remarks>
    _XSAPIIMP pplx::task<xbox::services::xbox_live_result<clubs_search_result>> search_clubs(
        _In_ const string_t& queryString,
        _In_ const std::vector<string_t>& titleIds,
        _In_ const std::vector<string_t>& tags
        );
      
    /// <summary>Gets auto complete search suggestions for a club search query given a partial search string</summary>
    /// <param name="query">The partial query string used to create suggestions.</param>
    /// <param name="tags">Tags used to filter the results.</param>
    /// <param name="titleIds">Title Ids used to filter the results.</param>
    /// <returns>A vector containing auto complete suggestions.</returns>
    /// <remarks>
    /// Calls V1 GET clubsearch.xboxlive.com/suggest
    /// </remarks>
    _XSAPIIMP pplx::task<xbox::services::xbox_live_result<std::vector<club_search_auto_complete>>> suggest_clubs(
        _In_ const string_t& queryString,
        _In_ const std::vector<string_t>& titleIds,
        _In_ const std::vector<string_t>& tags
        );
           
private:
    clubs_service() {}

    clubs_service(
        _In_ std::shared_ptr<xbox::services::user_context> userContext,
        _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
        _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
        );
    
    std::shared_ptr<clubs_service_impl> m_clubsServiceImpl;

    friend xbox_live_context_impl;
};

}}}