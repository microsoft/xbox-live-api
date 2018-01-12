// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi/profile.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN

class profile_service_impl : public std::enable_shared_from_this<profile_service_impl>
{
public:
    profile_service_impl() {}

    profile_service_impl(
        _In_ std::shared_ptr<user_context> userContext,
        _In_ std::shared_ptr<xbox_live_context_settings> xboxLiveContextSettings,
        _In_ std::shared_ptr<xbox_live_app_config> appConfig
        );

    typedef void(*get_user_profile_completion_routine)(
        _In_ xbox::services::xbox_live_result<xbox_user_profile> result,
        _In_opt_ void* context
        );

    // TODO these should use mem hooked types
    _XSAPIIMP xbox::services::xbox_live_result<void> get_user_profile(
        _In_ string_t xboxUserId,
        _In_ uint64_t taskGroupId,
        _In_ get_user_profile_completion_routine completionRoutine,
        _In_opt_ void* completionRoutineContext
        );

    typedef void(*get_user_profiles_completion_routine)(
        _In_ xbox::services::xbox_live_result<std::vector<xbox_user_profile>> result,
        _In_opt_ void* context
        );

    _XSAPIIMP xbox::services::xbox_live_result<void> get_user_profiles(
        _In_ const std::vector<string_t>& xboxUserIds,
        _In_ uint64_t taskGroupId,
        _In_ get_user_profiles_completion_routine completionRoutine,
        _In_opt_ void* completionRoutineContext
        );

    _XSAPIIMP xbox::services::xbox_live_result<void> get_user_profiles_for_social_group(
        _In_ const string_t& socialGroup,
        _In_ uint64_t taskGroupId,
        _In_ get_user_profiles_completion_routine completionRoutine,
        _In_opt_ void* completionRoutineContext
        );

private:
    static void handle_get_user_profiles_response(
        std::shared_ptr<http_call_response> response,
        void *context
        );

    static const string_t settings_query();

    static const string_t pathandquery_user_profiles_for_social_group(
        _In_ const string_t& socialGroup
        );

    static web::json::value serialize_settings_json();

    static const string_t SETTINGS_ARRAY[];

    static const web::json::value SETTINGS_SERIALIZED;

    static const string_t SETTINGS_QUERY;

    std::shared_ptr<xbox::services::user_context> m_userContext;
    std::shared_ptr<xbox::services::xbox_live_context_settings> m_xboxLiveContextSettings;
    std::shared_ptr<xbox::services::xbox_live_app_config> m_appConfig;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END