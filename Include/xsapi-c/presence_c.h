// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#if !defined(__cplusplus)
   #error C++11 required
#endif

#pragma once

#include "real_time_activity_c.h"

extern "C"
{

/// <summary>
/// Defines values used to indicate the device type associate with an XblSocialManagerPresenceTitleRecord.
/// </summary>
/// <memof><see cref="XblPresenceDeviceRecord"/></memof>
/// <memof><see cref="XblPresenceQueryFilters"/></memof>
/// <argof><see cref="XblPresenceDevicePresenceChangedHandler"/></argof>
enum class XblPresenceDeviceType : uint32_t
{
    /// <summary>
    /// Unknown device.
    /// </summary>
    Unknown,

    /// <summary>
    /// Windows Phone device.
    /// </summary>
    WindowsPhone,

    /// <summary>
    /// Windows Phone 7 device.
    /// </summary>
    WindowsPhone7,

    /// <summary>
    /// Web device, like Xbox.com.
    /// </summary>
    Web,

    /// <summary>
    /// Xbox360 device.
    /// </summary>
    Xbox360,

    /// <summary>
    /// PC Games for Windows Live.
    /// </summary>
    PC,

    /// <summary>
    /// Xbox Live for Windows device.
    /// </summary>
    Windows8,

    /// <summary>
    /// Xbox One device.
    /// </summary>
    XboxOne,

    /// <summary>
    /// Windows One Core devices.
    /// </summary>
    WindowsOneCore,

    /// <summary>
    /// Windows One Core Mobile devices.
    /// </summary>
    WindowsOneCoreMobile,

    /// <summary>
    /// iOS device.
    /// </summary>
    iOS,

    /// <summary>
    /// Android device.
    /// </summary>
    Android,

    /// <summary>
    /// AppleTV device.
    /// </summary>
    AppleTV,

    /// <summary>
    /// Nintendo device.
    /// </summary>
    Nintendo,

    /// <summary>
    /// PlayStation device.
    /// </summary>
    PlayStation,

    /// <summary>
    /// Win32 based device.
    /// </summary>
    Win32,

    /// <summary>
    /// Scarlett device.
    /// </summary>
    Scarlett
};

/// <summary>
/// Defines values used to indicate the state of the user with regard to the presence service.
/// </summary>
/// <argof><see cref="XblPresenceRecordGetUserState"/></argof>
enum class XblPresenceUserState : uint32_t
{
    /// <summary>
    /// The state is unknown.
    /// </summary>
    Unknown,

    /// <summary>
    /// User is signed in to Xbox Live and active in a title.
    /// </summary>
    Online,

    /// <summary>
    /// User is signed-in to Xbox Live, but inactive in all titles.
    /// </summary>
    Away,

    /// <summary>
    /// User is not signed in to Xbox Live.
    /// </summary>
    Offline
};

/// <summary>
/// Defines values used to indicate the states of the screen view of presence information.
/// </summary>
/// <memof><see cref="XblPresenceTitleRecord"/></memof>
enum class XblPresenceTitleViewState : uint32_t
{
    /// <summary>
    /// Unknown view state.
    /// </summary>
    Unknown,

    /// <summary>
    /// The title's view is using the full screen.
    /// </summary>
    FullScreen,

    /// <summary>
    /// The title's view is using part of the screen with another application snapped.
    /// </summary>
    Filled,

    /// <summary>
    /// The title's view is snapped with another application using a part of the screen.
    /// </summary>
    Snapped,

    /// <summary>
    /// The title's running in the background and is not visible.
    /// </summary>
    Background
};

/// <summary>
/// Defines values used to set the level of presence detail return from the service.  
/// Choosing proper detail level could help the performance of the API.
/// </summary>
/// <memof><see cref="XblPresenceQueryFilters"/></memof>
enum class XblPresenceDetailLevel : uint32_t
{
    /// <summary>
    /// Default detail level.
    /// </summary>
    Default,

    /// <summary>
    /// User detail level. User presence info only, no device, title or rich presence info.
    /// </summary>
    User,

    /// <summary>
    /// Device detail level. User and device presence info only, no title or rich presence info.
    /// </summary>
    Device,

    /// <summary>
    /// Title detail level. User, device and title presence info only, no rich presence info.
    /// </summary>
    Title,

    /// <summary>
    /// All detail possible. User, device, title and rich presence info will be provided.
    /// </summary>
    All
};

/// <summary>
/// Defines values used to indicate the media id types for media presence data.
/// </summary>
enum class XblPresenceMediaIdType : uint32_t
{
    /// <summary>
    /// Unknown media Id.
    /// </summary>
    Unknown,

    /// <summary>
    /// Bing media Id.
    /// </summary>
    Bing,

    /// <summary>
    /// MediaProvider media Id.
    /// </summary>
    MediaProvider
};

/// <summary>
/// Defines values used to indicate the title presence state for a user.
/// </summary>
/// <argof><see cref="XblPresenceTitlePresenceChangedHandler"/></argof>
enum class XblPresenceTitleState : uint32_t
{
    /// <summary>
    /// Indicates this is a Unknown state.
    /// </summary>
    Unknown,

    /// <summary>
    /// Indicates the user started playing the title.
    /// </summary>
    Started,

    /// <summary>
    /// Indicates the user ended playing the title.
    /// </summary>
    Ended
};

/// <summary>
/// Defines values representing the streaming provider.
/// </summary>
/// <memof><see cref="XblPresenceBroadcastRecord"/></memof>
enum class XblPresenceBroadcastProvider : uint32_t
{
    /// <summary>
    /// Unknown streaming provider.
    /// </summary>
    Unknown,

    /// <summary>
    /// Streaming using Twitch.
    /// </summary>
    Twitch
};

/// <summary>
/// The handle to an xbl presence record.
/// </summary>
typedef struct XblPresenceRecord* XblPresenceRecordHandle;

/// <summary>
/// Defines values representing the xbl presence device record.
/// </summary>
/// <argof><see cref="XblPresenceRecordGetDeviceRecords"/></argof>
typedef struct XblPresenceDeviceRecord
{
    /// <summary>
    /// The device type associated with this record.
    /// </summary>
    XblPresenceDeviceType deviceType;

    /// <summary>
    /// The records containing title presence data.
    /// </summary>
    const struct XblPresenceTitleRecord* titleRecords;

    /// <summary>
    /// The number of title records in the titleRecords array.
    /// </summary>
    size_t titleRecordsCount;
} XblPresenceDeviceRecord;

/// <summary>
/// Defines values representing the xbl presence title record.
/// </summary>
/// <memof><see cref="XblPresenceDeviceRecord"/></memof>
typedef struct XblPresenceTitleRecord
{
    /// <summary>
    /// The title ID.
    /// </summary>
    uint32_t titleId;

    /// <summary>
    /// The title name.
    /// </summary>
    _Field_z_ const char* titleName;

    /// <summary>
    /// Time when the record was last updated.
    /// </summary>
    time_t lastModified;

    /// <summary>
    /// The active state for the title.
    /// </summary>
    bool titleActive;

    /// <summary>
    /// The formatted and localized presence string.
    /// </summary>
    _Field_z_ const char* richPresenceString;

    /// <summary>
    /// The title view state.
    /// </summary>
    XblPresenceTitleViewState viewState;

    /// <summary>
    /// The broadcast information of what the user is broadcasting.
    /// </summary>
    struct XblPresenceBroadcastRecord* broadcastRecord;
} XblPresenceTitleRecord;

/// <summary>
/// The broadcast information of what the user is broadcasting.
/// </summary>
/// <memof><see cref="XblPresenceTitleRecord"/></memof>
typedef struct XblPresenceBroadcastRecord
{
    /// <summary>
    /// Id for this broadcast as defined by the broadcasting service.
    /// </summary>
    _Field_z_ const char* broadcastId;

    /// <summary>
    /// The GUID uniquely identifying the broadcasting session.
    /// </summary>
    char session[XBL_GUID_LENGTH];

    /// <summary>
    /// The streaming provider.
    /// </summary>
    XblPresenceBroadcastProvider provider;

    /// <summary>
    /// Approximate number of current viewers.
    /// </summary>
    uint32_t viewerCount;

    /// <summary>
    /// Time the broadcast was started.
    /// </summary>
    time_t startTime;
} XblPresenceBroadcastRecord;

/// <summary>
/// Ids needed to set Rich Presence.
/// </summary>
/// <argof><see cref="XblPresenceSetPresenceAsync"/></argof>
typedef struct XblPresenceRichPresenceIds
{
    /// <summary>
    /// ID of the service configuration containing the presence strings.
    /// </summary>
    char scid[XBL_SCID_LENGTH];

    /// <summary>
    /// The ID of a presence string that is defined in the service configuration.  
    /// For example, PresenceId = "1" could equal "Playing {0} on {1}" in the service configuration.  
    /// The service configuration might map token 0 to Maps and token 1 to MapId.
    /// </summary>
    _Field_z_ const char* presenceId;

    /// <summary>
    /// The IDs of the strings to replace the format string tokens found in the presence string.  
    /// These strings are also defined in the service configuration.  
    /// The ID values in the collection map to the strings associated with the token arguments found in the PresenceId.  
    /// For example let's say this vector view contained the values "4" and "1" and PresenceId = "1" equals "Playing {0} on {1}" in the service configuration.  
    /// The service configuration might map Token 0 = Maps, where MapId = "4" equals "Hometown".  
    /// The service configuration might map Token 1 = Difficulty, where DifficultyId = "1" equals "Casual".
    /// </summary>
    const char** presenceTokenIds;

    /// <summary>
    /// The number of Ids in the presenceTokenIds array.
    /// </summary>
    size_t presenceTokenIdsCount;
} XblPresenceRichPresenceIds;

/// <summary>
/// Struct passed to presence APIs to filter the presence records returned.
/// </summary>
/// <remarks>
/// If the filters are not provided, defaults will be used:<br/>
///   - Returns records for all possible titles on all devices.<br/>
///   - Defaults to XblPresenceDetailLevel::Default which is equivalent to XblPresenceDetailLevel::Title (get basic title level information).<br/>
///   - Does not filter out users who are offline or broadcasting.<br/>
/// </remarks>
/// <argof><see cref="XblPresenceGetPresenceForMultipleUsersAsync"/></argof>
/// <argof><see cref="XblPresenceGetPresenceForSocialGroupAsync"/></argof>
typedef struct XblPresenceQueryFilters
{
    /// <summary>
    /// Array of device types. If this field is null, defaults to all possible deviceTypes.
    /// </summary>
    const XblPresenceDeviceType* deviceTypes;

    /// <summary>
    /// Size of the deviceTypes array.
    /// </summary>
    size_t deviceTypesCount;

    /// <summary>
    /// List of titleIds for filtering the result. If the input is an empty vector, defaults to all possible titles.
    /// </summary>
    const uint32_t* titleIds;

    /// <summary>
    /// Size of the titleIds array.
    /// </summary>
    size_t titleIdsCount;

    /// <summary>
    /// Detail level of the result. Defaults to XblPresenceDetailLevel::Title which get basic title level information.
    /// To get rich presence info, set to XblPresenceDetailLevel::All
    /// </summary>
    XblPresenceDetailLevel detailLevel;

    /// <summary>
    /// If true, API will filter out records for users that are offline.
    /// </summary>
    bool onlineOnly;

    /// <summary>
    /// If true, API will filter out records for users that are not broadcasting.
    /// </summary>
    bool broadcastingOnly;
} XblPresenceQueryFilters;

/// <summary>
/// Get the Xuid for the user a presence record is associated with.
/// </summary>
/// <param name="handle">Handle for the presence record returned from a GetPresence API.</param>
/// <param name="xuid">Passes back the Xuid the record is associated with.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblPresenceRecordGetXuid(
    _In_ XblPresenceRecordHandle handle,
    _Out_ uint64_t* xuid
) XBL_NOEXCEPT;

