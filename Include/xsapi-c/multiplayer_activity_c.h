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
/// Enumerates the platforms on which a title can be activated.
/// </summary>
/// <memof><see cref="XblMultiplayerActivityInfo"/></memof>
/// <argof><see cref="XblMultiplayerActivitySetActivityAsync"/></argof>
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
    /// Windows OneCore (Universal Windows Platform [UWP] and {% term projname %} on PC).
    /// </summary>
    WindowsOneCore = 2,

    /// <summary>
    /// Win32-based device.
    /// </summary>
    Win32 = 3,

    /// <summary>
    /// {% term scarlett %} device.
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
/// Enumerates who can join a player's current activity.
/// </summary>
/// <memof><see cref="XblMultiplayerActivityInfo"/></memof>
/// <argof><see cref="XblMultiplayerActivitySetActivityAsync"/></argof>
/// <remarks>
/// To see how this enumeration is used, see "Setting an activity" in 
/// the <see href="live-mpa-client-how-to.md#activities">Activities</see> section 
/// of <see href="live-mpa-client-how-to.md">Example code for Multiplayer Activity</see>.
/// </remarks>
enum class XblMultiplayerActivityJoinRestriction : uint32_t
{
    /// <summary>
    /// Everyone.
    /// </summary>
    Public = 0,

    /// <summary>
    /// Only invited players.
    /// </summary>
    InviteOnly = 1,

    /// <summary>
    /// Only followed players.
    /// </summary>
    Followed = 2
};

