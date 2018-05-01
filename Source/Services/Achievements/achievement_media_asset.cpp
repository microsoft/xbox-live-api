// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "shared_macros.h"
#include "xsapi/achievements.h"
#include "achievements_internal.h"
#include "utils.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_BEGIN

achievement_media_asset::achievement_media_asset()
{
}

achievement_media_asset::achievement_media_asset(
    std::shared_ptr<achievement_media_asset_internal> internalObj
    ) :
    m_internalObj(std::move(internalObj))
{
}

DEFINE_GET_STRING(achievement_media_asset, name);
DEFINE_GET_OBJECT_REF(achievement_media_asset, achievement_media_asset_type, media_asset_type);
DEFINE_GET_URI(achievement_media_asset, url);

achievement_media_asset_internal::achievement_media_asset_internal(
    _In_ xsapi_internal_string name,
    _In_ achievement_media_asset_type type,
    _In_ web::uri url
    ) :
    m_name(std::move(name)),
    m_type(type),
    m_url(std::move(url))
{
}

const xsapi_internal_string&
achievement_media_asset_internal::name() const
{
    return m_name;
}

const achievement_media_asset_type&
achievement_media_asset_internal::media_asset_type() const
{
    return m_type;
}

const web::uri&
achievement_media_asset_internal::url() const
{
    return m_url;
}

achievement_media_asset_type
achievement_media_asset_internal::_Convert_string_to_media_asset_type(
    _In_ const string_t& value
    )
{
    if (utils::str_icmp(value, _T("Icon")) == 0)
    {
        return achievement_media_asset_type::icon;
    }
    else if (utils::str_icmp(value, _T("Art")) == 0)
    {
        return achievement_media_asset_type::art;
    }

    return achievement_media_asset_type::unknown;
}

xbox_live_result<std::shared_ptr<achievement_media_asset_internal>>
achievement_media_asset_internal::_Deserialize(
_In_ const web::json::value& json
)
{
    if (json.is_null()) return xbox_live_result<std::shared_ptr<achievement_media_asset_internal>>();

    std::error_code errc = xbox_live_error_code::no_error;
    auto achievementMediaAsset = xsapi_allocate_shared<achievement_media_asset_internal>(
        utils::extract_json_string(json, "name", errc, true),
        _Convert_string_to_media_asset_type(utils::extract_json_string(json, _T("type"), errc, true)),
        utils::extract_json_string(json, _T("url"), errc, true)
        );

    return xbox_live_result<std::shared_ptr<achievement_media_asset_internal>>(achievementMediaAsset, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_END