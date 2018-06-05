// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include <xsapi-c/presence_c.h>

/// <summary>
/// Detail level controls how much information is exposed in each xbox_live_social_graph_user
/// Detail level can only be set on construction of social_manager
/// </summary>
typedef enum XblSocialManagerExtraDetailLevel 
{
    /// <summary>Only get default PeopleHub information (presence, profile)</summary>
    XblSocialManagerExtraDetailLevel_NoExtraDetail,

    /// <summary>Add extra detail for the title history for the users</summary>
    XblSocialManagerExtraDetailLevel_TitleHistoryLevel = 0x1,

    /// <summary>Add extra detail for the preferred color for the users</summary>
    XblSocialManagerExtraDetailLevel_PreferredColorLevel = 0x2,

    /// <summary>Add all extra detail</summary>
    XblSocialManagerExtraDetailLevel_All = 0x3,
} XblSocialManagerExtraDetailLevel;

/// <summary>
/// The filter level of information
/// Title will only show users associated with a particular title
/// </summary>
typedef enum XblPresenceFilter
{
    /// <summary>Unknown</summary>
    XblPresenceFilter_Unknown,

    /// <summary>Is currently playing current title and is online</summary>
    XblPresenceFilter_TitleOnline,
    
    /// <summary>Has played this title and is offline</summary>
    XblPresenceFilter_TitleOffline,

    /// <summary>Everyone currently online</summary>
    XblPresenceFilter_AllOnline,

    /// <summary>Everyone currently offline</summary>
    XblPresenceFilter_AllOffline,

    /// <summary>Everyone who has played or is playing the title</summary>
    XblPresenceFilter_AllTitle,

    /// <summary>Everyone</summary>
    XblPresenceFilter_All
} XblPresenceFilter;

/// <summary>
/// The types of possible social manager events
/// </summary>
typedef enum XblSocialManagerEventType
{
    /// <summary>Users added to social graph</summary>
    XblSocialManagerEventType_UsersAddedToSocialGraph,

    /// <summary>Users removed from social graph</summary>
    XblSocialManagerEventType_UsersRemovedFromSocialGraph,

    /// <summary>Users presence record has changed</summary>
    XblSocialManagerEventType_PresenceChanged,

    /// <summary>Users profile information has changed</summary>
    XblSocialManagerEventType_ProfilesChanged,

    /// <summary>Relationship to users has changed</summary>
    XblSocialManagerEventType_SocialRelationshipsChanged,

    /// <summary>Social graph load complete from adding a local user</summary>
    XblSocialManagerEventType_LocalUserAdded,

    /// <summary>Social graph removal complete</summary>
    XblSocialManagerEventType_LocalUserRemoved,

    /// <summary>Xbox Social User Group load complete (will only trigger for views that take a list of users)</summary>
    XblSocialManagerEventType_SocialUserGroupLoaded,

    /// <summary>Social user group updated</summary>
    XblSocialManagerEventType_SocialUserGroupUpdated,

    /// <summary>Unknown</summary>
    XblSocialManagerEventType_UnknownEvent
} XblSocialManagerEventType;

/// <summary>
/// Possible relationship types to filter by
/// </summary>
typedef enum XblRelationshipFilter 
{
    /// <summary>Friends of the user (user is following)</summary>
    XblRelationshipFilter_Friends,

    /// <summary>Favorites of the user</summary>
    XblRelationshipFilter_Favorite
} XblRelationshipFilter;

/// <summary>
/// Identifies type of social user group created
/// </summary>
typedef enum XblSocialUserGroupType 
{
    /// <summary>Social user group based off of filters</summary>
    XblSocialUserGroupType_FilterType,

    /// <summary>Social user group based off of list of users</summary>
    XblSocialUserGroupType_UserListType
} XblSocialUserGroupType;

/// <summary>
/// Data about whether the user has played the title
/// </summary>
typedef struct XblTitleHistory
{
    /// <summary>
    /// Whether the user has played this title
    /// </summary>
    bool hasUserPlayed;

    /// <summary>
    /// The last time the user had played
    /// </summary>
    time_t lastTimeUserPlayed;
} XblTitleHistory;