/// <summary>
/// Enumerates types of recent player encounters.
/// </summary>
/// <memof><see cref="XblMultiplayerActivityRecentPlayerUpdate"/></memof>
/// <remarks>
/// To see how this enumeration is used, see "Setting an activity" in 
/// the <see href="live-mpa-client-how-to.md#activities">Activities</see> section 
/// of <see href="live-mpa-client-how-to.md">Example code for Multiplayer Activity</see>.
/// </remarks>
enum class XblMultiplayerActivityEncounterType : uint32_t
{
    /// <summary>
    /// No inherent meaning. The title interprets this value as appropriate.
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
/// Information about a player's activity while playing a title.
/// </summary>
/// <argof><see cref="XblMultiplayerActivityGetActivityResult"/></argof>
/// <argof><see cref="XblMultiplayerActivitySetActivityAsync"/></argof>
/// <remarks>
/// To see how this enumeration is used, see "Setting an activity" and "Getting activities" in 
/// the <see href="live-mpa-client-how-to.md#activities">Activities</see> section 
/// of <see href="live-mpa-client-how-to.md">Example code for Multiplayer Activity</see>.
/// </remarks>
typedef struct XblMultiplayerActivityInfo
{
    /// <summary>
    /// The Xbox user ID to which the activity info belongs.
    /// </summary>
    uint64_t xuid;

    /// <summary>
    /// The connection string passed to the connecting client to join a game;
    /// typically contains information such as the server IP address.
    /// When querying activities, this field is populated only if the activity is public
    /// or the player is following the caller.
    /// </summary>
    _Field_z_ const char* connectionString;

    /// <summary>
    /// Specifies who can join the player's current activity.
    /// </summary>
    XblMultiplayerActivityJoinRestriction joinRestriction;

    /// <summary>
    /// The maximum number of players who can join the player's current activity.  
    /// A value of 0 indicates that no players can join.
    /// </summary>
    size_t maxPlayers;

    /// <summary>
    /// The number of players already playing with the player in a multiplayer activity.  
    /// A value of 0 indicates that no other players are currently playing.
    /// </summary>
    size_t currentPlayers;

    /// <summary>
    /// A unique identifier to group all users playing as part of the same activity.  
    /// The title sets this identifier when it creates the activity.
    /// </summary>
    _Field_z_ const char* groupId;

    /// <summary>
    /// The platform on which the activity is happening.
    /// When setting an activity, the platform is automatically inferred; this field is ignored.
    /// </summary>
    XblMultiplayerActivityPlatform platform;
} XblMultiplayerActivityInfo;

/// <summary>
/// Describes a recent player encounter.
/// </summary>
/// <argof><see cref="XblMultiplayerActivityUpdateRecentPlayers"/></argof>
/// <remarks>
/// To see how this enumeration is used, see "Updating recent players" in 
/// the <see href="live-mpa-client-how-to.md#recent-players">Recent players</see> section 
/// of <see href="live-mpa-client-how-to.md">Example code for Multiplayer Activity</see>.
/// </remarks>
typedef struct XblMultiplayerActivityRecentPlayerUpdate
{
    /// <summary>
    /// Xbox user ID of the encountered user.
    /// </summary>
    uint64_t xuid;

    /// <summary>
    /// The type of encounter.
    /// </summary>
    XblMultiplayerActivityEncounterType encounterType;
} XblMultiplayerActivityRecentPlayerUpdate;

/// <summary>
/// Appends to a player's list of recent players.  
/// If an encountered user is already in the list, it updates the existing recent-player entry.
/// </summary>
/// <param name="xblContext">{% term xbox-live %} context for the local user.</param>
/// <param name="updates">List of <see cref="XblMultiplayerActivityRecentPlayerUpdate"/> objects to append to the recent players list.</param>
/// <param name="updatesCount">Size of the `updates` array.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// This call is unidirectional; it only affects the caller's recent-players list. <br/><br/>
/// Recent-player updates are batched and uploaded by XSAPI by using the background queue provided during `XblInitialize`. <br/><br/>
/// To force an immediate flush, call <see cref="XblMultiplayerActivityFlushRecentPlayersAsync"/>.
/// </remarks>
STDAPI XblMultiplayerActivityUpdateRecentPlayers(
    _In_ XblContextHandle xblContext,
    _In_reads_(updatesCount) const XblMultiplayerActivityRecentPlayerUpdate* updates,
    _In_ size_t updatesCount
) XBL_NOEXCEPT;

/// <summary>
/// Immediately writes any pending recent-players updates to {% term xbox-live %}.  
/// </summary>
/// <param name="xblContext">{% term xbox-live %} context for the local user.</param>
/// <param name="async">The `XAsyncBlock` for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Calling this API is optional; updates are periodically uploaded from a background task queue. <br/><br/>
/// When this API is used, the upload happens on the task queue supplied in this call. <br/><br/>
/// To get the result of the asynchronous operation, call <see cref="XAsyncGetStatus"/> 
/// inside the `XAsyncBlock` callback or after `XAsyncBlock` is complete. <br/><br/>
/// To see how this enumeration is used, see the 
/// <see href="live-mpa-client-how-to.md#recent-players">Recent players</see> section 
/// of <see href="live-mpa-client-how-to.md">Example code for Multiplayer Activity</see>.
/// </remarks>
/// <rest>Calls POST /titles/{titleId}/recentplayers</rest>
STDAPI XblMultiplayerActivityFlushRecentPlayersAsync(
    _In_ XblContextHandle xblContext,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Sets or updates the multiplayer activity for a local user.  
/// </summary>
/// <param name="xblContext">{% term xbox-live %} context for the local user.</param>
/// <param name="activityInfo">Information about the activity. 
/// The `maxPlayers` and `currentPlayers` fields are optional; they are ignored if set to 0. 
/// The value of the `platform` field is ignored; XSAPI automatically sets the activity 
/// on the appropriate local platform.</param>
/// <param name="allowCrossPlatformJoin">
/// True if the activity should be joinable on other platforms supported by the title.
/// </param>
/// <param name="async">The `XAsyncBlock` for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// When a title starts or joins a multiplayer experience, it should create an 
/// activity. Doing this lets both the shell and other players in your title 
/// see the player's activity. Your title can let other players join the game 
/// in progress. If a player wants to join an activity for your title and it is not 
/// running, it is activated and the connection string is passed to it. <br/><br/>
/// To see how this function is used, see "Setting an activity" in 
/// the <see href="live-mpa-client-how-to.md#activities">Activities</see> section 
/// of <see href="live-mpa-client-how-to.md">Example code for Multiplayer Activity</see>.
/// </remarks>
/// <rest>Calls PUT /titles/{titleId}/users/{xuid}/activites</rest>
STDAPI XblMultiplayerActivitySetActivityAsync(
    _In_ XblContextHandle xblContext,
    _In_ const XblMultiplayerActivityInfo* activityInfo,
    _In_ bool allowCrossPlatformJoin,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Get the multiplayer activity for a set of users. You can query at most 30 users with each call.
/// </summary>
/// <param name="xblContext">{% term xbox-live %} context for the local user.</param>
/// <param name="xuids">List of Xbox user IDs for whom to get multiplayer activity.</param>
/// <param name="xuidsCount">Size of the `xuids` array.</param>
/// <param name="async">The `XAsyncBlock` for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// To get the result, call <see cref="XblMultiplayerActivityGetActivityResultSize"/> and 
/// <see cref="XblMultiplayerActivityGetActivityResult"/> inside the `XAsyncBlock` callback or 
/// after the async operation is complete. <br/><br/>
/// To see how this function is used, see "Getting activities" in 
/// the <see href="live-mpa-client-how-to.md#activities">Activities</see> section 
/// of <see href="live-mpa-client-how-to.md">Example code for Multiplayer Activity</see>. <br/><br/>
/// For more information about multiplayer activities, see
/// <see href="live-mpa-activities.md">Activities</see>.
/// </remarks>
/// <rest>Calls POST /titles/{titleId}/activities/query</rest>
STDAPI XblMultiplayerActivityGetActivityAsync(
    _In_ XblContextHandle xblContext,
    _In_reads_(xuidsCount) const uint64_t* xuids,
    _In_ size_t xuidsCount,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Gets the buffer size needed to store the results of a get activity call.
/// </summary>
/// <param name="async">The `XAsyncBlock` for this operation.</param>
/// <param name="resultSizeInBytes">The size in bytes for the result buffer.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// To see how this function is used, see "Getting activities" in 
/// the <see href="live-mpa-client-how-to.md#activities">Activities</see> section 
/// of <see href="live-mpa-client-how-to.md">Example code for Multiplayer Activity</see>. <br/><br/>
/// For more information about multiplayer activities, see
/// <see href="live-mpa-activities.md">Activities</see>.
/// </remarks>
/// <seealso cref="XblMultiplayerActivityGetActivityAsync"/>
/// <seealso cref="XblMultiplayerActivityGetActivityResult"/>
STDAPI XblMultiplayerActivityGetActivityResultSize(
    _In_ XAsyncBlock* async,
    _Out_ size_t* resultSizeInBytes
) XBL_NOEXCEPT;

/// <summary>
/// Gets the results for a successful get activity call.
/// </summary>
/// <param name="async">The `XAsyncBlock` for this operation.</param>
/// <param name="bufferSize">The size of the result buffer.  
/// Use <see cref="XblMultiplayerActivityGetActivityResultSize"/> to get the required buffer size.</param>
/// <param name="buffer">A caller-allocated byte buffer that receives the result.</param>
/// <param name="ptrToBufferResults">Strongly typed array of <see cref="XblMultiplayerActivityInfo"/> that 
/// points into `buffer`. Do not free this array. Its lifecycle is tied to `buffer`.
/// </param>
/// <param name="resultCount">The number of entries in the `ptrToBufferResults` array.</param>
/// <param name="bufferUsed">The number of bytes in `buffer` that were used.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// To get the size of the buffer that you need to store the results, call the
/// <see cref="XblMultiplayerActivityGetActivityResultSize"/> function. <br/><br/>
/// To see how this function is used, see "Getting activities" in 
/// the <see href="live-mpa-client-how-to.md#activities">Activities</see> section 
/// of <see href="live-mpa-client-how-to.md">Example code for Multiplayer Activity</see>. <br/><br/>
/// For more information about multiplayer activities, see
/// <see href="live-mpa-activities.md">Activities</see>.
/// </remarks>
STDAPI XblMultiplayerActivityGetActivityResult(
    _In_ XAsyncBlock* async,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_(bufferSize, *bufferUsed) void* buffer,
    _Outptr_ XblMultiplayerActivityInfo** ptrToBufferResults,
    _Out_ size_t* resultCount,
    _Out_opt_ size_t* bufferUsed
) XBL_NOEXCEPT;

/// <summary>
/// Clears the multiplayer activity for the local user.
/// </summary>
/// <param name="xblContext">{% term xbox-live %} context for the local user.</param>
/// <param name="async">The `XAsyncBlock` for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Titles should delete the activity for a user as soon as they leave the multiplayer activity. <br/><br/>
/// If the title does not delete a user's activity, it is automatically cleared by a presence check. <br/><br/>
/// To get the result of the asynchronous operation, call <see cref="XAsyncGetStatus"/> 
/// inside the `XAsyncBlock` callback or after `XAsyncBlock` is complete. <br/><br/>
/// To see how this function is used, see "Deleting an activity" in 
/// the <see href="live-mpa-client-how-to.md#activities">Activities</see> section 
/// of <see href="live-mpa-client-how-to.md">Example code for Multiplayer Activity</see>. <br/><br/>
/// For more information about multiplayer activities, see
/// <see href="live-mpa-activities.md">Activities</see>.
/// </remarks>
/// <rest>Calls DELETE /titles/{titleId}/users/{xuid}/activites</rest>
STDAPI XblMultiplayerActivityDeleteActivityAsync(
    _In_ XblContextHandle xblContext,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Sends invites to Xbox user IDs to join the caller's current activity.
/// </summary>
/// <param name="xblContext">{% term xbox-live %} context for the local user.</param>
/// <param name="xuids">List of Xbox user IDs to invite.</param>
/// <param name="xuidsCount">Size of the `xuids` array.</param>
/// <param name="allowCrossPlatformJoin">If the title is configured for cross-platform invites, 
/// setting this parameter to true sends an invite to all platform endpoints that the title supports.  
/// If set to false, the invite is sent to the sender's platform platform only.  
/// If cross-platform invites are not configured, the invite is always sent to the sender's platform only.</param>
/// <param name="connectionString">(Optional) Connection string that the peer uses to join the game.</param>
/// <param name="async">The `XAsyncBlock` for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// To get the result of the asynchronous operation, call <see cref="XAsyncGetStatus"/> 
/// inside the `XAsyncBlock` callback or after `XAsyncBlock` is complete. <br/><br/>
/// To see how this function is used, see "Sending invites" in 
/// the <see href="live-mpa-client-how-to.md#invites">Invites</see> section 
/// of <see href="live-mpa-client-how-to.md">Example code for Multiplayer Activity</see>. <br/><br/>
/// For more information about multiplayer activities, see
/// <see href="live-mpa-activities.md">Activities</see>.
/// </remarks>
/// <rest>Calls POST /titles/{titleId}/invites</rest>
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
/// Describes multiplayer activity invites.
/// </summary>
/// <argof><see cref="XblMultiplayerActivityInviteHandler"/></argof>
/// <remarks>
/// For more information about multiplayer activities, see
/// <see href="live-mpa-activities.md">Activities</see>.
/// </remarks>
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
    /// URL of the sender's gamerpic.
    /// </summary>
    _Field_z_ const char* senderImageUrl;

    /// <summary>
    /// The UTF-8 encoded gamertag of the player.
    /// </summary>
    char senderGamertag[XBL_GAMERTAG_CHAR_SIZE];

    /// <summary>
    /// The UTF-8 encoded modern gamertag of the player.
    /// Not guaranteed to be unique.
    /// </summary>
    char senderModernGamertag[XBL_MODERN_GAMERTAG_CHAR_SIZE];

    /// <summary>
    /// The UTF-8 encoded suffix appended to the modern gamertag to ensure uniqueness.
    /// Can be empty in some cases.
    /// </summary>
    char senderModernGamertagSuffix[XBL_MODERN_GAMERTAG_SUFFIX_CHAR_SIZE];

    /// <summary>
    /// The UTF-8 encoded unique modern gamertag and suffix.
    /// Format is "modernGamertag#suffix".
    /// Guaranteed to be no more than 16 rendered characters.
    /// </summary>
    char senderUniqueModernGamertag[XBL_UNIQUE_MODERN_GAMERTAG_CHAR_SIZE];

    /// <summary>
    /// The name of the title for which the invite is being sent.
    /// </summary>
    _Field_z_ const char* titleName;

    /// <summary>
    /// URL of the title image used in the invite.
    /// </summary>
    _Field_z_ const char* titleImageUrl;

    /// <summary>
    /// Connection string to pass to the connecting client so that it can join a game.
    /// Typically contains information such as the server IP address.
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
/// <remarks>
/// The lifetime of the <see cref="XblMultiplayerActivityInviteData"/> object is limited to the callback. <br/><br/>
/// For more information about multiplayer activities, see
/// <see href="live-mpa-activities.md">Activities</see>.
/// </remarks>
typedef void CALLBACK XblMultiplayerActivityInviteHandler(
    _In_ const XblMultiplayerActivityInviteData* data,
    _In_opt_ void* context
);

/// <summary>
/// Registers an event handler for multiplayer activity invites.
/// </summary>
/// <param name="xblContextHandle">{% term xbox-live %} context for the local user.</param>
/// <param name="handler">The callback function that receives notifications.</param>
/// <param name="context">Caller context to be passed to the handler.</param>
/// <returns>An `XblFunctionContext` object that can be used to unregister the event handler.</returns>
/// <remarks>
/// To unregister an event handler for multiplayer activity invites, call the
/// <see cref="XblMultiplayerActivityRemoveInviteHandler"/> function. <br/><br/>
/// For more information about multiplayer activities, see
/// <see href="live-mpa-activities.md">Activities</see>.
/// </remarks>
STDAPI_(XblFunctionContext) XblMultiplayerActivityAddInviteHandler(
    _In_ XblContextHandle xblContextHandle,
    _In_ XblMultiplayerActivityInviteHandler* handler,
    _In_opt_ void* context
) XBL_NOEXCEPT;

/// <summary>
/// Unregisters an event handler for multiplayer activity invites.
/// </summary>
/// <param name="xblContextHandle">{% term xbox-live %} context for the local user.</param>
/// <param name="token">The `XblFunctionContext` object that was returned when the event handler was registered.</param>
/// <returns></returns>
/// <remarks>
/// To register an event handler for multiplayer activity invites, call the
/// <see cref="XblMultiplayerActivityAddInviteHandler"/> function. <br/><br/>
/// For more information about multiplayer activities, see
/// <see href="live-mpa-activities.md">Activities</see>.
/// </remarks>
STDAPI XblMultiplayerActivityRemoveInviteHandler(
    _In_ XblContextHandle xblContextHandle,
    _In_ XblFunctionContext token
) XBL_NOEXCEPT;

#endif
}