/// <summary>
/// Get the global presence state for a presence record.
/// </summary>
/// <param name="handle">Handle for the presence record returned from a GetPresence API.</param>
/// <param name="userState">A caller allocated struct that passes back the presence state of the record.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblPresenceRecordGetUserState(
    _In_ XblPresenceRecordHandle handle,
    _Out_ XblPresenceUserState* userState
) XBL_NOEXCEPT;

/// <summary>
/// Get the device presence records associated with a returned presence record.
/// </summary>
/// <param name="handle">Handle for the presence record returned from a GetPresence API.</param>
/// <param name="deviceRecords">Passes back a pointer to an array of device presence records.  
/// The memory for the returned pointer array remains valid for the life of the XblPresenceRecordHandle object until it is closed.</param>
/// <param name="deviceRecordsCount">Passes back the size of the returned array.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblPresenceRecordGetDeviceRecords(
    _In_ XblPresenceRecordHandle handle,
    _Out_ const XblPresenceDeviceRecord** deviceRecords,
    _Out_ size_t* deviceRecordsCount
) XBL_NOEXCEPT;

/// <summary>
/// Duplicates a XblPresenceRecordHandle.
/// </summary>
/// <param name="handle">The presence record handle.</param>
/// <param name="duplicatedHandle">Passe back the duplicated handle.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblPresenceRecordDuplicateHandle(
    _In_ XblPresenceRecordHandle handle,
    _Out_ XblPresenceRecordHandle* duplicatedHandle
) XBL_NOEXCEPT;