/// <summary>
/// Preferred color for the user. Set via the shell. 
/// </summary>
typedef struct XblPreferredColor
{
    /// <summary>
    /// UTF-8 encoded user's primary color
    /// </summary>
    char primaryColor[XBL_COLOR_CHAR_SIZE];

    /// <summary>
    /// UTF-8 encoded user's secondary color
    /// </summary>
    char secondaryColor[XBL_COLOR_CHAR_SIZE];

    /// <summary>
    /// UTF-8 encoded user's tertiary color
    /// </summary>
    char tertiaryColor[XBL_COLOR_CHAR_SIZE];
} XblPreferredColor;

/// <summary>
/// Social manager version of the presence title record
/// Gives information about different titles presence information
/// </summary>
typedef struct XblSocialManagerPresenceTitleRecord
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
    /// The UTF-8 encoded formatted and localized presence string.
    /// </summary>
    char presenceText[XBL_RICH_PRESENCE_CHAR_SIZE];

    /// <summary>
    /// The active state for the title.
    /// </summary>
    bool isBroadcasting;

    /// <summary>
    /// Device type
    /// </summary>
    XblPresenceDeviceType deviceType;
} XblSocialManagerPresenceTitleRecord;

/// <summary>
/// Social manager presence record. Shows information on users current presence status and stores title records
/// </summary>
typedef struct XblSocialManagerPresenceRecord
{
    /// <summary>
    /// The user's presence state.
    /// </summary>
    XblPresenceUserState userState;

    /// <summary>
    /// Collection of presence title record objects returned by a request.
    /// </summary>
    XblSocialManagerPresenceTitleRecord presenceTitleRecords[XBL_NUM_PRESENCE_RECORDS];

    /// <summary>
    /// Number of valid presence records in presenceTitleRecords array
    /// </summary>
    uint32_t presenceTitleRecordCount;
} XblSocialManagerPresenceRecord;

/// <summary>
/// Xbox Social User that contains profile, presence, preferred color, and title history data
/// </summary>
typedef struct XblSocialManagerUser
{
    /// <summary>
    /// The xbox user id
    /// </summary>
    uint64_t xboxUserId;

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
    /// The UTF-8 encoded display name
    /// </summary>
    char displayName[XBL_DISPLAY_NAME_CHAR_SIZE];

    /// <summary>
    /// The UTF-8 encoded real name
    /// </summary>
    char realName[XBL_REAL_NAME_CHAR_SIZE];

    /// <summary>
    /// The UTF-8 encoded display pic uri
    /// </summary>
    char displayPicUrlRaw[XBL_DISPLAY_PIC_URL_RAW_CHAR_SIZE];

    /// <summary>
    /// Whether to use the players avatar
    /// </summary>
    bool useAvatar;

    /// <summary>
    /// UTF-8 encoded player's gamerscore
    /// </summary>
    char gamerscore[XBL_GAMERSCORE_CHAR_SIZE];

    /// <summary>
    /// UTF-8 encoded player's gamertag
    /// </summary>
    char gamertag[XBL_GAMERTAG_CHAR_SIZE];

    /// <summary>
    /// Users presence record
    /// </summary>
    XblSocialManagerPresenceRecord presenceRecord;

    /// <summary>
    /// Title history for the user
    /// </summary>
    XblTitleHistory titleHistory;

    /// <summary>
    /// Preferred color for the user
    /// </summary>
    XblPreferredColor preferredColor;
} XblSocialManagerUser;

