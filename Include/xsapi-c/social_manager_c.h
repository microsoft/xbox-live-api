// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#if !defined(__cplusplus)
   #error C++11 required
#endif

#pragma once
#include <xsapi-c/presence_c.h>

extern "C"
{

#define XBL_SOCIAL_MANAGER_MAX_USERS_FROM_LIST 100

/// <summary>
/// Controls how much information is exposed in each xbox_live_social_graph_user.  
/// Detail level can only be set on construction of social_manager.
/// </summary>
/// <argof><see cref="XblSocialManagerAddLocalUser"/></argof>
enum class XblSocialManagerExtraDetailLevel : uint32_t
{
    /// <summary>
    /// Only get default PeopleHub information (presence, profile).
    /// </summary>
    NoExtraDetail,

    /// <summary>
    /// Add extra detail for the title history for the users.
    /// </summary>
    TitleHistoryLevel = 0x1,

    /// <summary>
    /// Add extra detail for the preferred color for the users.
    /// </summary>
    PreferredColorLevel = 0x2,

    /// <summary>
    /// Add all extra detail.
    /// </summary>
    All = 0x3,
};

DEFINE_ENUM_FLAG_OPERATORS(XblSocialManagerExtraDetailLevel);

/// <summary>
/// The filter level of information.  
/// Title will only show users associated with a particular title.
/// </summary>
/// <argof><see cref="XblSocialManagerUserGroupGetFilters"/></argof>
/// <argof><see cref="XblSocialManagerCreateSocialUserGroupFromFilters"/></argof>
enum class XblPresenceFilter : uint32_t
{
    /// <summary>
    /// Unknown.
    /// </summary>
    Unknown,

    /// <summary>
    /// Is currently playing current title and is online.
    /// </summary>
    TitleOnline,

    /// <summary>
    /// Has played this title and is offline.
    /// </summary>
    /// <remarks>
    /// This filter option requires <see cref="XblSocialManagerExtraDetailLevel"/>::TitleHistoryLevel to be set in <see cref="XblSocialManagerAddLocalUser"/>
    /// </remarks>
    TitleOffline,

    /// <summary>
    /// Has played this title, is online but not currently playing this title.
    /// </summary>
    /// <remarks>
    /// This filter option requires <see cref="XblSocialManagerExtraDetailLevel"/>::TitleHistoryLevel to be set in <see cref="XblSocialManagerAddLocalUser"/>
    /// </remarks>
    TitleOnlineOutsideTitle,

    /// <summary>
    /// Everyone currently online.
    /// </summary>
    AllOnline,

    /// <summary>
    /// Everyone currently offline.
    /// </summary>
    AllOffline,

    /// <summary>
    /// Everyone who has played or is playing the title.
    /// </summary>
    /// <remarks>
    /// This filter option requires <see cref="XblSocialManagerExtraDetailLevel"/>::TitleHistoryLevel to be set in <see cref="XblSocialManagerAddLocalUser"/>
    /// </remarks>
    AllTitle,

    /// <summary>
    /// Everyone.
    /// </summary>
    All
};

/// <summary>
/// The types of possible social manager events.
/// </summary>
/// <memof><see cref="XblSocialManagerEvent"/></memof>
enum class XblSocialManagerEventType : uint32_t
{
    /// <summary>
    /// Fired when one or more users are added to social graph. Users are added to the graph if they are tracked
    /// by a list group or if they are followed by the local user.
    /// </summary>
    UsersAddedToSocialGraph,

    /// <summary>
    /// Fired when one or more users are removed from social graph. User's will be removed from the social graph if
    /// they aren't followed by the local user, nor are they tracked by any list groups.
    /// </summary>
    UsersRemovedFromSocialGraph,

    /// <summary>
    /// Users presence record has changed. This event implies that the set of users tracked by filter groups
    /// may have changed (i.e. if the group was created with an XblPresenceFilter).
    /// </summary>
    PresenceChanged,

    /// <summary>
    /// Users profile information has changed.
    /// </summary>
    ProfilesChanged,

    /// <summary>
    /// Relationship to users has changed. This event implies that the set of users tracked by filter groups
    /// may have changed (i.e. if the group was created with an XblRelationshipFilter).
    /// </summary>
    SocialRelationshipsChanged,

    /// <summary>
    /// Fired when the initial social graph has been loaded for a local user.
    /// </summary>
    LocalUserAdded,

    /// <summary>
    /// Fired when all users tracked by a social group are in social graph.
    /// </summary>
    SocialUserGroupLoaded,

    /// <summary>
    /// After updating a list based user group with <see cref="XblSocialManagerUpdateSocialUserGroup"/>, this event
    /// is fired to indicate all of the new users are in the social graph. If they are not already part of the graph,
    /// the new users will be added. Note that this event doesn't apply for filter based user groups.
    /// </summary>
    SocialUserGroupUpdated,

    /// <summary>
    /// Unknown.
    /// </summary>
    UnknownEvent
};

/// <summary>
/// Possible relationship types to filter by.
/// </summary>
/// <argof><see cref="XblSocialManagerUserGroupGetFilters"/></argof>
/// <argof><see cref="XblSocialManagerCreateSocialUserGroupFromFilters"/></argof>
enum class XblRelationshipFilter : uint32_t
{
    /// <summary>
    /// Unknown.
    /// </summary>
    Unknown,

    /// <summary>
    /// Friends of the user (user is following).
    /// </summary>
    Friends,

    /// <summary>
    /// Favorites of the user.
    /// </summary>
    Favorite
};

/// <summary>
/// Identifies type of social user group created.
/// </summary>
/// <argof><see cref="XblSocialManagerUserGroupGetType"/></argof>
enum class XblSocialUserGroupType : uint32_t
{
    /// <summary>
    /// Social user group based off of filters.
    /// </summary>
    FilterType,

    /// <summary>
    /// Social user group based off of list of users.
    /// </summary>
    UserListType
};

/// <summary>
/// Data about whether the user has played the title.
/// </summary>
/// <memof><see cref="XblSocialManagerUser"/></memof>
typedef struct XblTitleHistory
{
    /// <summary>
    /// Whether the user has played this title.
    /// </summary>
    bool hasUserPlayed;

    /// <summary>
    /// The last time the user had played.
    /// </summary>
    /// <remarks>
    /// Do not use both this and lastTimeUserPlayedText.
    /// For playtime within the past 14 days, this will be accurate for the date and fuzzily accurate for the time. 
    /// For playtime older than 14 days, this will only be accurate to the year and month, up to a year ago. The date will be returned as the 1st of the month, 
    /// but the play time could have occurred anywhere within that month.
    /// </remarks>
    time_t lastTimeUserPlayed;

    /// <summary>
    /// The last time the user had played in a standardized plaintext format (e.g. "a few minutes ago" or "x hours/days/months ago" or "this/last month").
    /// </summary>
    /// <remarks>
    /// Do not use both this and lastTimeUserPlayed.
    /// </remarks>
    char lastTimeUserPlayedText[XBL_LAST_TIME_PLAYED_CHAR_SIZE];
} XblTitleHistory;

/// <summary>
/// Preferred color for the user. Set via the shell.
/// </summary>
/// <memof><see cref="XblSocialManagerUser"/></memof>
typedef struct XblPreferredColor
{
    /// <summary>
    /// UTF-8 encoded user's primary color.
    /// </summary>
    char primaryColor[XBL_COLOR_CHAR_SIZE];

    /// <summary>
    /// UTF-8 encoded user's secondary color.
    /// </summary>
    char secondaryColor[XBL_COLOR_CHAR_SIZE];

    /// <summary>
    /// UTF-8 encoded user's tertiary color.
    /// </summary>
    char tertiaryColor[XBL_COLOR_CHAR_SIZE];
} XblPreferredColor;

/// <summary>
/// Social manager version of the presence title record.  
/// Gives information about different titles presence information.
/// </summary>
/// <memof><see cref="XblSocialManagerPresenceRecord"/></memof>
typedef struct XblSocialManagerPresenceTitleRecord
{
    /// <summary>
    /// The title ID.
    /// </summary>
    uint32_t titleId;

    /// <summary>
    /// The title name.
    /// </summary>
    char titleName[XBL_TITLE_NAME_CHAR_SIZE];

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
    /// Device type.
    /// </summary>
    XblPresenceDeviceType deviceType;

    /// <summary>
    /// Whether or not this is the primary presence record.
    /// </summary>
    bool isPrimary;
} XblSocialManagerPresenceTitleRecord;

#define XBL_NUM_PRESENCE_RECORDS 6

/// <summary>
/// Social manager presence record. Shows information on users current presence status and stores title records.
/// </summary>
/// <argof><see cref="XblSocialManagerPresenceRecordIsUserPlayingTitle"/></argof>
/// <memof><see cref="XblSocialManagerUser"/></memof>
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
    /// Number of valid presence records in presenceTitleRecords array.
    /// </summary>
    uint32_t presenceTitleRecordCount;
} XblSocialManagerPresenceRecord;