/// <summary>
/// Closes a XblPresenceRecordHandle.
/// </summary>
/// <param name="handle">The presence record handle.</param>
/// <returns></returns>
/// <remarks>
/// When all outstanding handles have been closed, XblPresenceRecordCloseHandle will free the memory 
/// associated with the presence record handle.
/// </remarks>
STDAPI_(void) XblPresenceRecordCloseHandle(
    _In_ XblPresenceRecordHandle handle
) XBL_NOEXCEPT;

/// <summary>
/// Sets presence info for the current user context.
/// </summary>
/// <param name="xblContextHandle">Xbox live context for the local user.</param>
/// <param name="isUserActiveInTitle">Indicates if the current user context is currently active or inactive in the title.  
/// The application can choose to set this based on an amount of inactivity.</param>
/// <param name="richPresenceIds">Optional pointer to struct which controls the rich presence strings.</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblPresenceSetPresenceAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_ bool isUserActiveInTitle,
    _In_opt_ XblPresenceRichPresenceIds* richPresenceIds,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Gets presence info for a specific Xbox User Id.
/// </summary>
/// <param name="xblContextHandle">Xbox live context for the local user.</param>
/// <param name="xuid">The Xbox User ID of the user to get presence for.</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// If presence info is needed for multiple users, use the batch API instead: <see cref="XblPresenceGetPresenceForMultipleUsersAsync"/> 
/// </remarks>
STDAPI XblPresenceGetPresenceAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_ uint64_t xuid,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Get result for an XblPresenceGetPresenceAsync call.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="presenceRecordHandle">Returned handle to a presence record.  
/// The associated presence record must be released with <see cref="XblPresenceRecordCloseHandle"/> 
/// when it is no longer needed.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblPresenceGetPresenceResult(
    _In_ XAsyncBlock* async,
    _Out_ XblPresenceRecordHandle* presenceRecordHandle
) XBL_NOEXCEPT;

