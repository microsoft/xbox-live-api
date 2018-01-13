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
    _T("AppDisplayName"),
    _T("AppDisplayPicRaw"),
    _T("GameDisplayName"),
    _T("GameDisplayPicRaw"),
    _T("Gamerscore"),
    _T("Gamertag")
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
    _In_ get_user_profile_completion_routine completionRoutine,
    _In_opt_ void* completionRoutineContext
    )
{
    RETURN_CPP_INVALIDARGUMENT_IF(xboxUserId.empty(), void, "xboxUserId is empty");
    xsapi_internal_vector<xsapi_internal_string> xboxUserIds;
    xboxUserIds.push_back(std::move(xboxUserId));

    auto context = async_helpers::store_client_callback_info(completionRoutine, completionRoutineContext);
    return get_user_profiles(xboxUserIds, taskGroupId,
        [](xbox_live_result<xsapi_internal_vector<xbox_user_profile>> result, void *context)
    {
        auto clientCallbackInfo = async_helpers::remove_client_callback_info(context);
        auto completionRoutine = reinterpret_cast<get_user_profile_completion_routine>(clientCallbackInfo.completionFunction);

        if (result.payload().size() == 1)
        {
            completionRoutine(
                xbox_live_result<xbox_user_profile>(result.payload()[0], result.err(), result.err_message()), 
                clientCallbackInfo.clientContext
                );
        }
        else
        {
            completionRoutine(
                xbox_live_result<xbox_user_profile>(result.err(), result.err_message()),
                clientCallbackInfo.clientContext
                );
        }
    }, context);
}

_XSAPIIMP xbox_live_result<void> profile_service_impl::get_user_profiles(
    _In_ const xsapi_internal_vector<xsapi_internal_string>& xboxUserIds,
    _In_ uint64_t taskGroupId,
    _In_ get_user_profiles_completion_routine completionRoutine,
    _In_opt_ void* completionRoutineContext
    )
{
    RETURN_CPP_INVALIDARGUMENT_IF(xboxUserIds.size() == 0, void, "xbox user ids size is 0");
    for (xsapi_internal_string s : xboxUserIds)
    {
        RETURN_CPP_INVALIDARGUMENT_IF(s.empty(), void, "Found empty string in xbox user ids");
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
    request[_T("userIds")] = utils::serialize_vector<xsapi_internal_string>(utils::json_internal_string_serializer, xboxUserIds);
    request[_T("settings")] = SETTINGS_SERIALIZED;

    httpCall->set_request_body(request.serialize());

    auto context = async_helpers::store_client_callback_info(completionRoutine, completionRoutineContext);
    httpCall->get_response_with_auth(
        m_userContext,
        http_call_response_body_type::json_body,
        false,
        handle_get_user_profiles_response,
        context,
        taskGroupId);

    return xbox_live_result<void>();
}

_XSAPIIMP xbox_live_result<void> profile_service_impl::get_user_profiles_for_social_group(
    _In_ const xsapi_internal_string& socialGroup,
    _In_ uint64_t taskGroupId,
    _In_ get_user_profiles_completion_routine completionRoutine,
    _In_opt_ void* completionRoutineContext
    )
{
    RETURN_CPP_INVALIDARGUMENT_IF(socialGroup.empty(), void, "socialGroup is empty");

    xsapi_internal_string pathAndQuery = pathandquery_user_profiles_for_social_group(
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

    auto context = async_helpers::store_client_callback_info(completionRoutine, completionRoutineContext);
    httpCall->get_response_with_auth(
        m_userContext, 
        http_call_response_body_type::json_body, 
        false,
        handle_get_user_profiles_response,
        context, 
        taskGroupId);

    return xbox_live_result<void>();
}

void profile_service_impl::handle_get_user_profiles_response(
    std::shared_ptr<http_call_response> response,
    void *context
    )
{
    auto clientCallbackInfo = async_helpers::remove_client_callback_info(context);
    auto completionRoutine = reinterpret_cast<get_user_profiles_completion_routine>(clientCallbackInfo.completionFunction);

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

        auto result = utils::generate_xbox_live_result<xsapi_internal_vector<xbox_user_profile>>(
            profileVector,
            response
            );

        completionRoutine(result, clientCallbackInfo.clientContext);
    }
    else
    {
        completionRoutine(xbox_live_result<std::vector<xbox_user_profile>>(response->err_code(), response->err_message()), clientCallbackInfo.clientContext);
    }
}

const xsapi_internal_string profile_service_impl::pathandquery_user_profiles_for_social_group(
    _In_ const xsapi_internal_string& socialGroup
    )
{
    xsapi_internal_stringstream source;
    source << _T("/users/me/profile/settings/people/");
    source << socialGroup;
    source << _T("?settings=");
    source << SETTINGS_QUERY;

    return source.str();
}

const xsapi_internal_string profile_service_impl::settings_query()
{
    xsapi_internal_stringstream source;
    uint32_t arraySize = ARRAYSIZE(SETTINGS_ARRAY);
    for (uint32_t i = 0; i < arraySize; ++i)
    {
        // TODO change to all internal strings
        string_t setting(SETTINGS_ARRAY[i].begin(), SETTINGS_ARRAY[i].end());
        source << web::http::uri::encode_uri(setting);
        if (i + 1 != arraySize)
        {
            source << _T(",");
        }
    }
    return source.str();
}

web::json::value profile_service_impl::serialize_settings_json()
{
    std::vector<string_t> settingsVector(SETTINGS_ARRAY, SETTINGS_ARRAY + sizeof(SETTINGS_ARRAY) / sizeof(SETTINGS_ARRAY[0]));
    return utils::serialize_vector<string_t>(utils::json_string_serializer, settingsVector);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END