// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "pch.h"
#include "xsapi/achievements.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_BEGIN

class achievement_title_association_internal
{
public:
    _XSAPIIMP achievement_title_association_internal(
        _In_ xsapi_internal_string name,
        _In_ uint32_t id
    );

    const xsapi_internal_string& name() const;

    uint32_t title_id() const;

    static xbox_live_result<std::shared_ptr<achievement_title_association_internal>> _Deserialize(_In_ const web::json::value& json);

private:
    xsapi_internal_string m_name;
    uint32_t m_id;
};

class achievement_requirement_internal
{
public:
    achievement_requirement_internal(
        _In_ xsapi_internal_string id,
        _In_ xsapi_internal_string currentProgressValue,
        _In_ xsapi_internal_string targetProgressValue
    );

    const xsapi_internal_string& id() const;

    const xsapi_internal_string& current_progress_value() const;

    const xsapi_internal_string& target_progress_value() const;

    static xbox_live_result<std::shared_ptr<achievement_requirement_internal>> _Deserialize(_In_ const web::json::value& json);

private:
    xsapi_internal_string m_id;
    xsapi_internal_string m_currentProgressValue;
    xsapi_internal_string m_targetProgressValue;
};

class achievement_progression_internal
{
public:
    achievement_progression_internal(
        _In_ xsapi_internal_vector<std::shared_ptr<achievement_requirement_internal>> requirements,
        _In_ utility::datetime timeUnlocked
    );

    const xsapi_internal_vector<std::shared_ptr<achievement_requirement_internal>>& requirements() const;

    const utility::datetime& time_unlocked() const;

    static xbox_live_result<std::shared_ptr<achievement_progression_internal>> _Deserialize(_In_ const web::json::value& json);

private:
    xsapi_internal_vector<std::shared_ptr<achievement_requirement_internal>> m_requirements;
    utility::datetime m_timeUnlocked;
};

class achievement_media_asset_internal
{
public:
    achievement_media_asset_internal(
        _In_ xsapi_internal_string name,
        _In_ achievement_media_asset_type type,
        _In_ web::uri url
    );

    const xsapi_internal_string& name() const;

    const achievement_media_asset_type& media_asset_type() const;

    const web::uri& url() const;

    static achievement_media_asset_type _Convert_string_to_media_asset_type(
        _In_ const string_t& value
    );

    static xbox_live_result<std::shared_ptr<achievement_media_asset_internal>> _Deserialize(_In_ const web::json::value& json);

private:
    xsapi_internal_string m_name;
    achievement_media_asset_type m_type;
    web::uri m_url;
};

class achievement_reward_internal
{
public:
    achievement_reward_internal(
        _In_ xsapi_internal_string name,
        _In_ xsapi_internal_string description,
        _In_ xsapi_internal_string value,
        _In_ achievement_reward_type rewardType,
        _In_ xsapi_internal_string valuePropertyType,
        _In_ std::shared_ptr<achievement_media_asset_internal> mediaAsset
    );

    const xsapi_internal_string& name() const;

    const xsapi_internal_string& description() const;

    const xsapi_internal_string& value() const;

    achievement_reward_type reward_type() const;

    const xsapi_internal_string& value_type() const;

    const achievement_media_asset& media_asset() const;

    std::shared_ptr<achievement_media_asset_internal> media_asset_internal() const;

    static xbox_live_result<std::shared_ptr<achievement_reward_internal>> _Deserialize(_In_ const web::json::value& json);

private:
    static achievement_reward_type convert_string_to_reward_type(
        _In_ const string_t& value
    );

    xsapi_internal_string m_name;
    xsapi_internal_string m_description;
    xsapi_internal_string m_value;
    achievement_reward_type m_rewardType;
    xsapi_internal_string m_valuePropertyType;
    std::shared_ptr<achievement_media_asset_internal> m_mediaAssetInternal;
    achievement_media_asset m_mediaAsset;
};

class achievement_internal
{
public:
    achievement_internal();

    achievement_internal(
        _In_ xsapi_internal_string id,
        _In_ xsapi_internal_string serviceConfigurationId,
        _In_ xsapi_internal_string name,
        _In_ xsapi_internal_vector<std::shared_ptr<achievement_title_association_internal>> titleAssociations,
        _In_ achievement_progress_state progressState,
        _In_ xsapi_internal_vector<std::shared_ptr<achievement_media_asset_internal>> mediaAssets,
        _In_ xsapi_internal_vector<xsapi_internal_string> platformsAvailableOn,
        _In_ bool isSecret,
        _In_ xsapi_internal_string unlockedDescription,
        _In_ xsapi_internal_string lockedDescription,
        _In_ xsapi_internal_string productId,
        _In_ achievement_type achievementType,
        _In_ achievement_participation_type participationType,
        _In_ achievement_time_window available,
        _In_ xsapi_internal_vector<std::shared_ptr<achievement_reward_internal>> rewards,
        _In_ std::chrono::seconds estimatedUnlockTime,
        _In_ xsapi_internal_string deepLink,
        _In_ bool isRevoked,
        _In_ std::shared_ptr<achievement_progression_internal> progression
    );

