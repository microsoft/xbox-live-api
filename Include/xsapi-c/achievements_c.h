// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#if !defined(__cplusplus)
   #error C++11 required
#endif

#pragma once

extern "C"
{

/// <summary>
/// Enumeration values that indicate the achievement type.
/// </summary>
/// <memof><see cref="XblAchievement"/></memof>
/// <argof><see cref="XblAchievementsGetAchievementsForTitleIdAsync"/></argof>
enum class XblAchievementType : uint32_t
{
    /// <summary>
    /// The achievement type is unknown.
    /// </summary>
    Unknown,

    /// <summary>
    /// Gets all achievements regardless of type.
    /// </summary>
    All,

    /// <summary>
    /// A persistent achievement that may be unlocked at any time.  
    /// Persistent achievements can give Gamerscore as a reward.
    /// </summary>
    Persistent,

    /// <summary>
    /// A challenge achievement that may only be unlocked within a certain time period.  
    /// Challenge achievements can't give Gamerscore as a reward.
    /// </summary>
    Challenge
};

/// <summary>
/// Enumeration values that indicate the achievement sort order.
/// </summary>
/// <argof><see cref="XblAchievementsGetAchievementsForTitleIdAsync"/></argof>
enum class XblAchievementOrderBy : uint32_t
{
    /// <summary>
    /// Default order does not guarantee sort order.
    /// </summary>
    DefaultOrder,

    /// <summary>
    /// Sort by title id.
    /// </summary>
    TitleId,

    /// <summary>
    /// Sort by achievement unlock time.
    /// </summary>
    UnlockTime
};

/// <summary>
/// Enumeration values that indicate the state of a player's progress towards unlocking an achievement.
/// </summary>
/// <memof><see cref="XblAchievement"/></memof>
enum class XblAchievementProgressState : uint32_t
{
    /// <summary>
    /// Achievement progress is unknown.
    /// </summary>
    Unknown,

    /// <summary>
    /// Achievement has been earned.
    /// </summary>
    Achieved,

    /// <summary>
    /// Achievement progress has not been started.
    /// </summary>
    NotStarted,

    /// <summary>
    /// Achievement progress has started.
    /// </summary>
    InProgress
};

/// <summary>
/// Enumeration values that indicate the media asset type associated with the achievement.
/// </summary>
/// <memof><see cref="XblAchievementMediaAsset"/></memof>
enum class XblAchievementMediaAssetType : uint32_t
{
    /// <summary>
    /// The media asset type is unknown.
    /// </summary>
    Unknown,

    /// <summary>
    /// An icon media asset.
    /// </summary>
    Icon,

    /// <summary>
    /// An art media asset.
    /// </summary>
    Art
};

/// <summary>
/// Enumeration values that indicate the participation type for an achievement.
/// </summary>
/// <memof><see cref="XblAchievement"/></memof>
enum class XblAchievementParticipationType : uint32_t
{
    /// <summary>
    /// The participation type is unknown.
    /// </summary>
    Unknown,

    /// <summary>
    /// An achievement that can be earned as an individual participant.
    /// </summary>
    Individual,

    /// <summary>
    /// An achievement that can be earned as a group participant.
    /// </summary>
    Group
};

/// <summary>
/// Enumeration values that indicate the reward type for an achievement.
/// </summary>
/// <memof><see cref="XblAchievementReward"/></memof>
enum class XblAchievementRewardType : uint32_t
{
    /// <summary>
    /// The reward type is unknown.
    /// </summary>
    Unknown,

    /// <summary>
    /// A Gamerscore reward.
    /// </summary>
    Gamerscore,

    /// <summary>
    /// An in-app reward, defined and delivered by the title.
    /// </summary>
    InApp,

    /// <summary>
    /// A digital art reward.
    /// </summary>
    Art
};

/// <summary>
/// Enumeration values that indicate the rarity category for an achievement.
/// </summary>
enum class XblAchievementRarityCategory : uint32_t
{
    /// <summary>
    /// The rarity is incalculable (e.g. no one has played the title yet, denominator is 0).
    /// </summary>
    Unset = 0,

    /// <summary>
    /// The number of global users that have unlocked this achievement is between 0 - 10.9.
    /// </summary>
    Rare = 1,

    /// <summary>
    /// The number of global users that have unlocked this achievement is between 11.0 - 100.0.
    /// </summary>
    Common = 2
};

/// <summary>
/// Represents the association between a title and achievements.
/// </summary>
/// <memof><see cref="XblAchievement"/></memof>
typedef struct XblAchievementTitleAssociation
{
    /// <summary>
    /// The UTF-8 encoded localized name of the title.
    /// </summary>
    _Field_z_ const char* name;

    /// <summary>
    /// The title ID.
    /// </summary>
    uint32_t titleId;
} XblAchievementTitleAssociation;

/// <summary>
/// Represents requirements for unlocking the achievement.
/// </summary>
/// <memof><see cref="XblAchievementProgression"/></memof>
typedef struct XblAchievementRequirement
{
    /// <summary>
    /// The UTF-8 encoded achievement requirement ID.
    /// </summary>
    _Field_z_ const char* id;

    /// <summary>
    /// A UTF-8 encoded value that indicates the current progress of the player towards meeting the requirement.
    /// </summary>
    _Field_z_ const char* currentProgressValue;

    /// <summary>
    /// The UTF-8 encoded target progress value that the player must reach in order to meet 
    /// the requirement.
    /// </summary>
    _Field_z_ const char* targetProgressValue;
} XblAchievementRequirement;

/// <summary>
/// Represents progress details about the achievement, including requirements.
/// </summary>
/// <memof><see cref="XblAchievement"/></memof>
typedef struct XblAchievementProgression
{
    /// <summary>
    /// The actions and conditions that are required to unlock the achievement.
    /// </summary>
    XblAchievementRequirement* requirements;

    /// <summary>
    /// The size of **requirements**.
    /// </summary>
    size_t requirementsCount;

    /// <summary>
    /// The timestamp when the achievement was first unlocked.
    /// </summary>
    time_t timeUnlocked;
} XblAchievementProgression;

/// <summary>
/// Represents an interval of time during which an achievement can be unlocked.
/// </summary>
/// <remarks>
/// This class is only used when the achievement_type enumeration is set to challenge.
/// </remarks>
/// <memof><see cref="XblAchievement"/></memof>
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
/// <memof><see cref="XblAchievementReward"/></memof>
/// <memof><see cref="XblAchievement"/></memof>
typedef struct XblAchievementMediaAsset
{
    /// <summary>
    /// The UTF-8 encoded name of the media asset, such as "tile01".
    /// </summary>
    _Field_z_ const char* name;

    /// <summary>
    /// The type of media asset.
    /// </summary>
    XblAchievementMediaAssetType mediaAssetType;

    /// <summary>
    /// The UTF-8 encoded URL of the media asset.
    /// </summary>
    _Field_z_ const char* url;
} XblAchievementMediaAsset;

/// <summary>
/// Represents a reward that is associated with the achievement.
/// </summary>
/// <memof><see cref="XblAchievement"/></memof>
typedef struct XblAchievementReward
{
    /// <summary>
    /// The UTF-8 encoded localized reward name.
    /// </summary>
    _Field_z_ const char* name;

    /// <summary>
    /// The UTF-8 encoded description of the reward.
    /// </summary>
    _Field_z_ const char* description;

    /// <summary>
    /// The UTF-8 encoded title-defined reward value (data type and content varies by reward type).
    /// </summary>
    _Field_z_ const char* value;

    /// <summary>
    /// The reward type.
    /// </summary>
    XblAchievementRewardType rewardType;

    /// <summary>
    /// The UTF-8 encoded property type of the reward value string.
    /// </summary>
    _Field_z_ const char* valueType;

    /// <summary>
    /// The media asset associated with the reward.  
    /// If the reward type is gamerscore, this will be nullptr.  
    /// If the reward type is in_app, this will be a media asset.  
    /// If the reward type is art, this may be a media asset or nullptr.
    /// </summary>
    XblAchievementMediaAsset* mediaAsset;
} XblAchievementReward;

/// <summary>
/// An entry for XblAchievementProgressChangeEventArgs representing a progress update for a single achievement.
/// </summary>
typedef struct XblAchievementProgressChangeEntry
{
    /// <summary>
    /// The UTF-8 encoded achievement ID. Represents a uint.
    /// </summary>
    _Field_z_ const char* achievementId;

    /// <summary>
    /// The state of a user's progress towards the earning of the achievement.
    /// </summary>
    XblAchievementProgressState progressState;

    /// <summary>
    /// The progression object containing progress details about the achievement, 
    /// including requirements.
    /// </summary>
    XblAchievementProgression progression;
} XblAchievementProgressChangeEntry;

/// <summary>
/// Event arguments for achievement progress changes.
/// </summary>
typedef struct XblAchievementProgressChangeEventArgs
{
    /// <summary>
    /// Array of objects detailing the progress changes of each achievement that has been updated. 
    /// </summary>
    XblAchievementProgressChangeEntry* updatedAchievementEntries;

    /// <summary>
    /// The count of **updatedAchievementEntries**.
    /// </summary>
    size_t entryCount;
} XblAchievementProgressChangeEventArgs;

/// <summary>
/// Represents an achievement, a system-wide mechanism for directing and 
/// rewarding users' in-game actions consistently across all games.
/// </summary>
/// <argof><see cref="XblAchievementsResultGetAchievements"/></argof>
typedef struct XblAchievement
{
    /// <summary>
    /// The UTF-8 encoded achievement ID. Represents a uint.
    /// </summary>
    _Field_z_ const char* id;

    /// <summary>
    /// The Service Configuration ID (SCID) that is associated with the achievement. The SCID is considered case sensitive so paste it directly from the Partner Center
    /// </summary>
    _Field_z_ const char* serviceConfigurationId;

    /// <summary>
    /// The UTF-8 encoded localized achievement name.
    /// </summary>
    _Field_z_ const char* name;

    /// <summary>
    /// The game/app titles associated with the achievement.
    /// </summary>
    XblAchievementTitleAssociation* titleAssociations;

    /// <summary>
    /// The size of **titleAssociations**.
    /// </summary>
    size_t titleAssociationsCount;

    /// <summary>
    /// The state of a user's progress towards the earning of the achievement.
    /// </summary>
    XblAchievementProgressState progressState;

    /// <summary>
    /// The progression object containing progress details about the achievement, including requirements.
    /// </summary>
    XblAchievementProgression progression;

    /// <summary>
    /// The media assets associated with the achievement, such as image IDs.
    /// </summary>
    XblAchievementMediaAsset* mediaAssets;

    /// <summary>
    /// The size of **mediaAssets**.
    /// </summary>
    size_t mediaAssetsCount;

    /// <summary>
    /// The UTF-8 encoded collection of platforms that the achievement is available on.
    /// </summary>
    _Field_z_ const char** platformsAvailableOn;

    /// <summary>
    /// The size of **platformsAvailableOn**.
    /// </summary>
    size_t platformsAvailableOnCount;

    /// <summary>
    /// Whether or not the achievement is secret.
    /// </summary>
    bool isSecret;

    /// <summary>
    /// The UTF-8 encoded description of the unlocked achievement.
    /// </summary>
    _Field_z_ const char* unlockedDescription;

    /// <summary>
    /// The UTF-8 encoded description of the locked achievement.
    /// </summary>
    _Field_z_ const char* lockedDescription;

    /// <summary>
    /// The UTF-8 encoded product_id the achievement was released with.  
    /// This is a globally unique identifier that may correspond to an application, downloadable content, etc.
    /// </summary>
    _Field_z_ const char* productId;

    /// <summary>
    /// The type of achievement, such as a challenge achievement.
    /// </summary>
    XblAchievementType type;

    /// <summary>
    /// The participation type for the achievement, such as group or individual.
    /// </summary>
    XblAchievementParticipationType participationType;

    /// <summary>
    /// The time window during which the achievement is available.  Applies to Challenges.
    /// </summary>
    XblAchievementTimeWindow available;

    /// <summary>
    /// The collection of rewards that the player earns when the achievement is unlocked.
    /// </summary>
    XblAchievementReward* rewards;

    /// <summary>
    /// The size of **rewards**.
    /// </summary>
    size_t rewardsCount;

    /// <summary>
    /// The estimated time that the achievement takes to be earned.
    /// </summary>
    uint64_t estimatedUnlockTime;

    /// <summary>
    /// A UTF-8 encoded deep link for clients that enables the title to launch at a desired 
    /// starting point for the achievement.
    /// </summary>
    _Field_z_ const char* deepLink;

    /// <summary>
    /// A value that indicates whether or not the achievement is revoked by enforcement.
    /// </summary>
    bool isRevoked;
} XblAchievement;


/// <summary>
/// Represents an Achievement Unlock notification received from the notification service.
/// </summary>
typedef struct XblAchievementUnlockEvent
{    
    /// <summary>
    /// The name of the achievement in the locale of the DeviceEndpoint to which it's being sent (current char limit: 44).
    /// </summary>
    _Field_z_ const char* achievementName;

    /// <summary>
    /// The description of the achievement in the locale of the DeviceEndpoint to which it's being sent.
    /// </summary>
    _Field_z_ const char* achievementDescription;

    /// <summary>
    /// The URL to the image associated to the achievement (max length: 2048).
    /// </summary>
    _Field_z_ const char* achievementIcon;

    /// <summary>
    /// The UTF-8 encoded achievement ID.
    /// </summary>
    _Field_z_ const char* achievementId;

    /// <summary>
    /// The amount of gamerscore earned for unlocking the achievement (can be 0 - challenges cannot have gamerscore).
    /// </summary>
    uint64_t gamerscore;

    /// <summary>
    /// The base 10 ID of the title the achievement is defined for.
    /// </summary>
    uint32_t titleId;

    /// <summary>
    /// The person's Xbox user identifier.
    /// </summary>
    uint64_t xboxUserId;

    /// <summary>
    /// The deep link set on the achievement.
    /// </summary>
    _Field_z_ const char* deepLink;

    /// <summary>
    /// The ratio of the count of users who have unlocked the achievement / the total number unique users of that title expressed as a fractional value &gt;= 0.0 and &lt;= 1.0 rounded to 2 decimal places.
    /// </summary>
    float rarityPercentage;

    /// <summary>
    /// "Rare" or "Common" - where Rare achievements are those with a rarityPercentage &lt;= 9% (or 0.9) and "Common" is everything else. (This string is not localized).
    /// </summary>
    XblAchievementRarityCategory rarityCategory;

    /// <summary>
    /// The timestamp when the achievement was first unlocked for this user.
    /// </summary>
    time_t timeUnlocked;
    
} XblAchievementUnlockEvent;


/// <summary>
/// A handle to an achievement result.
/// </summary>
/// <remarks>
/// This handle is used by other APIs to get the achievement objects and to get the 
/// next page of achievements from the service if there is one.  
/// The handle must be closed using <see cref="XblAchievementsResultCloseHandle"/> 
/// when the result is no longer needed.
/// </remarks>
/// <memof><see cref="XblAchievementsResultGetAchievements"/></memof>
/// <memof><see cref="XblAchievementsResultHasNext"/></memof>
/// <memof><see cref="XblAchievementsResultGetNextAsync"/></memof>
/// <memof><see cref="XblAchievementsResultGetNextResult"/></memof>
/// <memof><see cref="XblAchievementsGetAchievementsForTitleIdResult"/></memof>
/// <memof><see cref="XblAchievementsGetAchievementResult"/></memof>
/// <memof><see cref="XblAchievementsResultDuplicateHandle"/></memof>
/// <memof><see cref="XblAchievementsResultCloseHandle"/></memof>
typedef struct XblAchievementsResult* XblAchievementsResultHandle;

/// <summary>
/// Get a list of XblAchievement objects.
/// </summary>
/// <param name="resultHandle">Achievement result handle.</param>
/// <param name="achievements">Pointer to an array of XblAchievement objects.  
/// The memory for the returned pointer will remain valid for the life of the 
/// XblAchievementsResultHandle object until it is closed.</param>
/// <param name="achievementsCount">The count of objects in the returned array.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// The returned array of XblAchievement objects is freed when all outstanding handles 
/// to the object have been closed with <see cref="XblAchievementsResultCloseHandle"/>.
/// </remarks>
STDAPI XblAchievementsResultGetAchievements(
    _In_ XblAchievementsResultHandle resultHandle,
    _Out_ const XblAchievement** achievements,
    _Out_ size_t* achievementsCount
) XBL_NOEXCEPT;

/// <summary>
/// Checks if there are more pages of achievements to retrieve from the service.
/// </summary>
/// <param name="resultHandle">Achievement result handle.</param>
/// <param name="hasNext">Passes back true if there are more results to retrieve, false otherwise.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblAchievementsResultHasNext(
    _In_ XblAchievementsResultHandle resultHandle,
    _Out_ bool* hasNext
) XBL_NOEXCEPT;

