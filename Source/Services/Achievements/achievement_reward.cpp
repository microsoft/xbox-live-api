// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/achievements.h"
#include "achievements_internal.h"
#include "utils.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_BEGIN

achievement_reward::achievement_reward()
{
}

achievement_reward::achievement_reward(
    _In_ std::shared_ptr<achievement_reward_internal> internalObj
    ) :
    m_internalObj(std::move(internalObj))
{
}

DEFINE_GET_STRING(achievement_reward, name);
DEFINE_GET_STRING(achievement_reward, description);
DEFINE_GET_STRING(achievement_reward, value);
DEFINE_GET_ENUM_TYPE(achievement_reward, achievement_reward_type, reward_type);
DEFINE_GET_STRING(achievement_reward, value_type);
DEFINE_GET_OBJECT_REF(achievement_reward, achievement_media_asset, media_asset);

achievement_reward_internal::achievement_reward_internal(
    _In_ xsapi_internal_string name,
    _In_ xsapi_internal_string description,
    _In_ xsapi_internal_string value,
    _In_ achievement_reward_type rewardType,
    _In_ xsapi_internal_string valuePropertyType,
    _In_ std::shared_ptr<achievement_media_asset_internal> mediaAsset
    ) :
    m_name(std::move(name)),
    m_description(std::move(description)),
    m_value(std::move(value)),
    m_rewardType(std::move(rewardType)),
    m_valuePropertyType(std::move(valuePropertyType)),
    m_mediaAssetInternal(std::move(mediaAsset)),
    m_mediaAsset(m_mediaAssetInternal)
{
}

const xsapi_internal_string&
achievement_reward_internal::name() const
{
    return m_name;
}

const xsapi_internal_string&
achievement_reward_internal::description() const
{
    return m_description;
}

const xsapi_internal_string&
achievement_reward_internal::value() const
{
    return m_value;
}

achievement_reward_type
achievement_reward_internal::reward_type() const
{
    return m_rewardType;
}

const xsapi_internal_string&
achievement_reward_internal::value_type() const
{
    return m_valuePropertyType;
}

const achievement_media_asset&
achievement_reward_internal::media_asset() const
{
    return m_mediaAsset;
}

std::shared_ptr<achievement_media_asset_internal>
achievement_reward_internal::media_asset_internal() const
{
    return m_mediaAssetInternal;
}

xbox_live_result<std::shared_ptr<achievement_reward_internal>>
achievement_reward_internal::_Deserialize(
    _In_ const web::json::value& json
    )
{
    if (json.is_null()) return xbox_live_result<std::shared_ptr<achievement_reward_internal>>();

    std::error_code errc = xbox_live_error_code::no_error;
    auto achievementReward = xsapi_allocate_shared<achievement_reward_internal>(
        utils::extract_json_string(json, "name", errc, true),
        utils::extract_json_string(json, "description", errc, true),
        utils::extract_json_string(json, "value", errc, true),
        convert_string_to_reward_type(utils::extract_json_string(json, _T("type"), errc, true)),
        utils::extract_json_string(json, "valueType", errc, true),
        utils::extract_json_object<std::shared_ptr<achievement_media_asset_internal>>(achievement_media_asset_internal::_Deserialize, json, _T("mediaAsset"), errc, true)
        );

    return xbox_live_result<std::shared_ptr<achievement_reward_internal>>(achievementReward, errc);
}

achievement_reward_type
achievement_reward_internal::convert_string_to_reward_type(
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