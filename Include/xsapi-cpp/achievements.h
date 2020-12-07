// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xbox_live_app_config.h"
#include "xsapi-cpp/system.h"
#include "xsapi-c/achievements_c.h"

namespace xbox { namespace services {
    class xbox_live_context;

    /// <summary>
    /// Contains classes and enumerations that let you retrieve
    /// information about player achievements from Xbox Live.
    /// </summary>
    namespace achievements {

    class achievement_service_internal;

/// <summary>Enumeration values that indicate the achievement type.</summary>
enum class achievement_type
{
    /// <summary>The achievement type is unknown.</summary>
    unknown,

    /// <summary>Used as a request input parameter.
    /// All means to get all achievements regardless of type.</summary>
    all,

    /// <summary>A persistent achievement that may be unlocked at any time.
    /// Persistent achievements can give Gamerscore as a reward.</summary>
    persistent,

    /// <summary>A challenge achievement that may only be unlocked within a certain time period.
    /// Challenge achievements can't give Gamerscore as a reward.</summary>
    challenge
};

/// <summary>Enumeration values that indicate the achievement sort order.</summary>
enum class achievement_order_by
{
    /// <summary>Default order does not guarantee sort order.</summary>
    default_order,

    /// <summary>Sort by title id.</summary>
    title_id,

    /// <summary>Sort by achievement unlock time.</summary>
    unlock_time
};

/// <summary>Enumeration values that indicate the state of a player's progress towards unlocking an achievement.</summary>
enum class achievement_progress_state
{
    /// <summary>Achievement progress is unknown.</summary>
    unknown,

    /// <summary>Achievement has been earned.</summary>
    achieved,

    /// <summary>Achievement progress has not been started.</summary>
    not_started,

    /// <summary>Achievement progress has started.</summary>
    in_progress
};

/// <summary>
/// Enumeration values that indicate the media asset type associated with
/// the achievement.
/// </summary>
enum class achievement_media_asset_type
{
    /// <summary>The media asset type is unknown.</summary>
    unknown,

    /// <summary>An icon media asset.</summary>
    icon,

    /// <summary>An art media asset.</summary>
    art
};

/// <summary>Enumeration values that indicate the participation type for an achievement.</summary>
enum class achievement_participation_type
{
    /// <summary>The participation type is unknown.</summary>
    unknown,

    /// <summary>An achievement that can be earned as an individual participant.</summary>
    individual,

    /// <summary>An achievement that can be earned as a group participant.</summary>
    group
};

/// <summary>Enumeration values that indicate the reward type for an achievement.</summary>
enum class achievement_reward_type
{
    /// <summary>The reward type is unknown.</summary>
    unknown,

    /// <summary>A Gamerscore reward.</summary>
    gamerscore,

    /// <summary>An in-app reward, defined and delivered by the title.</summary>
    in_app,

    /// <summary>A digital art reward.</summary>
    art
};


/// <summary>
/// Represents the association between a title and achievements.
/// </summary>
class achievement_title_association
{
public:
    /// <summary>
    /// The localized name of the title.
    /// </summary>
    inline const string_t& name() const;

    /// <summary>
    /// The title ID.
    /// </summary>
    inline uint32_t title_id() const;

    inline achievement_title_association(const XblAchievementTitleAssociation& association);

private:
    string_t m_name;
    uint32_t m_titleId;
};

/// <summary>
/// Represents requirements for unlocking the achievement.
/// </summary>
class achievement_requirement
{
public:
    /// <summary>
    /// The achievement requirement ID.
    /// </summary>
    inline const string_t& id() const;

    /// <summary>
    /// A value that indicates the current progress of the player towards meeting
    /// the requirement.
    /// </summary>
    inline const string_t& current_progress_value() const;

    /// <summary>
    /// The target progress value that the player must reach in order to meet
    /// the requirement.
    /// </summary>
    inline const string_t& target_progress_value() const;
    
    inline achievement_requirement(const XblAchievementRequirement& requirement);

private:
    string_t m_id;
    string_t m_currentProgressValue;
    string_t m_targetProgressValue;
};

/// <summary>
/// Represents progress details about the achievement, including requirements.
/// </summary>
class achievement_progression
{
public:
    /// <summary>
    /// The actions and conditions that are required to unlock the achievement.
    /// </summary>
    inline const std::vector<achievement_requirement>& requirements() const;