/// <summary>
/// Gets presence info for multiple users.
/// </summary>
/// <param name="xblContextHandle">Xbox live context for the local user.</param>
/// <param name="xuids">The Xbox User IDs of the users to get presence for.</param>
/// <param name="xuidsCount">Size of the xuids array.</param>
/// <param name="filters">Optional filters struct to filter results.</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblPresenceGetPresenceForMultipleUsersAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_ uint64_t* xuids,
    _In_ size_t xuidsCount,
    _In_opt_ XblPresenceQueryFilters* filters,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Get result count for an XblPresenceGetPresenceForMultipleUsersAsync call.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="resultCount">Passes back the number of presence records.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblPresenceGetPresenceForMultipleUsersResultCount(
    _In_ XAsyncBlock* async,
    _Out_ size_t* resultCount
) XBL_NOEXCEPT;

/// <summary>
/// Get result for an XblPresenceGetPresenceForMultipleUsers call.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="presenceRecordHandles">A caller allocated array that passes back the record handles result.  
/// Each handle will need to be released with <see cref="XblPresenceRecordCloseHandle"/> when they are no longer needed.</param>
/// <param name="presenceRecordHandlesCount">Size of the handles array.  
/// Use <see cref="XblPresenceGetPresenceForMultipleUsersResultCount"/> to get the count required.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblPresenceGetPresenceForMultipleUsersResult(
    _In_ XAsyncBlock* async,
    _Out_writes_(presenceRecordHandlesCount) XblPresenceRecordHandle* presenceRecordHandles,
    _In_ size_t presenceRecordHandlesCount
) XBL_NOEXCEPT;

