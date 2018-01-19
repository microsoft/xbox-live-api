// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/profile.h"
#include "profile_service_impl.h"
#include "utils.h"
#include "user_context.h"
#include "xbox_system_factory.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN

using namespace xbox::services;

const xsapi_internal_string profile_service_impl::SETTINGS_ARRAY[] = {
    "AppDisplayName",
    "AppDisplayPicRaw",
    "GameDisplayName",
    "GameDisplayPicRaw",
    "Gamerscore",
    "Gamertag"
};

const web::json::value profile_service_impl::SETTINGS_SERIALIZED = serialize_settings_json();

const xsapi_internal_string profile_service_impl::SETTINGS_QUERY = settings_query();

profile_service_impl::profile_service_impl(
    _In_ std::shared_ptr<user_context> userContext,
    _In_ std::shared_ptr<xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox_live_app_config> appConfig
    ) :
    m_userContext(std::move(userContext)),
    m_xboxLiveContextSettings(std::move(xboxLiveContextSettings)),
    m_appConfig(std::move(appConfig))
{
}

_XSAPIIMP xbox_live_result<void> profile_service_impl::get_user_profile(
    _In_ xsapi_internal_string xboxUserId,
    _In_ uint64_t taskGroupId,
    _In_ xsapi_callback<xbox_live_result<xbox_user_profile>> callback
    )
{
    RETURN_CPP_INVALIDARGUMENT_IF(xboxUserId.empty(), void, "xboxUserId is empty");
    xsapi_internal_vector<xsapi_internal_string> xboxUserIds;
    xboxUserIds.push_back(std::move(xboxUserId));

    return get_user_profiles(
        xboxUserIds,
        taskGroupId,
        [callback](xbox_live_result<xsapi_internal_vector<xbox_user_profile>> result) mutable
        {
            if (result.payload().size() == 1)
            {
                callback(xbox_live_result<xbox_user_profile>(result.payload()[0], result.err(), result.err_message()));
            }
            else
            {
                callback(xbox_live_result<xbox_user_profile>(result.err(), result.err_message()));
            }
        });

    callback(xbox::services::xbox_live_result<xbox_user_profile>());

    return xbox_live_result<void>();
}
_XSAPIIMP xbox_live_result<void> profile_service_impl::get_user_profiles(
    _In_ const xsapi_internal_vector<xsapi_internal_string>& xboxUserIds,
    _In_ uint64_t taskGroupId,
    _In_ xsapi_callback<xbox_live_result<xsapi_internal_vector<xbox_user_profile>>> callback
    )
{
    RETURN_CPP_INVALIDARGUMENT_IF(xboxUserIds.size() == 0, void, "xbox user ids size is 0");
    for (xsapi_internal_string s : xboxUserIds)
    {
        RETURN_CPP_INVALIDARGUMENT_IF(s.empty(), void, "Found empty string in xbox user ids");
    }

    std::shared_ptr<http_call> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        "POST",
        utils::create_xboxlive_endpoint("profile", m_appConfig),
        _T("/users/batch/profile/settings"),
        xbox_live_api::get_user_profiles
        );
    httpCall->set_xbox_contract_version_header_value(_T("2"));

    web::json::value request;
    request[_T("userIds")] = utils::serialize_vector<xsapi_internal_string>(utils::json_internal_string_serializer, xboxUserIds);
    request[_T("settings")] = SETTINGS_SERIALIZED;

    httpCall->set_request_body(request.serialize());

    auto httpCallImpl = std::dynamic_pointer_cast<http_call_impl>(httpCall);

    httpCallImpl->get_response_with_auth(
        m_userContext,
        http_call_response_body_type::json_body,
        false,
        taskGroupId,
        [callback](std::shared_ptr<http_call_response> response) { handle_get_user_profiles_response(response, callback); }
        );

    return xbox_live_result<void>();
}

_XSAPIIMP xbox_live_result<void> profile_service_impl::get_user_profiles_for_social_group(
    _In_ const xsapi_internal_string& socialGroup,
    _In_ uint64_t taskGroupId,
    _In_ xsapi_callback<xbox_live_result<xsapi_internal_vector<xbox_user_profile>>> callback
    )
{
    RETURN_CPP_INVALIDARGUMENT_IF(socialGroup.empty(), void, "socialGroup is empty");

    xsapi_internal_string pathAndQuery = pathandquery_user_profiles_for_social_group(
        socialGroup
        );

    std::shared_ptr<http_call> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        "GET",
        utils::create_xboxlive_endpoint("profile", m_appConfig),
        utils::external_string_from_internal_string(pathAndQuery), // TODO switch this after changing to internal uri impl
        xbox_live_api::get_user_profiles_for_social_group
        );
    httpCall->set_xbox_contract_version_header_value(_T("2"));

    auto httpCallImpl = std::dynamic_pointer_cast<http_call_impl>(httpCall);

    httpCallImpl->get_response_with_auth(
        m_userContext, 
        http_call_response_body_type::json_body, 
        false,
        taskGroupId,
        [callback](std::shared_ptr<http_call_response> response) mutable { handle_get_user_profiles_response(response, callback); }
        );

    return xbox_live_result<void>();
}

void profile_service_impl::handle_get_user_profiles_response(
    _In_ std::shared_ptr<http_call_response> response,
    _In_ xsapi_callback<xbox_live_result<xsapi_internal_vector<xbox_user_profile>>> callback
    )
{
    if (response->response_body_json().size() != 0)
    {
        // TODO change utils functions to return mem hooked types so we don't translate here
        std::error_code errc = xbox_live_error_code::no_error;
        auto profileVector = utils::extract_xbox_live_result_json_vector<xbox_user_profile>(
            xbox_user_profile::_Deserialize,
            response->response_body_json(),
            _T("profileUsers"),
            errc,
            true
            );

        auto result = utils::generate_xbox_live_result<xsapi_internal_vector<xbox_user_profile>>(
            utils::internal_vector_from_std_vector(profileVector.payload()),
            response
            );

        callback(result);
    }
    else
    {
        callback(xbox_live_result<xsapi_internal_vector<xbox_user_profile>>(response->err_code(), response->err_message()));
    }
}

const xsapi_internal_string profile_service_impl::pathandquery_user_profiles_for_social_group(
    _In_ const xsapi_internal_string& socialGroup
    )
{
    xsapi_internal_stringstream source;
    source << "/users/me/profile/settings/people/";
    source << socialGroup;
    source << "?settings=";
    source << SETTINGS_QUERY;

    return source.str();
}

const xsapi_internal_string profile_service_impl::settings_query()
{
    xsapi_internal_stringstream source;
    uint32_t arraySize = ARRAYSIZE(SETTINGS_ARRAY);
    for (uint32_t i = 0; i < arraySize; ++i)
    {
        // TODO change to all internal strings so we don't double convert here
        source << utils::internal_string_from_external_string(web::http::uri::encode_uri(utils::external_string_from_internal_string(SETTINGS_ARRAY[i])));
        if (i + 1 != arraySize)
        {
            source << _T(",");
        }
    }
    return source.str();
}

web::json::value profile_service_impl::serialize_settings_json()
{
    xsapi_internal_vector<xsapi_internal_string> settingsVector(SETTINGS_ARRAY, SETTINGS_ARRAY + sizeof(SETTINGS_ARRAY) / sizeof(SETTINGS_ARRAY[0]));
    return utils::serialize_vector<xsapi_internal_string>(utils::json_internal_string_serializer, settingsVector);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END