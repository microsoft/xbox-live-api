// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#if !defined(__cplusplus)
   #error C++11 required
#endif

#pragma once

extern "C"
{

/// <summary>
/// All of the things protected by the Privacy Engine.
/// </summary>
/// <memof><see cref="XblPermissionDenyReasonDetails"/></memof>
enum class XblPrivacySetting : uint32_t
{
    /// <summary>
    /// Unrecognized privacy setting (not one of the below values).
    /// </summary>
    Unknown = 0,

    /// <summary>
    /// List of friends.
    /// </summary>
    ShareFriendList = 1,

    /// <summary>
    /// Played game history.
    /// </summary>
    ShareGameHistory = 2,

    /// <summary>
    /// Messaging and Voice chat.
    /// </summary>
    CommunicateUsingTextAndVoice = 3,

    /// <summary>
    /// Online status.
    /// </summary>
    SharePresence = 4,

    /// <summary>
    /// User Profile.
    /// </summary>
    ShareProfile = 5,

    /// <summary>
    /// Video and Music Status.
    /// </summary>
    ShareVideoAndMusicStatus = 6,

    /// <summary>
    /// Video Communication.
    /// </summary>
    CommunicateUsingVideo = 7,

    /// <summary>
    /// Voice Data Collection.
    /// </summary>
    CollectVoiceData = 8,

    /// <summary>
    /// Share Xbox Music Activity.
    /// </summary>
    ShareXboxMusicActivity = 9,

    /// <summary>
    /// Fitness Data.
    /// </summary>
    ShareExerciseInfo = 11,

    /// <summary>
    /// Share identity - Real Name, aka MSA Name.
    /// </summary>
    ShareIdentity = 12,

    /// <summary>
    /// Real Identity Data for in game use.
    /// </summary>
    ShareIdentityInGame = 13,

    /// <summary>
    /// Game DVR.
    /// </summary>
    ShareRecordedGameSessions = 14,

    /// <summary>
    /// Allow Microsoft to collect data about Live TV watching.
    /// </summary>
    CollectLiveTvData = 15,

    /// <summary>
    /// Allow Microsoft to collect data about Xbox Video watching.
    /// </summary>
    CollectXboxVideoData = 16,

    /// <summary>
    /// Allow other users to view real identity via "transitive real name".
    /// </summary>
    ShareIdentityTransitively = 17,

    /// <summary>
    /// Allow other users to view the owner's video viewing history.
    /// </summary>
    ShareVideoHistory = 18,

    /// <summary>
    /// Allow other users to view the owner's music history.
    /// </summary>
    ShareMusicHistory = 19,

    /// <summary>
    /// Allow the user to view user created content of other users.  
    /// This is a parental control on the owner, not a privacy setting that affects other users.
    /// </summary>
    AllowUserCreatedContentViewing = 20,

    /// <summary>
    /// Allow the user to view the profiles of other users.  
    /// This is a parental control on the owner, not a privacy setting that affects other users.
    /// </summary>
    AllowProfileViewing = 21,

    /// <summary>
    /// Sometimes called the 'Cloaked' bit.
    /// </summary>
    ShowRealTimeActivity = 22,

    /// <summary>
    /// Allow full voice data (identifiable to user) to be collected on Xbox One.
    /// </summary>
    CollectVoiceDataXboxOneFull = 23,

    /// <summary>
    /// Enforcement setting to prevent a user from sharing identity.
    /// </summary>
    CanShareIdentity = 24,

    /// <summary>
    /// Allow other users to share owner's Xbox LIVE content to external social networks.
    /// </summary>
    ShareContentToExternalNetworks = 25,

    /// <summary>
    /// Allow voice search data to be collected on Xbox One.
    /// </summary>
    CollectVoiceSearchData = 26,

    /// <summary>
    /// Allow other users to see the public clubs that the user has joined.
    /// </summary>
    ShareClubMembership = 27,

    /// <summary>
    /// Allow voice data collection from game chats.
    /// </summary>
    CollectVoiceGameChatData = 28,

    /// <summary>
    /// Allow other users to view items posted by the owner on their activity feed.
    /// </summary>
    ShareActivityFeed = 29,

    /// <summary>
    /// Augments the existing communications settings to allow communications with 
    /// user on platforms other than Xbox Live.
    /// </summary>
    CommunicateDuringCrossNetworkPlay = 30,
};

/// <summary>
/// Controls user's privileges.
/// </summary>
/// <memof><see cref="XblPermissionDenyReasonDetails"/></memof>
enum class XblPrivilege : uint32_t
{
    /// <summary>
    /// Unrecognized privilege (not one of the below values).
    /// </summary>
    Unknown = 0,

    /// <summary>
    /// Controls the ability of the user to view the profile (bio, motto, etc) of other users.
    /// </summary>
    AllowIngameVoiceCommunications = 205,

    /// <summary>
    /// Controls the ability of the user to communicate with other users via video.
    /// </summary>
    AllowVideoCommunications = 235,

    /// <summary>
    /// Controls the ability of the user to view the profile (bio, motto, etc) of other users.
    /// </summary>
    AllowProfileViewing = 249,

    /// <summary>
    /// Controls the ability of the user to communicate with other users.
    /// </summary>
    AllowCommunications = 252,

    /// <summary>
    /// Controls the ability of the user to join parties with other users.
    /// </summary>
    AllowMultiplayer = 254,

    /// <summary>
    /// Controls the ability of the user to add friends.
    /// </summary>
    AllowAddFriend = 255
};

/// <summary>
/// Actions that a client can check permission for.
/// </summary>
/// <remarks>
/// Permission may be restricted by either a missing privilege of the caller or 
/// a privacy restriction of the target.
/// </remarks>
/// <argof><see cref="XblPrivacyCheckPermissionAsync"/></argof>
/// <argof><see cref="XblPrivacyCheckPermissionForAnonymousUserAsync"/></argof>
/// <argof><see cref="XblPrivacyBatchCheckPermissionAsync"/></argof>
/// <memof><see cref="XblPermissionCheckResult"/></memof>
enum class XblPermission : uint32_t
{
    /// <summary>
    /// Unrecognized permission (not one of the below values).
    /// </summary>
    Unknown = 0,

    /// <summary>
    /// Check whether or not the user can send a message with text content or an invitation to the target user.
    /// This value does not change if the player has muted the target user.  Use CommunicateUsingVoice instead.
    /// This value will be false if for example you have set your comms to friends only and the target is not a friend.
    /// This value will be false if for example if the target user has blocked you.
    /// This value will be false if for example you have set your comms settings to Blocked.
    /// </summary>
    CommunicateUsingText = 1000,

    /// <summary>
    /// Check whether or not the user can communicate using video with the target user.
    /// </summary>
    CommunicateUsingVideo = 1001,
    
    /// <summary>
    /// Check whether or not the user can communicate using voice with the target user.
    /// This will be false if the player has muted the target user.
    /// </summary>
    CommunicateUsingVoice = 1002,

    /// <summary>
    /// Check whether or not the user can view the profile of the target user.
    /// </summary>
    ViewTargetProfile = 1004,

    /// <summary>
    /// Check whether or not the user can view the game history of the target user.
    /// </summary>
    ViewTargetGameHistory = 1005,

    /// <summary>
    /// Check whether or not the user can view the detailed video watching history of the target user.
    /// </summary>
    ViewTargetVideoHistory = 1006,

    /// <summary>
    /// Check whether or not the user can view the detailed music listening history of the target user.
    /// </summary>
    ViewTargetMusicHistory = 1007,

    /// <summary>
    /// Check whether or not the user can view the exercise info of the target user.
    /// </summary>
    ViewTargetExerciseInfo = 1009,

    /// <summary>
    /// Check whether or not the user can view the online status of the target user.
    /// </summary>
    ViewTargetPresence = 1011,

    /// <summary>
    /// Check whether or not the user can view the details of the targets video status (extended online presence).
    /// </summary>
    ViewTargetVideoStatus = 1012,

    /// <summary>
    /// Check whether or not the user can view the details of the targets music status (extended online presence).
    /// </summary>
    ViewTargetMusicStatus = 1013,

    /// <summary>
    /// Check whether or not a user can play multiplayer with the target user.
    /// </summary>
    PlayMultiplayer = 1014,

    /// <summary>
    /// Check whether or not the user can view user created content produced by target user.
    /// </summary>
    ViewTargetUserCreatedContent = 1018,

    /// <summary>
    /// Check whether or not the user can broadcast sessions on Twitch.
    /// </summary>
    BroadcastWithTwitch = 1019,

    /// <summary>
    /// Check whether or not the user can write a comment on an object owned by the target.
    /// </summary>
    WriteComment = 1022,

    /// <summary>
    /// Check whether or not the user can share an item owned by the target.
    /// </summary>
    ShareItem = 1024,

    /// <summary>
    /// Check whether or not the user can share an item owned by the target to external social networks.
    /// </summary>
    ShareTargetContentToExternalNetworks = 1025,
};

/// <summary>
/// This describes the various ways that we expose to a requestor why a permission check may fail.
/// </summary>
/// <memof>  * <see cref="XblPermissionDenyReasonDetails"/></memof>
enum class XblPermissionDenyReason : uint32_t
{
    /// <summary>
    /// Permission was denied, but either no reason was given or the privacy service threw an 
    /// unexpected error.
    /// </summary>
    Unknown = 0,

    /// <summary>
    /// The request was processed successfully, but the requestor is not allowed to perform the action.  
    /// No reason is given.
    /// </summary>
    NotAllowed = 2,

    /// <summary>
    /// The requestor was missing a privilege necessary for the action.
    /// </summary>
    MissingPrivilege = 3,

    /// <summary>
    /// A privilege value for the requestor has a restriction that doesn't allow interaction with the target.  
    /// For instance, a parental control only allows interaction with friends and the target isn't a friend.
    /// </summary>
    PrivilegeRestrictsTarget = 4,

    /// <summary>
    /// The requestor has blocked the target user.
    /// </summary>
    BlockListRestrictsTarget = 5,

    /// <summary>
    /// The requestor has muted the target user.
    /// </summary>
    MuteListRestrictsTarget = 7,

    /// <summary>
    /// A privacy value for the requestor has a restriction that doesn't allow interaction with the target.  
    /// For instance, a parental control only allows interaction with friends and the target isn't a friend.
    /// </summary>
    PrivacySettingRestrictsTarget = 9,

    /// <summary>
    /// The target is a cross-network user, but cross-network privacy settings indicated only friends are allowed.  
    /// Cross-network friends are (currently) only managed at the title level, so the title must validate 
    /// that the user are friends.
    /// </summary>
    CrossNetworkUserMustBeFriend = 12
};

/// <summary>
/// Represents the different classes of non-Xbox Live users that we can check permissions for.
/// </summary>
/// <argof><see cref="XblPrivacyCheckPermissionForAnonymousUserAsync"/></argof>
/// <argof><see cref="XblPrivacyBatchCheckPermissionAsync"/></argof>
/// <memof><see cref="XblPermissionCheckResult"/></memof>
enum class XblAnonymousUserType : uint32_t
{
    /// <summary>
    /// Invalid XblAnonymousUserType. Returned if service returns unrecognized XblAnonymousUserType
    /// </summary>
    Unknown = 0,

    /// <summary>
    /// A non Xbox Live user.
    /// </summary>
    CrossNetworkUser,

    /// <summary>
    /// A non Xbox Live user that a title recognizes as an in-game friend.
    /// </summary>
    CrossNetworkFriend
};

/// <summary>
/// This struct gives details about why permission is denied.
/// </summary>
/// <memof>  * <see cref="XblPermissionCheckResult"/></memof>
typedef struct XblPermissionDenyReasonDetails
{
    /// <summary>
    /// Reason why permission was denied.  
    /// Additional detail maybe found in restrictedPrivilege or 
    /// restrictedPrivacySetting depending on what the reason is.
    /// </summary>
    XblPermissionDenyReason reason;

    /// <summary>
    /// Active when the deny reason is either XblPermissionDenyReason::MissingPrivilege or 
    /// XblPermissionDenyReason::PrivilegeRestrictsTarget.  
    /// Unknown otherwise.
    /// </summary>
    XblPrivilege restrictedPrivilege;

    /// <summary>
    /// Active when the deny reason is XblPermissionDenyReason::PrivacySettingRestrictsTarget.  
    /// Unknown otherwise.
    /// </summary>
    XblPrivacySetting restrictedPrivacySetting;
} XblPermissionDenyReasonDetails;

/// <summary>
/// Struct describing the result of a permission check request.
/// </summary>
/// <argof><see cref="XblPrivacyCheckPermissionResult"/></argof>
/// <argof><see cref="XblPrivacyCheckPermissionForAnonymousUserResult"/></argof>
/// <argof><see cref="XblPrivacyBatchCheckPermissionResult"/></argof>
typedef struct XblPermissionCheckResult
{
    /// <summary>
    /// Value indicating whether or not permission to take the requested action is granted.
    /// </summary>
    bool isAllowed;

    /// <summary>
    /// Target Xuid for the permission check request.  
    /// Will be 0 if the permission check was for an anonymous user.
    /// </summary>
    uint64_t targetXuid;

    /// <summary>
    /// The class of anonymous user the permission check was for.  
    /// Will be XblAnonymousUserType::Unknown if the permission check was for an Xbox Live user.
    /// </summary>
    XblAnonymousUserType targetUserType;

    /// <summary>
    /// The permission that was requested.
    /// </summary>
    XblPermission permissionRequested;

    /// <summary>
    /// Array of reasons why permission was denied.  
    /// Null when isAllowed is true.
    /// </summary>
    XblPermissionDenyReasonDetails* reasons;

    /// <summary>
    /// Number of entries in the reasons array.
    /// </summary>
    size_t reasonsCount;
} XblPermissionCheckResult;

/// <summary>
/// Get the list of Xuids the calling user should avoid during multiplayer matchmaking.
/// </summary>
/// <param name="xblContextHandle">Xbox live context for the local user.</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Call <see cref="XblPrivacyGetAvoidListResultCount"/> and <see cref="XblPrivacyGetAvoidListResult"/> 
/// upon completion to get the result.
/// </remarks>
STDAPI XblPrivacyGetAvoidListAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Get result count for an XblPrivacyGetAvoidListAsync call.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="xuidCount">Passes back the number of Xuids in the avoid list.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblPrivacyGetAvoidListResultCount(
    _In_ XAsyncBlock* async,
    _Out_ size_t* xuidCount
) XBL_NOEXCEPT;

/// <summary>
/// Get result for an XblPrivacyGetAvoidListAsync call.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="xuidCount">Size of the xuids array.
/// Use <see cref="XblPrivacyGetAvoidListResultCount"/> to get the count required.</param>
/// <param name="xuids">A caller allocated array that passes back the avoid list xuids result.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblPrivacyGetAvoidListResult(
    _In_ XAsyncBlock* async,
    _In_ size_t xuidCount,
    _Out_writes_(xuidCount) uint64_t* xuids
) XBL_NOEXCEPT;

