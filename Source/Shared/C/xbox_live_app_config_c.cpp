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

    auto singleton = get_xsapi_singleton();
    std::lock_guard<std::mutex> lock(singleton->m_singletonLock);

    if (singleton->m_cAppConfigSingleton == nullptr)
    {
        singleton->m_cAppConfigSingleton = xsapi_allocate_shared<XblAppConfig>();
        auto internalConfig = xbox_live_app_config_internal::get_app_config_singleton();

        singleton->m_cAppConfigSingleton->environment = internalConfig->environment().data();
        singleton->m_cAppConfigSingleton->scid = internalConfig->scid().data();
        singleton->m_cAppConfigSingleton->sandbox = internalConfig->sandbox().data();
        singleton->m_cAppConfigSingleton->titleId = internalConfig->title_id();
    }
    *appConfig = singleton->m_cAppConfigSingleton.get();

    return S_OK;
}
CATCH_RETURN()