/// <summary>
/// Xbox Social User that contains profile, presence, preferred color, and title history data.
/// </summary>
/// <memof><see cref="XblSocialManagerEvent"/></memof>
typedef struct XblSocialManagerUser
{
    /// <summary>
    /// The xbox user id.
    /// </summary>
    uint64_t xboxUserId;

    /// <summary>
    /// Whether they are a favorite.
    /// </summary>
    bool isFavorite;

    /// <summary>
    /// Whether the calling user is following them.
    /// </summary>
    bool isFollowingUser;

    /// <summary>
    /// Whether they calling user is followed by this person.
    /// </summary>
    bool isFollowedByCaller;

    /// <summary>
    /// The UTF-8 encoded display name.
    /// </summary>
    char displayName[XBL_DISPLAY_NAME_CHAR_SIZE];

    /// <summary>
    /// The UTF-8 encoded real name.
    /// </summary>
    char realName[XBL_REAL_NAME_CHAR_SIZE];

    /// <summary>
    /// The UTF-8 encoded display picture uri.
    /// </summary>
    char displayPicUrlRaw[XBL_DISPLAY_PIC_URL_RAW_CHAR_SIZE];

    /// <summary>
    /// Whether to use the players avatar.
    /// </summary>
    bool useAvatar;

    /// <summary>
    /// UTF-8 encoded player's gamerscore.
    /// </summary>
    char gamerscore[XBL_GAMERSCORE_CHAR_SIZE];

    /// <summary>
    /// UTF-8 encoded player's gamertag.
    /// </summary>
    char gamertag[XBL_GAMERTAG_CHAR_SIZE];

    /// <summary>
    /// Modern gamertag for the player. Not guaranteed to be unique.
    /// </summary>
    char modernGamertag[XBL_MODERN_GAMERTAG_CHAR_SIZE];

    /// <summary>
    /// Suffix appended to modern gamertag to ensure uniqueness.  May be empty in some cases.
    /// </summary>
    char modernGamertagSuffix[XBL_MODERN_GAMERTAG_SUFFIX_CHAR_SIZE];

    /// <summary>
    /// Combined modern gamertag and suffix. Format will be "MGT#suffix".  
    /// Guaranteed to be no more than 16 rendered characters.
    /// </summary>
    char uniqueModernGamertag[XBL_UNIQUE_MODERN_GAMERTAG_CHAR_SIZE];

    /// <summary>
    /// Users presence record.
    /// </summary>
    XblSocialManagerPresenceRecord presenceRecord;

    /// <summary>
    /// Title history for the user.
    /// </summary>
    XblTitleHistory titleHistory;

    /// <summary>
    /// Preferred color for the user.
    /// </summary>
    XblPreferredColor preferredColor;
} XblSocialManagerUser;