/// <summary>
/// A subset snapshot of the users social graph
/// </summary>
typedef struct XblSocialManagerUserGroup
{
    /// <summary>
    /// The count of items in the users array. This value is dynamic and may change
    /// with each call to XblSocialManagerDoWork
    /// </summary>
    uint32_t usersCount;

    /// <summary>
    /// Type of social user group
    /// </summary>
    XblSocialUserGroupType socialUserGroupType;

    /// <summary>
    /// The number of users tracked by this user group currently. For groups created
    /// from a list this value is static; for groups created from a filter this value is dynamic
    /// and may change with each call to XblSocialManagerDoWork
    /// </summary>
    uint32_t trackedUsersCount;

    /// <summary>
    /// The local user who the user group is related to
    /// </summary>
    xbl_user_handle localUser;

    /// <summary>
    /// Returns the presence filter used if group type is filter type
    /// </summary>
    XblPresenceFilter presenceFilterOfGroup;

    /// <summary>
    /// Returns the relationship filter used if group type is filter type
    /// </summary>
    XblRelationshipFilter relationshipFilterOfGroup;
} XblSocialManagerUserGroup;

/// <summary>
/// An event that something in the social graph has changed
/// </summary>
typedef struct XblSocialManagerEvent
{
    /// <summary>
    /// The user whose graph got changed
    /// </summary>
    xbl_user_handle user;

    /// <summary>
    /// The type of event this is 
    /// Tells the caller what to cast the event_args to
    /// </summary>
    XblSocialManagerEventType eventType;

    /// <summary>
    /// Number of items in the usersAffected array
    /// </summary>
    uint32_t usersAffectedCount;

    /// <summary>
    /// The user group that was loaded for XblSocialManagerEventType_SocialUserGroupLoaded events.
    /// This field is unused for other events types.
    /// </summary>
    XblSocialManagerUserGroup* loadedGroup;

    /// <summary>
    /// Error that occurred
    /// </summary>
    int32_t err;

    /// <summary>
    /// Internal use only
    /// </summary>
    void* internalPtr;

} XblSocialManagerEvent;

/// <summary>
/// Query whether the user associated with the provided presence record is playing a given title id.
/// </summary>
/// <param name="presenceRecord">A presence record returned from another social manager API.</param>
/// <param name="titleId">Title ID to query about.</param>
/// <returns>True if the user is playing the title and false otherwise.</returns>
STDAPI_(bool) XblSocialManagerPresenceRecordIsUserPlayingTitle(
    _In_ XblSocialManagerPresenceRecord* presenceRecord,
    _In_ uint32_t titleId
    ) XBL_NOEXCEPT;

/// <summary>
/// Populates the provided array with XblSocialManagerUser objects.
/// The returned data is a deep copy and still accessible after XblSocialManagerDoWork,
/// however the data may become stale.
/// The provided array must be large enough to hold all of the users, as indicated
/// by the value of the "usersCount" property of the group.
/// </summary>
/// <param name="group">The group from which to get users.</param>
/// <param name="xboxSocialUsersCount">The size of the xboxSocialUsers array passed in.  Should be at least as large as group->usersCount.</param>
/// <param name="xboxSocialUsers">User provided array to populate.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblSocialManagerUserGroupGetUsers(
    _In_ XblSocialManagerUserGroup* group,
    _In_ uint32_t xboxSocialUsersCount,
    _Out_writes_(xboxSocialUsersCount) XblSocialManagerUser* xboxSocialUsers
    ) XBL_NOEXCEPT;

/// <summary>
/// Populates a user provided array of XblSocialManagerUser objects. 
/// The returned data is a deep copy and still accessible after XblSocialManagerDoWork,
/// however the data may become stale.
/// If the provided array is not large enough, XBL_RESULT_INVALID_ARG will be returned and
/// xboxSocialUsersCount will be set to the required number of elements.
/// </summary>
/// <param name="group">The group the users are a part of.</param>
/// <param name="xboxUserIds">The xuids to get user objects for.</param>
/// <param name="xboxUserIdsCount">The number of xuids in the provided array.</param>
/// <param name="xboxSocialUsers">User provided array to populate.</param>
/// <param name="xboxSocialUsersCount">
/// The actual number of users written to the xboxSocialUsers array. If all of the xuids provided are members of the group,
/// this will be equal to the 'xboxUserIdsCount'.
/// </param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblSocialManagerUserGroupGetUsersFromXboxUserIds(
    _In_ XblSocialManagerUserGroup* group,
    _In_ const uint64_t* xboxUserIds,
    _In_ uint32_t xboxUserIdsCount,
    _Out_writes_to_(xboxUserIdsCount, *xboxSocialUsersCount) XblSocialManagerUser* xboxSocialUsers,
    _Out_opt_ uint32_t* xboxSocialUsersCount
    ) XBL_NOEXCEPT;

