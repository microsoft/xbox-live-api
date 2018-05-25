// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "pal.h"
#include "xsapi-c/errors_c.h"

/// <summary>Enumeration values that indicate the achievement type.</summary>
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

/// <summary>Enumeration values that indicate the achievement sort order.</summary>
typedef enum XblAchievementOrderBy
{
    /// <summary>Default order does not guarantee sort order.</summary>
    XblAchievementOrderBy_DefaultOrder,

    /// <summary>Sort by title id.</summary>
    XblAchievementOrderBy_TitleId,

    /// <summary>Sort by achievement unlock time.</summary>
    XblAchievementOrderBy_UnlockTime
} XblAchievementOrderBy;

/// <summary>Enumeration values that indicate the state of a player's progress towards unlocking an achievement.</summary>
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

/// <summary>
/// Enumeration values that indicate the media asset type associated with
/// the achievement.
/// </summary>
typedef enum XblAchievementMediaAssetType
{
    /// <summary>The media asset type is unknown.</summary>
    XblAchievementMediaAssetType_Unknown,

    /// <summary>An icon media asset.</summary>
    XblAchievementMediaAssetType_Icon,

    /// <summary>An art media asset.</summary>
    XblAchievementMediaAssetType_Art
} XblAchievementMediaAssetType;

/// <summary>Enumeration values that indicate the participation type for an achievement.</summary>
typedef enum XblAchievementParticipationType
{
    /// <summary>The participation type is unknown.</summary>
    XblAchievementParticipationType_Unknown,

    /// <summary>An achievement that can be earned as an individual participant.</summary>
    XblAchievementParticipationType_Individual,

    /// <summary>An achievement that can be earned as a group participant.</summary>
    XblAchievementParticipationType_Group
} XblAchievementParticipationType;

/// <summary>Enumeration values that indicate the reward type for an achievement.</summary>
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
    UTF8CSTR name;

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
    UTF8CSTR id;

    /// <summary>
    /// A value that indicates the current progress of the player towards meeting
    /// the requirement.
    /// </summary>
    UTF8CSTR currentProgressValue;

    /// <summary>
    /// The target progress value that the player must reach in order to meet
    /// the requirement.
    /// </summary>
    UTF8CSTR targetProgressValue;
} XblAchievementRequirement;

/// <summary>
/// Represents progress details about the achievement, including requirements.
/// </summary>
typedef struct XblAchievementProgression
{
    /// <summary>
    /// The actions and conditions that are required to unlock the achievement.
    /// </summary>
    XblAchievementRequirement* requirements;

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
    UTF8CSTR name;

    /// <summary>
    /// The type of media asset.
    /// </summary>
    XblAchievementMediaAssetType mediaAssetType;

    /// <summary>
    /// The URL of the media asset.
    /// </summary>
    UTF8CSTR url;
} XblAchievementMediaAsset;

/// <summary>
/// Represents a reward that is associated with the achievement.
/// </summary>
typedef struct XblAchievementReward
{
    /// <summary>
    /// The localized reward name.
    /// </summary>
    UTF8CSTR name;

    /// <summary>
    /// The description of the reward.
    /// </summary>
    UTF8CSTR description;

    /// <summary>
    /// The title-defined reward value (data type and content varies by reward type).
    /// </summary>
    UTF8CSTR value;

    /// <summary>
    /// The reward type.
    /// </summary>
    XblAchievementRewardType rewardType;

    /// <summary>
    /// The property type of the reward value string.
    /// </summary>
    UTF8CSTR valueType;

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
    UTF8CSTR id;

    /// <summary>
    /// The ID of the service configuration set associated with the achievement.
    /// </summary>
    UTF8CSTR serviceConfigurationId;

    /// <summary>
    /// The localized achievement name.
    /// </summary>
    UTF8CSTR name;

    /// <summary>
    /// The game/app titles associated with the achievement.
    /// </summary>
    XblAchievementTitleAssociation* titleAssociations;

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
    XblAchievementProgression progression;

    /// <summary>
    /// The media assets associated with the achievement, such as image IDs.
    /// </summary>
    XblAchievementMediaAsset* mediaAssets;

    /// <summary>
    /// The size of <ref>mediaAssets</ref>.
    /// </summary>
    uint32_t mediaAssetsCount;

    /// <summary>
    /// The collection of platforms that the achievement is available on.
    /// </summary>
    UTF8CSTR* platformsAvailableOn;

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
    UTF8CSTR unlockedDescription;

    /// <summary>
    /// The description of the locked achievement.
    /// </summary>
    UTF8CSTR lockedDescription;

    /// <summary>
    /// The product_id the achievement was released with. This is a globally unique identifier that
    /// may correspond to an application, downloadable content, etc.
    /// </summary>
    UTF8CSTR productId;

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
    XblAchievementTimeWindow available;

    /// <summary>
    /// The collection of rewards that the player earns when the achievement is unlocked.
    /// </summary>
    XblAchievementReward* rewards;

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
    UTF8CSTR deepLink;

    /// <summary>
    /// A value that indicates whether or not the achievement is revoked by enforcement.
    /// </summary>
    bool isRevoked;
} XblAchievement;