/// <summary>
/// Gets the result of next page of achievements for a player of the specified title.
/// </summary>
/// <param name="resultHandle">Handle to the achievement result.</param>
/// <param name="maxItems">The maximum number of items that the result can contain.  
/// Pass 0 to attempt to retrieve all items.</param>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// To get the result, call <see cref="XblAchievementsResultGetNextResult"/> inside the AsyncBlock callback 
/// or after the AsyncBlock is complete.
/// </remarks>
/// <rest>V2 GET /users/xuid({xuid})/achievements</rest>
STDAPI XblAchievementsResultGetNextAsync(
    _In_ XblAchievementsResultHandle resultHandle,
    _In_ uint32_t maxItems,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Get XblAchievementsResultHandle from an XblAchievementsResultGetNextAsync call.
/// </summary>
/// <param name="async">The same AsyncBlock that passed to XblAchievementsResultGetNextAsync.</param>
/// <param name="result">
/// Returns the next achievement result handle.  
/// Note that this is a separate handle than the one passed to the XblAchievementsResultGetNextAsync API.  
/// Each result handle must be closed separately.
/// </param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblAchievementsResultGetNextResult(
    _In_ XAsyncBlock* async,
    _Out_ XblAchievementsResultHandle* result
) XBL_NOEXCEPT;