/// <summary>
/// Check a single permission with a single target user.
/// </summary>
/// <param name="xblContextHandle">Xbox live context for the local user.</param>
/// <param name="permissionToCheck">The permission to check.</param>
/// <param name="targetXuid">The target user's Xuid for validation.</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// If multiple permissions and/or target users are needed, use the batch API instead: <see cref="XblPrivacyBatchCheckPermissionAsync"/>
/// Call <see cref="XblPrivacyCheckPermissionResultSize"/> and <see cref="XblPrivacyCheckPermissionResult"/> 
/// upon completion to get the result.
/// </remarks>
STDAPI XblPrivacyCheckPermissionAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_ XblPermission permissionToCheck,
    _In_ uint64_t targetXuid,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Get the result size for an XblPrivacyCheckPermissionAsync call.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="resultSizeInBytes">Passes bakc the size in bytes required to store the permission check result.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblPrivacyCheckPermissionResultSize(
    _In_ XAsyncBlock* async,
    _Out_ size_t* resultSizeInBytes
) XBL_NOEXCEPT;

/// <summary>
/// Get the result for an XblPrivacyCheckPermissionAsync call.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="bufferSize">The size of the provided buffer.
/// Use <see cref="XblPrivacyCheckPermissionResultSize"/> to get the size required.</param>
/// <param name="buffer">A caller allocated byte buffer that passes back the permission results.</param>
/// <param name="ptrToBuffer">Passes back a strongly typed pointer that points into buffer.  
/// Do not free this as its lifecycle is tied to buffer.</param>
/// <param name="bufferUsed">Passes back the number of bytes written to the buffer.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblPrivacyCheckPermissionResult(
    _In_ XAsyncBlock* async,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_(bufferSize, *bufferUsed) void* buffer,
    _Outptr_ XblPermissionCheckResult** ptrToBuffer,
    _Out_opt_ size_t* bufferUsed
) XBL_NOEXCEPT;