    /// <summary>
    /// The timestamp when the achievement was first unlocked.
    /// </summary>
    inline const utility::datetime& time_unlocked() const;

    inline achievement_progression(const XblAchievementProgression& progression);

private:
    std::vector<achievement_requirement> m_requirements;
    utility::datetime m_timeUnlocked;
};

/// <summary>
/// Represents an interval of time during which an achievement can be unlocked. 
/// This class is only used when the achievement_type enumeration is set to challenge.
/// </summary>
class achievement_time_window
{
public:
    /// <summary>
    /// The start date and time of the achievement time window.
    /// </summary>
    inline const utility::datetime& start_date() const;

    /// <summary>
    /// The end date and time of the achievement time window.
    /// </summary>
    inline const utility::datetime& end_date() const;
    
    inline achievement_time_window(const XblAchievementTimeWindow& timeWindow);

private:
    utility::datetime m_startDate;
    utility::datetime m_endDate;
};

/// <summary>
/// Represents a media asset for an achievement.
/// </summary>
class achievement_media_asset
{
public:
    /// <summary>
    /// The name of the media asset, such as "tile01".
    /// </summary>
    inline const string_t& name() const;

    /// <summary>
    /// The type of media asset.
    /// </summary>
    inline achievement_media_asset_type media_asset_type() const;

    /// <summary>
    /// The URL of the media asset.
    /// </summary>
    inline const web::uri& url() const;

    inline achievement_media_asset(const XblAchievementMediaAsset* mediaAsset);

private:
    string_t m_name;
    achievement_media_asset_type m_type{ achievement_media_asset_type::unknown };
    web::uri m_url;
};

#if !XSAPI_NO_PPL
/// <summary>
/// Represents a reward that is associated with the achievement.
/// </summary>
class achievement_reward
{
public:
    /// <summary>
    /// The localized reward name.
    /// </summary>
    inline const string_t& name() const;

    /// <summary>
    /// The description of the reward.
    /// </summary>
    inline const string_t& description() const;

    /// <summary>
    /// The title-defined reward value (data type and content varies by reward type).
    /// </summary>
    inline const string_t& value() const;

    /// <summary>
    /// The reward type.
    /// </summary>
    inline achievement_reward_type reward_type() const;

    /// <summary>
    /// The property type of the reward value string.
    /// </summary>
    inline const string_t& value_type() const;

    /// <summary>
    /// The media asset associated with the reward.
    /// If the reward type is gamerscore, this will be nullptr.
    /// If the reward type is in_app, this will be a media asset.
    /// If the reward type is art, this may be a media asset or nullptr.
    /// </summary>
    inline const achievement_media_asset& media_asset() const;

    inline achievement_reward(const XblAchievementReward& reward);

private:
    string_t m_name;
    string_t m_description;
    string_t m_value;
    achievement_reward_type m_rewardType;
    string_t m_valueType;
    achievement_media_asset m_mediaAsset;
};


/// <summary>
/// Represents an achievement, a system-wide mechanism for directing and
/// rewarding users' in-game actions consistently across all games.
/// </summary>
class achievement
{
public:
    /// <summary>
    /// The achievement ID. Can be a uint or a guid.
    /// </summary>
    inline string_t id() const;

    /// <summary>
    /// The ID of the service configuration set associated with the achievement.
    /// </summary>
    inline string_t service_configuration_id() const;

    /// <summary>
    /// The localized achievement name.
    /// </summary>
    inline string_t name() const;

    /// <summary>
    /// The game/app titles associated with the achievement.
    /// </summary>
    inline std::vector<achievement_title_association> title_associations() const;

    /// <summary>
    /// The state of a user's progress towards the earning of the achievement.
    /// </summary>
    inline achievement_progress_state progress_state() const;

    /// <summary>
    /// The progression object containing progress details about the achievement,
    /// including requirements.
    /// </summary>
    inline achievement_progression progression() const;

    /// <summary>
    /// The media assets associated with the achievement, such as image IDs.
    /// </summary>
    inline std::vector<achievement_media_asset> media_assets() const;