/// <summary>
/// Gets presence info for a specific group of users.
/// </summary>
/// <param name="xblContextHandle">Xbox live context for the local user.</param>
/// <param name="socialGroupName">The name of the group of users to get presence for.
/// This can be either "Favorites" or "People".</param>
/// <param name="socialGroupOwnerXuid">The user whose group should be targeted. If the input is null, current user will be used.</param>
/// <param name="filters">Optional filters struct to filter results.</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblPresenceGetPresenceForSocialGroupAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_z_ const char* socialGroupName,
    _In_opt_ uint64_t* socialGroupOwnerXuid,
    _In_opt_ XblPresenceQueryFilters* filters,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Get result count for an XblPresenceGetPresenceForSocialGroupAsync call.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="resultCount">Passes back the number of presence records.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblPresenceGetPresenceForSocialGroupResultCount(
    _In_ XAsyncBlock* async,
    _Out_ size_t* resultCount
) XBL_NOEXCEPT;

/// <summary>
/// Get result for an XblPresenceGetPresenceForSocialGroup call.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="presenceRecordHandles">A caller allocated array that passes back the record handles result.  
/// Each handle will need to be released with <see cref="XblPresenceRecordCloseHandle"/> when they are no longer needed.</param>
/// <param name="presenceRecordHandlesCount">Size of the handles array.  
/// Use <see cref="XblPresenceGetPresenceForSocialGroupResultCount"/> to get the count required.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblPresenceGetPresenceForSocialGroupResult(
    _In_ XAsyncBlock* async,
    _Out_ XblPresenceRecordHandle* presenceRecordHandles,
    _In_ size_t presenceRecordHandlesCount
) XBL_NOEXCEPT;

/// <summary>
/// Subscribes to device presence change notifications.
/// DEPRECATED. This API continues to work, however it will be removed in a future release.
/// Individual RTA subscription will be managed automatically by XSAPI as users are tracked with <see cref="XblPresenceTrackUsers"/>.
/// </summary>
/// <param name="xblContextHandle">Xbox live context for the local user.</param>
/// <param name="xuid">The Xbox User ID of the person of the subscription.</param>
/// <param name="subscriptionHandle">Passes back the subscription handle that will be used to unsubscribe.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI_XBL_DEPRECATED XblPresenceSubscribeToDevicePresenceChange(
    _In_ XblContextHandle xblContextHandle,
    _In_ uint64_t xuid,
    _Out_ XblRealTimeActivitySubscriptionHandle* subscriptionHandle
) XBL_NOEXCEPT;

/// <summary>
/// Unsubscribes a previously created device presence change subscription.
/// DEPRECATED. This API continues to work, however it will be removed in a future release.
/// Individual RTA subscription will be managed automatically by XSAPI as users are untracked with <see cref="XblPresenceStopTrackingUsers"/>.
/// </summary>
/// <param name="xblContextHandle">Xbox live context for the local user.</param>
/// <param name="subscriptionHandle">The RTA subscription handle created with <see cref="XblPresenceSubscribeToDevicePresenceChange"/>.  
/// This will cause the underlying object to be cleaned up,and will invalidate the subscription handle.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI_XBL_DEPRECATED XblPresenceUnsubscribeFromDevicePresenceChange(
    _In_ XblContextHandle xblContextHandle,
    _In_ XblRealTimeActivitySubscriptionHandle subscriptionHandle
) XBL_NOEXCEPT;