/// <summary>
/// Populates a user provided array of uint64s. For groups created from a list, the set of tracked users
/// is static, but for groups created from a filter this may change with each call to XblSocialManagerDoWork. 
/// The returned data is a deep copy and still accessible after XblSocialManagerDoWork,
/// however the data may become stale.
/// The provided array must be large enough to hold all of the users, as indicated by the value of the "trackedUsersCount" property of the group.
/// </summary>
/// <param name="group">The group from which to get users.</param>
/// <param name="trackedUsersCount">The size of the trackedUsers array passed in.  Should be at least as large as group->trackedUsersCount.</param>
/// <param name="trackedUsers">User provided array to populate.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblSocialManagerUserGroupGetUsersTrackedByGroup(
    _In_ XblSocialManagerUserGroup* group,
    _In_ uint32_t trackedUsersCount,
    _Out_writes_(trackedUsersCount) uint64_t* trackedUsers
    ) XBL_NOEXCEPT;

/// <summary>
/// Create a social graph for the specified local user.
/// The result of a local user being added will be triggered through the XblSocialManagerEventType_LocalUserAdded event in XblSocialManagerDoWork
/// </summary>
/// <param name="user">Xbox Live User</param>
/// <param name="extraLevelDetail">The level of verbosity that should be in the xbox_social_user</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblSocialManagerAddLocalUser(
    _In_ xbl_user_handle user,
    _In_ XblSocialManagerExtraDetailLevel extraLevelDetail
    ) XBL_NOEXCEPT;

/// <summary>
/// Removes a social graph for the specified local user
/// The result of a local user being added will be triggered through the 
/// XblSocialManagerEventType_LocalUserRemoved event in XblSocialManagerDoWork
/// </summary>
/// <param name="user">Xbox Live User</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblSocialManagerRemoveLocalUser(
    _In_ xbl_user_handle user
    ) XBL_NOEXCEPT;

/// <summary>
/// Called whenever the title wants to update the social graph and get list of change events
/// Must be called every frame for data to be up to date. 
/// The returned array of events is only valid until the next
/// call to XblSocialManagerDoWork.
/// </summary>
/// <param name="socialEvents">
/// Pointer to the array of social events that have occurred since the last call to do_work. This array is
/// only valid until the next call to XblSocialManagerDoWork. The internal array will automatically be cleaned
/// up when XblCleanup is called.
/// </param>
/// <param name="socialEventsCount">The number of events in the returned array.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblSocialManagerDoWork(
    _Outptr_ XblSocialManagerEvent** socialEvents,
    _Out_ uint32_t* socialEventsCount
    ) XBL_NOEXCEPT;

/// <summary>
/// Get the users affected by a social event. The caller provided array must be large enough to contain all affected users,
/// which is specified in the usersAffectedCount of the event.
/// </summary>
/// <param name="socialEvent">The social event, returned from XblSocialManagerDoWork.</param>
/// <param name="usersAffected">Array of afftected users to populate.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblSocialManagerEventGetUsersAffected(
    _In_ XblSocialManagerEvent* socialEvent,
    _Out_writes_(socialEvent->usersAffectedCount) uint64_t* usersAffected
    ) XBL_NOEXCEPT;

