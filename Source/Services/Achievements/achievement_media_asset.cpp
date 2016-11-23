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

achievement_media_asset::achievement_media_asset()
{
}

achievement_media_asset::achievement_media_asset(
    _In_ string_t name,
    _In_ achievement_media_asset_type type,
    _In_ web::uri url
    ) :
    m_name(std::move(name)),
    m_type(type),
    m_url(std::move(url))
{
}

const string_t&
achievement_media_asset::name() const
{
    return m_name;
}

achievement_media_asset_type
achievement_media_asset::media_asset_type() const
{
    return m_type;
}

const web::uri&
achievement_media_asset::url() const
{
    return m_url;
}

achievement_media_asset_type
achievement_media_asset::_Convert_string_to_media_asset_type(
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

xbox_live_result<achievement_media_asset>
achievement_media_asset::_Deserialize(
_In_ const web::json::value& json
)
{
    if (json.is_null()) return xbox_live_result<achievement_media_asset>();

    std::error_code errc = xbox_live_error_code::no_error;
    auto achievementMediaAsset = achievement_media_asset(
        utils::extract_json_string(json, _T("name"), errc, true),
        _Convert_string_to_media_asset_type(utils::extract_json_string(json, _T("type"), errc, true)),
        utils::extract_json_string(json, _T("url"), errc, true)
        );

    return xbox_live_result<achievement_media_asset>(achievementMediaAsset, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_END