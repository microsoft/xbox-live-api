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
#include "xbox_system_factory.h"
#include "xbox_live_context_server_impl.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

xbox_live_server_context::xbox_live_server_context(
    _In_ std::shared_ptr<system::xbox_live_server> server
    )
{
    m_xboxLiveContextImpl = std::make_shared<xbox_live_context_server_impl>(server);
    m_xboxLiveContextImpl->init();
}

/// <summary>
/// Returns the associated system User.
/// </summary>
std::shared_ptr<system::xbox_live_server>
xbox_live_server_context::server()
{
    return m_xboxLiveContextImpl->server();
}

std::shared_ptr<xbox_live_context_settings> 
xbox_live_server_context::settings()
{
    return m_xboxLiveContextImpl->settings();
}

std::shared_ptr<xbox_live_app_config> 
xbox_live_server_context::application_config()
{
    return m_xboxLiveContextImpl->application_config();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END