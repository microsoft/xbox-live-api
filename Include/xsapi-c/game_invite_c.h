// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#if !defined(__cplusplus)
#error C++11 required
#endif

#pragma once
#include "multiplayer_c.h"
#include "real_time_activity_c.h"

#if HC_PLATFORM == HC_PLATFORM_WIN32 || HC_PLATFORM_IS_EXTERNAL
extern "C"
{
/// <summary>
/// Contains information about received game invite notifications.
/// </summary>
typedef struct XblGameInviteNotificationEventArgs
{
    /// <summary>
    /// The Xbox user ID of the player.
    /// </summary>
    uint64_t invitedXboxUserId;

    /// <summary>
    /// The Xbox user ID of the player.
    /// </summary>
    uint64_t senderXboxUserId;

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
    /// Invite Handle ID.  
    /// The memory for the returned string pointer only remains valid inside the XblGameInviteHandler, 
    /// so deep copy the string if you need to refer to it outside the handler.
    /// </summary>
    _Field_z_ const char* inviteHandleId;

    /// <summary>
    /// Invite Protocol.  
    /// The memory for the returned string pointer only remains valid inside the XblGameInviteHandler,
    /// so deep copy the string if you need to refer to it outside the handler.
    /// </summary>
    _Field_z_ const char* inviteProtocol;

    /// <summary>
    /// Invite Context.  
    /// The memory for the returned string pointer only remains valid inside the XblGameInviteHandler,
    /// so deep copy the string if you need to refer to it outside the handler.
    /// </summary>
    _Field_z_ const char* inviteContext;

    /// <summary>
    /// Sender Image URL.  
    /// The memory for the returned string pointer only remains valid inside the XblGameInviteHandler,
    /// so deep copy the string if you need to refer to it outside the handler.
    /// </summary>
    _Field_z_ const char* senderImageUrl;

    /// <summary>
    /// Expiration Date.
    /// </summary>
    time_t expiration;

    /// <summary>
    /// Multiplayer Session Reference.
    /// </summary>
    XblMultiplayerSessionReference sessionReference;

} XblGameInviteNotificationEventArgs;

/// <summary>
/// Handle for Function handling Game Invite Event Notifications.
/// </summary>
/// <param name="args">The game invite notifications that are passed in.</param>
/// <param name="context">Caller context to be passed the handler.</param>
/// <returns></returns>
/// <remarks>The lifetime of the XblGameInviteNotifcationEventArgs object is limited to the callback.</remarks>
typedef void CALLBACK XblGameInviteHandler(
    _In_ const XblGameInviteNotificationEventArgs* args,
    _In_opt_ void* context
);

/// <summary>
/// Registers the title to receive notifications for game invites.  
/// Call XblGameInviteRegisterForEventResult() to get the result.
/// DEPRECATED. Calling this API is no longer required and it will be removed in a future release. Registration with appropriate
/// service endpoints is done automatically by XSAPI as <see cref="XblGameInviteHandler"/> are added and removed.
/// </summary>
/// <param name="xboxLiveContext">Xbox live context for the local user.</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI_XBL_DEPRECATED XblGameInviteRegisterForEventAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Gets the result of a XblGameInviteRegisterForEventAsync operation.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="subscriptionHandle">Handle for the subscription to be registered.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI_XBL_DEPRECATED XblGameInviteRegisterForEventResult(
    _In_ XAsyncBlock* async,
    _Out_ XblRealTimeActivitySubscriptionHandle* subscriptionHandle
) XBL_NOEXCEPT;

/// <summary>
/// Unregisters the title to stop receiving notifications for game invites.
/// DEPRECATED. Calling this API is no longer required and it will be removed in a future release. Registration with appropriate
/// service endpoints is done automatically by XSAPI as <see cref="XblGameInviteHandler"/> are added and removed.
/// </summary>
/// <param name="xblContextHandle">Xbox live context for the local user.</param>
/// <param name="subscriptionHandle">Handle for the subscription to be unregistered.</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>E_FAIL will be returned if this API before a Game Invite has been registered.</remarks>
STDAPI_XBL_DEPRECATED XblGameInviteUnregisterForEventAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_ XblRealTimeActivitySubscriptionHandle subscriptionHandle,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Registers an event handler for game invite notifications.
/// </summary>
/// <param name="xblContextHandle">Xbox live context for the local user.</param>
/// <param name="handler">The callback function that receives notifications.</param>
/// <param name="context">Caller context to be passed the handler.</param>
/// <returns>An XblFunctionContext object that can be used to unregister the event handler.</returns>
STDAPI_(XblFunctionContext) XblGameInviteAddNotificationHandler(
    _In_ XblContextHandle xblContextHandle,
    _In_ XblGameInviteHandler* handler,
    _In_opt_ void* context
) XBL_NOEXCEPT;

/// <summary>
/// Unregisters an event handler for game invite notifications.
/// </summary>
/// <param name="xblContextHandle">Xbox live context for the local user.</param>
/// <param name="token">The XblFunctionContext object that was returned when the event handler was registered.</param>
/// <returns></returns>
STDAPI_(void) XblGameInviteRemoveNotificationHandler(
    _In_ XblContextHandle xblContextHandle,
    _In_ XblFunctionContext token
) XBL_NOEXCEPT;

} // end extern C
#endif