// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "user_context.h"
#include "xsapi/entertainment_profile.h"
#if TV_API || UNIT_TEST_SERVICES

NAMESPACE_MICROSOFT_XBOX_SERVICES_ENTERTAINMENT_PROFILE_CPP_BEGIN

entertainment_profile_list_service::entertainment_profile_list_service(
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
    ) :
    m_userContext(std::move(userContext)),
    m_xboxLiveContextSettings(std::move(xboxLiveContextSettings)),
    m_appConfig(std::move(appConfig)),
    m_xboxOnePins(m_userContext, m_xboxLiveContextSettings, m_appConfig)
{
}

entertainment_profile_list_xbox_one_pins& entertainment_profile_list_service::xbox_one_pins()
{
    return m_xboxOnePins;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_ENTERTAINMENT_PROFILE_CPP_END
#endif
