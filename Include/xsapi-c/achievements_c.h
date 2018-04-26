// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "pal.h"
#include "xsapi-c/errors_c.h"
#include "xsapi-c/system_c.h"

#if defined(__cplusplus)
extern "C" {
#endif
typedef enum XblAchievementType
{
    /// <summary>The achievement type is unknown.</summary>
    XblAchievementType_Unknown,

    /// <summary>Gets all achievements regardless of type.</summary>
    XblAchievementType_All,

    /// <summary>A persistent achievement that may be unlocked at any time.
    /// Persistent achievements can give Gamerscore as a reward.</summary>
    XblAchievementType_Persistent,

    /// <summary>A challenge achievement that may only be unlocked within a certain time period.
    /// Challenge achievements can't give Gamerscore as a reward.</summary>
    XblAchievementType_Challenge
} XblAchievementType;

typedef enum XblAchievementOrderBy
{
    /// <summary>Default order does not guarantee sort order.</summary>
    XblAchievementOrderBy_DefaultOrder,

    /// <summary>Sort by title id.</summary>
    XblAchievementOrderBy_TitleId,

    /// <summary>Sort by achievement unlock time.</summary>
    XblAchievementOrderBy_UnlockTime
} XblAchievementOrderBy;

typedef enum XblAchievementProgressState
{
    /// <summary>Achievement progress is unknown.</summary>
    XblAchievementProgressState_Unknown,

    /// <summary>Achievement has been earned.</summary>
    XblAchievementProgressState_Achieved,

    /// <summary>Achievement progress has not been started.</summary>
    XblAchievementProgressState_NotStarted,

    /// <summary>Achievement progress has started.</summary>
    XblAchievementProgressState_InProgress
} XblAchievementProgressState;

typedef enum XblAchievementMediaAssetType
{
    /// <summary>The media asset type is unknown.</summary>
    XblAchievementMediaAssetType_Unknown,

    /// <summary>An icon media asset.</summary>
    XblAchievementMediaAssetType_Icon,

    /// <summary>An art media asset.</summary>
    XblAchievementMediaAssetType_Art
} XblAchievementMediaAssetType;

typedef enum XblAchievementParticipationType
{
    /// <summary>The participation type is unknown.</summary>
    XblAchievementParticipationType_Unknown,

    /// <summary>An achievement that can be earned as an individual participant.</summary>
    XblAchievementParticipationType_Individual,

    /// <summary>An achievement that can be earned as a group participant.</summary>
    XblAchievementParticipationType_Group
} XblAchievementParticipationType;

typedef enum XblAchievementRewardType
{
    /// <summary>The reward type is unknown.</summary>
    XblAchievementRewardType_Unknown,

    /// <summary>A Gamerscore reward.</summary>
    XblAchievementRewardType_Gamerscore,

    /// <summary>An in-app reward, defined and delivered by the title.</summary>
    XblAchievementRewardType_InApp,

    /// <summary>A digital art reward.</summary>
    XblAchievementRewardType_Art
} XblAchievementRewardType;

/// <summary>
/// Represents the association between a title and achievements.
/// </summary>
typedef struct XblAchievementTitleAssociation
{
    /// <summary>
    /// The localized name of the title.
    /// </summary>
    PCSTR name;

    /// <summary>
    /// The title ID.
    /// </summary>
    uint32_t titleId;
} XblAchievementTitleAssociation;

/// <summary>
/// Represents requirements for unlocking the achievement.
/// </summary>
typedef struct XblAchievementRequirement
{
    /// <summary>
    /// The achievement requirement ID.
    /// </summary>
    char id[XBL_GUID_CHAR_SIZE];

    /// <summary>
    /// A value that indicates the current progress of the player towards meeting
    /// the requirement.
    /// </summary>
    PCSTR currentProgressValue;

    /// <summary>
    /// The target progress value that the player must reach in order to meet
    /// the requirement.
    /// </summary>
    PCSTR targetProgressValue;
} XblAchievementRequirement;

/// <summary>
/// Represents progress details about the achievement, including requirements.
/// </summary>
typedef struct XblAchievementProgression
{
    /// <summary>
    /// The actions and conditions that are required to unlock the achievement.
    /// </summary>
    XblAchievementRequirement** requirements;

    /// <summary>
    /// The size of <ref>requirements</ref>.
    /// </summary>
    uint32_t requirementsCount;

    /// <summary>
    /// The timestamp when the achievement was first unlocked.
    /// </summary>
    time_t timeUnlocked;
} XblAchievementProgression;

/// <summary>
/// Represents an interval of time during which an achievement can be unlocked. 
/// This class is only used when the achievement_type enumeration is set to challenge.
/// </summary>
typedef struct XblAchievementTimeWindow
{
    /// <summary>
    /// The start date and time of the achievement time window.
    /// </summary>
    time_t startDate;

    /// <summary>
    /// The end date and time of the achievement time window.
    /// </summary>
    time_t endDate;
} XblAchievementTimeWindow;

/// <summary>
/// Represents a media asset for an achievement.
/// </summary>
typedef struct XblAchievementMediaAsset
{
    /// <summary>
    /// The name of the media asset, such as "tile01".
    /// </summary>
    PCSTR name;

    /// <summary>
    /// The type of media asset.
    /// </summary>
    XblAchievementMediaAssetType mediaAssetType;

    /// <summary>
    /// The URL of the media asset.
    /// </summary>
    PCSTR url;
} XblAchievementMediaAsset;

/// <summary>
/// Represents a reward that is associated with the achievement.
/// </summary>
typedef struct XblAchievementReward
{
    /// <summary>
    /// The localized reward name.
    /// </summary>
    char name[XBL_ACHIEVEMENT_REWARD_NAME_CHAR_SIZE];

    /// <summary>
    /// The description of the reward.
    /// </summary>
    char description[XBL_ACHIEVEMENT_REWARD_DESCRIPTION_CHAR_SIZE];

    /// <summary>
    /// The title-defined reward value (data type and content varies by reward type).
    /// </summary>
    PCSTR value;

    /// <summary>
    /// The reward type.
    /// </summary>
    XblAchievementRewardType rewardType;

    /// <summary>
    /// The property type of the reward value string.
    /// </summary>
    char valueType[XBL_ACHIEVEMENT_REWARD_VALUE_TYPE_CHAR_SIZE];

    /// <summary>
    /// The media asset associated with the reward.
    /// If the reward type is gamerscore, this will be nullptr.
    /// If the reward type is in_app, this will be a media asset.
    /// If the reward type is art, this may be a media asset or nullptr.
    /// </summary>
    XblAchievementMediaAsset* mediaAsset;
} XblAchievementReward;

/// <summary>
/// Represents an achievement, a system-wide mechanism for directing and
/// rewarding users' in-game actions consistently across all games.
/// </summary>
typedef struct XblAchievement
{
    /// <summary>
    /// The achievement ID. Can be a uint or a guid.
    /// </summary>
    char id[XBL_INT_32_CHAR_SIZE];

    /// <summary>
    /// The ID of the service configuration set associated with the achievement.
    /// </summary>
    char serviceConfigurationId[XBL_GUID_CHAR_SIZE];

    /// <summary>
    /// The localized achievement name.
    /// </summary>
    char name[XBL_ACHIEVEMENT_NAME_CHAR_SIZE];

    /// <summary>
    /// The game/app titles associated with the achievement.
    /// </summary>
    XblAchievementTitleAssociation** titleAssociations;

    /// <summary>
    /// The size of <ref>titleAssociations</ref>.
    /// </summary>
    uint32_t titleAssociationsCount;

    /// <summary>
    /// The state of a user's progress towards the earning of the achievement.
    /// </summary>
    XblAchievementProgressState progressState;

    /// <summary>
    /// The progression object containing progress details about the achievement,
    /// including requirements.
    /// </summary>
    XblAchievementProgression* progression;

    /// <summary>
    /// The media assets associated with the achievement, such as image IDs.
    /// </summary>
    XblAchievementMediaAsset** mediaAssets;

    /// <summary>
    /// The size of <ref>mediaAssets</ref>.
    /// </summary>
    uint32_t mediaAssetsCount;

    /// <summary>
    /// The collection of platforms that the achievement is available on.
    /// </summary>
    PCSTR* platformsAvailableOn;

    /// <summary>
    /// The size of <ref>platformsAvailableOn</ref>.
    /// </summary>
    uint32_t platformsAvailableOnCount;

    /// <summary>
    /// Whether or not the achievement is secret.
    /// </summary>
    bool isSecret;

    /// <summary>
    /// The description of the unlocked achievement.
    /// </summary>
    char unlockedDescription[XBL_101_CHAR_SIZE];

    /// <summary>
    /// The description of the locked achievement.
    /// </summary>
    char lockedDescription[XBL_101_CHAR_SIZE];

    /// <summary>
    /// The product_id the achievement was released with. This is a globally unique identifier that
    /// may correspond to an application, downloadable content, etc.
    /// </summary>
    char productId[XBL_GUID_CHAR_SIZE];

    /// <summary>
    /// The type of achievement, such as a challenge achievement.
    /// </summary>
    XblAchievementType type;

    /// <summary>
    /// The participation type for the achievement, such as group or individual.
    /// </summary>
    XblAchievementParticipationType participationType;

    /// <summary>
    /// The time window during which the achievement is available. Applies to Challenges.
    /// </summary>
    XblAchievementTimeWindow* available;

    /// <summary>
    /// The collection of rewards that the player earns when the achievement is unlocked.
    /// </summary>
    XblAchievementReward** rewards;

    /// <summary>
    /// The size of <ref>rewards</ref>.
    /// </summary>
    uint32_t rewardsCount;

    /// <summary>
    /// The estimated time that the achievement takes to be earned.
    /// </summary>
    uint64_t estimatedUnlockTime;

    /// <summary>
    /// A deeplink for clients that enables the title to launch at a desired starting point
    /// for the achievement.
    /// </summary>
    PCSTR deepLink;

    /// <summary>
    /// A value that indicates whether or not the achievement is revoked by enforcement.
    /// </summary>
    bool isRevoked;
} XblAchievement;

/// <summary>
/// Represents a collection of Achievement class objects returned by a request.
/// </summary>
typedef struct XblAchievementsResult
{
    /// <summary>
    /// The collection of achievement objects returned by a request.
    /// </summary>
    XblAchievement** items;

    /// <summary>
    /// The size of <ref>items</ref>.
    /// </summary>
    uint32_t itemsCount;

    /// <summary>
    /// Internal
    /// </summary>
    uint64_t xboxUserId;

    /// <summary>
    /// Internal
    /// </summary>
    uint32_t* titleIds;

    /// <summary>
    /// Internal
    /// </summary>
    uint32_t titleIdsCount;

    /// <summary>
    /// Internal
    /// </summary>
    XblAchievementType type;

    /// <summary>
    /// Internal
    /// </summary>
    bool unlockedOnly;

    /// <summary>
    /// Internal
    /// </summary>
    XblAchievementOrderBy orderBy;

    /// <summary>
    /// Internal
    /// </summary>
    PCSTR continuationToken;
} XblAchievementsResult;

/// <summary>
/// Returns a boolean value that indicates if there are more pages of achievements to retrieve.
/// </summary>
/// <param name="achievementsResult">The XblAchievementsResult to check.</param>
/// <returns>True if there are more pages, otherwise false.</returns>
XBL_API bool XBL_CALLING_CONV
XblAchievementsResultHasNext(
    _In_ XblAchievementsResult* achievementsResult
    ) XBL_NOEXCEPT;

/// <summary>
/// Returns a XblAchievementsResult object that contains the next page of achievements.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="achievementsResult">The XblAchievementsResult to check.</param>
/// <param name="maxItems">The maximum number of items that the result can contain.  Pass 0 to attempt
/// to retrieve all items.</param>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <remarks>
/// Returns a concurrency::task&lt;T&gt; object that represents the state of the asynchronous operation.
///
/// This method calls V2 GET /users/xuid({xuid})/achievements.
/// </remarks>
STDAPI XblAchievementsResultGetNext(
    _In_ xbl_context_handle xboxLiveContext,
    _In_ XblAchievementsResult* achievementsResult,
    _In_ uint32_t maxItems,
    _In_ AsyncBlock* async
    ) XBL_NOEXCEPT;

/// <summary>
/// Allow achievement progress to be updated and achievements to be unlocked.
/// This API will work even when offline. On PC and Xbox One, updates will be 
/// posted by the system when connection is re-established even if the title isn't running.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="xboxUserId">The Xbox User ID of the player.</param>
/// <param name="achievementId">The achievement ID as defined by XDP or Dev Center.</param>
/// <param name="percentComplete">The completion percentage of the achievement to indicate progress.
/// Valid values are from 1 to 100. Set to 100 to unlock the achievement.
/// Progress will be set by the server to the highest value sent</param>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <remarks>
/// Returns a task&lt;T&gt; object that represents the state of the asynchronous operation.
///
/// This method calls V2 POST /users/xuid({xuid})/achievements/{scid}/update
/// </remarks>
STDAPI XblAchievementServiceUpdateAchievement(
    _In_ xbl_context_handle xboxLiveContext,
    _In_ uint64_t xboxUserId,
    _In_opt_ uint32_t* titleId,
    _In_opt_ PCSTR serviceConfigurationId,
    _In_ PCSTR achievementId,
    _In_ uint32_t percentComplete,
    _In_ AsyncBlock* async
    ) XBL_NOEXCEPT;

/// <summary>
/// Returns an XblAchievementsResult object containing the first page of achievements
/// for a player of the specified title.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="xboxUserId">The Xbox User ID of the player.</param>
/// <param name="titleId">The title ID.</param>
/// <param name="type">The achievement type to retrieve.</param>
/// <param name="unlockedOnly">Indicates whether to return unlocked achievements only.</param>
/// <param name="orderby">Controls how the list of achievements is ordered.</param>
/// <param name="skipItems">The number of achievements to skip.</param>
/// <param name="maxItems">The maximum number of achievements the result can contain.  Pass 0 to attempt
/// to retrieve all items.</param>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <remarks>
/// Returns a task&lt;T&gt; object that represents the state of the asynchronous operation.
///
/// See achievements_result:get_next to page in the next set of results.
///
/// This method calls V2 GET /users/xuid({xuid})/achievements
/// </remarks>
STDAPI XblAchievementServiceGetAchievementsForTitleId(
    _In_ xbl_context_handle xboxLiveContext,
    _In_ uint64_t xboxUserId,
    _In_ uint32_t titleId,
    _In_ XblAchievementType type,
    _In_ bool unlockedOnly,
    _In_ XblAchievementOrderBy orderBy,
    _In_ uint32_t skipItems,
    _In_ uint32_t maxItems,
    _In_ AsyncBlock* async
    ) XBL_NOEXCEPT;

/// <summary>
/// Returns a XblAchievement object for a specified player.
/// If the achievement does not exist, the method returns E_FAIL.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="xboxUserId">The Xbox User ID of the player.</param>
/// <param name="serviceConfigurationId">The service configuration ID (SCID) for the title.</param>
/// <param name="achievementId">The unique identifier of the Achievement as defined by XDP or Dev Center.</param>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <remarks>
/// Returns a task&lt;T&gt; object that represents the state of the asynchronous operation.
///
/// This method calls V2 GET /users/xuid({xuid})/achievements/{scid}/{achievementId}.
/// </remarks>
STDAPI XblAchievementServiceGetAchievement(
    _In_ xbl_context_handle xboxLiveContext,
    _In_ uint64_t xboxUserId,
    _In_ PCSTR serviceConfigurationId,
    _In_ PCSTR achievementId,
    _In_ AsyncBlock* async
    ) XBL_NOEXCEPT;

/// <summary>
/// Get the size in bytes of the result from XblAchievementServiceGetAchievement.
/// </summary>
/// <param name="async">The async block that was used on the asyncronous call.</param>
/// <param name="resultSize">The result size.</param>
STDAPI XblGetAchievementSize(
    _In_ AsyncBlock* async,
    _Out_ size_t* resultSize
) XBL_NOEXCEPT;

/// <summary>
/// Get the result from any of XblAchievementServiceGetAchievement. 
/// The required buffer size should first be obtained XblGetAchievementSize.
/// <summary>
/// <param name="async">The async block that was used on the asyncronous call.</param>
/// <param name="resultSize">The size of the provided buffer.</param>
/// <param name="result">The buffer to be written to.</param>
/// <param name="bufferUser">The actual number of bytes written to the buffer.</param>
STDAPI XblGetAchievement(
    _In_ AsyncBlock* async,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_opt_(bufferSize, *bufferUsed) XblAchievement* buffer,
    _Out_opt_ size_t* bufferUsed
) XBL_NOEXCEPT;

/// <summary>
/// Get the size in bytes of the result from XblAchievementServiceGetAchievementsForTitleId
/// and XblAchievementsResultGetNext.
/// </summary>
/// <param name="async">The async block that was used on the asyncronous call.</param>
/// <param name="resultSize">The result size.</param>
STDAPI XblGetAchievementsResultSize(
    _In_ AsyncBlock* async,
    _Out_ size_t* resultSize
) XBL_NOEXCEPT;

/// <summary>
/// Get the result from any of XblAchievementServiceGetAchievementsForTitleId
/// and XblAchievementsResultGetNext. 
/// The required buffer size should first be obtained XblGetAchievementSize.
/// <summary>
/// <param name="async">The async block that was used on the asyncronous call.</param>
/// <param name="resultSize">The size of the provided buffer.</param>
/// <param name="result">The buffer to be written to.</param>
/// <param name="bufferUser">The actual number of bytes written to the buffer.</param>
STDAPI XblGetAchievementsResult(
    _In_ AsyncBlock* async,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_opt_(bufferSize, *bufferUsed) XblAchievementsResult* buffer,
    _Out_opt_ size_t* bufferUsed
) XBL_NOEXCEPT;
#if defined(__cplusplus)
} // end extern "C"
#endif // defined(__cplusplus)