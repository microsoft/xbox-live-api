//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#include "shared_macros.h"
#include "xsapi/services.h"
#include "user_context.h"
#include "xbox_system_factory.h"
#include "xbox_live_context_server_impl.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

xbox_live_context_server_impl::xbox_live_context_server_impl(
    _In_ std::shared_ptr<system::xbox_live_server> server
    )
{
    m_userContext = std::make_shared<xbox::services::user_context>(server);
}

std::shared_ptr<system::xbox_live_server>
xbox_live_context_server_impl::server()
{
    return m_userContext->server();
}


void xbox_live_context_server_impl::init()
{
    xbox_live_result<void> servicesConfigFileReadResult;

    m_appConfig = xbox_live_app_config::get_app_config_singleton();

    m_xboxLiveContextSettings = std::make_shared<xbox_live_context_settings>();

}

std::shared_ptr<user_context> xbox_live_context_server_impl::user_context()
{
    return m_userContext;
}

std::shared_ptr<xbox_live_context_settings> 
xbox_live_context_server_impl::settings()
{
    return m_xboxLiveContextSettings;
}

std::shared_ptr<xbox_live_app_config> 
xbox_live_context_server_impl::application_config()
{
    return m_appConfig;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END