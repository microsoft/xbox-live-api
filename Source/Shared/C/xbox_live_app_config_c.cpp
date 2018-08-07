// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi-c/xbox_live_app_config_c.h"
#include "xbox_live_app_config_internal.h"

using namespace xbox::services;

STDAPI
XblGetXboxLiveAppConfig(
    _Out_ const XblAppConfig** appConfig
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF_NULL(appConfig);

    auto internalConfig = xbox_live_app_config_internal::get_app_config_singleton();
    auto singleton = get_xsapi_singleton();
    std::lock_guard<std::mutex> lock(singleton->m_singletonLock);

    if (singleton->m_cAppConfigSingleton == nullptr)
    {
        singleton->m_cAppConfigSingleton = xsapi_allocate_shared<XblAppConfig>();

        singleton->m_cAppConfigSingleton->environment = internalConfig->environment().data();
        singleton->m_cAppConfigSingleton->scid = internalConfig->scid().data();
        singleton->m_cAppConfigSingleton->sandbox = internalConfig->sandbox().data();
        singleton->m_cAppConfigSingleton->titleId = internalConfig->title_id();
    }
    *appConfig = singleton->m_cAppConfigSingleton.get();

    return S_OK;
}
CATCH_RETURN()

STDAPI
XblDisableAssertsForXboxLiveThrottlingInDevSandboxes(
    _In_ XblConfigSetting setting
    ) XBL_NOEXCEPT
try
{
    UNREFERENCED_PARAMETER(setting);
    auto appConfig = xbox_live_app_config_internal::get_app_config_singleton();
    appConfig->disable_asserts_for_xbox_live_throttling_in_dev_sandboxes(xbox_live_context_throttle_setting::this_code_needs_to_be_changed_to_avoid_throttling);
    return S_OK;
}
CATCH_RETURN()

STDAPI
XblDisableAssertsForMaximumNumberOfWebsocketsActivated(
    _In_ XblConfigSetting setting
    ) XBL_NOEXCEPT
try
{
    UNREFERENCED_PARAMETER(setting);
    auto appConfig = xbox_live_app_config_internal::get_app_config_singleton();
    appConfig->disable_asserts_for_maximum_number_of_websockets_activated(xbox_live_context_recommended_setting::this_code_needs_to_be_changed_to_follow_best_practices);
    return S_OK;
}
CATCH_RETURN()