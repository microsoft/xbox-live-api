// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "types_c.h"
#include "errors_c.h"

#if defined(__cplusplus)
extern "C" {
#endif

/// <summary>List of gaming privilege that a user can have.</summary>
typedef enum XSAPI_GAMING_PRIVILEGE
{
    /// <summary>The user can broadcast live gameplay.</summary>
    XSAPI_GAMING_PRIVILEGE_BROADCAST = 190,

    /// <summary>The user can view other user's friends list if this privilege is present.</summary>
    XSAPI_GAMING_PRIVILEGE_VIEW_FRIENDS_LIST = 197,

    /// <summary>The user can upload recorded in-game videos to the cloud if this privilege is present. Viewing GameDVRs is subject to privacy controls.</summary>
    XSAPI_GAMING_PRIVILEGE_GAME_DVR = 198,

    /// <summary>Kinect recorded content can be uploaded to the cloud for the user and made accessible to anyone if this privilege is present. Viewing other user's Kinect content is subject to a privacy setting.</summary>
    XSAPI_GAMING_PRIVILEGE_SHARE_KINECT_CONTENT = 199,

    /// <summary>The user can join a party session if this privilege is present</summary>
    XSAPI_GAMING_PRIVILEGE_MULTIPLAYER_PARTIES = 203,

    /// <summary>The user can participate in voice chat during parties and multiplayer game sessions if this privilege is present. Communicating with other users is subject to additional privacy permission checks</summary>
    XSAPI_GAMING_PRIVILEGE_COMMUNICATION_VOICE_INGAME = 205,

    /// <summary>The user can use voice communication with Skype on Xbox One if this privilege is present</summary>
    XSAPI_GAMING_PRIVILEGE_COMMUNICATION_VOICE_SKYPE = 206,

    /// <summary>The user can allocate a cloud compute cluster and manage a cloud compute cluster for a hosted game session if this privilege is present</summary>
    XSAPI_GAMING_PRIVILEGE_CLOUD_GAMING_MANAGE_SESSION = 207,

    /// <summary>The user can join a cloud compute session if this privilege is present</summary>
    XSAPI_GAMING_PRIVILEGE_CLOUD_GAMING_JOIN_SESSION = 208,

    /// <summary>The user can save games in cloud title storage if this privilege is present</summary>
    XSAPI_GAMING_PRIVILEGE_CLOUD_SAVED_GAMES = 209,

    /// <summary>The user can share content with others if this privilege is present</summary>
    XSAPI_GAMING_PRIVILEGE_SHARE_CONTENT = 211,

    /// <summary>The user can purchase, download and launch premium content available with the Xbox LIVE Gold subscription if this privilege is present</summary>
    XSAPI_GAMING_PRIVILEGE_PREMIUM_CONTENT = 214,

    /// <summary>The user can purchase and download premium subscription content and use premium subscription features when this privilege is present</summary>
    XSAPI_GAMING_PRIVILEGE_SUBSCRIPTION_CONTENT = 219,

    /// <summary>The user is allowed to share progress information on social networks when this privilege is present</summary>
    XSAPI_GAMING_PRIVILEGE_SOCIAL_NETWORK_SHARING = 220,

    /// <summary>The user can access premium video services if this privilege is present</summary>
    XSAPI_GAMING_PRIVILEGE_PREMIUM_VIDEO = 224,

    /// <summary>The user can use video communication with Skype or other providers when this privilege is present. Communicating with other users is subject to additional privacy permission checks</summary>
    XSAPI_GAMING_PRIVILEGE_VIDEO_COMMUNICATIONS = 235,

    /// <summary>The user is authorized to purchase content when this privilege is present</summary>
    XSAPI_GAMING_PRIVILEGE_PURCHASE_CONTENT = 245,

    /// <summary>The user is authorized to download and view online user created content when this privilege is present.</summary>
    XSAPI_GAMING_PRIVILEGE_USER_CREATED_CONTENT = 247,

    /// <summary>The user is authorized to view other user's profiles when this privilege is present. Viewing other user's profiles is subject to additional privacy checks</summary>
    XSAPI_GAMING_PRIVILEGE_PROFILE_VIEWING = 249,

    /// <summary>The user can use asynchronous text messaging with anyone when this privilege is present. Extra privacy permissions checks are required to determine who the user is authorized to communicate with. Communicating with other users is subject to additional privacy permission checks</summary>
    XSAPI_GAMING_PRIVILEGE_COMMUNICATIONS = 252,

    /// <summary>The user can join a multiplayer sessions for a game when this privilege is present.</summary>
    XSAPI_GAMING_PRIVILEGE_MULTIPLAYER_SESSIONS = 254,

    /// <summary>The user can follow other Xbox LIVE users and add Xbox LIVE friends when this privilege is present.</summary>
    XSAPI_GAMING_PRIVILEGE_ADD_FRIEND = 255
} XSAPI_GAMING_PRIVILEGE;

typedef void(*XSAPI_SHOW_PROFILE_CARD_UI_COMPLETION_ROUTINE)(
    _In_ XBL_RESULT_INFO result,
    _In_opt_ void* completionRoutineContext
    );

typedef void(*XSAPI_CHECK_GAMING_PRIVILEGE_COMPLETION_ROUTINE)(
    _In_ XBL_RESULT_INFO result,
    _In_ bool hasPrivilege,
    _In_opt_ void* completionRoutineContext
    );

XBL_API XBL_RESULT XBL_CALLING_CONV
TCUIShowProfileCardUI(
    _In_ PCSTR targetXboxUserId,
    _In_ XSAPI_SHOW_PROFILE_CARD_UI_COMPLETION_ROUTINE completionRoutine,
    _In_opt_ void* completionRoutineContext,
    _In_ uint64_t taskGroupId
    ) XBL_NOEXCEPT;

XBL_API XBL_RESULT XBL_CALLING_CONV
TCUICheckGamingPrivilegeSilently(
    _In_ XSAPI_GAMING_PRIVILEGE privilege,
    _In_ XSAPI_CHECK_GAMING_PRIVILEGE_COMPLETION_ROUTINE completionRoutine,
    _In_opt_ void* completionRoutineContext,
    _In_ uint64_t taskGroupId
    ) XBL_NOEXCEPT;

XBL_API XBL_RESULT XBL_CALLING_CONV
TCUICheckGamingPrivilegeWithUI(
    _In_ XSAPI_GAMING_PRIVILEGE privilege,
    _In_ PCSTR friendlyMessage,
    _In_ XSAPI_CHECK_GAMING_PRIVILEGE_COMPLETION_ROUTINE completionRoutine,
    _In_opt_ void* completionRoutineContext,
    _In_ uint64_t taskGroupId
    ) XBL_NOEXCEPT;

#if defined(__cplusplus)
} // end extern "C"
#endif // defined(__cplusplus)
