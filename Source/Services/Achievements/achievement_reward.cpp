//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#include "xsapi/achievements.h"
#include "xbox_system_factory.h"
#include "utils.h"
#include "user_context.h"

using namespace pplx;

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_BEGIN

achievement_reward::achievement_reward()
{
}

achievement_reward::achievement_reward(
    _In_ string_t name,
    _In_ string_t description,
    _In_ string_t value,
    _In_ achievement_reward_type rewardType,
    _In_ string_t valuePropertyType,
    _In_ achievement_media_asset mediaAsset
    ) :
    m_name(std::move(name)),
    m_description(std::move(description)),
    m_value(std::move(value)),
    m_rewardType(std::move(rewardType)),
    m_valuePropertyType(std::move(valuePropertyType)),
    m_mediaAsset(std::move(mediaAsset))
{
}

const string_t& achievement_reward::name() const
{
    return m_name;
}

const string_t& achievement_reward::description() const
{
    return m_description;
}

const string_t& achievement_reward::value() const
{
    return m_value;
}

achievement_reward_type achievement_reward::reward_type() const
{
    return m_rewardType;
}

const string_t& achievement_reward::value_type() const
{
    return m_valuePropertyType;
}

const achievement_media_asset& achievement_reward::media_asset() const
{
    return m_mediaAsset;
}

xbox_live_result<achievement_reward>
achievement_reward::_Deserialize(
    _In_ const web::json::value& json
    )
{
    if (json.is_null()) return xbox_live_result<achievement_reward>();

    std::error_code errc = xbox_live_error_code::no_error;
    auto achievementReward = achievement_reward(
        utils::extract_json_string(json, _T("name"), errc, true),
        utils::extract_json_string(json, _T("description"), errc, true),
        utils::extract_json_string(json, _T("value"), errc, true),
        convert_string_to_reward_type(utils::extract_json_string(json, _T("type"), errc, true)),
        utils::extract_json_string(json, _T("valueType"), errc, true),
        utils::extract_json_object<achievement_media_asset>(achievement_media_asset::_Deserialize, json, _T("mediaAsset"), errc, true)
        );

    return xbox_live_result<achievement_reward>(achievementReward, errc);
}

achievement_reward_type achievement_reward::convert_string_to_reward_type(
    _In_ const string_t& value
    )
{
    if (value.empty())
    {
        return achievement_reward_type::unknown;
    }
    else if (utils::str_icmp(value, _T("Gamerscore")) == 0)
    {
        return achievement_reward_type::gamerscore;
    }
    else if (utils::str_icmp(value, _T("InApp")) == 0)
    {
        return achievement_reward_type::in_app;
    }
    else if (utils::str_icmp(value, _T("Art")) == 0)
    {
        return achievement_reward_type::art;
    }

    return achievement_reward_type::unknown;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_END