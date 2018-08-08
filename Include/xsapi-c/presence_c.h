// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi-c/real_time_activity_c.h"

/// <summary>Defines values used to indicate the device type associate with an XblSocialManagerPresenceTitleRecord.</summary>
typedef enum XblPresenceDeviceType
{
    /// <summary>
	/// Unknown device
	/// </summary>
    XblPresenceDeviceType_Unknown,

    /// <summary>
	/// Windows Phone device
	/// </summary>
    XblPresenceDeviceType_WindowsPhone,

    /// <summary>
	/// Windows Phone 7 device
	/// </summary>
    XblPresenceDeviceType_WindowsPhone7,

    /// <summary>
	/// Web device, like Xbox.com
	/// </summary>
    XblPresenceDeviceType_Web,

    /// <summary>
	/// Xbox360 device
	/// </summary>
    XblPresenceDeviceType_Xbox360,

    /// <summary>
	/// Games for Windows LIVE device
	/// </summary>
    XblPresenceDeviceType_PC,

    /// <summary>
	/// Xbox LIVE for Windows device
	/// </summary>
    XblPresenceDeviceType_Windows8,

    /// <summary>
	/// Xbox One device
	/// </summary>
    XblPresenceDeviceType_XboxOne,

    /// <summary>
	/// Windows One Core devices
	/// </summary>
    XblPresenceDeviceType_WindowsOneCore,

    /// <summary>
	/// Windows One Core Mobile devices
	/// </summary>
    XblPresenceDeviceType_WindowsOneCoreMobile
} XblPresenceDeviceType;

/// <summary>
/// Defines values used to indicate the state of the user with regard to the presence service.
/// </summary>
typedef enum XblPresenceUserState 
{
    /// <summary>
	/// The state is unknown.
	/// </summary>
    XblPresenceUserState_Unknown,

    /// <summary>
	/// User is signed in to Xbox LIVE and active in a title.
	/// </summary>
    XblPresenceUserState_Online,

    /// <summary>
	/// User is signed-in to Xbox LIVE, but inactive in all titles.
	/// </summary>
    XblPresenceUserState_Away,

    /// <summary>
	/// User is not signed in to Xbox LIVE.
	/// </summary>
    XblPresenceUserState_Offline
} XblPresenceUserState;

/// <summary>
/// Defines values used to indicate the states of the screen view of presence information.
/// </summary>
typedef enum XblPresenceTitleViewState 
{
    /// <summary>
	/// Unknown view state.
	/// </summary>
    XblPresenceTitleViewState_Unknown,

    /// <summary>
	/// The title's view is using the full screen.
	/// </summary>
    XblPresenceTitleViewState_FullScreen,

    /// <summary>
	/// The title's view is using part of the screen with another application snapped.
	/// </summary>
    XblPresenceTitleViewState_Filled,

    /// <summary>
	/// The title's view is snapped with another application using a part of the screen.
	/// </summary>
    XblPresenceTitleViewState_Snapped,

    /// <summary>
	/// The title's running in the background and is not visible.
	/// </summary>
    XblPresenceTitleViewState_Background
} XblPresenceTitleViewState;

/// <summary>
/// Defines values used to set the level of presence detail return from the service.
/// Choosing proper detail level could help the performance of the API.
/// </summary>
typedef enum XblPresenceDetailLevel 
{
    /// <summary>
	/// Default detail level.
	/// </summary>
    XblPresenceDetailLevel_Default,

    /// <summary>
	/// User detail level. User presence info only, no device, title or rich presence info.
	/// </summary>
    XblPresenceDetailLevel_User,

    /// <summary>
	/// Device detail level. User and device presence info only, no title or rich presence info.
	/// </summary>
    XblPresenceDetailLevel_Device,

    /// <summary>
	/// Title detail level. User, device and title presence info only, no rich presence info.
	/// </summary>
    XblPresenceDetailLevel_Title,

    /// <summary>
	/// All detail possible. User, device, title and rich presence info will be provided.
	/// </summary>
    XblPresenceDetailLevel_All
} XblPresenceDetailLevel;

/// <summary>
/// Defines values used to indicate the media id types for media presence data.
/// </summary>
typedef enum XblPresenceMediaIdType 
{
    /// <summary>
	/// Unknown media Id.
	/// </summary>
    XblPresenceMediaIdType_Unknown,

    /// <summary>
	/// Bing media Id.
	/// </summary>
    XblPresenceMediaIdType_Bing,

    /// <summary>
	/// MediaProvider media Id.
	/// </summary>
    XblPresenceMediaIdType_MediaProvider
} XblPresenceMediaIdType;

/// <summary>
/// Defines values used to indicate the title presence state for a user.
/// </summary>
typedef enum XblTitlePresenceState
{
    /// <summary>
    /// Indicates this is a Unknown state.
    /// </summary>
    XblTitlePresenceState_Unknown,

    /// <summary>
    /// Indicates the user started playing the title.
    /// </summary>
    XblTitlePresenceState_Started,

    /// <summary>
    /// Indicates the user ended playing the title.
    /// </summary>
    XblTitlePresenceState_Ended
} XblTitlePresenceState;