/// <summary>
/// Subscribes to title presence change notifications.
/// DEPRECATED. This API will be removed in a future release. Individual RTA subscription will be managed automatically by XSAPI as
/// titles are tracked with <see cref="XblPresenceTrackAdditionalTitles"/>.
/// </summary>
/// <param name="xblContextHandle">Xbox live context for the local user.</param>
/// <param name="xuid">The Xbox User ID of the person of the subscription.</param>
/// <param name="titleId">The title ID.</param>
/// <param name="subscriptionHandle">Passes back the RTA subscription handle that will be used to unsubscribe.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI_XBL_DEPRECATED XblPresenceSubscribeToTitlePresenceChange(
    _In_ XblContextHandle xblContextHandle,
    _In_ uint64_t xuid,
    _In_ uint32_t titleId,
    _Out_ XblRealTimeActivitySubscriptionHandle* subscriptionHandle
) XBL_NOEXCEPT;

/// <summary>
/// Unsubscribes a previously created title presence change subscription.
/// DEPRECATED. This API will be removed in a future release. Individual RTA subscription will be managed automatically by XSAPI as
/// titles are untracked with <see cref="XblPresenceStopTrackingAdditionalTitles"/>.
/// </summary>
/// <param name="xblContextHandle">Xbox live context for the local user.</param>
/// <param name="subscriptionHandle">Handle for the subscription created with <see cref="XblPresenceSubscribeToTitlePresenceChange"/>.  
/// This will cause the underlying object to be cleaned up,and will invalidate the subscription handle.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI_XBL_DEPRECATED XblPresenceUnsubscribeFromTitlePresenceChange(
    _In_ XblContextHandle xblContextHandle,
    _In_ XblRealTimeActivitySubscriptionHandle subscriptionHandle
) XBL_NOEXCEPT;

/// <summary>
/// Event handler for device presence change notifications.
/// </summary>
/// <param name="context">Caller context that will be passed back to the handler.</param>
/// <param name="xuid">The XboxUserID of the User whose device presence changed.</param>
/// <param name="deviceType">The associated device type.</param>
/// <param name="isUserLoggedOnDevice">Boolean for if user is logged on device.</param>
/// <returns></returns>
/// <argof><see cref="XblPresenceAddDevicePresenceChangedHandler"/></argof>
typedef void CALLBACK XblPresenceDevicePresenceChangedHandler(
    _In_opt_ void* context,
    _In_ uint64_t xuid,
    _In_ XblPresenceDeviceType deviceType,
    _In_ bool isUserLoggedOnDevice
);

/// <summary>
/// Registers an event handler for device presence change notifications. Notifications will
/// only be received for the Users configured with <see cref="XblPresenceTrackUsers"/>.
/// </summary>
/// <param name="xblContextHandle">Xbox live context for the local user.</param>
/// <param name="handler">The callback function that receives notifications.</param>
/// <param name="context">Caller context that will be passed back to the handler.</param>
/// <returns> An XblFunctionContext object that can be used to unregister the event handler.</returns>
STDAPI_(XblFunctionContext) XblPresenceAddDevicePresenceChangedHandler(
    _In_ XblContextHandle xblContextHandle,
    _In_ XblPresenceDevicePresenceChangedHandler* handler,
    _In_opt_ void* context
) XBL_NOEXCEPT;

/// <summary>
/// Unregisters an event handler for device presence change notifications.
/// </summary>
/// <param name="xblContextHandle">Xbox live context for the local user.</param>
/// <param name="token">The XblFunctionContext object that was returned when the event handler was registered.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblPresenceRemoveDevicePresenceChangedHandler(
    _In_ XblContextHandle xblContextHandle,
    _In_ XblFunctionContext token
) XBL_NOEXCEPT;

/// <summary>
/// Event handler for title presence change notifications.
/// </summary>
/// <param name="context">Caller context that will be passed back to the handler.</param>
/// <param name="xuid">The XboxUserID of the User whose title presence changed.</param>
/// <param name="titleId">The title ID.</param>
/// <param name="titleState">The title presence state for the user.</param>
/// <returns></returns>
/// <argof><see cref="XblPresenceAddTitlePresenceChangedHandler"/></argof>
typedef void CALLBACK XblPresenceTitlePresenceChangedHandler(
    _In_opt_ void* context,
    _In_ uint64_t xuid,
    _In_ uint32_t titleId,
    _In_ XblPresenceTitleState titleState
);

