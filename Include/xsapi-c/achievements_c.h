// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "pal.h"
#include "xsapi-c/errors_c.h"
#include "xsapi-c/system_c.h"

#if defined(__cplusplus)
extern "C" {
#endif

struct XBL_ACHIEVEMENT_TITLE_ASSOCIATION_IMPL;
struct XBL_ACHIEVEMENT_REQUIREMENT_IMPL;
struct XBL_ACHIEVEMENT_PROGRESSION_IMPL;
struct XBL_ACHIEVEMENT_MEDIA_ASSET_IMPL;
struct XBL_ACHIEVEMENT_REWARD_IMPL;
struct XBL_ACHIEVEMENT_IMPL;
struct XBL_ACHIEVEMENTS_RESULT_IMPL;

typedef enum XBL_ACHIEVEMENT_TYPE
{
    XBL_ACHIEVEMENT_TYPE_UNKNOWN,
    XBL_ACHIEVEMENT_TYPE_ALL,
    XBL_ACHIEVEMENT_TYPE_PERSISTENT,
    XBL_ACHIEVEMENT_TYPE_CHALLENGE
} XBL_ACHIEVEMENT_TYPE;

typedef enum XBL_ACIEVEMENT_ORDER_BY
{
    XBL_ACIEVEMENT_ORDER_BY_DEFAULT_ORDER,
    XBL_ACIEVEMENT_ORDER_BY_TITLE_ID,
    XBL_ACIEVEMENT_ORDER_BY_UNLOCK_TIME
} XBL_ACIEVEMENT_ORDER_BY;

typedef enum XBL_ACHIEVEMENT_PROGRESS_STATE
{
    XBL_ACHIEVEMENT_PROGRESS_STATE_UNKNOWN,
    XBL_ACHIEVEMENT_PROGRESS_STATE_ACHIEVED,
    XBL_ACHIEVEMENT_PROGRESS_STATE_NOT_STARTED,
    XBL_ACHIEVEMENT_PROGRESS_STATE_IN_PROGRESS
} XBL_ACHIEVEMENT_PROGRESS_STATE;

typedef enum XBL_ACHIEVEMENT_MEDIA_ASSET_TYPE
{
    XBL_ACHIEVEMENT_MEDIA_ASSET_TYPE_UNKNOWN,
    XBL_ACHIEVEMENT_MEDIA_ASSET_TYPE_ICON,
    XBL_ACHIEVEMENT_MEDIA_ASSET_TYPE_ART
} XBL_ACHIEVEMENT_MEDIA_ASSET_TYPE;

typedef enum XBL_ACHIEVEMENT_PARTICIPATION_TYPE
{
    XBL_ACHIEVEMENT_PARTICIPATION_TYPE_UNKNOWN,
    XBL_ACHIEVEMENT_PARTICIPATION_TYPE_INDIVIDUAL,
    XBL_ACHIEVEMENT_PARTICIPATION_TYPE_GROUP
} XBL_ACHIEVEMENT_PARTICIPATION_TYPE;

typedef enum XBL_ACHIEVEMENT_REWARD_TYPE
{
    XBL_ACHIEVEMENT_REWARD_TYPE_UNKNOWN,
    XBL_ACHIEVEMENT_REWARD_TYPE_GAMERSCORE,
    XBL_ACHIEVEMENT_REWARD_TYPE_IN_APP,
    XBL_ACHIEVEMENT_REWARD_TYPE_ART
} XBL_ACHIEVEMENT_REWARD_TYPE;

typedef struct XBL_ACHIEVEMENT_TITLE_ASSOCIATION
{
    PCSTR name;
    uint32_t titleId;

    XBL_ACHIEVEMENT_TITLE_ASSOCIATION_IMPL* pImpl;
} XBL_ACHIEVEMENT_TITLE_ASSOCIATION;

typedef struct XBL_ACHIEVEMENT_REQUIREMENT
{
    PCSTR id;
    PCSTR currentProgressValue;
    PCSTR targetProgressValue;

    XBL_ACHIEVEMENT_REQUIREMENT_IMPL* pImpl;
} XBL_ACHIEVEMENT_REQUIREMENT;

typedef struct XBL_ACHIEVEMENT_PROGRESSION
{
    XBL_ACHIEVEMENT_REQUIREMENT** requirements;
    uint32_t requirementsCount;
    time_t timeUnlocked;

    XBL_ACHIEVEMENT_PROGRESSION_IMPL* pImpl;
} XBL_ACHIEVEMENT_PROGRESSION;

typedef struct XBL_ACHIEVEMENT_TIME_WINDOW
{
    time_t startDate;
    time_t endDate;
} XBL_ACHIEVEMENT_TIME_WINDOW;

typedef struct XBL_ACHIEVEMENT_MEDIA_ASSET
{
    PCSTR name;
    XBL_ACHIEVEMENT_MEDIA_ASSET_TYPE mediaAssetType;
    PCSTR url;

    XBL_ACHIEVEMENT_MEDIA_ASSET_IMPL* pImpl;
} XBL_ACHIEVEMENT_MEDIA_ASSET;

typedef struct XBL_ACHIEVEMENT_REWARD
{
    PCSTR name;
    PCSTR description;
    PCSTR value;
    XBL_ACHIEVEMENT_REWARD_TYPE rewardType;
    PCSTR valueType;
    XBL_ACHIEVEMENT_MEDIA_ASSET* mediaAsset;

    XBL_ACHIEVEMENT_REWARD_IMPL* pImpl;
} XBL_ACHIEVEMENT_REWARD;

typedef struct XBL_ACHIEVEMENT
{
    PCSTR id;
    PCSTR serviceConfigurationId;
    PCSTR name;
    XBL_ACHIEVEMENT_TITLE_ASSOCIATION** titleAssociations;
    uint32_t titleAssociationsCount;
    XBL_ACHIEVEMENT_PROGRESS_STATE progressState;
    XBL_ACHIEVEMENT_PROGRESSION* progression;
    XBL_ACHIEVEMENT_MEDIA_ASSET** mediaAssets;
    uint32_t mediaAssetsCount;
    PCSTR* platformsAvailableOn;
    uint32_t platformsAvailableOnCount;
    bool isSecret;
    PCSTR unlockedDescription;
    PCSTR lockedDescription;
    PCSTR productId;
    XBL_ACHIEVEMENT_TYPE type;
    XBL_ACHIEVEMENT_PARTICIPATION_TYPE participationType;
    XBL_ACHIEVEMENT_TIME_WINDOW* available;
    XBL_ACHIEVEMENT_REWARD** rewards;
    uint32_t rewardsCount;
    uint64_t estimatedUnlockTime;
    PCSTR deepLink;
    bool isRevoked;

    XBL_ACHIEVEMENT_IMPL* pImpl;
} XBL_ACHIEVEMENT;

typedef struct XBL_ACHIEVEMENTS_RESULT
{
    XBL_ACHIEVEMENT** items;
    uint32_t itemsCount;

    XBL_ACHIEVEMENTS_RESULT_IMPL* pImpl;
} XBL_ACHIEVEMENTS_RESULT;

XBL_API bool XBL_CALLING_CONV
XblAchievementsResultHasNext(
    _In_ XBL_ACHIEVEMENTS_RESULT* achievementsResult
    ) XBL_NOEXCEPT;

typedef void(*XBL_GET_NEXT_COMPLETION_ROUTINE)(
    _In_ XBL_RESULT result,
    _In_ XBL_ACHIEVEMENTS_RESULT* achievementsResult,
    _In_opt_ void* context
    );

XBL_API XBL_RESULT XBL_CALLING_CONV
XblAchievementsResultGetNext(
    _In_ XBL_ACHIEVEMENTS_RESULT* achievementsResult,
    _In_ uint32_t maxItems,
    _In_ XBL_ASYNC_QUEUE queue,
    _In_opt_ void* completionRoutineContext,
    _In_ XBL_GET_NEXT_COMPLETION_ROUTINE completionRoutine
    ) XBL_NOEXCEPT;

typedef void(*XBL_UPDATE_ACVHIEVEMENT_COMPLETION_ROUTINE)(
    _In_ XBL_RESULT result,
    _In_opt_ void* context
    );

XBL_API XBL_RESULT XBL_CALLING_CONV
XblAchievementServiceUpdateAchievement(
    _In_ xbl_context_handle xboxLiveContext,
    _In_ PCSTR xboxUserId,
    _In_opt_ uint32_t* titleId,
    _In_opt_ PCSTR serviceConfigurationId,
    _In_ PCSTR achievementId,
    _In_ uint32_t percentComplete,
    _In_ XBL_ASYNC_QUEUE queue,
    _In_opt_ void* completionRoutineContext,
    _In_ XBL_UPDATE_ACVHIEVEMENT_COMPLETION_ROUTINE completionRoutine
    ) XBL_NOEXCEPT;

typedef void(*XBL_GET_ACHIEVEMENTS_FOR_TITLE_ID_COMPLETION_ROUTINE)(
    _In_ XBL_RESULT result,
    _In_ XBL_ACHIEVEMENTS_RESULT* achievementsResult,
    _In_opt_ void* context
    );

XBL_API XBL_RESULT XBL_CALLING_CONV
XblAchievementServiceGetAchievementsForTitleId(
    _In_ xbl_context_handle xboxLiveContext,
    _In_ PCSTR xboxUserId,
    _In_ uint32_t titleId,
    _In_ XBL_ACHIEVEMENT_TYPE type,
    _In_ bool unlockedOnly,
    _In_ XBL_ACIEVEMENT_ORDER_BY orderBy,
    _In_ uint32_t skipItems,
    _In_ uint32_t maxItems,
    _In_ XBL_ASYNC_QUEUE queue,
    _In_opt_ void* completionRoutineContext,
    _In_ XBL_GET_ACHIEVEMENTS_FOR_TITLE_ID_COMPLETION_ROUTINE completionRoutine
    ) XBL_NOEXCEPT;

typedef void(*XBL_GET_ACHIEVEMENT_COMPLETION_ROUTINE)(
    _In_ XBL_RESULT result,
    _In_ XBL_ACHIEVEMENT* achievement,
    _In_opt_ void* context
    );

XBL_API XBL_RESULT XBL_CALLING_CONV
XblAchievementServiceGetAchievement(
    _In_ xbl_context_handle xboxLiveContext,
    _In_ PCSTR xboxUserId,
    _In_ PCSTR serviceConfigurationId,
    _In_ PCSTR achievementId,
    _In_ XBL_ASYNC_QUEUE queue,
    _In_opt_ void* completionRoutineContext,
    _In_ XBL_GET_ACHIEVEMENT_COMPLETION_ROUTINE completionRoutine
    ) XBL_NOEXCEPT;

XBL_API XBL_ACHIEVEMENTS_RESULT* XBL_CALLING_CONV
XblCopyAchievementsResult(
    _In_ CONST XBL_ACHIEVEMENTS_RESULT *source,
    _In_ void *buffer,
    _Inout_ uint64_t *cbBuffer
    ) XBL_NOEXCEPT;

XBL_API XBL_ACHIEVEMENT* XBL_CALLING_CONV
XblCopyAchievement(
    _In_ CONST XBL_ACHIEVEMENT *source,
    _In_ void *buffer,
    _Inout_ uint64_t *cbBuffer
    ) XBL_NOEXCEPT;

XBL_API XBL_RESULT XBL_CALLING_CONV
XblAchievementServiceReleaseAchievementsResult(
    _In_ XBL_ACHIEVEMENTS_RESULT* achievementsResult
    ) XBL_NOEXCEPT;

XBL_API XBL_RESULT XBL_CALLING_CONV
XblAchievementServiceReleaseAchievement(
    _In_ XBL_ACHIEVEMENT* achievement
    ) XBL_NOEXCEPT;

#if defined(__cplusplus)
} // end extern "C"
#endif // defined(__cplusplus)