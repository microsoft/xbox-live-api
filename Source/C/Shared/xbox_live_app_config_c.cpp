// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi-c/xbox_live_app_config_c.h"

using namespace xbox::services;

XBL_API XBL_RESULT XBL_CALLING_CONV
GetXboxLiveAppConfigSingleton(
    _Out_ CONST XSAPI_XBOX_LIVE_APP_CONFIG** ppConfig
    ) XBL_NOEXCEPT
try
{
    if (ppConfig == nullptr)
    {
        return XBL_RESULT_E_HC_INVALIDARG;
    }
    
    auto singleton = get_xsapi_singleton();
    std::lock_guard<std::mutex> lock(singleton->m_singletonLock);

    if (singleton->m_appConfigSingletonC == nullptr)
    {
        singleton->m_appConfigSingletonC = std::make_shared<XSAPI_XBOX_LIVE_APP_CONFIG>();

        auto cppConfig = xbox::services::xbox_live_app_config::get_app_config_singleton();

        singleton->m_scid = utils::utf8_from_utf16(cppConfig->scid());
        singleton->m_appConfigSingletonC->scid = singleton->m_scid.data();

        singleton->m_environment = utils::utf8_from_utf16(cppConfig->environment());
        singleton->m_appConfigSingletonC->environment = singleton->m_environment.data();

        singleton->m_sandbox = utils::utf8_from_utf16(cppConfig->sandbox());
        singleton->m_appConfigSingletonC->sandbox = singleton->m_sandbox.data();

        singleton->m_appConfigSingletonC->titleId = cppConfig->title_id();
    }
    *ppConfig = singleton->m_appConfigSingletonC.get();

    return XBL_RESULT_OK;
}
CATCH_RETURN()