/// <summary>
/// Gets the first page of achievements for a player of the specified title.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="xboxUserId">The Xbox User ID of the player.</param>
/// <param name="titleId">The title ID.</param>
/// <param name="type">The achievement type to retrieve.</param>
/// <param name="unlockedOnly">Indicates whether to return unlocked achievements only.</param>
/// <param name="orderBy">Controls how the list of achievements is ordered.</param>
/// <param name="skipItems">The number of achievements to skip.</param>
/// <param name="maxItems">The maximum number of achievements the result can contain.  
/// Pass 0 to attempt to retrieve all items.</param>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// To get the result, call <see cref="XblAchievementsGetAchievementsForTitleIdResult"/> inside the AsyncBlock callback 
/// or after the AsyncBlock is complete.
/// </remarks>
/// <rest>V2 GET /users/xuid({xuid})/achievements</rest>
STDAPI XblAchievementsGetAchievementsForTitleIdAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_ uint64_t xboxUserId,
    _In_ uint32_t titleId,
    _In_ XblAchievementType type,
    _In_ bool unlockedOnly,
    _In_ XblAchievementOrderBy orderBy,
    _In_ uint32_t skipItems,
    _In_ uint32_t maxItems,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Get XblAchievementsResultHandle from an XblAchievementsGetAchievementsForTitleIdAsync call.