/// <summary>
/// A handle to a social manager user group.
/// </summary>
typedef struct XblSocialManagerUserGroup* XblSocialManagerUserGroupHandle;

#define XBL_SOCIAL_MANAGER_MAX_AFFECTED_USERS_PER_EVENT 10

/// <summary>
/// An event that something in the social graph has changed.
/// </summary>
/// <argof><see cref="XblSocialManagerDoWork"/></argof>
typedef struct XblSocialManagerEvent
{
    /// <summary>
    /// The user whose graph got changed.
    /// </summary>
    XblUserHandle user;

    /// <summary>
    /// The type of event this is.
    /// </summary>
    XblSocialManagerEventType eventType;

    /// <summary>
    /// Error that occurred, or S_OK.
    /// </summary>
    HRESULT hr;

    /// <summary>
    /// The user group that was loaded for XblSocialManagerEventType::SocialUserGroupLoaded or updated.  
    /// For XblSocialManagerEventType::SocialUserGroupUpdated. Will be null for other types of events.
    /// </summary>
    XblSocialManagerUserGroupHandle groupAffected;

    /// <summary>
    /// The users affected. Returned pointers valid until the next XblSocialManagerDoWork call.
    /// </summary>
    XblSocialManagerUser* usersAffected[XBL_SOCIAL_MANAGER_MAX_AFFECTED_USERS_PER_EVENT];

} XblSocialManagerEvent;