/// <summary>
/// Check a single permission for class of anonymous users.
/// </summary>
/// <param name="xblContextHandle">Xbox live context for the local user.</param>
/// <param name="permissionToCheck">The permission to check.</param>
/// <param name="userType">The class of anonymous user to check permission for.</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Call <see cref="XblPrivacyCheckPermissionResultSize"/> and <see cref="XblPrivacyCheckPermissionResult"/> 
/// upon completion to get the result.
/// </remarks>
STDAPI XblPrivacyCheckPermissionForAnonymousUserAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_ XblPermission permissionToCheck,
    _In_ XblAnonymousUserType userType,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Get the result size for an XblPrivacyCheckPermissionForAnonymousUserAsync call.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="resultSizeInBytes">Passes back the size in bytes required to store the permission check result.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblPrivacyCheckPermissionForAnonymousUserResultSize(
    _In_ XAsyncBlock* async,
    _Out_ size_t* resultSizeInBytes
) XBL_NOEXCEPT;

/// <summary>
/// Get the result for an XblPrivacyCheckPermissionForAnonymousUserAsync call.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="bufferSize">The size of the provided buffer.  
/// Use <see cref="XblPrivacyCheckPermissionForAnonymousUserResultSize"/> to get the size required.</param>
/// <param name="buffer">A caller allocated byte buffer that passes back the permission results.</param>
/// <param name="ptrToBuffer">Passes back a strongly typed pointer that points into buffer.  
/// Do not free this as its lifecycle is tied to buffer.</param>
/// <param name="bufferUsed">Passes back the number of bytes written to the buffer.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblPrivacyCheckPermissionForAnonymousUserResult(
    _In_ XAsyncBlock* async,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_(bufferSize, *bufferUsed) void* buffer,
    _Outptr_ XblPermissionCheckResult** ptrToBuffer,
    _Out_opt_ size_t* bufferUsed
) XBL_NOEXCEPT;