    const xsapi_internal_string& id() const;

    const xsapi_internal_string& service_configuration_id() const;

    const xsapi_internal_string& name() const;

    const xsapi_internal_vector<std::shared_ptr<achievement_title_association_internal>>& title_associations() const;

    achievement_progress_state progress_state() const;

    const achievement_progression& progression() const;

    std::shared_ptr<achievement_progression_internal> progression_internal() const;

    const xsapi_internal_vector<std::shared_ptr<achievement_media_asset_internal>>& media_assets() const;

    const xsapi_internal_vector<xsapi_internal_string>& platforms_available_on() const;

    bool is_secret() const;

    const xsapi_internal_string& unlocked_description() const;

    const xsapi_internal_string& locked_description() const;

    const xsapi_internal_string& product_id() const;

    achievement_type type() const;

    achievement_participation_type participation_type() const;

    const achievement_time_window& available() const;

    const xsapi_internal_vector<std::shared_ptr<achievement_reward_internal>>& rewards() const;

    const std::chrono::seconds& estimated_unlock_time() const;

    const xsapi_internal_string& deep_link() const;

    bool is_revoked() const;

    static xbox_live_result<std::shared_ptr<achievement_internal>> _Deserialize(_In_ const web::json::value& json);

private:
    xsapi_internal_string m_id;
    xsapi_internal_string m_serviceConfigurationId;
    xsapi_internal_string m_name;
    xsapi_internal_vector<std::shared_ptr<achievement_title_association_internal>> m_titleAssociations;
    achievement_progress_state m_progressState;
    xsapi_internal_vector<std::shared_ptr<achievement_media_asset_internal>> m_mediaAssets;
    xsapi_internal_vector<xsapi_internal_string> m_platformsAvailableOn;
    bool m_isSecret;
    xsapi_internal_string m_unlockedDescription;
    xsapi_internal_string m_lockedDescription;
    xsapi_internal_string m_productId;
    achievement_type m_achievementType;
    achievement_participation_type m_participationType;
    achievement_time_window m_available;
    xsapi_internal_vector<std::shared_ptr<achievement_reward_internal>> m_rewards;
    std::chrono::seconds m_estimatedUnlockTime;
    xsapi_internal_string m_deepLink;
    bool m_isRevoked;
    achievement_progression m_progression;
    std::shared_ptr<achievement_progression_internal> m_progressionInternal;
};

class achievements_result_internal
{
public:
    achievements_result_internal(
        xsapi_internal_vector<std::shared_ptr<achievement_internal>> achievements,
        xsapi_internal_string continuationToken
    );

    void _Init_next_page_info(
        _In_ std::shared_ptr<xbox::services::user_context> userContext,
        _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
        _In_ std::shared_ptr<xbox::services::xbox_live_app_config_internal> appConfig,
        _In_ std::weak_ptr<xbox_live_context_impl> xboxLiveContextImpl,
        _In_ xsapi_internal_string xboxUserId,
        _In_ xsapi_internal_vector<uint32_t> titleIds,
        _In_ achievement_type type,
        _In_ bool unlockedOnly,
        _In_ achievement_order_by orderBy
    );

    const xsapi_internal_vector<std::shared_ptr<achievement_internal>>& items() const;

    bool has_next() const;

    _XSAPIIMP  xbox_live_result<void> get_next(
        _In_ uint32_t maxItems,
        _In_opt_ async_queue_handle_t queue,
        _In_ xbox_live_callback<xbox_live_result<std::shared_ptr<achievements_result_internal>>> callback
    );

    static xbox_live_result<std::shared_ptr<achievements_result_internal>> _Deserialize(_In_ const web::json::value& json);

    // Internal
    const xsapi_internal_string& xbox_user_id() const;
    const xsapi_internal_vector<uint32_t>& title_ids() const;
    const achievement_type& type() const;
    const bool& unlocked_only() const;
    const achievement_order_by& order_by() const;
    const xsapi_internal_string& continuation_token() const;

private:
    std::shared_ptr<xbox::services::user_context> m_userContext;
    std::shared_ptr<xbox::services::xbox_live_context_settings> m_xboxLiveContextSettings;
    std::shared_ptr<xbox::services::xbox_live_app_config_internal> m_appConfig;
    std::weak_ptr<xbox_live_context_impl> m_xboxLiveContextImpl;

    xsapi_internal_string m_xboxUserId;
    xsapi_internal_vector<uint32_t> m_titleIds;
    achievement_type m_achievementType;
    bool m_unlockedOnly;
    achievement_order_by m_orderBy;
    xsapi_internal_vector<std::shared_ptr<achievement_internal>> m_items;
    xsapi_internal_string m_continuationToken;
};

class achievement_service_internal
{
public:
    achievement_service_internal(
        _In_ std::shared_ptr<xbox::services::user_context> userContext,
        _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
        _In_ std::shared_ptr<xbox_live_app_config_internal> appConfig,
        _In_ std::weak_ptr<xbox_live_context_impl> xboxLiveContextImpl
    );

