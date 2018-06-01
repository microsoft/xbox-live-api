// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/achievements.h"
#include "achievements_internal.h"
#include "xbox_system_factory.h"
#include "utils.h"
#include "user_context.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_BEGIN

static achievement_progress_state convert_string_to_progress_state(
    _In_ const string_t& value
    )
{
    if (value.empty())
    {
        return achievement_progress_state::unknown;
    }
    else if (utils::str_icmp(value, _T("Achieved")) == 0)
    {
        return achievement_progress_state::achieved;
    }
    else if (utils::str_icmp(value, _T("NotStarted")) == 0)
    {
        return achievement_progress_state::not_started;
    }
    else if (utils::str_icmp(value, _T("InProgress")) == 0)
    {
        return achievement_progress_state::in_progress;
    }

    return achievement_progress_state::unknown;
}

static achievement_type convert_string_to_achievement_type(
    _In_ const string_t& value
    )
{
    if (value.empty())
    {
        return achievement_type::unknown;
    }
    else if (utils::str_icmp(value, _T("Persistent")) == 0)
    {
        return achievement_type::persistent;
    }
    else if (utils::str_icmp(value, _T("Challenge")) == 0)
    {
        return achievement_type::challenge;
    }

    return achievement_type::unknown;
}

static achievement_participation_type convert_string_to_participation_type(
    _In_ const string_t& value
    )
{
    if (value.empty())
    {
        return achievement_participation_type::unknown;
    }
    else if (utils::str_icmp(value, _T("Individual")) == 0)
    {
        return achievement_participation_type::individual;
    }
    else if (utils::str_icmp(value, _T("Group")) == 0)
    {
        return achievement_participation_type::group;
    }

    return achievement_participation_type::unknown;
}

achievement::achievement() :
    m_internalObj(xsapi_allocate_shared<achievement_internal>())
{
}

achievement::achievement(
    _In_ std::shared_ptr<achievement_internal> internalObj
    ) :
    m_internalObj(std::move(internalObj))
{
}

DEFINE_GET_STRING(achievement, id);
DEFINE_GET_STRING(achievement, service_configuration_id);
DEFINE_GET_STRING(achievement, name);
DEFINE_GET_VECTOR_INTERNAL_TYPE(achievement, achievement_title_association, title_associations);
DEFINE_GET_ENUM_TYPE(achievement, achievement_progress_state, progress_state);
DEFINE_GET_OBJECT_REF(achievement, achievement_progression, progression);
DEFINE_GET_VECTOR_INTERNAL_TYPE(achievement, achievement_media_asset, media_assets);
DEFINE_GET_STRING_VECTOR(achievement, platforms_available_on);
DEFINE_GET_BOOL(achievement, is_secret);
DEFINE_GET_STRING(achievement, unlocked_description);
DEFINE_GET_STRING(achievement, locked_description);
DEFINE_GET_STRING(achievement, product_id);
DEFINE_GET_ENUM_TYPE(achievement, achievement_type, type);
DEFINE_GET_ENUM_TYPE(achievement, achievement_participation_type, participation_type);
DEFINE_GET_OBJECT_REF(achievement, achievement_time_window, available);
DEFINE_GET_VECTOR_INTERNAL_TYPE(achievement, achievement_reward, rewards);
DEFINE_GET_OBJECT_REF(achievement, std::chrono::seconds, estimated_unlock_time);
DEFINE_GET_STRING(achievement, deep_link);
DEFINE_GET_BOOL(achievement, is_revoked);

achievement_internal::achievement_internal() :
    m_progressState(achievement_progress_state::unknown),
    m_achievementType(achievement_type::unknown),
    m_participationType(achievement_participation_type::unknown),
    m_estimatedUnlockTime(0)
{
}

achievement_internal::achievement_internal(
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
    ) :
    m_id(std::move(id)),
    m_serviceConfigurationId(std::move(serviceConfigurationId)),
    m_name(std::move(name)),
    m_titleAssociations(std::move(titleAssociations)),
    m_progressState(progressState),
    m_mediaAssets(std::move(mediaAssets)),
    m_platformsAvailableOn(std::move(platformsAvailableOn)),
    m_isSecret(isSecret),
    m_unlockedDescription(std::move(unlockedDescription)),
    m_lockedDescription(std::move(lockedDescription)),
    m_productId(std::move(productId)),
    m_achievementType(achievementType),
    m_participationType(participationType),
    m_available(std::move(available)),
    m_rewards(std::move(rewards)),
    m_estimatedUnlockTime(std::move(estimatedUnlockTime)),
    m_deepLink(std::move(deepLink)),
    m_isRevoked(isRevoked),
    m_progressionInternal(std::move(progression))
{
    m_progression = achievement_progression(m_progressionInternal);
}

