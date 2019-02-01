// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "shared_macros.h"
#include "xsapi/system.h"
#include "xsapi/profile.h"
#include "utils.h"

using namespace pplx;
using namespace xbox::services;
using namespace xbox::services::system;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN

xbox_user_profile::xbox_user_profile()
{
}

xbox_user_profile::xbox_user_profile(
    _In_ string_t appDisplayName,
    _In_ web::uri appDisplayPictureResizeUri,
    _In_ string_t gameDisplayName,
    _In_ web::uri gameDisplayPictureResizeUri,
    _In_ string_t gamerscore,
    _In_ string_t gamertag,
    _In_ string_t xboxUserId,
    _In_ bool narratorEnabled
    ) :
    m_appDisplayName(std::move(appDisplayName)),
    m_appDisplayPictureResizeUri(std::move(appDisplayPictureResizeUri)),
    m_gameDisplayName(std::move(gameDisplayName)),
    m_gameDisplayPictureResizeUri(std::move(gameDisplayPictureResizeUri)),
    m_gamerscore(std::move(gamerscore)),
    m_gamertag(std::move(gamertag)),
    m_xboxUserId(std::move(xboxUserId)),
    m_narratorEnabled(narratorEnabled)
{
}

const string_t& xbox_user_profile::app_display_name() const
{
    return m_appDisplayName;
}

const web::uri& xbox_user_profile::app_display_picture_resize_uri() const
{
    return m_appDisplayPictureResizeUri;
}

const string_t& xbox_user_profile::game_display_name() const
{
    return m_gameDisplayName;
}

const web::uri& xbox_user_profile::game_display_picture_resize_uri() const
{
    return m_gameDisplayPictureResizeUri;
}

const string_t& xbox_user_profile::gamerscore() const
{
    return m_gamerscore;
}

const string_t& xbox_user_profile::gamertag() const
{
    return m_gamertag;
}

const string_t& xbox_user_profile::xbox_user_id() const
{
    return m_xboxUserId;
}

bool xbox_user_profile::narratorEnabled() const
{
    return m_narratorEnabled;
}

xbox_live_result<xbox_user_profile>
xbox_user_profile::_Deserialize(
    _In_ const web::json::value& json
    )
{
    if (json.is_null()) return xbox_live_result<xbox_user_profile>();

    std::error_code errc = xbox_live_error_code::no_error;
    web::json::value jsonSettings = utils::extract_json_field(json, _T("settings"), errc, true);

    web::json::array setttings = jsonSettings.as_array();

    string_t appDisplayName;
    web::uri appDisplayPictureResizeUri;
    string_t gameDisplayName;
    web::uri gameDisplayPictureResizeUri;
    string_t gamerscore;
    string_t gamertag;
    string_t xboxUserId = utils::extract_json_string(json, _T("id"), errc, true);
    bool narratorEnabled = false;

    for (const auto& setting : setttings)
    {
        string_t name = utils::extract_json_string(setting, _T("id"), errc, true);
        string_t stringValue = utils::extract_json_string(setting, _T("value"), errc, true);

        if (name == _T("AppDisplayName"))
        {
            appDisplayName = std::move(stringValue);
        }
        else if (name == _T("AppDisplayPicRaw"))
        {
            appDisplayPictureResizeUri = std::move(stringValue);
        }
        else if (name == _T("GameDisplayName"))
        {
            gameDisplayName = std::move(stringValue);
        }
        else if (name == _T("GameDisplayPicRaw"))
        {
            gameDisplayPictureResizeUri = std::move(stringValue);
        }
        else if (name == _T("Gamerscore"))
        {
            gamerscore = std::move(stringValue);
        }
        else if (name == _T("Gamertag"))
        {
            gamertag = std::move(stringValue);
        }
        else if (name == _T("SpeechAccessibility") && !stringValue.empty())
        {
            web::json::value jsonAcessibilitySettings = web::json::value::parse(stringValue);

            narratorEnabled = utils::extract_json_bool(jsonAcessibilitySettings, _T("GameTextSS"), errc, true);
            }
    }

    auto result = xbox_user_profile(
        std::move(appDisplayName),
        std::move(appDisplayPictureResizeUri),
        std::move(gameDisplayName),
        std::move(gameDisplayPictureResizeUri),
        std::move(gamerscore),
        std::move(gamertag),
        std::move(xboxUserId),
        narratorEnabled
        );

    return xbox_live_result<xbox_user_profile>(result, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END