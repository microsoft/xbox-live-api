// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include <mutex>

#if !XSAPI_CPP
#include "User_WinRT.h"
#else
#include "xsapi/system.h"
#endif

namespace xbox { namespace services {

class xbox_live_context_server_impl : public std::enable_shared_from_this < xbox_live_context_server_impl >
{
public:

    xbox_live_context_server_impl(
        _In_ std::shared_ptr<system::xbox_live_server> server
        );

    std::shared_ptr<system::xbox_live_server> server();

    /// <summary>
    /// Returns an object containing settings that apply to all REST calls made such as retry and diagnostic settings.
    /// </summary>
    std::shared_ptr<xbox_live_context_settings> settings();

    /// <summary>
    /// Returns an object containing Xbox Live app config such as title ID
    /// </summary>
    std::shared_ptr<xbox_live_app_config> application_config();

    /// <summary>
    /// Returns the current user context.
    /// </summary>
    std::shared_ptr<user_context> user_context();

    void init();

private:
    std::shared_ptr<xbox::services::user_context> m_userContext;
    std::shared_ptr<xbox::services::xbox_live_context_settings> m_xboxLiveContextSettings;
    std::shared_ptr<xbox_live_app_config> m_appConfig;
};

}}


