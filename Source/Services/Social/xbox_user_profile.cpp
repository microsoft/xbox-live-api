// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "shared_macros.h"
#include "xsapi/system.h"
#include "xsapi/profile.h"
#include "profile_internal.h"
#include "utils.h"

using namespace xbox::services;
using namespace xbox::services::system;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN

xbox_user_profile::xbox_user_profile()
{
}

xbox_user_profile::xbox_user_profile(
    _In_ std::shared_ptr<xbox_user_profile_internal> internalObj
    ) :
    m_internalObj(std::move(internalObj))
{
}

DEFINE_GET_STRING(xbox_user_profile, app_display_name);
DEFINE_GET_URI(xbox_user_profile, app_display_picture_resize_uri);
DEFINE_GET_STRING(xbox_user_profile, game_display_name);
DEFINE_GET_URI(xbox_user_profile, game_display_picture_resize_uri);
DEFINE_GET_STRING(xbox_user_profile, gamerscore);
DEFINE_GET_STRING(xbox_user_profile, gamertag);
DEFINE_GET_STRING(xbox_user_profile, xbox_user_id);

xbox_user_profile_internal::xbox_user_profile_internal(
    _In_ xsapi_internal_string appDisplayName,
    _In_ web::uri appDisplayPictureResizeUri,
    _In_ xsapi_internal_string gameDisplayName,
    _In_ web::uri gameDisplayPictureResizeUri,
    _In_ xsapi_internal_string gamerscore,
    _In_ xsapi_internal_string gamertag,
    _In_ xsapi_internal_string xboxUserId
    ) :
    m_appDisplayName(std::move(appDisplayName)),
    m_appDisplayPictureResizeUri(std::move(appDisplayPictureResizeUri)),
    m_gameDisplayName(std::move(gameDisplayName)),
    m_gameDisplayPictureResizeUri(std::move(gameDisplayPictureResizeUri)),
    m_gamerscore(std::move(gamerscore)),
    m_gamertag(std::move(gamertag)),
    m_xboxUserId(std::move(xboxUserId))
{
}

const xsapi_internal_string& xbox_user_profile_internal::app_display_name() const
{
    return m_appDisplayName;
}

const web::uri& xbox_user_profile_internal::app_display_picture_resize_uri() const
{
    return m_appDisplayPictureResizeUri;
}

const xsapi_internal_string& xbox_user_profile_internal::game_display_name() const
{
    return m_gameDisplayName;
}

const web::uri& xbox_user_profile_internal::game_display_picture_resize_uri() const
{
    return m_gameDisplayPictureResizeUri;
}

const xsapi_internal_string& xbox_user_profile_internal::gamerscore() const
{
    return m_gamerscore;
}

const xsapi_internal_string& xbox_user_profile_internal::gamertag() const
{
    return m_gamertag;
}

const xsapi_internal_string& xbox_user_profile_internal::xbox_user_id() const
{
    return m_xboxUserId;
}

xbox_live_result<std::shared_ptr<xbox_user_profile_internal>>
xbox_user_profile_internal::deserialize(
    _In_ const web::json::value& json
    )
{
    if (json.is_null()) return xbox_live_result<std::shared_ptr<xbox_user_profile_internal>>();

    std::error_code errc = xbox_live_error_code::no_error;
    web::json::value jsonSettings = utils::extract_json_field(json, _T("settings"), errc, true);

    web::json::array setttings = jsonSettings.as_array();

    xsapi_internal_string appDisplayName;
    web::uri appDisplayPictureResizeUri;
    xsapi_internal_string gameDisplayName;
    web::uri gameDisplayPictureResizeUri;
    xsapi_internal_string gamerscore;
    xsapi_internal_string gamertag;
    xsapi_internal_string xboxUserId = utils::extract_json_string(json, "id", errc, true);

    for (const auto& setting : setttings)
    {
        xsapi_internal_string name = utils::extract_json_string(setting, "id", errc, true);
        xsapi_internal_string stringValue = utils::extract_json_string(setting, "value", errc, true);

        if (name == "AppDisplayName")
        {
            appDisplayName = std::move(stringValue);
        }
        else if (name == "AppDisplayPicRaw")
        {
            appDisplayPictureResizeUri = utils::string_t_from_internal_string(stringValue);
        }
        else if (name == "GameDisplayName")
        {
            gameDisplayName = std::move(stringValue);
        }
        else if (name == "GameDisplayPicRaw")
        {
            gameDisplayPictureResizeUri = utils::string_t_from_internal_string(stringValue);
        }
        else if (name == "Gamerscore")
        {
            gamerscore = std::move(stringValue);
        }
        else if (name == "Gamertag")
        {
            gamertag = std::move(stringValue);
        }
    }

    auto result = xsapi_allocate_shared<xbox_user_profile_internal>(
        std::move(appDisplayName),
        std::move(appDisplayPictureResizeUri),
        std::move(gameDisplayName),
        std::move(gameDisplayPictureResizeUri),
        std::move(gamerscore),
        std::move(gamertag),
        std::move(xboxUserId)
        );

    return xbox_live_result<std::shared_ptr<xbox_user_profile_internal>>(result, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END