/// <summary>
/// Check multiple permissions with multiple target users.  
/// Each permission will be checked against each target user.
/// </summary>
/// <param name="xblContextHandle">Xbox live context for the local user.</param>
/// <param name="permissionsToCheck">Array of permissions to check.</param>
/// <param name="permissionsCount">Number of entries in the permissions array.</param>
/// <param name="targetXuids">Array of target Xuids to check permissions against.</param>
/// <param name="xuidsCount">Number of entries in the xuids array.</param>
/// <param name="targetAnonymousUserTypes">Array of anonymous user types to check permissions against.</param>
/// <param name="targetAnonymousUserTypesCount">Number of entries in the user types array.</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Call <see cref="XblPrivacyBatchCheckPermissionResultSize"/> and <see cref="XblPrivacyBatchCheckPermissionResult"/> 
/// upon completion to get result.
/// </remarks>
STDAPI XblPrivacyBatchCheckPermissionAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_reads_(permissionsCount) XblPermission* permissionsToCheck,
    _In_ size_t permissionsCount,
    _In_reads_(xuidsCount) uint64_t* targetXuids,
    _In_ size_t xuidsCount,
    _In_reads_(targetAnonymousUserTypesCount) XblAnonymousUserType* targetAnonymousUserTypes,
    _In_ size_t targetAnonymousUserTypesCount,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Get the result size for an XblPrivacyBatchCheckPermissionAsync call.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="resultSizeInBytes">Passes back the size in bytes required to store the permission check results.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblPrivacyBatchCheckPermissionResultSize(
    _In_ XAsyncBlock* async,
    _Out_ size_t* resultSizeInBytes
) XBL_NOEXCEPT;

