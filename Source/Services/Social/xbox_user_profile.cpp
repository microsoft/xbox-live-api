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
    _In_ string_t xboxUserId
    ) :
    m_appDisplayName(utils::internal_string_from_external_string(appDisplayName)),
    m_appDisplayPictureResizeUri(std::move(appDisplayPictureResizeUri)),
    m_gameDisplayName(utils::internal_string_from_external_string(gameDisplayName)),
    m_gameDisplayPictureResizeUri(std::move(gameDisplayPictureResizeUri)),
    m_gamerscore(utils::internal_string_from_external_string(gamerscore)),
    m_gamertag(utils::internal_string_from_external_string(gamertag)),
    m_xboxUserId(utils::internal_string_from_external_string(xboxUserId))
{
}

xbox_user_profile::xbox_user_profile(
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

string_t xbox_user_profile::app_display_name() const
{
    return utils::external_string_from_internal_string(m_appDisplayName);
}

const web::uri& xbox_user_profile::app_display_picture_resize_uri() const
{
    return m_appDisplayPictureResizeUri;
}

string_t xbox_user_profile::game_display_name() const
{
    return utils::external_string_from_internal_string(m_gameDisplayName);
}

const web::uri& xbox_user_profile::game_display_picture_resize_uri() const
{
    return m_gameDisplayPictureResizeUri;
}

string_t xbox_user_profile::gamerscore() const
{
    return utils::external_string_from_internal_string(m_gamerscore);
}

string_t xbox_user_profile::gamertag() const
{
    return utils::external_string_from_internal_string(m_gamertag);
}

string_t xbox_user_profile::xbox_user_id() const
{
    return utils::external_string_from_internal_string(m_xboxUserId);
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
            appDisplayPictureResizeUri = utils::external_string_from_internal_string(stringValue); // TODO don't double convert here
        }
        else if (name == "GameDisplayName")
        {
            gameDisplayName = std::move(stringValue);
        }
        else if (name == "GameDisplayPicRaw")
        {
            gameDisplayPictureResizeUri = utils::external_string_from_internal_string(stringValue); // TODO don't double convert here
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

    auto result = xbox_user_profile(
        std::move(appDisplayName),
        std::move(appDisplayPictureResizeUri),
        std::move(gameDisplayName),
        std::move(gameDisplayPictureResizeUri),
        std::move(gamerscore),
        std::move(gamertag),
        std::move(xboxUserId)
        );

    return xbox_live_result<xbox_user_profile>(result, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END