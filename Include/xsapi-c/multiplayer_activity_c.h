// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#if !defined(__cplusplus)
#error C++11 required
#endif

#pragma once

#include "xsapi-c/pal.h"

extern "C"
{

/// <summary>
/// Different platforms on which a title can be activated.
/// </summary>
enum class XblMultiplayerActivityPlatform : uint32_t
{
    /// <summary>
    /// Unknown device.
    /// </summary>
    Unknown = 0,

    /// <summary>
    /// Xbox One device.
    /// </summary>
    XboxOne = 1,

    /// <summary>
    /// WindowsOneCore (UWP & GameCore on PC).
    /// </summary>
    WindowsOneCore = 2,

    /// <summary>
    /// Win32 based device.
    /// </summary>
    Win32 = 3,

    /// <summary>
    /// Scarlett device.
    /// </summary>
    Scarlett = 4,

    /// <summary>
    /// iOS device.
    /// </summary>
    iOS = 20,

    /// <summary>
    /// Android device.
    /// </summary>
    Android = 30,

    /// <summary>
    /// Nintendo device.
    /// </summary>
    Nintendo = 40,

    /// <summary>
    /// PlayStation device.
    /// </summary>
    PlayStation = 50,

    /// <summary>
    /// Activity is joinable on all platforms supported by the title.
    /// </summary>
    All = 60
};

/// <summary>
/// Setting which dictates who can join a player's current activity.
/// </summary>
enum class XblMultiplayerActivityJoinRestriction : uint32_t
{
    /// <summary>
    /// Public.
    /// </summary>
    Public = 0,

    /// <summary>
    /// InviteOnly.
    /// </summary>
    InviteOnly = 1,

    /// <summary>
    /// Followed.
    /// </summary>
    Followed = 2
};

/// <summary>
/// Different types of recent players encounters.
/// </summary>
enum class XblMultiplayerActivityEncounterType : uint32_t
{
    /// <summary>
    /// Default.  
    /// This has no inherent meaning - it is up to the title to interpret this value appropriately.
    /// </summary>
    Default = 0,

    /// <summary>
    /// Teammate.
    /// </summary>
    Teammate = 1,

    /// <summary>
    /// Opponent.
    /// </summary>
    Opponent = 2
};

/// <summary>
/// Information about a User's activity playing a certain title.
/// </summary>
typedef struct XblMultiplayerActivityInfo
{
    /// <summary>
    /// Xbox User ID the activity info belongs to.
    /// </summary>
    uint64_t xuid;

    /// <summary>
    /// The connection string is passed to the connecting client in order to join a game.
    /// Typically contains information like the server IP address.
    /// When querying activities, this field will only be populated if the activity is public
    /// or if the user is following the caller.
    /// </summary>
    _Field_z_ const char* connectionString;

    /// <summary>
    /// Setting which dictates who can join the player's current activity.
    /// </summary>
    XblMultiplayerActivityJoinRestriction joinRestriction;

    /// <summary>
    /// The maximum number of players who can join the players current activity.  
    /// A value of 0 indicates that maxPlayers is not set.
    /// </summary>
    size_t maxPlayers;

    /// <summary>
    /// The number of players already playing with the user in a multiplayer activity.  
    /// A value of 0 indicates that currentPlayers is not set.
    /// </summary>
    size_t currentPlayers;

    /// <summary>
    /// A unique identifier to group all users playing as part of the same activity.  
    /// Set by title when the activity is created.
    /// </summary>
    _Field_z_ const char* groupId;

    /// <summary>
    /// The platform on which that activity is happening.
    /// When setting an activity, the platform will be automatically inferred (this field will be ignored).
    /// </summary>
    XblMultiplayerActivityPlatform platform;
} XblMultiplayerActivityInfo;

/// <summary>
/// Struct describing a recent player encounter.
/// </summary>
typedef struct XblMultiplayerActivityRecentPlayerUpdate
{
    /// <summary>
    /// Xuid of the encountered user.
    /// </summary>
    uint64_t xuid;

    /// <summary>
    /// The type of encounter.
    /// </summary>
    XblMultiplayerActivityEncounterType encounterType;
} XblMultiplayerActivityRecentPlayerUpdate;

/// <summary>
/// Appends to a user's list of recent players.  
/// If an encountered user is already in the list, it updates the existing recent player entry.
/// </summary>
/// <param name="xblContext">Xbox live context for the local user.</param>
/// <param name="updates">List of XblMultiplayerActivityRecentPlayerUpdates to append to the recent players list.</param>
/// <param name="updatesCount">Size of the updates array.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// This call only affects the caller's recent players list (unidirectional).  
/// Recent player updates will be batched and uploaded by XSAPI using the background queue provided during XblInitialize.  
/// To force an immediate flush, call XblMultiplayerActivityFlushRecentPlayersAsync.
/// </remarks>
STDAPI XblMultiplayerActivityUpdateRecentPlayers(
    _In_ XblContextHandle xblContext,
    _In_reads_(updatesCount) const XblMultiplayerActivityRecentPlayerUpdate* updates,
    _In_ size_t updatesCount
) XBL_NOEXCEPT;

/// <summary>
/// Immediately writes any pending recent players updates to XboxLive service.  
/// Note that calling this API is optional; updates will automatically be uploaded periodically on a background task queue.  
/// </summary>
/// <param name="xblContext">Xbox live context for the local user.</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// When this API is used, the upload will happen on the task queue supplied in this call.  
/// The result of the asynchronous operation can be obtained by calling <see cref="XAsyncGetStatus"/> 
/// inside the AsyncBlock callback or after the AsyncBlock is complete. 
/// </remarks>
STDAPI XblMultiplayerActivityFlushRecentPlayersAsync(
    _In_ XblContextHandle xblContext,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Sets or updates the multiplayer activity for a local user.  
/// </summary>
/// <param name="xblContext">Xbox live context for the local user.</param>
/// <param name="activityInfo">Struct describing the activity.  
/// The "maxPlayers" and "currentPlayers" fields are optional and will be ignored if they are set to 0.  
/// The value of the "platform" field will be ignored and XSAPI will automatically set the activity as 
/// being on the correct local platform.</param>
/// <param name="allowCrossPlatformJoin">True if the activity should be joinable on other platforms supported by the title.</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerActivitySetActivityAsync(
    _In_ XblContextHandle xblContext,
    _In_ const XblMultiplayerActivityInfo* activityInfo,
    _In_ bool allowCrossPlatformJoin,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Get the multiplayer activity for a set of users. You can query at most 30 users with each call.
/// </summary>
/// <param name="xblContext">Xbox live context for the local user.</param>
/// <param name="xuids">List of Xbox User IDs for whom to get multiplayer activity.</param>
/// <param name="xuidsCount">Size of the xuids array.</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// To get the result, call <see cref="XblMultiplayerActivityGetActivityResultSize"/> and 
/// <see cref="XblMultiplayerActivityGetActivityResult"/> inside the XAsyncBlock callback or 
/// after the async operation is complete.
/// </remarks>
STDAPI XblMultiplayerActivityGetActivityAsync(
    _In_ XblContextHandle xblContext,
    _In_reads_(xuidsCount) const uint64_t* xuids,
    _In_ size_t xuidsCount,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Gets the buffer size needed to store the results of a get activity call.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="resultSizeInBytes">The size in bytes for the result buffer.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerActivityGetActivityResultSize(
    _In_ XAsyncBlock* async,
    _Out_ size_t* resultSizeInBytes
) XBL_NOEXCEPT;

/// <summary>
/// Gets the results for a successful get activity call.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="bufferSize">The size of the result buffer.  
/// Use <see cref="XblMultiplayerActivityGetActivityResultSize"/> to get the required buffer size.</param>
/// <param name="buffer">A caller allocated byte buffer to write result into.</param>
/// <param name="ptrToBufferResults">Strongly typed array ofXblMultiplayerActivityInfo that points into buffer.  
/// Do not free this as its lifecycle is tied to buffer.</param>
/// <param name="resultCount">The number of entries in the ptrToBufferResults array.</param>
/// <param name="bufferUsed">The number of bytes in the provided buffer that were used.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerActivityGetActivityResult(
    _In_ XAsyncBlock* async,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_(bufferSize, *bufferUsed) void* buffer,
    _Outptr_ XblMultiplayerActivityInfo** ptrToBufferResults,
    _Out_ size_t* resultCount,
    _Out_opt_ size_t* bufferUsed
) XBL_NOEXCEPT;

/// <summary>
/// Clear the multiplayer activity for the local user.
/// </summary>
/// <param name="xblContext">Xbox live context for the local user.</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Titles should delete the activity for a user as soon as they leave the multiplayer activity.  
/// A user's activity will be automatically cleared by a presence check if the title fails to delete it manually.  
/// The result of the asynchronous operation can be obtained by calling <see cref="XAsyncGetStatus"/> 
/// inside the AsyncBlock callback or after the AsyncBlock is complete. 
/// </remarks>
STDAPI XblMultiplayerActivityDeleteActivityAsync(
    _In_ XblContextHandle xblContext,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Sends invites to specified Xuids to join the callers current activity.
/// </summary>
/// <param name="xblContext">Xbox live context for the local user.</param>
/// <param name="xuids">List of Xbox User IDs to invite.</param>
/// <param name="xuidsCount">Size of the xuids array.</param>
/// <param name="allowCrossPlatformJoin">If the title is configured for cross platform invites, 
/// setting this to true will send an invitation to all platform endpoints the title supports.  
/// If set to false, the invite will only be sent to the platform of the sender.  
/// If cross platform invites are not configured, the invite will always be sent to just the sender's platform.</param>
/// <param name="connectionString">(Optional) Connection string that the peer will use to join the game.</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// The result of the asynchronous operation can be obtained by calling <see cref="XAsyncGetStatus"/> 
/// inside the AsyncBlock callback or after the AsyncBlock is complete.
/// </remarks>
STDAPI XblMultiplayerActivitySendInvitesAsync(
    _In_ XblContextHandle xblContext,
    _In_ const uint64_t* xuids,
    _In_ size_t xuidsCount,
    _In_ bool allowCrossPlatformJoin,
    _In_opt_z_ const char* connectionString,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

#if HC_PLATFORM == HC_PLATFORM_WIN32 || HC_PLATFORM_IS_EXTERNAL
/// <summary>
/// Contains information about received game multiplayer activity invites.
/// </summary>
typedef struct XblMultiplayerActivityInviteData
{
    /// <summary>
    /// The Xbox user ID of the invited user.
    /// </summary>
    uint64_t invitedXuid;

    /// <summary>
    /// The Xbox user ID of the user sending the invite.
    /// </summary>
    uint64_t senderXuid;

    /// <summary>
    /// Sender Image URL.
    /// </summary>
    _Field_z_ const char* senderImageUrl;

    /// <summary>
    /// The UTF-8 encoded gamertag of the player.
    /// </summary>
    char senderGamertag[XBL_GAMERTAG_CHAR_SIZE];

    /// <summary>
    /// The UTF-8 encoded modern gamertag for the player.
    /// Not guaranteed to be unique.
    /// </summary>
    char senderModernGamertag[XBL_MODERN_GAMERTAG_CHAR_SIZE];

    /// <summary>
    /// The UTF-8 encoded suffix appended to modern gamertag to ensure uniqueness.
    /// May be empty in some cases.
    /// </summary>
    char senderModernGamertagSuffix[XBL_MODERN_GAMERTAG_SUFFIX_CHAR_SIZE];

    /// <summary>
    /// The UTF-8 encoded unique modern gamertag and suffix.
    /// Format will be "modernGamertag#suffix".
    /// Guaranteed to be no more than 16 rendered characters.
    /// </summary>
    char senderUniqueModernGamertag[XBL_UNIQUE_MODERN_GAMERTAG_CHAR_SIZE];

    /// <summary>
    /// The name of the title for which the invite is being sent.
    /// </summary>
    _Field_z_ const char* titleName;

    /// <summary>
    /// Url to the title image used in the invite.
    /// </summary>
    _Field_z_ const char* titleImageUrl;

    /// <summary>
    /// The connection string is passed to the connecting client in order to join a game.
    /// Typically contains information like the server IP address.
    /// </summary>
    _Field_z_ const char* connectionString;

    /// <summary>
    /// Expiration time.
    /// </summary>
    time_t expirationTime;
} XblMultiplayerActivityInviteData;

/// <summary>
/// Event handler for multiplayer activity invites.
/// </summary>
/// <param name="data">Data needed by the invitee to respond to a game invite.</param>
/// <param name="context">Client context provided when the handler was added.</param>
/// <returns></returns>
/// <remarks>The lifetime of the XblMultiplayerActivityInviteData object is limited to the callback.</remarks>
typedef void CALLBACK XblMultiplayerActivityInviteHandler(
    _In_ const XblMultiplayerActivityInviteData* data,
    _In_opt_ void* context
);

/// <summary>
/// Registers an event handler for multiplayer activity invites.
/// </summary>
/// <param name="xblContextHandle">Xbox live context for the local user.</param>
/// <param name="handler">The callback function that receives notifications.</param>
/// <param name="context">Caller context to be passed the handler.</param>
/// <returns>An XblFunctionContext object that can be used to unregister the event handler.</returns>
STDAPI_(XblFunctionContext) XblMultiplayerActivityAddInviteHandler(
    _In_ XblContextHandle xblContextHandle,
    _In_ XblMultiplayerActivityInviteHandler* handler,
    _In_opt_ void* context
) XBL_NOEXCEPT;

/// <summary>
/// Unregisters an event handler for multiplayer activity invites.
/// </summary>
/// <param name="xblContextHandle">Xbox live context for the local user.</param>
/// <param name="token">The XblFunctionContext object that was returned when the event handler was registered.</param>
/// <returns></returns>
STDAPI XblMultiplayerActivityRemoveInviteHandler(
    _In_ XblContextHandle xblContextHandle,
    _In_ XblFunctionContext token
) XBL_NOEXCEPT;

#endif
}