// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

namespace xbox { namespace services { namespace social {

typedef void (*get_user_profile_completion_routine)(
    _In_ xbox::services::xbox_live_result<xbox_user_profile> result,
    _In_opt_ void* context
    );

typedef void(*get_user_profiles_completion_routine)(
    _In_ xbox::services::xbox_live_result<std::vector<xbox_user_profile>> result,
    _In_opt_ void* context
    );

class profile_service_impl : public std::enable_shared_from_this<profile_service_impl>
{
public:
    profile_service_impl(
        _In_ std::shared_ptr<user_context> userContext,
        _In_ std::shared_ptr<xbox_live_context_settings> xboxLiveContextSettings,
        _In_ std::shared_ptr<xbox_live_app_config> appConfig
        );

    _XSAPIIMP xbox::services::xbox_live_result<void> get_user_profile(
        _In_ string_t xboxUserId,
        _In_ get_user_profile_completion_routine completionRoutine,
        _In_opt_ void* completionRoutineContext,
        _In_ uint64_t taskGroupId
        );

    _XSAPIIMP xbox::services::xbox_live_result<void> get_user_profiles(
        _In_ const std::vector<string_t>& xboxUserIds,
        _In_ get_user_profiles_completion_routine completionRoutine,
        _In_opt_ void* completionRoutineContext,
        _In_ uint64_t taskGroupId
        );

private:

    std::shared_ptr<xbox::services::user_context> m_userContext;
    std::shared_ptr<xbox::services::xbox_live_context_settings> m_xboxLiveContextSettings;
    std::shared_ptr<xbox::services::xbox_live_app_config> m_appConfig;

    static web::json::value serialize_settings_json();
    static const string_t SETTINGS_ARRAY[];
    static const web::json::value SETTINGS_SERIALIZED;
};

}}}