/// <summary>
/// Get the results for an XblPrivacyBatchCheckPermissionAsync call.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="bufferSize">The size of the provided buffer.  
/// Use <see cref="XblPrivacyBatchCheckPermissionResultSize"/> to get the size required.</param>
/// <param name="buffer">A caller allocated byte buffer that passes back the permission result.</param>
/// <param name="ptrToBufferResults">Passes back a strongly typed array of XblPermissionCheckResult that points into buffer.  
/// Do not free this as its lifecycle is tied to buffer.</param>
/// <param name="ptrToBufferCount">Passes back the number of entries in the ptrToBufferResults array.</param>
/// <param name="bufferUsed">Passes back the number of bytes written to the buffer.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblPrivacyBatchCheckPermissionResult(
    _In_ XAsyncBlock* async,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_(bufferSize, *bufferUsed) void* buffer,
    _Outptr_ XblPermissionCheckResult** ptrToBufferResults,
    _Out_ size_t* ptrToBufferCount,
    _Out_opt_ size_t* bufferUsed
) XBL_NOEXCEPT;

/// <summary>
/// Get the list of Xuids that the calling user should not hear (mute) during multiplayer matchmaking.
/// </summary>
/// <param name="xblContextHandle">Xbox live context for the local user.</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Call <see cref="XblPrivacyGetMuteListResultCount"/> and <see cref="XblPrivacyGetMuteListResult"/> 
/// upon completion to get result.
/// </remarks>
STDAPI XblPrivacyGetMuteListAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Get result count for an XblPrivacyGetMuteListAsync call.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="xuidCount">Passes back the number of Xuids in the mute list.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblPrivacyGetMuteListResultCount(
    _In_ XAsyncBlock* async,
    _Out_ size_t* xuidCount
) XBL_NOEXCEPT;

/// <summary>
/// Get result for an XblPrivacyGetMuteListAsync call.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="xuidCount">Size of the xuids array.
/// Use <see cref="XblPrivacyGetMuteListResultCount"/> to get the count required.</param>
/// <param name="xuids">A caller allocated array that passes back the mute list xuids result.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblPrivacyGetMuteListResult(
    _In_ XAsyncBlock* async,
    _In_ size_t xuidCount,
    _Out_writes_(xuidCount) uint64_t* xuids
) XBL_NOEXCEPT;

}