/// <summary>
/// A handle to an achievement result. This handle is used by other APIs to get the achievement objects
/// and to get the next page of achievements from the service if there is is one. The handle must be closed
/// using XblAchievementsResultCloseHandle when the result is no longer needed.
/// </summary>
typedef struct xbl_achievements_result* xbl_achievements_result_handle;

/// <summary>
/// Returns an XblAchievementsResult object containing the first page of achievements
/// for a player of the specified title.
/// </summary>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="xboxUserId">The Xbox User ID of the player.</param>
/// <param name="titleId">The title ID.</param>
/// <param name="type">The achievement type to retrieve.</param>
/// <param name="unlockedOnly">Indicates whether to return unlocked achievements only.</param>
/// <param name="orderby">Controls how the list of achievements is ordered.</param>
/// <param name="skipItems">The number of achievements to skip.</param>
/// <param name="maxItems">The maximum number of achievements the result can contain.  Pass 0 to attempt
/// to retrieve all items.</param>
/// <remarks>
/// See XblAchievementsResultGetNextAsync to page in the next set of results.
///
/// This method calls V2 GET /users/xuid({xuid})/achievements
/// </remarks>
STDAPI XblAchievementsGetAchievementsForTitleIdAsync(
    _In_ AsyncBlock* async,
    _In_ xbl_context_handle xboxLiveContext,
    _In_ uint64_t xboxUserId,
    _In_ uint32_t titleId,
    _In_ XblAchievementType type,
    _In_ bool unlockedOnly,
    _In_ XblAchievementOrderBy orderBy,
    _In_ uint32_t skipItems,
    _In_ uint32_t maxItems
    ) XBL_NOEXCEPT;

/// <summary>
/// Get the result from an XblAchievementsGetAchievementsForTitleIdAsync call.
/// <summary>
/// <param name="async">The async block that was used on the asyncronous call.</param>
/// <param name="resultHandle">Acheivement result handle.</param>
STDAPI XblAchievementsGetAchievementsForTitleIdResult(
    _In_ AsyncBlock* async,
    _Out_ xbl_achievements_result_handle* result
    ) XBL_NOEXCEPT;

/// <summary>
/// Get the actual achievement objects from an xbl_achievements_result_handle.
/// The returned achievements are owned by XSAPI and will be cleaned up when the the xbl_achievements_result_handle
/// is closed.
/// <summary>
/// <param name="resultHandle">Acheivement result handle.</param>
/// <param name="achievements">Pointer to an array of XblAchievement objects.</param>
/// <param name="achievementsCount">The count of objects in the returned array.</param>
STDAPI XblAchievementsResultGetAchievements(
    _In_ xbl_achievements_result_handle resultHandle,
    _Out_ XblAchievement** achievements,
    _Out_ uint32_t* achievementsCount
    ) XBL_NOEXCEPT;

/// <summary>
/// Checks if there are more pages of achiements to retrieve from the service.
/// </summary>
/// <param name="resultHandle">Acheivement result handle.</param>
/// <param name="hasNext">Return value. True if there are more results to retrieve, false otherwise.</param>
STDAPI XblAchievementsResultHasNext(
    _In_ xbl_achievements_result_handle resultHandle,
    _Out_ bool* hasNext
    ) XBL_NOEXCEPT;

