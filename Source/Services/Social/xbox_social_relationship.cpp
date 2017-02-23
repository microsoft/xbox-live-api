// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/social.h"
#include "xbox_system_factory.h"
#include "utils.h"
#include "user_context.h"
#include "utils.h"

using namespace pplx;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN

xbox_social_relationship::xbox_social_relationship():
    m_isFavorite(false),
    m_isFollowingCaller(false)
{
}

xbox_social_relationship::xbox_social_relationship(
    _In_ string_t xboxUserId,
    _In_ bool isFavorite,
    _In_ bool isFollowingCaller,
    _In_ std::vector<string_t> socialNetworks
    ) : 
    m_xboxUserId(std::move(xboxUserId)),
    m_isFavorite(isFavorite),
    m_isFollowingCaller(isFollowingCaller),
    m_socialNetworks(std::move(socialNetworks))
{
}

const string_t& xbox_social_relationship::xbox_user_id() const
{
    return m_xboxUserId;
}

bool xbox_social_relationship::is_favorite() const
{
    return m_isFavorite;
}

bool xbox_social_relationship::is_following_caller() const
{
    return m_isFollowingCaller;
}

const std::vector< string_t >& xbox_social_relationship::social_networks() const
{
    return m_socialNetworks;
}

xbox_live_result<xbox_social_relationship>
xbox_social_relationship::_Deserialize(_In_ const web::json::value& json)
{
    if (json.is_null()) return xbox_live_result<xbox_social_relationship>();

    std::error_code errc = xbox_live_error_code::no_error;

    auto result = xbox_social_relationship(
        utils::extract_json_string(json, _T("xuid"), errc, true),
        utils::extract_json_bool(json, _T("isFavorite"), errc),
        utils::extract_json_bool(json, _T("isFollowingCaller"), errc),
        utils::extract_json_vector<string_t>(utils::json_string_extractor, json, _T("socialNetworks"), errc, false)
        );

    return xbox_live_result<xbox_social_relationship>(result, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END