/// </summary>
/// <param name="async">The same AsyncBlock that passed to XblAchievementsGetAchievementsForTitleIdAsync.</param>
/// <param name="result">Achievement result handle.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Use <see cref="XblAchievementsResultGetAchievements"/> to get the list.
/// </remarks>
STDAPI XblAchievementsGetAchievementsForTitleIdResult(
    _In_ XAsyncBlock* async,
    _Out_ XblAchievementsResultHandle* result
) XBL_NOEXCEPT;

/// <summary>
/// Allow achievement progress to be updated and achievements to be unlocked.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="xboxUserId">The Xbox User ID of the player.</param>
/// <param name="achievementId">The UTF-8 encoded achievement ID as defined by XDP or Dev Center.</param>
/// <param name="percentComplete">The completion percentage of the achievement to indicate progress.  
/// Valid values are from 1 to 100. Set to 100 to unlock the achievement.  
/// Progress will be set by the server to the highest value sent</param>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// This API will work even when offline on PC and Xbox One. Offline updates will be 
/// posted by the system when connection is re-established even if the title isn't running.  
/// The result of the asynchronous operation can be obtained by calling <see cref="XAsyncGetStatus"/> 
/// inside the AsyncBlock callback or after the AsyncBlock is complete.  
///
/// If the achievement has already been unlocked or the progress value is less than or 
/// equal to what is currently recorded on the server, then XAsyncGetStatus() inside the callback 
/// will return HTTP_E_STATUS_NOT_MODIFIED (0x80190130L).
/// </remarks>
/// <rest>V2 POST /users/xuid({xuid})/achievements/{scid}/update</rest>
STDAPI XblAchievementsUpdateAchievementAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_ uint64_t xboxUserId,
    _In_z_ const char* achievementId,
    _In_ uint32_t percentComplete,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Allow achievement progress to be updated and achievements to be unlocked.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="xboxUserId">The Xbox User ID of the player.</param>
