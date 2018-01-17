// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi-c/types_c.h"
#include "xsapi-c/errors_c.h"

#if defined(__cplusplus)
extern "C" {
#endif

struct XBL_XBOX_LIVE_CONTEXT;

/// <summary>
/// Constant values for permission IDs.
/// </summary>
static PCSTR XsapiPrivacyCommunicateUsingText = "CommunicateUsingText";
static PCSTR XsapiPrivacyCommunicateUsingVideo = "CommunicateUsingVideo";
static PCSTR XsapiPrivacyCommunicateUsingVoice = "CommunicateUsingVoice";
static PCSTR XsapiPrivacyViewTargetProfile = "ViewTargetProfile";
static PCSTR XsapiPrivacyViewTargetGameHistory = "ViewTargetGameHistory";
static PCSTR XsapiPrivacyViewTargetVideoHistory = "ViewTargetVideoHistory";
static PCSTR XsapiPrivacyViewTargetMusicHistory = "ViewTargetMusicHistory";
static PCSTR XsapiPrivacyViewTargetExerciseInfo = "ViewTargetExerciseInfo";
static PCSTR XsapiPrivacyViewTargetPresence = "ViewTargetPresence";
static PCSTR XsapiPrivacyViewTargetVideoStatus = "ViewTargetVideoStatus";
static PCSTR XsapiPrivacyViewTargetMusicStatus = "ViewTargetMusicStatus";
static PCSTR XsapiPrivacyPlayMultiplayer = "PlayMultiplayer";
static PCSTR XsapiPrivacyBroadcastWithTwitch = "BroadcastWithTwitch";
static PCSTR XsapiPrivacyViewTargetUserCreatedContent = "ViewTargetUserCreatedContent";

typedef struct XSAPI_PRIVACY_PERMISSION_DENY_REASON
{
    PCSTR reason;
    PCSTR restrictedSetting;
} XSAPI_PRIVACY_PERMISSION_DENY_REASON;

typedef struct XSAPI_PRIVACY_PERMISSION_CHECK_RESULT
{
    bool isAllowed;
    PCSTR permissionRequested;
    const XSAPI_PRIVACY_PERMISSION_DENY_REASON *denyReasons;
    uint32_t denyReasonsCount;
} XSAPI_PRIVACY_PERMISSION_CHECK_RESULT;

typedef struct XSAPI_PRIVACY_MULTIPLE_PERMISSIONS_CHECK_RESULT
{
    PCSTR xboxUserId;
    const XSAPI_PRIVACY_PERMISSION_CHECK_RESULT *items;
    uint32_t itemsCount;
} XSAPI_PRIVACY_MULTIPLE_PERMISSIONS_CHECK_RESULT;

/// <summary>
/// Callback routine called after PrivacyGetAvoidList or PrivacyGetMuteList complete.
/// </summary>
/// <remarks>
/// The 'result' fields and the 'xboxUserIds' array will only be valid until the completion routine returns.
/// </remarks>
typedef void(*XSAPI_PRIVACY_GET_USER_LIST_COMPLETION_ROUTINE)(
    _In_ XBL_RESULT result,
    _In_ PCSTR* xboxUserIds,
    _In_ uint32_t xboxUserIdsCount,
    _In_opt_ void* context
    );

/// <summary>
/// Get the list of Xbox Live Ids the calling user should avoid during multiplayer matchmaking.
/// </summary>
/// <param name="pContext">A XBL_XBOX_LIVE_CONTEXT created with XboxLiveContextCreate.</param>
/// <param name="completionRoutine">The method to be called when the async operation completes.</param>
/// <param name="completionRoutineContext">Optional context to pass back to the completion routine.</param>
/// <param name="taskGroupId">An Id that can be used to filter callbacks to specific threads.</param>
/// <returns>
/// A XBL_RESULT indicating success or failure. The payload of the completion routine will contain an
/// array of xuids which the calling user should avoid.
///</returns>
XBL_API XBL_RESULT XBL_CALLING_CONV
PrivacyGetAvoidList(
    _In_ XBL_XBOX_LIVE_CONTEXT* pContext,
    _In_ XSAPI_PRIVACY_GET_USER_LIST_COMPLETION_ROUTINE completionRoutine,
    _In_opt_ void* completionRoutineContext,
    _In_ uint64_t taskGroupId
    ) XBL_NOEXCEPT;

/// <summary>
/// Get the list of Xbox Live Ids that the calling user should not hear (mute) during multiplayer matchmaking.
/// </summary>
/// <param name="pContext">A XBL_XBOX_LIVE_CONTEXT created with XboxLiveContextCreate.</param>
/// <param name="completionRoutine">The method to be called when the async operation completes.</param>
/// <param name="completionRoutineContext">Optional context to pass back to the completion routine.</param>
/// <param name="taskGroupId">An Id that can be used to filter callbacks to specific threads.</param>
/// <returns>
/// A XBL_RESULT indicating success or failure. The payload of the completion routine will contain an
/// array of xuids which the calling user has muted.
///</returns>
XBL_API XBL_RESULT XBL_CALLING_CONV
PrivacyGetMuteList(
    _In_ XBL_XBOX_LIVE_CONTEXT* pContext,
    _In_ XSAPI_PRIVACY_GET_USER_LIST_COMPLETION_ROUTINE completionRoutine,
    _In_opt_ void* completionRoutineContext,
    _In_ uint64_t taskGroupId
    ) XBL_NOEXCEPT;

/// <summary>
/// Callback routine called when PrivacyCheckPermissionWithTargetUser completes.
/// </summary>
/// <remarks>
/// The fields of 'result' and 'payload' will only be valid until the completion routine returns.
/// </remarks>
typedef void(*XSAPI_PRIVACY_CHECK_PERMISSION_WITH_TARGET_USER_COMPLETION_ROUTINE)(
    _In_ XBL_RESULT result,
    _In_ XSAPI_PRIVACY_PERMISSION_CHECK_RESULT payload,
    _In_opt_ void* context
    );

/// <summary>
/// Check a single permission with a single target user.
/// </summary>
/// <param name="pContext">A XBL_XBOX_LIVE_CONTEXT created with XboxLiveContextCreate.</param>
/// <param name="permissionId">The ID of the permission to check.</param>
/// <param name="xboxUserId">The target user's xbox user ID for validation.</param>
/// <param name="completionRoutine">The method to be called when the async operation completes.</param>
/// <param name="completionRoutineContext">Optional context to pass back to the completion routine.</param>
/// <param name="taskGroupId">An Id that can be used to filter callbacks to specific threads.</param>
/// <returns>
/// A XBL_RESULT indicating success or failure. The payload of the completion routine will contain an
/// XSAPI_PRIVACY_PERMISSION_CHECK_RESULT indicating the users permission.
///</returns>
XBL_API XBL_RESULT XBL_CALLING_CONV
PrivacyCheckPermissionWithTargetUser(
    _In_ XBL_XBOX_LIVE_CONTEXT* pContext,
    _In_ PCSTR permissionId,
    _In_ PCSTR xboxUserId,
    _In_ XSAPI_PRIVACY_CHECK_PERMISSION_WITH_TARGET_USER_COMPLETION_ROUTINE completionRoutine,
    _In_opt_ void* completionRoutineContext,
    _In_ uint64_t taskGroupId
    ) XBL_NOEXCEPT;

/// <summary>
/// Callback routine called when PrivacyCheckPermissionWithTargetUser completes.
/// </summary>
/// <remarks>
/// The 'result' fields and the 'privacyCheckResults' array will only be valid until the completion routine returns.
/// </remarks>
typedef void(*XSAPI_PRIVACY_CHECK_PERMISSION_WITH_MULTIPLE_TARGET_USERS_COMPLETION_ROUTINE)(
    _In_ XBL_RESULT result,
    _In_ XSAPI_PRIVACY_MULTIPLE_PERMISSIONS_CHECK_RESULT* privacyCheckResults,
    _In_ uint32_t privacyCheckResultsCount,
    _In_opt_ void* context
    );

/// <summary>
/// Check multiple permissions with multiple target users.
/// </summary>
/// <param name="pContext">A XBL_XBOX_LIVE_CONTEXT created with XboxLiveContextCreate.</param>
/// <param name="permissionIds">The array of IDs of permissions to check.</param>
/// <param name="permissionIdsCount">Size of the permissionIds array.</param>
/// <param name="xboxUserIds">The array of xbox user IDs to check permission against.</param>
/// <param name="xboxUserIdsCount">Size of the xboxUserIds array.</param>
/// <param name="completionRoutine">The method to be called when the async operation completes.</param>
/// <param name="completionRoutineContext">Optional context to pass back to the completion routine.</param>
/// <param name="taskGroupId">An Id that can be used to filter callbacks to specific threads.</param>
/// <returns>
/// A XBL_RESULT indicating success or failure. The payload of the completion routine will contain an
/// XSAPI_PRIVACY_MULTIPLE_PERMISSIONS_CHECK_RESULT indicating the users' permissions.
///</returns>
XBL_API XBL_RESULT XBL_CALLING_CONV
PrivacyCheckMultiplePermissionsWithMultipleTargetUsers(
    _In_ XBL_XBOX_LIVE_CONTEXT* pContext,
    _In_ PCSTR* permissionIds,
    _In_ uint32_t permissionIdsCount,
    _In_ PCSTR* xboxUserIds,
    _In_ uint32_t xboxUserIdsCount,
    _In_ XSAPI_PRIVACY_CHECK_PERMISSION_WITH_MULTIPLE_TARGET_USERS_COMPLETION_ROUTINE completionRoutine,
    _In_opt_ void* completionRoutineContext,
    _In_ uint64_t taskGroupId
    ) XBL_NOEXCEPT;

#if defined(__cplusplus)
} // end extern "C"
#endif // defined(__cplusplus)