// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "types_c.h"
#include "xsapi-c/errors_c.h"
#include "xsapi-c/system_c.h"

#if defined(__cplusplus)
extern "C" {
#endif

struct XSAPI_ACHIEVEMENT_TITLE_ASSOCIATION_IMPL;
struct XSAPI_ACHIEVEMENT_REQUIREMENT_IMPL;
struct XSAPI_ACHIEVEMENT_PROGRESSION_IMPL;
struct XSAPI_ACHIEVEMENT_MEDIA_ASSET_IMPL;
struct XSAPI_ACHIEVEMENT_REWARD_IMPL;
struct XSAPI_ACHIEVEMENT_IMPL;
struct XSAPI_ACHIEVEMENTS_RESULT_IMPL;

typedef enum XSAPI_ACHIEVEMENT_TYPE
{
    XSAPI_ACHIEVEMENT_TYPE_UNKNOWN,
    XSAPI_ACHIEVEMENT_TYPE_ALL,
    XSAPI_ACHIEVEMENT_TYPE_PERSISTENT,
    XSAPI_ACHIEVEMENT_TYPE_CHALLENGE
} XSAPI_ACHIEVEMENT_TYPE;

typedef enum XSAPI_ACIEVEMENT_ORDER_BY
{
    XSAPI_ACIEVEMENT_ORDER_BY_DEFAULT_ORDER,
    XSAPI_ACIEVEMENT_ORDER_BY_TITLE_ID,
    XSAPI_ACIEVEMENT_ORDER_BY_UNLOCK_TIME
} XSAPI_ACIEVEMENT_ORDER_BY;

typedef enum XSAPI_ACHIEVEMENT_PROGRESS_STATE
{
    XSAPI_ACHIEVEMENT_PROGRESS_STATE_UNKNOWN,
    XSAPI_ACHIEVEMENT_PROGRESS_STATE_ACHIEVED,
    XSAPI_ACHIEVEMENT_PROGRESS_STATE_NOT_STARTED,
    XSAPI_ACHIEVEMENT_PROGRESS_STATE_IN_PROGRESS
} XSAPI_ACHIEVEMENT_PROGRESS_STATE;

typedef enum XSAPI_ACHIEVEMENT_MEDIA_ASSET_TYPE
{
    XSAPI_ACHIEVEMENT_MEDIA_ASSET_TYPE_UNKNOWN,
    XSAPI_ACHIEVEMENT_MEDIA_ASSET_TYPE_ICON,
    XSAPI_ACHIEVEMENT_MEDIA_ASSET_TYPE_ART
} XSAPI_ACHIEVEMENT_MEDIA_ASSET_TYPE;

typedef enum XSAPI_ACHIEVEMENT_PARTICIPATION_TYPE
{
    XSAPI_ACHIEVEMENT_PARTICIPATION_TYPE_UNKNOWN,
    XSAPI_ACHIEVEMENT_PARTICIPATION_TYPE_INDIVIDUAL,
    XSAPI_ACHIEVEMENT_PARTICIPATION_TYPE_GROUP
} XSAPI_ACHIEVEMENT_PARTICIPATION_TYPE;

typedef enum XSAPI_ACHIEVEMENT_REWARD_TYPE
{
    XSAPI_ACHIEVEMENT_REWARD_TYPE_UNKNOWN,
    XSAPI_ACHIEVEMENT_REWARD_TYPE_GAMERSCORE,
    XSAPI_ACHIEVEMENT_REWARD_TYPE_IN_APP,
    XSAPI_ACHIEVEMENT_REWARD_TYPE_ART
} XSAPI_ACHIEVEMENT_REWARD_TYPE;

typedef struct XSAPI_ACHIEVEMENT_TITLE_ASSOCIATION
{
    PCSTR name;
    uint32_t titleId;

    XSAPI_ACHIEVEMENT_TITLE_ASSOCIATION_IMPL* pImpl;
} XSAPI_ACHIEVEMENT_TITLE_ASSOCIATION;

typedef struct XSAPI_ACHIEVEMENT_REQUIREMENT
{
    PCSTR id;
    PCSTR currentProgressValue;
    PCSTR targetProgressValue;

    XSAPI_ACHIEVEMENT_REQUIREMENT_IMPL* pImpl;
} XSAPI_ACHIEVEMENT_REQUIREMENT;

typedef struct XSAPI_ACHIEVEMENT_PROGRESSION
{
    XSAPI_ACHIEVEMENT_REQUIREMENT** requirements;
    uint32_t requirementsCount;
    time_t timeUnlocked;

    XSAPI_ACHIEVEMENT_PROGRESSION_IMPL* pImpl;
} XSAPI_ACHIEVEMENT_PROGRESSION;

typedef struct XSAPI_ACHIEVEMENT_TIME_WINDOW
{
    time_t startDate;
    time_t endDate;
} XSAPI_ACHIEVEMENT_TIME_WINDOW;

typedef struct XSAPI_ACHIEVEMENT_MEDIA_ASSET
{
    PCSTR name;
    XSAPI_ACHIEVEMENT_MEDIA_ASSET_TYPE mediaAssetType;
    PCSTR url;

    XSAPI_ACHIEVEMENT_MEDIA_ASSET_IMPL* pImpl;
} XSAPI_ACHIEVEMENT_MEDIA_ASSET;

typedef struct XSAPI_ACHIEVEMENT_REWARD
{
    PCSTR name;
    PCSTR description;
    PCSTR value;
    XSAPI_ACHIEVEMENT_REWARD_TYPE rewardType;
    PCSTR valueType;
    XSAPI_ACHIEVEMENT_MEDIA_ASSET* mediaAsset;

    XSAPI_ACHIEVEMENT_REWARD_IMPL* pImpl;
} XSAPI_ACHIEVEMENT_REWARD;

typedef struct XSAPI_ACHIEVEMENT
{
    PCSTR id;
    PCSTR serviceConfigurationId;
    PCSTR name;
    XSAPI_ACHIEVEMENT_TITLE_ASSOCIATION** titleAssociations;
    uint32_t titleAssociationsCount;
    XSAPI_ACHIEVEMENT_PROGRESS_STATE progressState;
    XSAPI_ACHIEVEMENT_PROGRESSION progression;
    XSAPI_ACHIEVEMENT_MEDIA_ASSET** mediaAssets;
    uint32_t mediaAssetsCount;
    PCSTR* platformsAvailableOn;
    uint32_t platformsAvailableOnCount;
    bool isSecret;
    PCSTR unlockedDescription;
    PCSTR lockedDescription;
    PCSTR productId;
    XSAPI_ACHIEVEMENT_TYPE type;
    XSAPI_ACHIEVEMENT_PARTICIPATION_TYPE participationType;
    XSAPI_ACHIEVEMENT_TIME_WINDOW* available;
    XSAPI_ACHIEVEMENT_REWARD** rewards;
    uint32_t rewardsCount;
    uint64_t estimatedUnlockTime;
    PCSTR deepLink;
    bool isRevoked;

    XSAPI_ACHIEVEMENT_IMPL* pImpl;
} XSAPI_ACHIEVEMENT;

typedef struct XSAPI_ACHIEVEMENTS_RESULT
{
    XSAPI_ACHIEVEMENT** items;
    uint32_t itemsCount;
    bool hasNext;

    XSAPI_ACHIEVEMENTS_RESULT_IMPL* pImpl;
} XSAPI_ACHIEVEMENTS_RESULT;

XBL_API bool XBL_CALLING_CONV
AchievementsResultHasNext(
    _In_ XSAPI_ACHIEVEMENTS_RESULT* achievementsResult
    ) XBL_NOEXCEPT;

typedef void(*XSAPI_GET_NEXT_COMPLETION_ROUTINE)(
    _In_ XBL_RESULT result,
    _In_ XSAPI_ACHIEVEMENTS_RESULT* achievementsResult,
    _In_opt_ void* context
    );

XBL_API XBL_RESULT XBL_CALLING_CONV
AchievementsResultGetNext(
    _In_ XSAPI_ACHIEVEMENTS_RESULT* achievementsResult,
    _In_ uint32_t maxItems,
    _In_ XSAPI_GET_NEXT_COMPLETION_ROUTINE completionRoutine,
    _In_opt_ void* completionRoutineContext,
    _In_ uint64_t taskGroupId
    ) XBL_NOEXCEPT;

typedef void(*XSAPI_UPDATE_ACVHIEVEMENT_COMPLETION_ROUTINE)(
    _In_ XBL_RESULT result,
    _In_opt_ void* context
    );

XBL_API XBL_RESULT XBL_CALLING_CONV
AchievementServiceUpdateAchievement(
    _In_ XBL_XBOX_LIVE_CONTEXT_HANDLE xboxLiveContext,
    _In_ PCSTR xboxUserId,
    _In_opt_ uint32_t* titleId,
    _In_opt_ PCSTR serviceConfigurationId,
    _In_ PCSTR achievementId,
    _In_ uint32_t percentComplete,
    _In_ XSAPI_UPDATE_ACVHIEVEMENT_COMPLETION_ROUTINE completionRoutine,
    _In_opt_ void* completionRoutineContext,
    _In_ uint64_t taskGroupId
    ) XBL_NOEXCEPT;

typedef void(*XSAPI_GET_ACHIEVEMENTS_FOR_TITLE_ID_COMPLETION_ROUTINE)(
    _In_ XBL_RESULT result,
    _In_ XSAPI_ACHIEVEMENTS_RESULT* achievementsResult,
    _In_opt_ void* context
    );

XBL_API XBL_RESULT XBL_CALLING_CONV
AchievementServiceGetAchievementsForTitleId(
    _In_ XBL_XBOX_LIVE_CONTEXT_HANDLE xboxLiveContext,
    _In_ PCSTR xboxUserId,
    _In_ uint32_t titleId,
    _In_ XSAPI_ACHIEVEMENT_TYPE type,
    _In_ bool unlockedOnly,
    _In_ XSAPI_ACIEVEMENT_ORDER_BY orderBy,
    _In_ uint32_t skipItems,
    _In_ uint32_t maxItems,
    _In_ XSAPI_GET_ACHIEVEMENTS_FOR_TITLE_ID_COMPLETION_ROUTINE completionRoutine,
    _In_opt_ void* completionRoutineContext,
    _In_ uint64_t taskGroupId
    ) XBL_NOEXCEPT;

typedef void(*XSAPI_GET_ACHIEVEMENT_COMPLETION_ROUTINE)(
    _In_ XBL_RESULT result,
    _In_ XSAPI_ACHIEVEMENT* achievement,
    _In_opt_ void* context
    );

XBL_API XBL_RESULT XBL_CALLING_CONV
AchievementServiceGetAchievement(
    _In_ XBL_XBOX_LIVE_CONTEXT_HANDLE xboxLiveContext,
    _In_ PCSTR xboxUserId,
    _In_ PCSTR serviceConfigurationId,
    _In_ PCSTR achievementId,
    _In_ XSAPI_GET_ACHIEVEMENT_COMPLETION_ROUTINE completionRoutine,
    _In_opt_ void* completionRoutineContext,
    _In_ uint64_t taskGroupId
    ) XBL_NOEXCEPT;

XBL_API XBL_RESULT XBL_CALLING_CONV
AchievementServiceReleaseAchievementsResult(
    _In_ XSAPI_ACHIEVEMENTS_RESULT* achievementsResult
    ) XBL_NOEXCEPT;

XBL_API XBL_RESULT XBL_CALLING_CONV
AchievementServiceReleaseAchievement(
    _In_ XSAPI_ACHIEVEMENT* achievement
    ) XBL_NOEXCEPT;

#if defined(__cplusplus)
} // end extern "C"
#endif // defined(__cplusplus)