/// <param name="titleId">The title ID.</param>
/// <param name="serviceConfigurationId">The Service Configuration ID (SCID) for the title. The SCID is considered case sensitive so paste it directly from the Partner Center.</param>
/// <param name="achievementId">The UTF-8 encoded achievement ID as defined by XDP or Dev Center.</param>
/// <param name="percentComplete">The completion percentage of the achievement to indicate progress.  
/// Valid values are from 1 to 100. Set to 100 to unlock the achievement.  
/// Progress will be set by the server to the highest value sent.</param>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// This API will work even when offline on PC and Xbox One.  
/// Offline updates will be posted by the system when connection is re-established even if the title isn't running.  
/// The result of the asynchronous operation can be obtained by calling <see cref="XAsyncGetStatus"/> 
/// inside the AsyncBlock callback or after the AsyncBlock is complete.
/// If this unexpectedly fails, ensure the SCID is correctly in the XblInitArgs as it is case-sensitive.
/// </remarks>
/// <rest>V2 POST /users/xuid({xuid})/achievements/{scid}/update</rest>
STDAPI XblAchievementsUpdateAchievementForTitleIdAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_ uint64_t xboxUserId,
    _In_ const uint32_t titleId,
    _In_z_ const char* serviceConfigurationId,
    _In_z_ const char* achievementId,
    _In_ uint32_t percentComplete,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Gets an achievement for a player with a specific achievement ID.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="xboxUserId">The Xbox User ID of the player.</param>
