// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/profile.h"
#include "utils.h"
#include "user_context.h"
#include "xbox_system_factory.h"

using namespace pplx;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN

const string_t profile_service::SETTINGS_ARRAY[] = {
    _T("AppDisplayName"),
    _T("AppDisplayPicRaw"),
    _T("GameDisplayName"),
    _T("GameDisplayPicRaw"),
    _T("Gamerscore"),
    _T("Gamertag"),
    _T("SpeechAccessibility")
};

const web::json::value profile_service::SETTINGS_SERIALIZED = serialize_settings_json();

const string_t profile_service::SETTINGS_QUERY = settings_query();

profile_service::profile_service(
    _In_ std::shared_ptr<user_context> userContext,
    _In_ std::shared_ptr<xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox_live_app_config> appConfig
    ) :
    m_userContext(std::move(userContext)),
    m_xboxLiveContextSettings(std::move(xboxLiveContextSettings)),
    m_appConfig(std::move(appConfig))
{
}

pplx::task<xbox_live_result<xbox_user_profile>>
profile_service::get_user_profile()
{
    std::shared_ptr<http_call> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("GET"),
        utils::create_xboxlive_endpoint(_T("profile"), m_appConfig),
        _T("/users/me/profile/settings?Settings=") + SETTINGS_QUERY,
        xbox_live_api::get_user_profiles
    );
    httpCall->set_xbox_contract_version_header_value(_T("3"));

    auto task = httpCall->get_response_with_auth(m_userContext)
        .then([](std::shared_ptr<http_call_response> response)
    {
        if (response->response_body_json().size() != 0)
        {
            std::error_code errc = xbox_live_error_code::no_error;
            auto profileVector = utils::extract_xbox_live_result_json_vector<xbox_user_profile>(
                xbox_user_profile::_Deserialize,
                response->response_body_json(),
                _T("profileUsers"),
                errc,
                true
                );

            if (profileVector.payload().size() == 1)
            {
                return xbox_live_result<xbox_user_profile>(profileVector.payload()[0], profileVector.err(), profileVector.err_message());
            }
            else
            {
                return xbox_live_result<xbox_user_profile>(profileVector.err(), profileVector.err_message());
            }
        }
        else
        {
            return xbox_live_result<xbox_user_profile>(response->err_code(), response->err_message());
        }
    });

    return utils::create_exception_free_task<xbox_user_profile>(
        task
        );

}

pplx::task<xbox_live_result<xbox_user_profile>>
profile_service::get_user_profile(
    _In_ string_t xboxUserId
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(xboxUserId.empty(), xbox_user_profile, "xboxUserId is empty");
    std::vector< string_t> xboxUserIds;
    xboxUserIds.push_back(std::move(xboxUserId));

    auto task = get_user_profiles(xboxUserIds)
    .then([](xbox_live_result<std::vector<xbox_user_profile>> result)
    {
        if (result.payload().size() == 1)
        {
            return xbox_live_result<xbox_user_profile>(result.payload()[0], result.err(), result.err_message());
        }
        else
        {
            return xbox_live_result<xbox_user_profile>(result.err(), result.err_message());
        }
    });
    
    return utils::create_exception_free_task<xbox_user_profile>(
        task
        );
}


pplx::task<xbox_live_result<std::vector<xbox_user_profile>>>
profile_service::get_user_profiles(
    _In_ const std::vector< string_t >& xboxUserIds
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(xboxUserIds.size() == 0, std::vector<xbox_user_profile>, "xbox user ids size is 0");
    for (string_t s : xboxUserIds)
    {
        RETURN_TASK_CPP_INVALIDARGUMENT_IF(s.empty(), std::vector<xbox_user_profile>, "Found empty string in xbox user ids");
    }

    std::shared_ptr<http_call> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("POST"),
        utils::create_xboxlive_endpoint(_T("profile"), m_appConfig),
        _T("/users/batch/profile/settings"),
        xbox_live_api::get_user_profiles
        );
    httpCall->set_xbox_contract_version_header_value(_T("3"));

    web::json::value request;
    request[_T("userIds")] = utils::serialize_vector<string_t>(utils::json_string_serializer, xboxUserIds);
    request[_T("settings")] = SETTINGS_SERIALIZED;

    httpCall->set_request_body(request.serialize());

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([](std::shared_ptr<http_call_response> response) 
    {
        if (response->response_body_json().size() != 0)
        {
            std::error_code errc = xbox_live_error_code::no_error;
            auto profileVector = utils::extract_xbox_live_result_json_vector<xbox_user_profile>(
                xbox_user_profile::_Deserialize,
                response->response_body_json(), 
                _T("profileUsers"),
                errc,
                true
                );

            return utils::generate_xbox_live_result<std::vector<xbox_user_profile>>(
                profileVector,
                response
                );
        }
        else
        {
            return xbox_live_result<std::vector<xbox_user_profile>>(response->err_code(), response->err_message());
        }
    });

    return utils::create_exception_free_task<std::vector<xbox_user_profile>>(
        task
        );
}

pplx::task<xbox_live_result<std::vector<xbox_user_profile>>> 
profile_service::get_user_profiles_for_social_group(
    _In_ const string_t& socialGroup
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(socialGroup.empty(), std::vector<xbox_user_profile>, "socialGroup is empty");

    string_t pathAndQuery = pathandquery_user_profiles_for_social_group(
        socialGroup
        );

    std::shared_ptr<http_call> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("GET"),
        utils::create_xboxlive_endpoint(_T("profile"), m_appConfig),
        pathAndQuery,
        xbox_live_api::get_user_profiles_for_social_group
        );
    httpCall->set_xbox_contract_version_header_value(_T("2"));

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([](std::shared_ptr<http_call_response> response)
    {
        if (response->response_body_json().size() != 0) // I don't think this is possible with this call?
        {
            std::error_code errc = xbox_live_error_code::no_error;
            auto profileVector = utils::extract_xbox_live_result_json_vector<xbox_user_profile>(
                xbox_user_profile::_Deserialize,
                response->response_body_json(),
                _T("profileUsers"),
                errc,
                true
                );

            return utils::generate_xbox_live_result<std::vector<xbox_user_profile>>(
                profileVector,
                response
                );
        }
        else
        {
            return xbox_live_result<std::vector<xbox_user_profile>>(response->err_code(), response->err_message());
        }
    });

    return utils::create_exception_free_task<std::vector<xbox_user_profile>>(
        task
        );
}

const string_t
profile_service::pathandquery_user_profiles_for_social_group(
    _In_ const string_t& socialGroup
    )
{
    stringstream_t source;
    source << _T("/users/me/profile/settings/people/");
    source << socialGroup;
    source << _T("?settings=");
    source << SETTINGS_QUERY;

    return source.str();
}

const string_t
profile_service::settings_query()
{
    stringstream_t source;
    uint32_t arraySize = ARRAYSIZE(SETTINGS_ARRAY);
    for (uint32_t i = 0; i < arraySize; ++i)
    {
        const string_t& setting = SETTINGS_ARRAY[i];
        source << web::http::uri::encode_uri(setting);
        if (i + 1 != arraySize)
        {
            source << _T(",");
        }
    }

    return source.str();
}

web::json::value
profile_service::serialize_settings_json()
{
    std::vector<string_t> settingsVector(SETTINGS_ARRAY, SETTINGS_ARRAY + sizeof(SETTINGS_ARRAY) / sizeof(SETTINGS_ARRAY[0]));
    return utils::serialize_vector<string_t>(utils::json_string_serializer, settingsVector);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END