/// <summary>
/// Query whether the user associated with the provided presence record is playing a given title id.
/// </summary>
/// <param name="presenceRecord">A presence record returned from another social manager API.</param>
/// <param name="titleId">Title ID to query about.</param>
/// <returns>True if the user is playing the title and false otherwise.</returns>
STDAPI_(bool) XblSocialManagerPresenceRecordIsUserPlayingTitle(
    _In_ const XblSocialManagerPresenceRecord* presenceRecord,
    _In_ uint32_t titleId
) XBL_NOEXCEPT;

/// <summary>
/// Gets the type of a Social Manager user group.
/// </summary>
/// <param name="group">The group handle.</param>
/// <param name="type">Passes back the group type.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Call this API after calling either <see cref="XblSocialManagerCreateSocialUserGroupFromFilters"/>
/// or <see cref="XblSocialManagerCreateSocialUserGroupFromList"/> to create an XblSocialManagerUserGroup.  
/// The XblSocialManagerUserGroupHandle is returned by the <see cref="XblSocialManagerEventType"/>::SocialUserGroupLoaded
/// event in <see cref="XblSocialManagerDoWork"/>.
/// </remarks>
STDAPI XblSocialManagerUserGroupGetType(
    _In_ XblSocialManagerUserGroupHandle group,
    _Out_ XblSocialUserGroupType* type
) XBL_NOEXCEPT;

/// <summary>
/// Gets local user the group is associated with.
/// </summary>
/// <param name="group">The group handle.</param>
/// <param name="localUser">Passes back the local user handle.  
/// This user handle does not need to be closed.  
/// It remains valid for as long as the user is added to the social manager.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Call this API after calling either <see cref="XblSocialManagerCreateSocialUserGroupFromFilters"/>
/// or <see cref="XblSocialManagerCreateSocialUserGroupFromList"/> to create an XblSocialManagerUserGroup.  
/// The XblSocialManagerUserGroupHandle is returned by the <see cref="XblSocialManagerEventType"/>::SocialUserGroupLoaded event 
/// in <see cref="XblSocialManagerDoWork"/>.
/// </remarks>
STDAPI XblSocialManagerUserGroupGetLocalUser(
    _In_ XblSocialManagerUserGroupHandle group,
    _Out_ XblUserHandle* localUser
) XBL_NOEXCEPT;

/// <summary>
/// Gets the filters for a filter group.
/// </summary>
/// <param name="group">The group handle.</param>
/// <param name="presenceFilter">Passes back the presence filter.</param>
/// <param name="relationshipFilter">Passes back the relationship filter.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// If the group is not a filter group, E_UNEXPECTED will be returned.  
/// Call this API after either <see cref="XblSocialManagerCreateSocialUserGroupFromFilters"/>
/// or <see cref="XblSocialManagerCreateSocialUserGroupFromList"/> to create an XblSocialManagerUserGroup.  
/// The XblSocialManagerUserGroupHandle is returned by the <see cref="XblSocialManagerEventType"/>::SocialUserGroupLoaded 
/// event in <see cref="XblSocialManagerDoWork"/>.<br/>
/// </remarks>
STDAPI XblSocialManagerUserGroupGetFilters(
    _In_ XblSocialManagerUserGroupHandle group,
    _Out_opt_ XblPresenceFilter* presenceFilter,
    _Out_opt_ XblRelationshipFilter* relationshipFilter
) XBL_NOEXCEPT;