/// <param name="serviceConfigurationId">The Service Configuration ID (SCID) for the title. The SCID is considered case sensitive so paste it directly from the Partner Center.</param>
/// <param name="achievementId">The UTF-8 encoded unique identifier of the Achievement as defined by XDP or Dev Center.</param>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// If multiple achievements are required, use the batch API instead: <see cref="XblAchievementsGetAchievementsForTitleIdAsync"/> 
/// To get the result, call <see cref="XblAchievementsGetAchievementResult"/> inside the AsyncBlock callback 
/// or after the AsyncBlock is complete.
/// </remarks>
/// <rest>V2 GET /users/xuid({xuid})/achievements/{scid}/{achievementId}</rest>
STDAPI XblAchievementsGetAchievementAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_ uint64_t xboxUserId,
    _In_z_ const char* serviceConfigurationId,
    _In_z_ const char* achievementId,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Get the result handle from an XblAchievementsGetAchievementAsync call.
/// </summary>
/// <param name="async">The same AsyncBlock that passed to XblAchievementsGetAchievementAsync.</param>
/// <param name="result">The achievement result handle.  
/// This handle is used by other APIs to get the achievement objects 
/// and to get the next page of achievements from the service if there is one.  
/// The handle must be closed using <see cref="XblAchievementsResultCloseHandle"/> when the result is no longer needed.
/// </param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblAchievementsGetAchievementResult(
    _In_ XAsyncBlock* async,
    _Out_ XblAchievementsResultHandle* result
) XBL_NOEXCEPT;

/// <summary>
/// Duplicates a XblAchievementsResultHandle.
/// </summary>
/// <param name="handle">The XblAchievementsResultHandle to duplicate.</param>
/// <param name="duplicatedHandle">The duplicated handle.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblAchievementsResultDuplicateHandle(
    _In_ XblAchievementsResultHandle handle,
    _Out_ XblAchievementsResultHandle* duplicatedHandle
) XBL_NOEXCEPT;