/// <summary>
/// Registers an event handler for title presence change notifications. Notifications will
/// only be received for the Users and Titles configured with <see cref="XblPresenceTrackUsers"/> and <see cref="XblPresenceTrackAdditionalTitles"/>
/// respectively.
/// </summary>
/// <param name="xblContextHandle">Xbox live context for the local user.</param>
/// <param name="handler">The callback function that receives notifications.</param>
/// <param name="context">Caller context that will be passed back to the handler.</param>
/// <returns> An XblFunctionContext object that can be used to unregister the event handler.</returns>
STDAPI_(XblFunctionContext) XblPresenceAddTitlePresenceChangedHandler(
    _In_ XblContextHandle xblContextHandle,
    _In_ XblPresenceTitlePresenceChangedHandler* handler,
    _In_opt_ void* context
) XBL_NOEXCEPT;

/// <summary>
/// Unregisters an event handler for title presence change notifications.
/// </summary>
/// <param name="xblContextHandle">Xbox live context for the local user.</param>
/// <param name="token">The XblFunctionContext object that was returned when the event handler was registered.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblPresenceRemoveTitlePresenceChangedHandler(
    _In_ XblContextHandle xblContextHandle,
    _In_ XblFunctionContext token
) XBL_NOEXCEPT;

/// <summary>
/// Configures the list of users for whom real-time device and title presence updates will be tracked.
/// </summary>
/// <param name="xblContextHandle">Xbox live context for the local user.</param>
/// <param name="xuids">Array of XboxUserIDs to append to the existing list of tracked Users.</param>
/// <param name="xuidsCount">Length of xuids array.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Updates will be delivered via XblPresenceDevicePresenceChangedHandlers and XblPresenceTitlePresenceChangedHandlers.
/// Note that the set of tracked users can be updated independent from the handlers.
/// </remarks>
STDAPI XblPresenceTrackUsers(
    _In_ XblContextHandle xblContextHandle,
    _In_ const uint64_t* xuids,
    _In_ size_t xuidsCount
) XBL_NOEXCEPT;

/// <summary>
/// Configures the list of users for whom real-time device and title presence updates will be tracked.
/// Presence updates for the specified Users will no longer be received.
/// </summary>
/// <param name="xblContextHandle">Xbox live context for the local user.</param>
/// <param name="xuids">Array of XboxUserIDs to remove from the list of tracked Users.</param>
/// <param name="xuidsCount">Length of xuids array.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblPresenceStopTrackingUsers(
    _In_ XblContextHandle xblContextHandle,
    _In_ const uint64_t* xuids,
    _In_ size_t xuidsCount
) XBL_NOEXCEPT;

/// <summary>
/// Configures the list of titles for which real-time title presence will be tracked. To receive title
/// presence updates for titles other than the current title, they must be added using this API.
/// </summary>
/// <param name="xblContextHandle">Xbox live context for the local user.</param>
/// <param name="titleIds">Array of title IDs to append to the existing list of tracked titles. Note that
/// the current title will be tracked by default.</param>
/// <param name="titleIdsCount">Length of the titleIds array.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Updates will be delivered via XblPresenceTitlePresenceChangedHandlers.
/// Note that the set of tracked titles can be updated independent from the handlers.
/// </remarks>
STDAPI XblPresenceTrackAdditionalTitles(
    _In_ XblContextHandle xblContextHandle,
    _In_ const uint32_t* titleIds,
    _In_ size_t titleIdsCount
) XBL_NOEXCEPT;

/// <summary>
/// Configures the list of titles for which real-time title presence will be tracked.
/// Title presence updates for the specified titles will no longer be received.
/// </summary>
/// <param name="xblContextHandle">Xbox live context for the local user.</param>
/// <param name="titleIds">Array of title IDs to remove from the list of tracked titles.</param>
/// <param name="titleIdsCount">Length of the titleIds array.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblPresenceStopTrackingAdditionalTitles(
    _In_ XblContextHandle xblContextHandle,
    _In_ const uint32_t* titleIds,
    _In_ size_t titleIdsCount
) XBL_NOEXCEPT;

}