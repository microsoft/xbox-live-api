// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/achievements.h"
#include "xbox_system_factory.h"
#include "utils.h"
#include "user_context.h"

using namespace pplx;

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
    m_progressState(achievement_progress_state::unknown),
    m_achievementType(achievement_type::unknown),
    m_participationType(achievement_participation_type::unknown),
    m_estimatedUnlockTime(0)
{
}

achievement::achievement(
    _In_ string_t id,
    _In_ string_t serviceConfigurationId,
    _In_ string_t name,
    _In_ std::vector<achievement_title_association> titleAssociations,
    _In_ achievement_progress_state progressState,
    _In_ std::vector<achievement_media_asset> mediaAssets,
    _In_ std::vector<string_t> platformsAvailableOn,
    _In_ bool isSecret,
    _In_ string_t unlockedDescription,
    _In_ string_t lockedDescription,
    _In_ string_t productId,
    _In_ achievement_type achievementType,
    _In_ achievement_participation_type participationType,
    _In_ achievement_time_window available,
    _In_ std::vector<achievement_reward> rewards,
    _In_ std::chrono::seconds estimatedUnlockTime,
    _In_ string_t deepLink,
    _In_ bool isRevoked,
    _In_ achievement_progression progression
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
    m_progression(std::move(progression))
{
}

const string_t& achievement::id() const
{
    return m_id;
}

const string_t& achievement::service_configuration_id() const
{
    return m_serviceConfigurationId;
}

const string_t& achievement::name() const
{
    return m_name;
}

const std::vector<achievement_title_association>& achievement::title_associations() const
{
    return m_titleAssociations;
}

achievement_progress_state achievement::progress_state() const
{
    return m_progressState;
}

const achievement_progression& achievement::progression() const
{
    return m_progression;
}

const std::vector<achievement_media_asset>& achievement::media_assets() const
{
    return m_mediaAssets;
}

const std::vector<string_t>& achievement::platforms_available_on() const
{
    return m_platformsAvailableOn;
}

bool achievement::is_secret() const
{
    return m_isSecret;
}

const string_t& achievement::unlocked_description() const
{
    return m_unlockedDescription;
}

const string_t& achievement::locked_description() const
{
    return m_lockedDescription;
}

const string_t& achievement::product_id() const
{
    return m_productId;
}

achievement_type achievement::type() const
{
    return m_achievementType;
}

achievement_participation_type achievement::participation_type() const
{
    return m_participationType;
}

const achievement_time_window& achievement::available() const
{
    return m_available;
}

const std::vector<achievement_reward>& achievement::rewards() const
{
    return m_rewards;
}

const std::chrono::seconds& achievement::estimated_unlock_time() const
{
    return m_estimatedUnlockTime;
}

const string_t& achievement::deep_link() const
{
    return m_deepLink;
}

bool achievement::is_revoked() const
{
    return m_isRevoked;
}

xbox_live_result<achievement>
achievement::_Deserialize(
    _In_ const web::json::value& json
    )
{
    if (json.is_null()) return xbox_live_result<achievement>();

    std::error_code errCode = xbox_live_error_code::no_error;

    achievement returnAchievement( 
        utils::extract_json_string(json, _T("id"), errCode, true),
        utils::extract_json_string(json, _T("serviceConfigId"), errCode, true),
        utils::extract_json_string(json, _T("name"), errCode, true),
        utils::extract_json_vector<achievement_title_association>(achievement_title_association::_Deserialize, json, _T("titleAssociations"), errCode, true),
        convert_string_to_progress_state(utils::extract_json_string(json, _T("progressState"), true)),
        utils::extract_json_vector<achievement_media_asset>(achievement_media_asset::_Deserialize, json, _T("mediaAssets"), errCode, true),
        utils::extract_json_vector<string_t>(utils::json_string_extractor, json, _T("platforms"), errCode, true),
        utils::extract_json_bool(json, _T("isSecret")),
        utils::extract_json_string(json, _T("description"), errCode, true),
        utils::extract_json_string(json, _T("lockedDescription"), errCode, true),
        utils::extract_json_string(json, _T("productId"), errCode, true),
        convert_string_to_achievement_type(utils::extract_json_string(json, _T("achievementType"), errCode, true)),
        convert_string_to_participation_type(utils::extract_json_string(json, _T("participationType"), errCode, true)),
        utils::extract_json_object<achievement_time_window>(achievement_time_window::_Deserialize, json, _T("timeWindow"), errCode, true),
        utils::extract_json_vector<achievement_reward>(achievement_reward::_Deserialize, json, _T("rewards"), errCode, true),
        utils::extract_json_string_timespan_in_seconds(json, _T("estimatedTime"), true),
        utils::extract_json_string(json, _T("deeplink"), errCode, true),
        utils::extract_json_bool(json, _T("isRevoked"), true, false),
        utils::extract_json_object<achievement_progression>(achievement_progression::_Deserialize, json, _T("progression"), errCode, true)
    );

    return xbox_live_result<achievement>(returnAchievement, errCode);
}
NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_END