    /// <summary>
    /// The collection of platforms that the achievement is available on.
    /// </summary>
    inline std::vector<string_t> platforms_available_on() const;

    /// <summary>
    /// Whether or not the achievement is secret.
    /// </summary>
    inline bool is_secret() const;

    /// <summary>
    /// The description of the unlocked achievement.
    /// </summary>
    inline string_t unlocked_description() const;

    /// <summary>
    /// The description of the locked achievement.
    /// </summary>
    inline string_t locked_description() const;

    /// <summary>
    /// The product_id the achievement was released with. This is a globally unique identifier that
    /// may correspond to an application, downloadable content, etc.
    /// </summary>
    inline string_t product_id() const;

    /// <summary>
    /// The type of achievement, such as a challenge achievement.
    /// </summary>
    inline achievement_type type() const;

    /// <summary>
    /// The participation type for the achievement, such as group or individual.
    /// </summary>
    inline achievement_participation_type participation_type() const;

    /// <summary>
    /// The time window during which the achievement is available. Applies to Challenges.
    /// </summary>
    inline achievement_time_window available() const;

    /// <summary>
    /// The collection of rewards that the player earns when the achievement is unlocked.
    /// </summary>
    inline std::vector<achievement_reward> rewards() const;

    /// <summary>
    /// The estimated time that the achievement takes to be earned.
    /// </summary>
    inline std::chrono::seconds estimated_unlock_time() const;

    /// <summary>
    /// A deeplink for clients that enables the title to launch at a desired starting point
    /// for the achievement.
    /// </summary>
    inline string_t deep_link() const;

    /// <summary>
    /// A value that indicates whether or not the achievement is revoked by enforcement.
    /// </summary>
    inline bool is_revoked() const;

    achievement() = default;
    inline achievement(XblAchievementsResultHandle handle, const XblAchievement* achievement);
    inline achievement(const achievement& other);
    inline achievement& operator=(achievement other);
    inline ~achievement();

private:
    XblAchievementsResultHandle m_handle{ nullptr };
    const XblAchievement* m_achievement{ nullptr };
};

/// <summary>
/// Represents a collection of Achievement class objects returned by a request.
/// </summary>
class achievements_result
{
public:
    /// <summary>
    /// The collection of achievement objects returned by a request.
    /// </summary>
    inline std::vector<achievement> items() const;

    /// <summary>
    /// Returns a boolean value that indicates if there are more pages of achievements to retrieve.
    /// </summary>
    /// <returns>True if there are more pages, otherwise false.</returns>
    inline bool has_next() const;

    /// <summary>
    /// Returns an achievements_result object that contains the next page of achievements.
    /// </summary>
    /// <param name="maxItems">The maximum number of items that the result can contain.  Pass 0 to attempt
    /// to retrieve all items.</param>
    /// <returns>An achievements_result object that contains a list of achievement objects.</returns>
    /// <remarks>
    /// Returns a concurrency::task&lt;T&gt; object that represents the state of the asynchronous operation.
    ///
    /// This method calls V2 GET /users/xuid({xuid})/achievements.
    /// </remarks>
    inline pplx::task<xbox::services::xbox_live_result<achievements_result>> get_next(
        _In_ uint32_t maxItems
    );

    achievements_result() = default;
    inline achievements_result(XblAchievementsResultHandle handle);
    inline achievements_result(const achievements_result& other);
    inline achievements_result& operator=(achievements_result other);
    inline ~achievements_result();

private:
    XblAchievementsResultHandle m_handle{ nullptr };
};

/// <summary>
/// Represents an endpoint that you can use to access the Achievement service.
/// </summary>
class achievement_service
{
public:
    /// <summary>
    /// Allow achievement progress to be updated and achievements to be unlocked.
    /// This API will work even when offline. On PC and Xbox One, updates will be 
    /// posted by the system when connection is re-established even if the title isn't running.
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID of the player.</param>
    /// <param name="achievementId">The achievement ID as defined by XDP or Dev Center.</param>
    /// <param name="percentComplete">The completion percentage of the achievement to indicate progress.
    /// Valid values are from 1 to 100. Set to 100 to unlock the achievement.
    /// Progress will be set by the server to the highest value sent</param>
    /// <remarks>
    /// Returns a task&lt;T&gt; object that represents the state of the asynchronous operation.
    ///
    /// This method calls V2 POST /users/xuid({xuid})/achievements/{scid}/update
    /// </remarks>
    inline pplx::task<xbox::services::xbox_live_result<void>> update_achievement(
        _In_ const string_t& xboxUserId,
        _In_ const string_t& achievementId,
        _In_ uint32_t percentComplete
        );

