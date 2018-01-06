// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/profile.h"
#include "profile_internal.h"
#include "utils.h"
#include "user_context.h"
#include "xbox_system_factory.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN

using namespace xbox::services;

const string_t profile_service_impl::SETTINGS_ARRAY[] = {
    _T("AppDisplayName"),
    _T("AppDisplayPicRaw"),
    _T("GameDisplayName"),
    _T("GameDisplayPicRaw"),
    _T("Gamerscore"),
    _T("Gamertag")
};

const web::json::value profile_service_impl::SETTINGS_SERIALIZED = serialize_settings_json();

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
    _In_ string_t xboxUserId,
    _In_ get_user_profile_completion_routine completionRoutine,
    _In_opt_ void* completionRoutineContext,
    _In_ uint64_t taskGroupId
    )
{
    RETURN_CPP_INVALIDARGUMENT_IF(xboxUserId.empty(), void, "xboxUserId is empty");
    std::vector< string_t> xboxUserIds;
    xboxUserIds.push_back(std::move(xboxUserId));

    auto context = callback_context_helper::store_client_callback_info(completionRoutine, completionRoutineContext);
    return get_user_profiles(xboxUserIds, 
        [](xbox_live_result<std::vector<xbox_user_profile>> result, void *context)
    {
        auto clientCallbackInfo = callback_context_helper::remove_client_callback_info(context);
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
        delete context;
    }, context, taskGroupId);
}

_XSAPIIMP xbox_live_result<void> profile_service_impl::get_user_profiles(
    _In_ const std::vector<string_t>& xboxUserIds,
    _In_ get_user_profiles_completion_routine completionRoutine,
    _In_opt_ void* completionRoutineContext,
    _In_ uint64_t taskGroupId
    )
{
    RETURN_CPP_INVALIDARGUMENT_IF(xboxUserIds.size() == 0, void, "xbox user ids size is 0");
    for (string_t s : xboxUserIds)
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
    request[_T("userIds")] = utils::serialize_vector<string_t>(utils::json_string_serializer, xboxUserIds);
    request[_T("settings")] = SETTINGS_SERIALIZED;

    httpCall->set_request_body(request.serialize());

    auto context = callback_context_helper::store_client_callback_info(completionRoutine, completionRoutineContext);
    httpCall->get_response_with_auth(m_userContext, http_call_response_body_type::json_body, false,
        [](std::shared_ptr<http_call_response> response, void *context)
    {
        auto clientCallbackInfo = callback_context_helper::remove_client_callback_info(context);
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

            auto result = utils::generate_xbox_live_result<std::vector<xbox_user_profile>>(
                profileVector,
                response
                );

            completionRoutine(result, clientCallbackInfo.clientContext);
        }
        else
        {
            completionRoutine(xbox_live_result<std::vector<xbox_user_profile>>(response->err_code(), response->err_message()), clientCallbackInfo.clientContext);
        }
    }, context, 0);

    return xbox_live_result<void>();
}

web::json::value profile_service_impl::serialize_settings_json()
{
    std::vector<string_t> settingsVector(SETTINGS_ARRAY, SETTINGS_ARRAY + sizeof(SETTINGS_ARRAY) / sizeof(SETTINGS_ARRAY[0]));
    return utils::serialize_vector<string_t>(utils::json_string_serializer, settingsVector);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END