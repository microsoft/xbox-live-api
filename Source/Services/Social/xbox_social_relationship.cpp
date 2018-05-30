// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/social.h"
#include "social_internal.h"
#include "utils.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN

xbox_social_relationship::xbox_social_relationship(
    _In_ std::shared_ptr<xbox_social_relationship_internal> internalObj
    ) :
    m_internalObj(std::move(internalObj))
{
}

DEFINE_GET_STRING(xbox_social_relationship, xbox_user_id);
DEFINE_GET_BOOL(xbox_social_relationship, is_favorite);
DEFINE_GET_BOOL(xbox_social_relationship, is_following_caller);
DEFINE_GET_STRING_VECTOR(xbox_social_relationship, social_networks);

xbox_social_relationship_internal::xbox_social_relationship_internal(
    _In_ xsapi_internal_string xboxUserId,
    _In_ bool isFavorite,
    _In_ bool isFollowingCaller,
    _In_ xsapi_internal_vector<xsapi_internal_string> socialNetworks
    ) :
    m_xboxUserId(std::move(xboxUserId)),
    m_isFavorite(isFavorite),
    m_isFollowingCaller(isFollowingCaller),
    m_socialNetworks(std::move(socialNetworks))
{
}

const xsapi_internal_string& xbox_social_relationship_internal::xbox_user_id() const
{
    return m_xboxUserId;
}

bool xbox_social_relationship_internal::is_favorite() const
{
    return m_isFavorite;
}

bool xbox_social_relationship_internal::is_following_caller() const
{
    return m_isFollowingCaller;
}

const xsapi_internal_vector<xsapi_internal_string>& xbox_social_relationship_internal::social_networks() const
{
    return m_socialNetworks;
}

xbox_live_result<std::shared_ptr<xbox_social_relationship_internal>>
xbox_social_relationship_internal::deserialize(_In_ const web::json::value& json)
{
    if (json.is_null()) return xbox_live_result<std::shared_ptr<xbox_social_relationship_internal>>();

    std::error_code errc = xbox_live_error_code::no_error;

    auto result = xsapi_allocate_shared<xbox_social_relationship_internal>(
        utils::extract_json_string(json, "xuid", errc, true),
        utils::extract_json_bool(json, "isFavorite", errc),
        utils::extract_json_bool(json, "isFollowingCaller", errc),
        utils::extract_json_vector<xsapi_internal_string>(utils::json_internal_string_extractor, json, "socialNetworks", errc, false)
        );

    return xbox_live_result<std::shared_ptr<xbox_social_relationship_internal>>(result, errc);
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END