/// <summary>
/// An array of pointers to internally owned XblSocialManagerUsers.
/// </summary>
/// <remarks>
/// This will be returned from <see cref="XblSocialManagerUserGroupGetUsers"/> and the array may change 
/// with each call to <see cref="XblSocialManagerDoWork"/>.
/// </remarks>
typedef const XblSocialManagerUser* const* XblSocialManagerUserPtrArray;

/// <summary>
/// Gets an XblSocialManagerUserPtrArray of the users tracked by the user group.
/// </summary>
/// <param name="group">The group handle from which to get users.</param>
/// <param name="users">Passes back a pointer to an array of XblSocialManagerUser objects.  
/// The memory for the returned pointer remains valid until the next time <see cref="XblSocialManagerDoWork"/> is called.</param>
/// <param name="usersCount">Passes back the size of the users array.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// The XblSocialManagerUserPtrArray is only guaranteed to be valid until the next call to <see cref="XblSocialManagerDoWork"/>.  
/// If the user objects are needed beyond the scope of the next <see cref="XblSocialManagerDoWork"/> call, 
/// they are statically sized and trivially copyable.  
/// Call this API after calling either <see cref="XblSocialManagerCreateSocialUserGroupFromFilters"/>
/// or <see cref="XblSocialManagerCreateSocialUserGroupFromList"/> to create an XblSocialManagerUserGroup.  
/// Wait for <see cref="XblSocialManagerEventType"/>::SocialUserGroupLoaded event in <see cref="XblSocialManagerDoWork"/>.  Prior to this the group will be empty.
/// </remarks>
STDAPI XblSocialManagerUserGroupGetUsers(
    _In_ XblSocialManagerUserGroupHandle group,
    _Outptr_result_maybenull_ XblSocialManagerUserPtrArray* users,
    _Out_ size_t* usersCount
) XBL_NOEXCEPT;

/// <summary>
/// Gets a pointer to an XSAPI owned array of xuids tracked by the group.
/// </summary>
/// <param name="group">The group from which to get users.</param>
/// <param name="trackedUsers">Passes back a pointer to tracked xuids array.  
/// The memory for the returned pointer remains valid until the next time <see cref="XblSocialManagerDoWork"/> is called.</param>
/// <param name="trackedUsersCount">Passes back the size of the tracked users array.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// For list-based groups, the set of tracked users is static (i.e. the list of xuids provided when the group is created), 
/// but for filter-based groups, the set tracked users changes as the local user's relationships change.  
/// Call this API after calling either <see cref="XblSocialManagerCreateSocialUserGroupFromFilters"/>
/// or <see cref="XblSocialManagerCreateSocialUserGroupFromList"/> to create an XblSocialManagerUserGroup.  
/// Wait for <see cref="XblSocialManagerEventType"/>::SocialUserGroupLoaded event in <see cref="XblSocialManagerDoWork"/>.  Prior to this the group will be empty.
/// </remarks>
STDAPI XblSocialManagerUserGroupGetUsersTrackedByGroup(
    _In_ XblSocialManagerUserGroupHandle group,
    _Outptr_result_maybenull_ const uint64_t** trackedUsers,
    _Out_ size_t* trackedUsersCount
) XBL_NOEXCEPT;

/// <summary>
/// Create a social graph for the specified local user.
/// </summary>
/// <param name="user">Xbox Live User to create a graph for.</param>
/// <param name="extraLevelDetail">The level of verbosity that should be in the service calls for this user.</param>
/// <param name="queue">Queue to be used for background operation for this user (Optional).
/// Note: Using XTaskQueueDispatchMode::Immediate for this queue may cause poor performance. </param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// The result of a local user being added will be triggered through 
/// the <see cref="XblSocialManagerEventType"/>::LocalUserAdded event in <see cref="XblSocialManagerDoWork"/>.  
/// To remove the social graph for the specified local user, call <see cref="XblSocialManagerRemoveLocalUser"/>.
/// </remarks>
STDAPI XblSocialManagerAddLocalUser(
    _In_ XblUserHandle user,
    _In_ XblSocialManagerExtraDetailLevel extraLevelDetail,
    _In_opt_ XTaskQueueHandle queue
) XBL_NOEXCEPT;