/// <summary>
/// Closes the XblAchievementsResultHandle.
/// </summary>
/// <param name="handle">The XblAchievementsResultHandle to close.</param>
/// <returns></returns>
/// <remarks>
/// When all outstanding handles have been closed, the memory associated with the achievement result will be freed.
/// </remarks>
STDAPI_(void) XblAchievementsResultCloseHandle(
    _In_ XblAchievementsResultHandle handle
) XBL_NOEXCEPT;

#if HC_PLATFORM == HC_PLATFORM_WIN32 || HC_PLATFORM_IS_EXTERNAL
/// <summary>
/// Handle for Function handling achievement unlock events.
/// </summary>
/// <param name="args">An achievement unlock event.</param>
/// <param name="context">Caller context to be passed the handler.</param>
/// <returns></returns>
/// <remarks>The lifetime of the XblAchievementUnlockEvent object is limited to the callback.</remarks>
typedef void CALLBACK XblAchievementUnlockHandler(
    _In_ const XblAchievementUnlockEvent* args,
    _In_opt_ void* context
);

/// <summary>
/// Registers an event handler for achievement unlock notifications.
/// </summary>
/// <param name="xblContextHandle">Xbox live context for the local user.</param>
/// <param name="handler">The callback function that receives notifications.</param>
/// <param name="handlerContext">Caller context to be passed the handler.</param>
/// <returns>An XblFunctionContext object that can be used to unregister the event handler.</returns>
STDAPI_(XblFunctionContext) XblAchievementUnlockAddNotificationHandler(
    _In_ XblContextHandle xblContextHandle,
    _In_ XblAchievementUnlockHandler* handler,
    _In_opt_ void* handlerContext
) XBL_NOEXCEPT;

/// <summary>
/// Unregisters an event handler for achievement unlock notifications.
/// </summary>
/// <param name="xblContextHandle">Xbox live context for the local user.</param>
/// <param name="functionContext">The XblFunctionContext that was returned when the event handler was registered. </param>
/// <returns></returns>
STDAPI_(void) XblAchievementUnlockRemoveNotificationHandler(
    _In_ XblContextHandle xblContextHandle,
    _In_ XblFunctionContext functionContext
) XBL_NOEXCEPT;
#endif

/// <summary>
/// A callback invoked when a progress is made on an achievement.
/// </summary>
/// <param name="eventArgs">The arguments associated with the change in achievement state.
/// The fields of the struct are only valid during the callback.</param>
/// <param name="context">Context provided by when the handler is added.</param>
/// <returns></returns>
/// <remarks>
/// For the callback to work properly, you must be subscribed to achievement progress changes for at least one user.
/// </remarks>
typedef void CALLBACK XblAchievementsProgressChangeHandler(
    _In_ const XblAchievementProgressChangeEventArgs* eventArgs,
    _In_opt_ void* context
);

/// <summary>
/// Registers an event handler for achievement progress change notifications.
/// </summary>
/// <param name="xblContextHandle">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="handler">The callback function that receives notifications.</param>
/// <param name="handlerContext">Client context pointer to be passed back to the handler.</param>
/// <returns>A XblFunctionContext used to remove the handler.</returns>
/// <remarks>
/// Call <see cref="XblAchievementsRemoveAchievementProgressChangeHandler"/> to un-register event handler.
/// </remarks>
STDAPI_(XblFunctionContext) XblAchievementsAddAchievementProgressChangeHandler(
    _In_ XblContextHandle xblContextHandle,
    _In_ XblAchievementsProgressChangeHandler handler,
    _In_opt_ void* handlerContext
) XBL_NOEXCEPT;

/// <summary>
/// Removes an event handler for achievement progress change notifications.
/// </summary>
/// <param name="xblContextHandle">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="functionContext">XblFunctionContext for the handler to remove.</param>
/// <returns></returns>
/// <remarks>
/// <prereq/>
/// Call this API only if <see cref="XblAchievementsAddAchievementProgressChangeHandler"/> was used to register an event handler.
/// </remarks>
STDAPI XblAchievementsRemoveAchievementProgressChangeHandler(
    _In_ XblContextHandle xblContextHandle,
    _In_ XblFunctionContext functionContext
) XBL_NOEXCEPT;

}