    /// <summary>
    /// Allow achievement progress to be updated and achievements to be unlocked.
    /// This API will work even when offline. On PC and Xbox One, updates will be 
    /// posted by the system when connection is re-established even if the title isn't running.
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID of the player.</param>
    /// <param name="titleId">The title ID.</param>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) for the title.</param>
    /// <param name="achievementId">The achievement ID as defined by XDP or Dev Center.</param>
    /// <param name="percentComplete">The completion percentage of the achievement to indicate progress.
    /// Valid values are from 1 to 100. Set to 100 to unlock the achievement.
    /// Progress will be set by the server to the highest value sent</param>
    /// <remarks>
    /// Returns a task&lt;T&gt; object that represents the state of the asynchronous operation.
    ///
    /// This method calls V2 POST /users/xuid({xuid})/achievements/{scid}/update
    /// </remarks>
    inline pplx::task<xbox::services::xbox_live_result<void>> update_achievement(
        _In_ const string_t& xboxUserId,
        _In_ uint32_t titleId,
        _In_ const string_t& serviceConfigurationId,
        _In_ const string_t& achievementId,
        _In_ uint32_t percentComplete
        );

    /// <summary>
    /// Returns an achievements_result object containing the first page of achievements
    /// for a player of the specified title.
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID of the player.</param>
    /// <param name="titleId">The title ID.</param>
    /// <param name="type">The achievement type to retrieve.</param>
    /// <param name="unlockedOnly">Indicates whether to return unlocked achievements only.</param>
    /// <param name="orderby">Controls how the list of achievements is ordered.</param>
    /// <param name="skipItems">The number of achievements to skip.</param>
    /// <param name="maxItems">The maximum number of achievements the result can contain.  Pass 0 to attempt
    /// to retrieve all items.</param>
    /// <returns>An AchievementsResult object that contains a list of Achievement objects.</returns>
    /// <remarks>
    /// Returns a task&lt;T&gt; object that represents the state of the asynchronous operation.
    ///
    /// See achievements_result:get_next to page in the next set of results.
    ///
    /// This method calls V2 GET /users/xuid({xuid})/achievements
    /// </remarks>
    inline pplx::task<xbox::services::xbox_live_result<achievements_result>> get_achievements_for_title_id(
        _In_ const string_t& xboxUserId,
        _In_ uint32_t titleId,
        _In_ achievement_type type,
        _In_ bool unlockedOnly,
        _In_ achievement_order_by orderBy,
        _In_ uint32_t skipItems,
        _In_ uint32_t maxItems
        );

    /// <summary>
    /// Returns a specific achievement object for a specified player.
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID of the player.</param>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) for the title.</param>
    /// <param name="achievementId">The unique identifier of the Achievement as defined by XDP or Dev Center.</param>
    /// <returns>The requested achievement object if it exists.
    /// If the achievement does not exist, the method returns xbox_live_error_code::runtime_error .</returns>
    /// <remarks>
    /// Returns a task&lt;T&gt; object that represents the state of the asynchronous operation.
    ///
    /// This method calls V2 GET /users/xuid({xuid})/achievements/{scid}/{achievementId}.
    /// </remarks>
    inline pplx::task<xbox::services::xbox_live_result<achievement>> get_achievement(
        _In_ const string_t& xboxUserId,
        _In_ const string_t& serviceConfigurationId,
        _In_ const string_t& achievementId
        );

    inline achievement_service(const achievement_service& other);
    inline achievement_service& operator=(achievement_service other);
    inline ~achievement_service();

private:
    inline achievement_service(XblContextHandle xblContextHandle);

    XblContextHandle m_xblContextHandle;

    friend xbox_live_context;
};
#endif // !XSAPI_NO_PPL

}}}

#if !XSAPI_NO_PPL
#include "impl/achievements.hpp"
#endif