/// <summary>
/// Immediately removes a social graph for the specified local user.
/// </summary>
/// <param name="user">Xbox Live User to remove.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// When a user is removed, their social graph and user groups will be destroyed as well.  
/// There will be no future events associated with the user after they are removed.  
/// Call this API only if <see cref="XblSocialManagerAddLocalUser"/> was used to create a social graph for a local user.
/// </remarks>
STDAPI XblSocialManagerRemoveLocalUser(
    _In_ XblUserHandle user
) XBL_NOEXCEPT;

/// <summary>
/// Called whenever the title wants to update the social graph and get list of change events.
/// </summary>
/// <param name="socialEvents">Passes back a pointer to the array of social events that have occurred since the last call to XblSocialManagerDoWork.  
/// This array is only valid until the next call to XblSocialManagerDoWork.  
/// The internal array will automatically be cleaned up when XblCleanup is called.</param>
/// <param name="socialEventsCount">Passes back the number of events in the social events array.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Must be called every frame for data to be up to date.  
/// The array of social events that is sent back is only valid until the next call to <see cref="XblSocialManagerDoWork"/>.  
/// Make sure to check if there were social events sent back.  
/// If the social events array is null, no results.  
/// If the social events count is 0, no results.  
/// If there were social events sent back then handle each <see cref="XblSocialManagerEvent"/> 
/// by their respective <see cref="XblSocialManagerEventType"/>.
/// </remarks>
STDAPI XblSocialManagerDoWork(
    _Outptr_result_maybenull_ const XblSocialManagerEvent** socialEvents,
    _Out_ size_t* socialEventsCount
) XBL_NOEXCEPT;

/// <summary>
/// Constructs a XblSocialManagerUserGroup, which is a collection of users with social information.
/// </summary>
/// <param name="user">Xbox Live User the group is associated with.</param>
/// <param name="presenceFilter">The restriction of users based on their presence and title activity.</param>
/// <param name="relationshipFilter">The restriction of users based on their relationship to the calling user.</param>
/// <param name="group">Passes back a handle to the created group.  
/// This group can be later be cleaned up with XblSocialManagerDestroySocialUserGroup. The group will also be cleaned up 
/// (invalidating the returned handle) if the associated user is removed from Social Manager.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Wait for <see cref="XblSocialManagerEventType"/>::SocialUserGroupLoaded event in <see cref="XblSocialManagerDoWork"/>.  Prior to this the group will be empty.
/// </remarks>
STDAPI XblSocialManagerCreateSocialUserGroupFromFilters(
    _In_ XblUserHandle user,
    _In_ XblPresenceFilter presenceFilter,
    _In_ XblRelationshipFilter relationshipFilter,
    _Outptr_result_maybenull_ XblSocialManagerUserGroupHandle* group
) XBL_NOEXCEPT;

/// <summary>
/// Constructs a social XblSocialManagerUserGroup, which is a collection of users with social information.
/// </summary>
/// <param name="user">Xbox Live User.</param>
/// <param name="xboxUserIdList">List of users to populate the Xbox Social User Group with.  
/// The list cannot exceed XBL_SOCIAL_MANAGER_MAX_USERS_FROM_LIST.</param>
/// <param name="xboxUserIdListCount">The number of items in the xboxUserIdList.</param>
/// <param name="group">Passes back a handle to the created group.
/// This group may later be cleaned up with XblSocialManagerDestroySocialUserGroup. The group will also be cleaned up 
/// (invalidating the returned handle) if the associated user is removed from Social Manager.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Wait for <see cref="XblSocialManagerEventType"/>::SocialUserGroupLoaded event in <see cref="XblSocialManagerDoWork"/>.  Prior to this the group will be empty.
/// </remarks>
STDAPI XblSocialManagerCreateSocialUserGroupFromList(
    _In_ XblUserHandle user,
    _In_ uint64_t* xboxUserIdList,
    _In_ size_t xboxUserIdListCount,
    _Outptr_result_maybenull_ XblSocialManagerUserGroupHandle* group
) XBL_NOEXCEPT;

