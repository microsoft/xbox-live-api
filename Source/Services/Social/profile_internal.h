// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi/profile.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN

class xbox_user_profile_internal
{
public:
    _XSAPIIMP xbox_user_profile_internal(
        _In_ xsapi_internal_string appDisplayName,
        _In_ web::uri appDisplayPictureResizeUri,
        _In_ xsapi_internal_string gameDisplayName,
        _In_ web::uri gameDisplayPictureResizeUri,
        _In_ xsapi_internal_string gamerscore,
        _In_ xsapi_internal_string gamertag,
        _In_ xsapi_internal_string xboxUserId
        );

    const xsapi_internal_string& app_display_name() const;

    const web::uri& app_display_picture_resize_uri() const;

    const xsapi_internal_string& game_display_name() const;

    const web::uri& game_display_picture_resize_uri() const;

    const xsapi_internal_string& gamerscore() const;

    const xsapi_internal_string& gamertag() const;

    const xsapi_internal_string& xbox_user_id() const;

    static xbox_live_result<std::shared_ptr<xbox_user_profile_internal>> deserialize(_In_ const web::json::value& json);

private:
    xsapi_internal_string m_appDisplayName;
    web::uri m_appDisplayPictureResizeUri;
    xsapi_internal_string m_gameDisplayName;
    web::uri m_gameDisplayPictureResizeUri;
    xsapi_internal_string m_gamerscore;
    xsapi_internal_string m_gamertag;
    xsapi_internal_string m_xboxUserId;
};

//typedef XblAsyncBlock<xbox_live_result<std::shared_ptr<xbox_user_profile_internal>>> GetProfileAsyncBlock;
//typedef XblAsyncBlock<xbox_live_result<xsapi_internal_vector<std::shared_ptr<xbox_user_profile_internal>>>> GetProfilesAsyncBlock;

class profile_service_impl : public std::enable_shared_from_this<profile_service_impl>
{
public:
    profile_service_impl(
        _In_ std::shared_ptr<user_context> userContext,
        _In_ std::shared_ptr<xbox_live_context_settings> xboxLiveContextSettings,
        _In_ std::shared_ptr<xbox_live_app_config_internal> appConfig
        );

    _XSAPIIMP xbox::services::xbox_live_result<void> get_user_profile(
        _In_ xsapi_internal_string xboxUserId,
        _In_opt_ async_queue_handle_t queue,
        _In_ xbox_live_callback<xbox_live_result<std::shared_ptr<xbox_user_profile_internal>>> callback
        );

    _XSAPIIMP xbox::services::xbox_live_result<void> get_user_profiles(
        _In_ const xsapi_internal_vector<xsapi_internal_string>& xboxUserIds,
        _In_opt_ async_queue_handle_t queue,
        _In_ xbox_live_callback<xbox_live_result<xsapi_internal_vector<std::shared_ptr<xbox_user_profile_internal>>>> callback
        );

    _XSAPIIMP xbox::services::xbox_live_result<void> get_user_profiles_for_social_group(
        _In_ const xsapi_internal_string& socialGroup,
        _In_opt_ async_queue_handle_t queue,
        _In_ xbox_live_callback<xbox_live_result<xsapi_internal_vector<std::shared_ptr<xbox_user_profile_internal>>>> callback
        );

private:
    static void handle_get_user_profiles_response(
        _In_ std::shared_ptr<http_call_response_internal> response,
        _In_ xbox_live_callback<xbox::services::xbox_live_result<xsapi_internal_vector<std::shared_ptr<xbox_user_profile_internal>>>> callback
        );

    static const xsapi_internal_string settings_query();

    static const xsapi_internal_string pathandquery_user_profiles_for_social_group(
        _In_ const xsapi_internal_string& socialGroup
        );

    static web::json::value serialize_settings_json();

    static const xsapi_internal_string SETTINGS_ARRAY[];

    static const web::json::value SETTINGS_SERIALIZED;

    static const xsapi_internal_string SETTINGS_QUERY;

    std::shared_ptr<xbox::services::user_context> m_userContext;
    std::shared_ptr<xbox::services::xbox_live_context_settings> m_xboxLiveContextSettings;
    std::shared_ptr<xbox::services::xbox_live_app_config_internal> m_appConfig;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END