/// <summary>
/// Represents data supporting Rich Presence features.
/// </summary>
typedef struct XblPresenceData
{
	/// <summary>
	/// ID of the service configuration containing the presence strings.
	/// </summary>
	_Field_z_ const char* serviceConfigurationId;

	/// <summary>
	/// The ID of a presence string that is defined in the service configuration.
	/// For example, PresenceId = "1" could equal "Playing {0} on {1}" in the service configuration.
	/// The service configuration might map token 0 to Maps and token 1 to MapId.
	/// </summary>
	_Field_z_ const char* presenceId;

	/// <summary>
	/// The IDs of the strings to replace the format string tokens found in the presence string.  These strings are also defined in the service configuration.
	/// The ID values in the collection map to the strings associated with the token arguments found in the PresenceId.
	/// For example let's say this vector view contained the values "4" and "1" and PresenceId = "1" equals "Playing {0} on {1}" in the service configuration.
	/// The service configuration might map Token 0 = Maps, where MapId = "4" equals "Hometown".
	/// The service configuration might map Token 1 = Difficulty, where DifficultyId = "1" equals "Casual".
	/// </summary>
	const char** presenceTokenIds;

} XblPresenceData;

typedef struct XblPresenceBroadcastRecord
{
	/// <summary>
	/// Id for this broadcast as defined by the broadcasting service.
	/// </summary>
	_Field_z_ const char* broadcastId;

	/// <summary>
	/// The GUID uniquely identifying the broadcasting session. 
	/// </summary>
	_Field_z_ const char* session;

	/// <summary>
	/// Name of the streaming provider.
	/// </summary>
	_Field_z_ const char* provider;

	/// <summary>
	/// Approximate number of current viewers. 
	/// </summary>
	uint32_t viewerCount;

	/// <summary>
	/// UTC timestamp when the broadcast was started.
	/// </summary>
	time_t startTime;
} XblPresenceBroadcastRecord;

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
	/// The UTC timestamp when the record was last updated.
	/// </summary>
	time_t lastModifiedDate;

	/// <summary>
	/// The active state for the title.
	/// </summary>
	bool isTitleActive;

	/// <summary>
	/// The formatted and localized presence string.
	/// </summary>
	_Field_z_ const char* presence;

	/// <summary>
	/// The title view state.
	/// </summary>
	XblPresenceTitleViewState presenceTitleView;

	/// <summary>
	/// The broadcast information of what the user is broadcasting. 
	/// </summary>
	XblPresenceBroadcastRecord broadcastRecord;
} XblPresenceTitleRecord;

typedef struct XblPresenceDeviceRecord
{
	/// <summary>
	/// The device type associated with this record.
	/// </summary>
	XblPresenceDeviceType deviceType;

	/// <summary>
	/// The record containing title presence data.
	/// </summary>
	XblPresenceTitleRecord* presenceTitleRecords;
} XblPresenceDeviceRecord;

typedef struct XblPresenceMediaRecord
{
	/// <summary>
	/// ID of the media used by the Bing catalog or the provider catalog. 
	/// </summary>
	_Field_z_ const char* mediaId;

	/// <summary>
	/// The ID type of the media.
	/// </summary>
	XblPresenceMediaIdType mediaIdType;

	/// <summary>
	/// Localized name of the media content.
	/// </summary>
	_Field_z_ const char* name;
} XblPresenceMediaRecord;

typedef struct XblPresenceRecord
{
	/// <summary>
	/// The Xbox user ID.
	/// </summary>
	_Field_z_ const char* xboxUserId;

	/// <summary>
	/// The user's presence state.
	/// </summary>
	XblPresenceUserState userState;

	/// <summary>
	/// Collection of PresenceDeviceRecord objects returned by a request.
	/// </summary>
	XblPresenceDeviceRecord* presenceDeviceRecords;
} XblPresenceRecord;

/// <summary> 
/// Used to identify the Xbox user, device, and log-in status presence values.
/// </summary>
typedef struct XblDevicePresenceChangeEventArgs
{
	/// <summary>
	/// The Xbox user ID.
	/// </summary>
	_Field_z_ const char* xboxUserId;

	/// <summary>
	/// The type of device.
	/// </summary>
	XblPresenceDeviceType deviceType;

	/// <summary>
	/// Value used to indicate if the Xbox user is logged onto the device.
	/// </summary>
	bool isUserLoggedOnDevice;
} XblDevicePresenceChangeEventArgs;

/// <summary>
/// Subscribes to changes to an Xbox user's presence on a device.
/// </summary>
typedef struct XblDevicePresenceChangeSubscription
{
	/// <summary>
	/// The Xbox user ID.
	/// </summary>
	_Field_z_ const char* xboxUserId;

	/// <summary>
	/// The title ID.
	/// </summary>
	uint32_t titleId;

	// TODO remove these after migrating real time activity service
	/// <summary>
	/// The state of the subscription request.
	/// </summary>
	XblRealTimeActivitySubscriptionState state;

	/// <summary>
	/// The resource uri for the request.
	/// </summary>
	_Field_z_ const char* resourceUri;

	/// <summary>
	/// The unique subscription id for the request.
	/// </summary>
	uint32_t subscriptionId;
} XblDevicePresenceChangeSubscription;

/// <summary>
/// Used to identify a Xbox user, title, and presence states that can be subscribed to.
/// </summary>
typedef struct XblTitlePresenceChangeEventArgs
{
	/// <summary> 
	/// The Xbox user ID.
	/// </summary> 
	_Field_z_ const char* xboxUserId;

	/// <summary>
	/// The title ID.
	/// </summary>
	uint32_t titleId;

	/// <summary>
	/// Object that defines possible presence states.
	/// </summary>
	XblTitlePresenceState titleState;
} XblTitlePresenceChangeEventArgs;