// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "pal.h"
#include "xsapi-c/errors_c.h"

#if defined(__cplusplus)
extern "C" {
#endif

/// <summary>Defines values used to indicate the device type associate with an XblSocialManagerPresenceTitleRecord.</summary>
typedef enum XblPresenceDeviceType
{
    /// <summary>Unknown device</summary>
    XblPresenceDeviceType_Unknown,

    /// <summary>Windows Phone device</summary>
    XblPresenceDeviceType_WindowsPhone,

    /// <summary>Windows Phone 7 device</summary>
    XblPresenceDeviceType_WindowsPhone7,

    /// <summary>Web device, like Xbox.com</summary>
    XblPresenceDeviceType_Web,

    /// <summary>Xbox360 device</summary>
    XblPresenceDeviceType_Xbox360,

    /// <summary>Games for Windows LIVE device</summary>
    XblPresenceDeviceType_PC,

    /// <summary>Xbox LIVE for Windows device</summary>
    XblPresenceDeviceType_Windows8,

    /// <summary>Xbox One device</summary>
    XblPresenceDeviceType_XboxOne,

    /// <summary>Windows One Core devices</summary>
    XblPresenceDeviceType_WindowsOneCore,

    /// <summary>Windows One Core Mobile devices</summary>
    XblPresenceDeviceType_WindowsOneCoreMobile
} XblPresenceDeviceType;

/// <summary>
/// Defines values used to indicate the state of the user with regard to the presence service.
/// </summary>
typedef enum XblUserPresenceState 
{
    /// <summary>The state is unknown.</summary>
    XblUserPresenceState_Unknown,

    /// <summary>User is signed in to Xbox LIVE and active in a title.</summary>
    XblUserPresenceState_Online,

    /// <summary>User is signed-in to Xbox LIVE, but inactive in all titles.</summary>
    XblUserPresenceState_Away,

    /// <summary>User is not signed in to Xbox LIVE.</summary>
    XblUserPresenceState_Offline
} XblUserPresenceState;

/// <summary>
/// Defines values used to indicate the states of the screen view of presence information.
/// </summary>
typedef enum XblPresenceTitleViewState 
{
    /// <summary>Unknown view state.</summary>
    XblPresenceTitleViewState_Unknown,

    /// <summary>The title's view is using the full screen.</summary>
    XblPresenceTitleViewState_FullScreen,

    /// <summary>The title's view is using part of the screen with another application snapped.</summary>
    XblPresenceTitleViewState_Filled,

    /// <summary>The title's view is snapped with another application using a part of the screen.</summary>
    XblPresenceTitleViewState_Snapped,

    /// <summary>The title's running in the background and is not visible.</summary>
    XblPresenceTitleViewState_Background
} XblPresenceTitleViewState;

/// <summary>
/// Defines values used to set the level of presence detail return from the service.
/// Choosing proper detail level could help the performance of the API.
/// </summary>
typedef enum XblPresenceDetailLevel 
{
    /// <summary>Default detail level.</summary>
    XblPresenceDetailLevel_Default,

    /// <summary>User detail level. User presence info only, no device, title or rich presence info.</summary>
    XblPresenceDetailLevel_User,

    /// <summary>Device detail level. User and device presence info only, no title or rich presence info.</summary>
    XblPresenceDetailLevel_Device,

    /// <summary>Title detail level. User, device and title presence info only, no rich presence info.</summary>
    XblPresenceDetailLevel_Title,

    /// <summary>All detail possible. User, device, title and rich presence info will be provided.</summary>
    XblPresenceDetailLevel_All
} XblPresenceDetailLevel;

/// <summary>Defines values used to indicate the media id types for media presence data.</summary>
typedef enum XblPresenceMediaIdType 
{
    /// <summary>Unknown media Id.</summary>
    XblPresenceMediaIdType_Unknown,

    /// <summary>Bing media Id.</summary>
    XblPresenceMediaIdType_Bing,

    /// <summary>MediaProvider media Id.</summary>
    XblPresenceMediaIdType_MediaProvider
} XblPresenceMediaIdType;

/// <summary>Defines values used to indicate the title presence state for a user.</summary>
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

#if defined(__cplusplus)
} // end extern "C"
#endif // defined(__cplusplus)