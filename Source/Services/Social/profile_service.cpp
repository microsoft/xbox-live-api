// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/profile.h"
#include "utils.h"
#include "user_context.h"
#include "xbox_system_factory.h"
#include "profile_internal.h"

using namespace pplx;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN

const string_t profile_service::SETTINGS_ARRAY[] = {
    _T("AppDisplayName"),
    _T("AppDisplayPicRaw"),
    _T("GameDisplayName"),
    _T("GameDisplayPicRaw"),
    _T("Gamerscore"),
    _T("Gamertag")
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
    m_appConfig(std::move(appConfig)) // TODO this should eventually only be needed in the internal impl class
{
    m_serviceImpl = std::make_shared<profile_service_impl>(
        m_userContext,
        m_xboxLiveContextSettings,
        m_appConfig);
}

profile_service::profile_service(
    _In_ std::shared_ptr<profile_service_impl> serviceImpl,
    _In_ std::shared_ptr<user_context> userContext,
    _In_ std::shared_ptr<xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox_live_app_config> appConfig
    ) :
    m_serviceImpl(std::move(serviceImpl)),
    m_userContext(std::move(userContext)),
    m_xboxLiveContextSettings(std::move(xboxLiveContextSettings)),
    m_appConfig(std::move(appConfig)) // TODO this should eventually only be needed in the internal impl class
{
}

pplx::task<xbox_live_result<xbox_user_profile>>
profile_service::get_user_profile(
    _In_ string_t xboxUserId
    )
{
    // Maybe create some global store of these
    auto context = new task_completion_event<xbox_live_result<xbox_user_profile>>();

    auto result = m_serviceImpl->get_user_profile(xboxUserId, 
        [](xbox_live_result<xbox_user_profile> result, void* context) 
    {
        auto tce = static_cast<task_completion_event<xbox_live_result<xbox_user_profile>>*>(context);
        tce->set(result);
        delete context;
    }, context, XSAPI_DEFAULT_TASKGROUP);

    if (result.err())
    {
        delete context;
        return pplx::task_from_result(xbox_live_result<xbox_user_profile>(result.err(), result.err_message()));
    }
    return pplx::task<xbox_live_result<xbox_user_profile>>(*context);
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
    httpCall->set_xbox_contract_version_header_value(_T("2"));

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