    _XSAPIIMP xbox_live_result<void> update_achievement(
        _In_ const xsapi_internal_string& xboxUserId,
        _In_ const xsapi_internal_string& achievementId,
        _In_ uint32_t percentComplete,
        _In_opt_ async_queue_handle_t queue,
        _In_ xbox_live_callback<xbox_live_result<void>> callback
    );

    _XSAPIIMP xbox_live_result<void> update_achievement(
        _In_ const xsapi_internal_string& xboxUserId,
        _In_ uint32_t titleId,
        _In_ const xsapi_internal_string& serviceConfigurationId,
        _In_ const xsapi_internal_string& achievementId,
        _In_ uint32_t percentComplete,
        _In_opt_ async_queue_handle_t queue,
        _In_ xbox_live_callback<xbox_live_result<void>> callback
    );
    
    _XSAPIIMP xbox_live_result<void> get_achievements_for_title_id(
        _In_ const xsapi_internal_string& xboxUserId,
        _In_ uint32_t titleId,
        _In_ achievement_type type,
        _In_ bool unlockedOnly,
        _In_ achievement_order_by orderBy,
        _In_ uint32_t skipItems,
        _In_ uint32_t maxItems,
        _In_opt_ async_queue_handle_t queue,
        _In_ xbox_live_callback<xbox_live_result<std::shared_ptr<achievements_result_internal>>> callback
    );

    _XSAPIIMP xbox_live_result<void> get_achievement(
        _In_ const xsapi_internal_string& xboxUserId,
        _In_ const xsapi_internal_string& serviceConfigurationId,
        _In_ const xsapi_internal_string& achievementId,
        _In_opt_ async_queue_handle_t queue,
        _In_ xbox_live_callback<xbox_live_result<std::shared_ptr<achievement_internal>>> callback
    );
    
    // Internal
    xbox_live_result<void> get_achievements(
        _In_ const xsapi_internal_string& xboxUserId,
        _In_ const xsapi_internal_vector<uint32_t>& titleIds,
        _In_ achievement_type type,
        _In_ bool unlockedOnly,
        _In_ achievement_order_by orderBy,
        _In_ uint32_t skipItems,
        _In_ uint32_t maxItems,
        _In_ const xsapi_internal_string& continuationToken,
        _In_opt_ async_queue_handle_t queue,
        _In_ xbox_live_callback<xbox_live_result<std::shared_ptr<achievements_result_internal>>> callback
    );

private:
    // Achievements endpoints
    static const xsapi_internal_string achievements_sub_path(
        _In_ const xsapi_internal_string& xboxUserId,
        _In_ const xsapi_internal_vector<uint32_t>& titleIds,
        _In_ const xsapi_internal_string& types,
        _In_ bool unlockedOnly,
        _In_ const xsapi_internal_string& orderBy,
        _In_ uint32_t skipItems,
        _In_ uint32_t maxItems,
        _In_ const xsapi_internal_string& continuationToken
    );

    static const xsapi_internal_string update_achievement_sub_path(
        _In_ const xsapi_internal_string& xboxUserId,
        _In_ const xsapi_internal_string& serviceConfigurationId
    );

    static const xsapi_internal_string achievement_by_id_sub_path(
        _In_ const xsapi_internal_string& xboxUserId,
        _In_ const xsapi_internal_string& serviceConfigurationId,
        _In_ const xsapi_internal_string& achievementId
    );

    static xbox_live_result<xsapi_internal_string> convert_type_to_string(
        _In_ achievement_type type
    );

    static xbox_live_result<xsapi_internal_string> convert_order_by_to_string(
        _In_ achievement_order_by orderBy
    );

    std::shared_ptr<xbox::services::user_context> m_userContext;
    std::shared_ptr<xbox::services::xbox_live_context_settings> m_xboxLiveContextSettings;
    std::shared_ptr<xbox::services::xbox_live_app_config_internal> m_appConfig;
    std::weak_ptr<xbox_live_context_impl> m_xboxLiveContextImpl;

#if TV_API
    static xbox::services::xbox_live_result<void> write_offline_update_achievement(
        _In_ std::shared_ptr<xbox::services::xbox_live_context_impl> xboxLiveContextImpl,
        _In_ const xsapi_internal_string& achievementId,
        _In_ uint32_t percentComplete
    );

    static ULONG event_write_achievement_update(
        _In_ PCWSTR userId,
        _In_ PCWSTR achievementId,
        _In_ const uint32_t percentComplete
    );

#elif UWP_API
    static xbox::services::xbox_live_result<void> write_offline_update_achievement(
        _In_ std::shared_ptr<xbox::services::xbox_live_context_impl> xboxLiveContextImpl,
        _In_ const xsapi_internal_string& achievementId,
        _In_ uint32_t percentComplete
    );
#endif

    friend class xbox_live_context_impl;
    friend class achievements_result_internal;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_END