/// <summary>
/// Constructs a XblSocialManagerUserGroup, which is a collection of users with social information
/// The result of a user group being loaded will be triggered through the 
/// XblSocialManagerEventType_SocialUserGroupLoaded event in XblSocialManagerDoWork.
/// </summary>
/// <param name="user">Xbox Live User</param>
/// <param name="presenceDetailLevel">The restriction of users based on their presence and title activity</param>
/// <param name="filter">The restriction of users based on their relationship to the calling user</param>
/// <param name="group">A pointer to the created group. This group must later be cleaned up with XblSocialManagerDestroySocialUserGroup.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblSocialManagerCreateSocialUserGroupFromFilters(
    _In_ xbl_user_handle user,
    _In_ XblPresenceFilter presenceDetailLevel,
    _In_ XblRelationshipFilter filter,
    _Outptr_result_maybenull_ XblSocialManagerUserGroup** group
    ) XBL_NOEXCEPT;

/// <summary>
/// Constructs a social XblSocialManagerUserGroup, which is a collection of users with social information
/// The result of a user group being loaded will be triggered through the 
/// XblSocialManagerEventType_SocialUserGroupLoaded event in XblSocialManagerDoWork.
/// </summary>
/// <param name="user">Xbox Live User</param>
/// <param name="xboxUserIdList">List of users to populate the Xbox Social User Group with. This is currently capped at 100 users total.</param>
/// <param name="xboxUserIdListCount">The number of items in the xboxUserIdList.</param>
/// <param name="group">A pointer to the created group. This group must later be cleaned up with XblSocialManagerDestroySocialUserGroup.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblSocialManagerCreateSocialUserGroupFromList(
    _In_ xbl_user_handle user,
    _In_ uint64_t* xboxUserIdList,
    _In_ uint32_t xboxUserIdListCount,
    _Outptr_result_maybenull_ XblSocialManagerUserGroup** group
    ) XBL_NOEXCEPT;

/// <summary>
/// Destroys a created social Xbox Social User Group
/// This will stop updating the Xbox Social User Group and remove tracking for any users 
/// the XblSocialManagerUserGroup holds.
/// </summary>
/// <param name="group">The social Xbox Social User Group to destroy and stop tracking</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblSocialManagerDestroySocialUserGroup(
    _In_ XblSocialManagerUserGroup* group
    ) XBL_NOEXCEPT;

/// <summary>
/// Returns all local users who have been added to the social manager.
/// </summary>
/// <param name="users">Array of handles for users that have been added to social manager.</param>
/// <param name="userCount">The number of items in the returned array</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblSocialManagerGetLocalUsers(
    _Outptr_ xbl_user_handle** users,
    _Out_ uint32_t* userCount
    ) XBL_NOEXCEPT;

/// <summary>
/// Updates specified social user group to new group of users
/// Does a diff to see which users have been added or removed from
/// The result of a user group being updated will be triggered through the XblSocialManagerEventType_SocialUserGroupUpdated event in XblSocialManagerDoWork.
/// </summary>
/// <param name="group">The xbox social user group to add users to.</param>
/// <param name="users">List of users to add to the xbox social user group. Total number of users not in social graph is limited at 100.</param>
/// <param name="usersCount">Number of items in the users array.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblSocialManagerUpdateSocialUserGroup(
    _In_ XblSocialManagerUserGroup* group,
    _In_ uint64_t* users,
    _In_ uint32_t usersCount
    ) XBL_NOEXCEPT;

/// <summary>
/// Whether to enable social manager to poll every 30 seconds from the presence service.
/// </summary>
/// <param name="user">Xbox Live User</param>
/// <param name="shouldEnablePolling">Whether or not polling should enabled</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblSocialManagerSetRichPresencePollingStatus(
    _In_ xbl_user_handle user,
    _In_ bool shouldEnablePolling
    ) XBL_NOEXCEPT;

/// <summary>
/// Sets a async queue to be used by all background social graph updates.
/// </summary>
/// <param name="queue">async_queue_handle_t to be used for Social Manager background work.</param>
STDAPI_(void) XblSocialManagerSetBackgroundWorkAsyncQueue(
    _In_opt_ async_queue_handle_t queue
    ) XBL_NOEXCEPT;
