// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#if !TV_API
#include "xbox_system_factory.h"
#endif
#include "xbox_system_factory.h"
#if XSAPI_A
#include "a/user_impl_a.h"
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN


std::shared_ptr<xbox_system_factory> 
xbox_system_factory::get_factory()
{
    auto xsapiSingleton = get_xsapi_singleton();
    std::lock_guard<std::mutex> hold(xsapiSingleton->m_singletonLock);
    if (xsapiSingleton->m_factoryInstance == nullptr)
    {
        xsapiSingleton->m_factoryInstance = std::make_shared<xbox_system_factory>();
    }
    return xsapiSingleton->m_factoryInstance;
}

void xbox_system_factory::set_factory(
    _In_ std::shared_ptr<xbox_system_factory> factory
    )
{
    auto xsapiSingleton = get_xsapi_singleton();
    std::lock_guard<std::mutex> hold(xsapiSingleton->m_singletonLock);
    xsapiSingleton->m_factoryInstance = factory;
}

#if XSAPI_U
std::shared_ptr<xsts_token_service>
xbox_system_factory::create_xsts_token()
{
    return std::make_shared<xsts_token_service_impl>();
}

std::shared_ptr<user_token_service>
xbox_system_factory::create_user_token()
{
    return std::make_shared<user_token_service_impl>();
}

std::shared_ptr<title_token_service>
xbox_system_factory::create_title_token()
{
    return std::make_shared<title_token_service_impl>();
}

std::shared_ptr<device_token_service>
xbox_system_factory::create_device_token()
{
    return std::make_shared<device_token_service_impl>();
}

std::shared_ptr<xtitle_service>
xbox_system_factory::create_xtitle_service()
{
    return std::shared_ptr<xtitle_service>(new xtitle_service_impl());
}
#endif

std::shared_ptr<http_call>
xbox_system_factory::create_http_call(
    _In_ const std::shared_ptr<xbox_live_context_settings>& xboxLiveContextSettings,
    _In_ const string_t& _httpMethod,
    _In_ const string_t& _serverName,
    _In_ const web::uri& pathQueryFragment,
    _In_ xbox_live_api xboxLiveApi
    )
{
    xsapi_internal_string httpMethod(_httpMethod.begin(), _httpMethod.end());
    xsapi_internal_string serverName(_serverName.begin(), _serverName.end());

    return std::make_shared<http_call_impl>(
        xboxLiveContextSettings, 
        httpMethod,
        serverName,
        pathQueryFragment,
        xboxLiveApi
        );
}

std::shared_ptr<http_call_internal>
xbox_system_factory::create_http_call(
    _In_ const std::shared_ptr<xbox_live_context_settings>& xboxLiveContextSettings,
    _In_ const xsapi_internal_string& httpMethod,
    _In_ const xsapi_internal_string& serverName,
    _In_ const web::uri& pathQueryFragment,
    _In_ xbox_live_api xboxLiveApi
    )
{
    return xsapi_allocate_shared<http_call_impl>(
        xboxLiveContextSettings,
        httpMethod,
        serverName,
        pathQueryFragment,
        xboxLiveApi
        );
}

std::shared_ptr<local_config>
xbox_system_factory::create_local_config()
{
    return local_config::get_local_config_singleton();
}

#if !XSAPI_I
std::shared_ptr<user_impl>
xbox_system_factory::create_user_impl(user_creation_context userCreationContext)
{
#if UWP_API
    return std::make_shared<user_impl_idp>(userCreationContext);
#elif XSAPI_A
    return std::make_shared<user_impl_android>();
#else
    UNREFERENCED_PARAMETER(userCreationContext);
    throw std::bad_function_call(); 
#endif
}
#endif

std::shared_ptr<xbox_web_socket_client> 
xbox_system_factory::create_web_socket_client()
{
    return std::make_shared<xbox_web_socket_client>();
}

std::shared_ptr<multiplayer::multiplayer_subscription>
xbox_system_factory::create_multiplayer_subscription(
    _In_ const std::function<void(const multiplayer::multiplayer_session_change_event_args&)>& multiplayerSessionChangeHandler,
    _In_ const std::function<void()>& multiplayerSubscriptionLostHandler,
    _In_ const std::function<void(const xbox::services::real_time_activity::real_time_activity_subscription_error_event_args&)>& subscriptionErrorHandler
)
{
    return std::make_shared<multiplayer::multiplayer_subscription>(
        multiplayerSessionChangeHandler,
        multiplayerSubscriptionLostHandler,
        subscriptionErrorHandler
        );
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
