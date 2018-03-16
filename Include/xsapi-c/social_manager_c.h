// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "types_c.h"
#include "xsapi-c/errors_c.h"
#include "xsapi-c/system_c.h"
#include "xsapi-c/presence_c.h"

#if defined(__cplusplus)
extern "C" {
#endif

static const uint32_t XBL_GAMERSCORE_CHAR_SIZE = 16;
static const uint32_t XBL_GAMERTAG_CHAR_SIZE = 16;
static const uint32_t XBL_XBOX_USER_ID_CHAR_SIZE = 21;
static const uint32_t XBL_DISPLAY_NAME_CHAR_SIZE = 30;
static const uint32_t XBL_REAL_NAME_CHAR_SIZE = 255;
static const uint32_t XBL_DISPLAY_PIC_URL_RAW_CHAR_SIZE = 225;
static const uint32_t XBL_COLOR_CHAR_SIZE = 7;
static const uint32_t XBL_RICH_PRESENCE_CHAR_SIZE = 100;
static const uint32_t XBL_NUM_PRESENCE_RECORDS = 6;

typedef enum XBL_SOCIAL_MANAGER_EXTRA_DETAIL_LEVEL 
{
    /// <summary>Only get default PeopleHub information (presence, profile)</summary>
    XBL_SOCIAL_MANAGER_EXTRA_DETAIL_LEVEL_NO_EXTRA_DETAIL,

    /// <summary>Add extra detail for the title history for the users</summary>
    XBL_SOCIAL_MANAGER_EXTRA_DETAIL_LEVEL_TITLE_HISTORY_LEVEL = 0x1,

    /// <summary>Add extra detail for the preferred color for the users</summary>
    XBL_SOCIAL_MANAGER_EXTRA_DETAIL_LEVEL_PREFERRED_COLOR_LEVEL = 0x2,

    /// <summary>Add all extra detail</summary>
    XBL_SOCIAL_MANAGER_EXTRA_DETAIL_LEVEL_ALL = 0x3,
} XBL_SOCIAL_MANAGER_EXTRA_DETAIL_LEVEL;

typedef enum XBL_PRESENCE_FILTER
{
    /// <summary>Unknown</summary>
    XBL_PRESENCE_FILTER_UNKNOWN,

    /// <summary>Is currently playing current title and is online</summary>
    XBL_PRESENCE_FILTER_TITLE_ONLINE,
    
    /// <summary>Has played this title and is offline</summary>
    XBL_PRESENCE_FILTER_TITLE_OFFLINE,

    /// <summary>Everyone currently online</summary>
    XBL_PRESENCE_FILTER_ALL_ONLINE,

    /// <summary>Everyone currently offline</summary>
    XBL_PRESENCE_FILTER_ALL_OFFLINE,

    /// <summary>Everyone who has played or is playing the title</summary>
    XBL_PRESENCE_FILTER_ALL_TITLE,

    /// <summary>Everyone</summary>
    XBL_PRESENCE_FILTER_ALL
} XBL_PRESENCE_FILTER;

typedef enum XBL_SOCIAL_EVENT_TYPE
{
    /// <summary>Users added to social graph</summary>
    XBL_SOCIAL_EVENT_TYPE_USERS_ADDED_TO_SOCIAL_GRAPH,

    /// <summary>Users removed from social graph</summary>
    XBL_SOCIAL_EVENT_TYPE_USERS_REMOVED_FROM_SOCIAL_GRAPH,

    /// <summary>Users presence record has changed</summary>
    XBL_SOCIAL_EVENT_TYPE_PRESENCE_CHANGED,

    /// <summary>Users profile information has changed</summary>
    XBL_SOCIAL_EVENT_TYPE_PROFILES_CHANGED,

    /// <summary>Relationship to users has changed</summary>
    XBL_SOCIAL_EVENT_TYPE_SOCIAL_RELATIONSHIPS_CHANGED,

    /// <summary>Social graph load complete from adding a local user</summary>
    XBL_SOCIAL_EVENT_TYPE_LOCAL_USER_ADDED,

    /// <summary>Social graph removal complete</summary>
    XBL_SOCIAL_EVENT_TYPE_LOCAL_USER_REMOVED,

    /// <summary>Xbox Social User Group load complete (will only trigger for views that take a list of users)</summary>
    XBL_SOCIAL_EVENT_TYPE_SOCIAL_USER_GROUP_LOADED,

    /// <summary>Social user group updated</summary>
    XBL_SOCIAL_EVENT_TYPE_SOCIAL_USER_GROUP_UPDATED,

    /// <summary>Unknown</summary>
    XBL_SOCIAL_EVENT_TYPE_UNKNOWN_EVENT
} XBL_SOCIAL_EVENT_TYPE;

typedef enum XBL_RELATIONSHIP_FILTER 
{
    /// <summary>Friends of the user (user is following)</summary>
    XBL_RELATIONSHIP_FILTER_FRIENDS,

    /// <summary>Favorites of the user</summary>
    XBL_RELATIONSHIP_FILTER_FAVORITE
} XBL_RELATIONSHIP_FILTER;

typedef enum XBL_SOCIAL_USER_GROUP_TYPE 
{
    /// <summary>Social user group based off of filters</summary>
    XBL_SOCIAL_USER_GROUP_TYPE_FILTER_TYPE,

    /// <summary>Social user group based off of list of users</summary>
    XBL_SOCIAL_USER_GROUP_TYPE_USER_LIST_TYPE
} XBL_SOCIAL_USER_GROUP_TYPE;

typedef struct XBL_TITLE_HISTORY
{
    /// <summary>
    /// Whether the user has played this title
    /// </summary>
    bool hasUserPlayed;

    /// <summary>
    /// The last time the user had played
    /// </summary>
    time_t lastTimeUserPlayed;
} XBL_TITLE_HISTORY;

typedef struct XBL_PREFERRED_COLOR
{
    /// <summary>
    /// Users primary color
    /// </summary>
    char primaryColor[XBL_COLOR_CHAR_SIZE];

    /// <summary>
    /// Users secondary color
    /// </summary>
    char secondaryColor[XBL_COLOR_CHAR_SIZE];

    /// <summary>
    /// Users tertiary color
    /// </summary>
    char tertiaryColor[XBL_COLOR_CHAR_SIZE];
} XBL_PREFERRED_COLOR;

typedef struct XBL_SOCIAL_MANAGER_PRESENCE_TITLE_RECORD
{
    /// <summary>
    /// The title ID.
    /// </summary>
    uint32_t titleId;

    /// <summary>
    /// The active state for the title.
    /// </summary>
    bool isTitleActive;

    /// <summary>
    /// The formatted and localized presence string.
    /// </summary>
    char presenceText[XBL_RICH_PRESENCE_CHAR_SIZE];

    /// <summary>
    /// The active state for the title.
    /// </summary>
    bool isBroadcasting;

    /// <summary>
    /// Device type
    /// </summary>
    XBL_PRESENCE_DEVICE_TYPE deviceType;
} XBL_SOCIAL_MANAGER_PRESENCE_TITLE_RECORD;

/// <summary>
/// Social manager presence record. Shows information on users current presence status and stores title records
/// </summary>
typedef struct XBL_SOCIAL_MANAGER_PRESENCE_RECORD
{
    /// <summary>
    /// The user's presence state.
    /// </summary>
    XBL_USER_PRESENCE_STATE userState;

    /// <summary>
    /// Collection of presence title record objects returned by a request.
    /// </summary>
    XBL_SOCIAL_MANAGER_PRESENCE_TITLE_RECORD presenceTitleRecords[XBL_NUM_PRESENCE_RECORDS];

    /// <summary>
    /// Number of valid presence records in presenceTitleRecords array
    /// </summary>
    uint32_t presenceTitleRecordCount;
} XBL_SOCIAL_MANAGER_PRESENCE_RECORD;

/// <summary>
/// Xbox Social User that contains profile, presence, preferred color, and title history data
/// </summary>
typedef struct XBL_XBOX_SOCIAL_USER
{
    /// <summary>
    /// The xbox user id
    /// </summary>
    char xboxUserId[XBL_GAMERSCORE_CHAR_SIZE];

    /// <summary>
    /// Whether they are a favorite
    /// </summary>
    bool isFavorite;

    /// <summary>
    /// Whether the calling user is following them
    /// </summary>
    bool isFollowingUser;

    /// <summary>
    /// Whether they calling user is followed by this person
    /// </summary>
    bool isFollowedByCaller;

    /// <summary>
    /// The display name
    /// </summary>
    char displayName[XBL_DISPLAY_NAME_CHAR_SIZE];

    /// <summary>
    /// The real name
    /// </summary>
    char realName[XBL_REAL_NAME_CHAR_SIZE];

    /// <summary>
    /// The display pic uri
    /// </summary>
    char displayPicUrlRaw[XBL_DISPLAY_PIC_URL_RAW_CHAR_SIZE];

    /// <summary>
    /// Whether to use the players avatar
    /// </summary>
    bool useAvatar;

    /// <summary>
    /// Players gamerscore
    /// </summary>
    char gamerscore[XBL_GAMERSCORE_CHAR_SIZE];

    /// <summary>
    /// Players gamertag
    /// </summary>
    char gamertag[XBL_GAMERTAG_CHAR_SIZE];

    /// <summary>
    /// Users presence record
    /// </summary>
    XBL_SOCIAL_MANAGER_PRESENCE_RECORD presenceRecord;

    /// <summary>
    /// Title history for the user
    /// </summary>
    XBL_TITLE_HISTORY titleHistory;

    /// <summary>
    /// Preferred color for the user
    /// </summary>
    XBL_PREFERRED_COLOR preferredColor;
} XBL_XBOX_SOCIAL_USER;

/// <summary>
/// Wrapper for a fixed length xbox user id string for purposes of storing in an array
/// </summary>
typedef struct XBL_XBOX_USER_ID_CONTAINER
{
    /// <summary>
    /// A users xbox user id
    /// </summary>
    char xboxUserId[XBL_XBOX_USER_ID_CHAR_SIZE];
} XBL_XBOX_USER_ID_CONTAINER;


/// <summary>
/// An event that something in the social graph has changed
/// </summary>
typedef struct XBL_SOCIAL_EVENT
{
    /// <summary>
    /// The user whose graph got changed
    /// </summary>
    XBL_XBOX_LIVE_USER_PTR user;

    /// <summary>
    /// The type of event this is 
    /// Tells the caller what to cast the event_args to
    /// </summary>
    XBL_SOCIAL_EVENT_TYPE eventType;

    /// <summary>
    /// Number of items in the usersAffected array
    /// </summary>
    uint32_t usersAffectedCount;

    /// <summary>
    /// Error that occurred
    /// </summary>
    int32_t err;

    /// <summary>
    /// Internal
    /// </summary>
    void* internalEvent;

} XBL_SOCIAL_EVENT;

/// <summary>
/// A subset snapshot of the users social graph
/// </summary>
typedef struct XBL_XBOX_SOCIAL_USER_GROUP
{
    /// <summary>
    /// The count of items in the users array. This value is dynamic and may change
    /// with each call to XblSocialManagerDoWork
    /// </summary>
    uint32_t usersCount;

    /// <summary>
    /// Type of social user group
    /// </summary>
    XBL_SOCIAL_USER_GROUP_TYPE socialUserGroupType;

    /// <summary>
    /// The number of users tracked by this user group currently. For groups created
    /// from a list this value is static; for groups created from a filter this value is dynamic
    /// and may change with each call to XblSocialManagerDoWork
    /// </summary>
    uint32_t trackedUsersCount;

    /// <summary>
    /// The local user who the user group is related to
    /// </summary>
    XBL_XBOX_LIVE_USER_PTR localUser;

    /// <summary>
    /// Returns the presence filter used if group type is filter type
    /// </summary>
    XBL_PRESENCE_FILTER presenceFilterOfGroup;

    /// <summary>
    /// Returns the relationship filter used if group type is filter type
    /// </summary>
    XBL_RELATIONSHIP_FILTER relationshipFilterOfGroup;
} XBL_XBOX_SOCIAL_USER_GROUP;

/// <summary>
/// Query whether the user associated with the provided presence record is playing a given title id.
/// </summary>
/// <param name="presenceRecord">A presence record returned from another social manager API.</param>
/// <param name="titleId">Title ID to query about.</param>
/// <returns>True if the user is playing the title and false otherwise.</returns>
XBL_API bool XBL_CALLING_CONV
XblSocialManagerPresenceRecordIsUserPlayingTitle(
    _In_ XBL_SOCIAL_MANAGER_PRESENCE_RECORD* presenceRecord,
    _In_ uint32_t titleId
    ) XBL_NOEXCEPT;

/// <summary>
/// Populates a users provided array of XBL_XBOX_SOCIAL_USER objects. Returned data may be stale after future calls
/// to XblSocialManagerDoWork. The provided array must be large enough to hold all of the users, as indicated
/// by the value of the "usersCount" property of the group.
/// </summary>
/// <param name="group">The group from which to get users.</param>
/// <param name="xboxSocialUsers">User provided array to populate.</param>
/// <returns>An XBL_RESULT to report any potential error</returns>
XBL_API XBL_RESULT XBL_CALLING_CONV
XblXboxSocialUserGroupGetUsers(
    _In_ XBL_XBOX_SOCIAL_USER_GROUP* group,
    _Out_ XBL_XBOX_SOCIAL_USER* xboxSocialUsers
    ) XBL_NOEXCEPT;

/// <summary>
/// Populates a user provided array of XBL_XBOX_SOCIAL_USER objects. Returned data may be stale after future calls
/// to XblSocialManagerDoWork. If the provided array is not large enough, XBL_RESULT_INVALID_ARG will be returned and
/// xboxSocialUsersCount will be set to the required number of elements.
/// </summary>
/// <param name="group">The group the users are a part of.</param>
/// <param name="xboxUserIds">The xuids to get user objects for.</param>
/// <param name="xboxUserIdsCount">The number of xuids in the provided array.</param>
/// <param name="xboxSocialUsers">User provided array to populate.</param>
/// <param name="xboxSocialUsersCount">
/// The size of the input array. On success this will be set to the actual number of elements writted to the output array.
/// If the provided array is not large enough, XBL_RESULT_INVALID_ARG will be returned and this will be set to the required number of elements.
/// </param>
/// <returns>An XBL_RESULT to report any potential error</returns>
XBL_API XBL_RESULT XBL_CALLING_CONV
XblXboxSocialUserGroupGetUsersFromXboxUserIds(
    _In_ XBL_XBOX_SOCIAL_USER_GROUP* group,
    _In_ XBL_XBOX_USER_ID_CONTAINER* xboxUserIds,
    _In_ uint32_t xboxUserIdsCount,
    _Out_ XBL_XBOX_SOCIAL_USER* xboxSocialUsers,
    _Inout_ uint32_t* xboxSocialUsersCount
    ) XBL_NOEXCEPT;

/// <summary>
/// Populates a user provided array of XBL_XBOX_USER_ID_CONTAINER objects. For groups created from a list, the set of tracked users
/// is static, but for groups created from a filter this may change with each call to XblSocialManagerDoWork. 
/// The provided array must be large enough to hold all of the users, as indicated by the value of the "trackedUsersCount" property of the group.
/// </summary>
/// <param name="group">The group from which to get users.</param>
/// <param name="trackedUsers">User provided array to populate.</param>
/// <returns>An XBL_RESULT to report any potential error</returns>
XBL_API XBL_RESULT XBL_CALLING_CONV
XblSocialUserGroupGetUsersTrackedByGroup(
    _In_ XBL_XBOX_SOCIAL_USER_GROUP* group,
    _Out_ XBL_XBOX_USER_ID_CONTAINER* trackedUsers
    ) XBL_NOEXCEPT;

/// <summary>
/// Create a social graph for the specified local user
/// The result of a local user being added will be triggered through the XBL_SOCIAL_EVENT_TYPE_LOCAL_USER_ADDED event in XblSocialManagerDoWork
/// </summary>
/// <param name="user">Xbox Live User</param>
/// <param name="extraDetailLevel">The level of verbosity that should be in the xbox_social_user</param>
/// <returns>An XBL_RESULT to report any potential error</returns>
XBL_API XBL_RESULT XBL_CALLING_CONV
XblSocialManagerAddLocalUser(
    _In_ XBL_XBOX_LIVE_USER_PTR user,
    _In_ XBL_SOCIAL_MANAGER_EXTRA_DETAIL_LEVEL extraLevelDetail
    ) XBL_NOEXCEPT;

/// <summary>
/// Removes a social graph for the specified local user
/// The result of a local user being added will be triggered through the XBL_SOCIAL_EVENT_TYPE_LOCAL_USER_REMOVED event in XblSocialManagerDoWork
/// </summary>
/// <param name="user">Xbox Live User</param>
/// <returns>An XBL_RESULT to report any potential error</returns>
XBL_API XBL_RESULT XBL_CALLING_CONV
XblSocialManagerRemoveLocalUser(
    _In_ XBL_XBOX_LIVE_USER_PTR user
    ) XBL_NOEXCEPT;

/// <summary>
/// Called whenever the title wants to update the social graph and get list of change events
/// Must be called every frame for data to be up to date. The returned array of events is only valid until the next
/// call to XblSocialManagerDoWork.
/// </summary>
/// <param name="socialEventsCount">Used to return the number of events in the returned array.</param>
/// <returns> The list of what has changed in between social graph updates</returns>
XBL_API XBL_SOCIAL_EVENT* XBL_CALLING_CONV
XblSocialManagerDoWork(
    _Out_ uint32_t* socialEventsCount
    ) XBL_NOEXCEPT;

/// <summary>
/// Get the users affected by a social event. The caller provided array must be large enough to contain all affected users,
/// which is specified in the usersAffectedCount of the event.
/// </summary>
/// <param name="socialEvent">The social event, returned from XblSocialManagerDoWork.</param>
/// <param name="usersAffected">Array of afftected users to populate.</param>
/// <returns>An XBL_RESULT to report any errors.</returns>
XBL_API XBL_RESULT XBL_CALLING_CONV
XblSocialEventGetUsersAffected(
    _In_ XBL_SOCIAL_EVENT* socialEvent,
    _Out_ XBL_XBOX_USER_ID_CONTAINER* usersAffected
    ) XBL_NOEXCEPT;

/// <summary>
/// Gets the loaded group associated with an XBL_SOCIAL_EVENT_TYPE_SOCIAL_USER_GROUP_LOADED event. If the event is
/// a different type of event XBL_RESULT_INVALID_ARG will be returned.
/// </summary>
/// <param name="socialEvent">The social event, returned from XblSocialManagerDoWork.</param>
/// <param name="loadedGroup">The loaded group.</param>
/// <returns>An XBL_RESULT to report any errors.</returns>
XBL_API XBL_RESULT XBL_CALLING_CONV
XblSocialEventGetSocialUserGroup(
    _In_ XBL_SOCIAL_EVENT* socialEvent,
    _Out_ XBL_XBOX_SOCIAL_USER_GROUP** loadedGroup
    ) XBL_NOEXCEPT;

/// <summary>
/// Constructs a social Xbox Social User Group, which is a collection of users with social information
/// The result of a user group being loaded will be triggered through the XBL_SOCIAL_EVENT_TYPE_SOCIAL_USER_GROUP_LOADED event in XblSocialManagerDoWork
/// </summary>
/// <param name="user">Xbox Live User</param>
/// <param name="presenceDetailLevel">The restriction of users based on their presence and title activity</param>
/// <param name="filter">The restriction of users based on their relationship to the calling user</param>
/// <param name="group">A pointer to the created group. This group must later be cleaned up with XblSocialManagerDestroySocialUserGroup.</param>
/// <returns>An XBL_RESULT to report any errors.</returns>
XBL_API XBL_RESULT XBL_CALLING_CONV
XblSocialManagerCreateSocialUserGroupFromFilters(
    _In_ XBL_XBOX_LIVE_USER_PTR user,
    _In_ XBL_PRESENCE_FILTER presenceDetailLevel,
    _In_ XBL_RELATIONSHIP_FILTER filter,
    _Out_ XBL_XBOX_SOCIAL_USER_GROUP** group
    ) XBL_NOEXCEPT;

/// <summary>
/// Constructs a social Xbox Social User Group, which is a collection of users with social information
/// The result of a user group being loaded will be triggered through the XBL_SOCIAL_EVENT_TYPE_SOCIAL_USER_GROUP_LOADED event in XblSocialManagerDoWork
/// </summary>
/// <param name="user">Xbox Live User</param>
/// <param name="xboxUserIdList">List of users to populate the Xbox Social User Group with. This is currently capped at 100 users total.</param>
/// <param name="xboxUserIdListCount">The number of items in the xboxUserIdList.</param>
/// <param name="group">A pointer to the created group. This group must later be cleaned up with XblSocialManagerDestroySocialUserGroup.</param>
/// <returns>An XBL_RESULT to report any errors.</returns>
XBL_API XBL_RESULT XBL_CALLING_CONV
XblSocialManagerCreateSocialUserGroupFromList(
    _In_ XBL_XBOX_LIVE_USER_PTR user,
    _In_ PCSTR* xboxUserIdList,
    _In_ uint32_t xboxUserIdListCount,
    _Out_ XBL_XBOX_SOCIAL_USER_GROUP** group
    ) XBL_NOEXCEPT;

/// <summary>
/// Destroys a created social Xbox Social User Group
/// This will stop updating the Xbox Social User Group and remove tracking for any users the Xbox Social User Group holds
/// </summary>
/// <param name="group">The social Xbox Social User Group to destroy and stop tracking</param>
/// <returns>An XBL_RESULT to report any potential error</returns>
XBL_API XBL_RESULT XBL_CALLING_CONV
XblSocialManagerDestroySocialUserGroup(
    _In_ XBL_XBOX_SOCIAL_USER_GROUP *group
    ) XBL_NOEXCEPT;

/// <summary>
/// Returns all local users who have been added to the social manager
/// </summary>
/// <param name="userCount">The number of items in the returned array</param>
/// <returns>Array of XBL_XBOX_LIVE_USER objects that are managed by social manager.</returns>
XBL_API XBL_XBOX_LIVE_USER_PTR const* XBL_CALLING_CONV
XblSocialManagerGetLocalUsers(
    _Out_ uint32_t* userCount
    ) XBL_NOEXCEPT;

/// <summary>
/// Updates specified social user group to new group of users
/// Does a diff to see which users have been added or removed from 
/// The result of a user group being updated will be triggered through the XBL_SOCIAL_EVENT_TYPE_SOCIAL_USER_GROUP_UPDATED event in XblSocialManagerDoWork
/// </summary>
/// <param name="group">The xbox social user group to add users to.</param>
/// <param name="users">List of users to add to the xbox social user group. Total number of users not in social graph is limited at 100.</param>
/// <param name="usersCount">Number of items in the users array.</param>
/// <returns>An XBL_RESULT representing the success of adding the users to the group</returns>
XBL_API XBL_RESULT XBL_CALLING_CONV
XblSocialManagerUpdateSocialUserGroup(
    _In_ XBL_XBOX_SOCIAL_USER_GROUP *group,
    _In_ PCSTR* users,
    _In_ uint32_t usersCount
    ) XBL_NOEXCEPT;

/// <summary>
/// Whether to enable social manager to poll every 30 seconds from the presence service 
/// </summary>
/// <param name="user">Xbox Live User</param>
/// <param name="shouldEnablePolling">Whether or not polling should enabled</param>
/// <returns>An XBL_RESULT representing the success enabling polling</returns>
XBL_API XBL_RESULT XBL_CALLING_CONV
XblSocialManagerSetRichPresencePollingStatus(
    _In_ XBL_XBOX_LIVE_USER_PTR user,
    _In_ bool shouldEnablePolling
    ) XBL_NOEXCEPT;

/// TODO should this be per user of for all social graphs?
/// <summary>
/// Sets a async queue to be used by all background social graph updates. Social Manager will add work
/// to the provided queue and it must be dispatched manually.
/// </summary>
/// <param name="queue">XBL_ASYNC_QUEUE handle to be used for Social Manager background work.</param>
XBL_API void XBL_CALLING_CONV
XblSocialManagerSetBackgroundWorkAsyncQueue(
    _In_ XBL_ASYNC_QUEUE queue
    ) XBL_NOEXCEPT;


#if defined(__cplusplus)
} // end extern "C"
#endif // defined(__cplusplus)