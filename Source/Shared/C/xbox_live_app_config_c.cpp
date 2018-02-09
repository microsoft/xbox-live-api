// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi-c/xbox_live_app_config_c.h"
#include "xbox_live_app_config_internal.h"

using namespace xbox::services;

XBL_API XBL_RESULT XBL_CALLING_CONV
XblGetXboxLiveAppConfigSingleton(
    _Out_ CONST XBL_XBOX_LIVE_APP_CONFIG** ppConfig
    ) XBL_NOEXCEPT
try
{
    if (ppConfig == nullptr)
    {
        return XBL_RESULT_INVALID_ARG;
    }

    auto singleton = get_xsapi_singleton();
    std::lock_guard<std::mutex> lock(singleton->m_singletonLock);

    if (singleton->m_cAppConfigSingleton == nullptr)
    {
        singleton->m_cAppConfigSingleton = xsapi_allocate_shared<XBL_XBOX_LIVE_APP_CONFIG>();
        auto internalConfig = xbox_live_app_config_internal::get_app_config_singleton();

        singleton->m_cAppConfigSingleton->environment = internalConfig->environment().data();
        singleton->m_cAppConfigSingleton->scid = internalConfig->scid().data();
        singleton->m_cAppConfigSingleton->sandbox = internalConfig->sandbox().data();
        singleton->m_cAppConfigSingleton->titleId = internalConfig->title_id();
    }
    *ppConfig = singleton->m_cAppConfigSingleton.get();

    return XBL_RESULT_OK;
}
CATCH_RETURN()