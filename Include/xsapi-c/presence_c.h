// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "types_c.h"
#include "xsapi-c/errors_c.h"

#if defined(__cplusplus)
extern "C" {
#endif

/// <summary>Defines values used to indicate the device type associate with an XBL_SOCIAL_MANAGER_PRESENCE_TITLE_RECORD.</summary>
typedef enum XBL_PRESENCE_DEVICE_TYPE
{
    /// <summary>Unknown device</summary>
    XBL_PRESENCE_DEVICE_TYPE_UNKNOWN,

    /// <summary>Windows Phone device</summary>
    XBL_PRESENCE_DEVICE_TYPE_WINDOWS_PHONE,

    /// <summary>Windows Phone 7 device</summary>
    XBL_PRESENCE_DEVICE_TYPE_WINDOWS_PHONE_7,

    /// <summary>Web device, like Xbox.com</summary>
    XBL_PRESENCE_DEVICE_TYPE_WEB,

    /// <summary>Xbox360 device</summary>
    XBL_PRESENCE_DEVICE_TYPE_XBOX_360,

    /// <summary>Games for Windows LIVE device</summary>
    XBL_PRESENCE_DEVICE_TYPE_PC,

    /// <summary>Xbox LIVE for Windows device</summary>
    XBL_PRESENCE_DEVICE_TYPE_WINDOWS_8,

    /// <summary>Xbox One device</summary>
    XBL_PRESENCE_DEVICE_TYPE_XBOX_ONE,

    /// <summary>Windows One Core devices</summary>
    XBL_PRESENCE_DEVICE_TYPE_WINDOWS_ONE_CORE,

    /// <summary>Windows One Core Mobile devices</summary>
    XBL_PRESENCE_DEVICE_TYPE_WINDOWS_ONE_CORE_MOBILE
} XBL_PRESENCE_DEVICE_TYPE;

/// <summary>
/// Defines values used to indicate the state of the user with regard to the presence service.
/// </summary>
typedef enum XBL_USER_PRESENCE_STATE 
{
    /// <summary>The state is unknown.</summary>
    XBL_USER_PRESENCE_STATE_UNKNOWN,

    /// <summary>User is signed in to Xbox LIVE and active in a title.</summary>
    XBL_USER_PRESENCE_STATE_ONLINE,

    /// <summary>User is signed-in to Xbox LIVE, but inactive in all titles.</summary>
    XBL_USER_PRESENCE_STATE_AWAY,

    /// <summary>User is not signed in to Xbox LIVE.</summary>
    XBL_USER_PRESENCE_STATE_OFFLINE
} XBL_USER_PRESENCE_STATE;

/// <summary>
/// Defines values used to indicate the states of the screen view of presence information.
/// </summary>
typedef enum XBL_PRESENCE_TITLE_VIEW_STATE 
{
    /// <summary>Unknown view state.</summary>
    XBL_PRESENCE_TITLE_VIEW_STATE_UNKNOWN,

    /// <summary>The title's view is using the full screen.</summary>
    XBL_PRESENCE_TITLE_VIEW_STATE_FULL_SCREEN,

    /// <summary>The title's view is using part of the screen with another application snapped.</summary>
    XBL_PRESENCE_TITLE_VIEW_STATE_FILLED,

    /// <summary>The title's view is snapped with another application using a part of the screen.</summary>
    XBL_PRESENCE_TITLE_VIEW_STATE_SNAPPED,

    /// <summary>The title's running in the background and is not visible.</summary>
    XBL_PRESENCE_TITLE_VIEW_STATE_BACKGROUND
} XBL_PRESENCE_TITLE_VIEW_STATE;

/// <summary> Defines values used to set the level of presence detail return from the service. Choosing proper detail level could help the performance of the API.</summary>
typedef enum XBL_PRESENCE_DETAIL_LEVEL 
{
    /// <summary>Default detail level.</summary>
    XBL_PRESENCE_DETAIL_LEVEL_DEFAULT,

    /// <summary>User detail level. User presence info only, no device, title or rich presence info.</summary>
    XBL_PRESENCE_DETAIL_LEVEL_USER,

    /// <summary>Device detail level. User and device presence info only, no title or rich presence info.</summary>
    XBL_PRESENCE_DETAIL_LEVEL_DEVICE,

    /// <summary>Title detail level. User, device and title presence info only, no rich presence info.</summary>
    XBL_PRESENCE_DETAIL_LEVEL_TITLE,

    /// <summary>All detail possible. User, device, title and rich presence info will be provided.</summary>
    XBL_PRESENCE_DETAIL_LEVEL_ALL
} XBL_PRESENCE_DETAIL_LEVEL;

/// <summary>Defines values used to indicate the media id types for media presence data.</summary>
typedef enum XBL_PRESENCE_MEDIA_ID_TYPE 
{
    /// <summary>Unknown media Id.</summary>
    XBL_PRESENCE_MEDIA_ID_TYPE_UNKNOWN,

    /// <summary>Bing media Id.</summary>
    XBL_PRESENCE_MEDIA_ID_TYPE_BING,

    /// <summary>MediaProvider media Id.</summary>
    XBL_PRESENCE_MEDIA_ID_TYPE_MEDIA_PROVIDER
} XBL_PRESENCE_MEDIA_ID_TYPE;

/// <summary>Defines values used to indicate the title presence state for a user.</summary>
typedef enum XBL_TITLE_PRESENCE_STATE 
{
    /// <summary>
    /// Indicates this is a Unknown state.
    /// </summary>
    XBL_TITLE_PRESENCE_STATE_UNKNOWN,

    /// <summary>
    /// Indicates the user started playing the title.
    /// </summary>
    XBL_TITLE_PRESENCE_STATE_STARTED,

    /// <summary>
    /// Indicates the user ended playing the title.
    /// </summary>
    XBL_TITLE_PRESENCE_STATE_ENDED
} XBL_TITLE_PRESENCE_STATE;

#if defined(__cplusplus)
} // end extern "C"
#endif // defined(__cplusplus)