/// <summary>
/// Returns a XblAchievementsResult object that contains the next page of achievements.
/// </summary>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="resultHandle">Handle to the achievement result.</param>
/// <param name="maxItems">The maximum number of items that the result can contain. Pass 0 to attempt
/// to retrieve all items.</param>
/// <remarks>
/// This method calls V2 GET /users/xuid({xuid})/achievements.
/// </remarks>
STDAPI XblAchievementsResultGetNextAsync(
    _In_ AsyncBlock* async,
    _In_ xbl_context_handle xboxLiveContext,
    _In_ xbl_achievements_result_handle resultHandle,
    _In_ uint32_t maxItems
    ) XBL_NOEXCEPT;

/// <summary>
/// Get the result from an XblAchievementsResultGetNextAsync call.
/// <summary>
/// <param name="async">The async block that was used on the asyncronous call.</param>
/// <param name="resultHandle">
/// Returns the next achievement result handle. Note that this is a seperate handle than the one passed to the
/// XblAchievementsResultGetNextAsync API. Each result handle must be closed seperately.
/// </param>
STDAPI XblAchievementsResultGetNextResult(
    _In_ AsyncBlock* async,
    _Out_ xbl_achievements_result_handle* result
    ) XBL_NOEXCEPT;

/// <summary>
/// Allow achievement progress to be updated and achievements to be unlocked.
/// This API will work even when offline on PC and Xbox One. Offline updates will be 
/// posted by the system when connection is re-established even if the title isn't running.
/// The result of the asyncronous operation can be obtained with GetAsyncStatus.
/// </summary>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="xboxUserId">The Xbox User ID of the player.</param>
/// <param name="titleId">The title ID.</param>
/// <param name="serviceConfigurationId">The service configuration ID (SCID) for the title.</param>
/// <param name="achievementId">The achievement ID as defined by XDP or Dev Center.</param>
/// <param name="percentComplete">The completion percentage of the achievement to indicate progress.
/// Valid values are from 1 to 100. Set to 100 to unlock the achievement.
/// Progress will be set by the server to the highest value sent</param>
/// <remarks>
/// This method calls V2 POST /users/xuid({xuid})/achievements/{scid}/update
/// </remarks>
STDAPI XblAchievementsUpdateAchievementAsync(
    _In_ AsyncBlock* async,
    _In_ xbl_context_handle xboxLiveContext,
    _In_ uint64_t xboxUserId,
    _In_opt_ uint32_t* titleId,
    _In_opt_ UTF8CSTR serviceConfigurationId,
    _In_ UTF8CSTR achievementId,
    _In_ uint32_t percentComplete
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
/// This method calls V2 GET /users/xuid({xuid})/achievements/{scid}/{achievementId}.
/// </remarks>
STDAPI XblAchievementsGetAchievementAsync(
    _In_ AsyncBlock* async,
    _In_ xbl_context_handle xboxLiveContext,
    _In_ uint64_t xboxUserId,
    _In_ UTF8CSTR serviceConfigurationId,
    _In_ UTF8CSTR achievementId
    ) XBL_NOEXCEPT;

/// <summary>
/// Get the result handle from an XblAchievementsGetAchievementAsync call.
/// <summary>
/// <param name="async">The async block that was used on the asyncronous call.</param>
/// <param name="resultHandle">
/// The achievement result handle. This handle is used by other APIs to get the achievement objects
/// and to get the next page of achievements from the service if there is is one. The handle must be closed
/// using XblAchievementsResultCloseHandle when the result is no longer needed.
/// </param>
STDAPI XblAchievementsGetAchievementResult(
    _In_ AsyncBlock* async,
    _Out_ xbl_achievements_result_handle* result
    ) XBL_NOEXCEPT;

/// <summary>
/// Increments the reference count of an xbl_achievements_result_handle.
/// </summary>
/// <param name="handle">The xbl_achievements_result_handle to duplicate.</param>
/// <returns>Returns the duplicated handle.</returns>
STDAPI_(xbl_achievements_result_handle) XblAchievementsResultDuplicateHandle(
    _In_ xbl_achievements_result_handle handle
    ) XBL_NOEXCEPT;

/// <summary>
/// Decrement the reference count for an xbl_achievements_result_handle.
/// When the reference count for reaches 0, the memory associated with the achievement result will be freed.
/// </summary>
/// <param name="handle">The xbl_achievements_result_handle to close.</param>
STDAPI_(void) XblAchievementsResultCloseHandle(
    _In_ xbl_achievements_result_handle handle
    ) XBL_NOEXCEPT;