const xsapi_internal_string&
achievement_internal::id() const
{
    return m_id;
}

const xsapi_internal_string&
achievement_internal::service_configuration_id() const
{
    return m_serviceConfigurationId;
}

const xsapi_internal_string&
achievement_internal::name() const
{
    return m_name;
}

const xsapi_internal_vector<std::shared_ptr<achievement_title_association_internal>>&
achievement_internal::title_associations() const
{
    return m_titleAssociations;
}

achievement_progress_state
achievement_internal::progress_state() const
{
    return m_progressState;
}

const achievement_progression&
achievement_internal::progression() const
{
    return m_progression;
}

std::shared_ptr<achievement_progression_internal>
achievement_internal::progression_internal() const
{
    return m_progressionInternal;
}

const xsapi_internal_vector<std::shared_ptr<achievement_media_asset_internal>>&
achievement_internal::media_assets() const
{
    return m_mediaAssets;
}

const xsapi_internal_vector<xsapi_internal_string>&
achievement_internal::platforms_available_on() const
{
    return m_platformsAvailableOn;
}

bool 
achievement_internal::is_secret() const
{
    return m_isSecret;
}

const xsapi_internal_string&
achievement_internal::unlocked_description() const
{
    return m_unlockedDescription;
}

const xsapi_internal_string&
achievement_internal::locked_description() const
{
    return m_lockedDescription;
}

const xsapi_internal_string&
achievement_internal::product_id() const
{
    return m_productId;
}

achievement_type
achievement_internal::type() const
{
    return m_achievementType;
}

achievement_participation_type 
achievement_internal::participation_type() const
{
    return m_participationType;
}

const achievement_time_window&
achievement_internal::available() const
{
    return m_available;
}

const xsapi_internal_vector<std::shared_ptr<achievement_reward_internal>>&
achievement_internal::rewards() const
{
    return m_rewards;
}

const std::chrono::seconds& 
achievement_internal::estimated_unlock_time() const
{
    return m_estimatedUnlockTime;
}

const xsapi_internal_string&
achievement_internal::deep_link() const
{
    return m_deepLink;
}

bool 
achievement_internal::is_revoked() const
{
    return m_isRevoked;
}

xbox_live_result<std::shared_ptr<achievement_internal>>
achievement_internal::_Deserialize(
    _In_ const web::json::value& json
    )
{
    if (json.is_null()) return xbox_live_result<std::shared_ptr<achievement_internal>>();

    std::error_code errCode = xbox_live_error_code::no_error;

    auto returnAchievement = xsapi_allocate_shared<achievement_internal>( 
        utils::extract_json_string(json, "id", errCode, true),
        utils::extract_json_string(json, "serviceConfigId", errCode, true),
        utils::extract_json_string(json, "name", errCode, true),
        utils::extract_json_vector<std::shared_ptr<achievement_title_association_internal>>(achievement_title_association_internal::_Deserialize, json, "titleAssociations", errCode, true),
        convert_string_to_progress_state(utils::extract_json_string(json, _T("progressState"), true)),
        utils::extract_json_vector<std::shared_ptr<achievement_media_asset_internal>>(achievement_media_asset_internal::_Deserialize, json, "mediaAssets", errCode, true),
        utils::internal_string_vector_from_std_string_vector(utils::extract_json_vector<string_t>(utils::json_string_extractor, json, _T("platforms"), errCode, true)),
        utils::extract_json_bool(json, _T("isSecret")),
        utils::extract_json_string(json, "description", errCode, true),
        utils::extract_json_string(json, "lockedDescription", errCode, true),
        utils::extract_json_string(json, "productId", errCode, true),
        convert_string_to_achievement_type(utils::extract_json_string(json, _T("achievementType"), errCode, true)),
        convert_string_to_participation_type(utils::extract_json_string(json, _T("participationType"), errCode, true)),
        utils::extract_json_object<achievement_time_window>(achievement_time_window::_Deserialize, json, _T("timeWindow"), errCode, true),
        utils::extract_json_vector<std::shared_ptr<achievement_reward_internal>>(achievement_reward_internal::_Deserialize, json, "rewards", errCode, true),
        utils::extract_json_string_timespan_in_seconds(json, _T("estimatedTime"), true),
        utils::extract_json_string(json, "deeplink", errCode, true),
        utils::extract_json_bool(json, _T("isRevoked"), true, false),
        utils::extract_json_object<std::shared_ptr<achievement_progression_internal>>(achievement_progression_internal::_Deserialize, json, _T("progression"), errCode, true)
    );

    return xbox_live_result<std::shared_ptr<achievement_internal>>(returnAchievement, errCode);
}
NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_END