/// <summary>
/// Destroys a created Social User Group.
/// </summary>
/// <param name="group">The Social User Group to destroy and stop tracking.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// This will stop updaing the Xbox Social User Group and remove tracking for any users the XblSocialManagerUserGroup holds.  
/// Call this API after calling either <see cref="XblSocialManagerCreateSocialUserGroupFromFilters"/>
/// or <see cref="XblSocialManagerCreateSocialUserGroupFromList"/> to create an XblSocialManagerUserGroup.  
/// The XblSocialManagerUserGroupHandle is returned by the <see cref="XblSocialManagerEventType"/>::SocialUserGroupLoaded event 
/// in <see cref="XblSocialManagerDoWork"/>.
/// </remarks>
STDAPI XblSocialManagerDestroySocialUserGroup(
    _In_ XblSocialManagerUserGroupHandle group
) XBL_NOEXCEPT;

/// <summary>
/// Get the number of local users currently tracked by SocialManager.
/// </summary>
/// <returns>The number of local users currently tracked by SocialManager.</returns>
/// <remarks>
/// To add local users to be tracked, call <see cref="XblSocialManagerAddLocalUser"/>.  
/// To remove local users from being tracked, call <see cref="XblSocialManagerRemoveLocalUser"/>.  
/// This function must be called before calling <see cref="XblSocialManagerGetLocalUsers"/>.
/// </remarks>
STDAPI_(size_t) XblSocialManagerGetLocalUserCount() XBL_NOEXCEPT;

/// <summary>
/// Returns user handles for all users tracked by SocialManager.
/// </summary>
/// <param name="usersCount">The size of the user handle array.</param>
/// <param name="users">Passes back a pointer to an array to populate with local users.  
/// This array is only valid until the next call to XblSocialManagerDoWork.  
/// The user handles returned are not duplicated copies so do not call close on them.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Duplicate handle is not called before returning the handles, so if the user 
/// is needed after removing it from SocialManager, duplicate handle must be called.  
/// Make sure to have created a social graph for at least one local user by calling <see cref="XblSocialManagerAddLocalUser"/>.  
/// Also, call <see cref="XblSocialManagerGetLocalUserCount"/> to get the number of local users currently tracked.
/// </remarks>
STDAPI XblSocialManagerGetLocalUsers(
    _In_ size_t usersCount,
    _Out_writes_(usersCount) XblUserHandle* users
) XBL_NOEXCEPT;

/// <summary>
/// Updates specified social user group to new group of users.
/// </summary>
/// <param name="group">The xbox social user group to update.</param>
/// <param name="users">New list of users to track. Note that this replaces the existing list of tracked users.  
/// The list cannot exceed XBL_SOCIAL_MANAGER_MAX_USERS_FROM_LIST.</param>
/// <param name="usersCount">Number of items in the users array.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Does a diff to see which users have been added or removed.  
/// The result of a user group being updated will be triggered through the 
/// <see cref="XblSocialManagerEventType"/>::SocialUserGroupUpdated event in <see cref="XblSocialManagerDoWork"/>.  
/// Call this API after calling either <see cref="XblSocialManagerCreateSocialUserGroupFromFilters"/>
/// or <see cref="XblSocialManagerCreateSocialUserGroupFromList"/> to create an XblSocialManagerUserGroup.  
/// The XblSocialManagerUserGroupHandle is returned by the <see cref="XblSocialManagerEventType"/>::SocialUserGroupLoaded
/// event in <see cref="XblSocialManagerDoWork"/>.
/// </remarks>
STDAPI XblSocialManagerUpdateSocialUserGroup(
    _In_ XblSocialManagerUserGroupHandle group,
    _In_ uint64_t* users,
    _In_ size_t usersCount
) XBL_NOEXCEPT;

/// <summary>
/// Whether to enable social manager to poll every 30 seconds from the presence service.
/// </summary>
/// <param name="user">Xbox Live User to enable polling for.</param>
/// <param name="shouldEnablePolling">Whether or not polling should enabled.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblSocialManagerSetRichPresencePollingStatus(
    _In_ XblUserHandle user,
    _In_ bool shouldEnablePolling
) XBL_NOEXCEPT;

}