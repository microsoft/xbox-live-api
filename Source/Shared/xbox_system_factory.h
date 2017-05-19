// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include <cpprest/ws_client.h>
#include "shared_macros.h"
#include "xsapi/system.h"
#include "http_client.h"

#if XSAPI_SERVER || UNIT_TEST_SYSTEM || XSAPI_U
#include "xsts_token_service.h"
#include "user_token_service.h"
#include "title_token_service.h"
#include "device_token_service.h"
#if XSAPI_SERVER
#include "service_token_service.h"
#endif
#include "xtitle_service.h"

#if XSAPI_SERVER
#include "xbox_live_server_impl.h"
#endif
#endif

#if (!TV_API || UNIT_TEST_SYSTEM)
#include "user_impl.h"
#endif

#include "web_socket_client.h"
#include "multiplayer_internal.h"
#include "local_config.h"

#include "http_call_impl.h"
#include <mutex>
#include "initiator.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN


class xbox_system_factory
{
public:
    xbox_system_factory()
    {
        static initiator s_initiator;
    }

#if XSAPI_SERVER || UNIT_TEST_SYSTEM || XSAPI_U
    virtual std::shared_ptr<xsts_token_service> create_xsts_token();

    virtual std::shared_ptr<user_token_service> create_user_token();

    virtual std::shared_ptr<title_token_service> create_title_token();

    virtual std::shared_ptr<device_token_service> create_device_token();

#if XSAPI_SERVER
    virtual std::shared_ptr<service_token_service> create_service_token();
#endif

    virtual std::shared_ptr<xtitle_service> create_xtitle_service();
#endif

    virtual std::shared_ptr<xbox_http_client> create_http_client(
        _In_ const web::http::uri& base_uri,
        _In_ const web::http::client::http_client_config& client_config
        );

    virtual std::shared_ptr<local_config> create_local_config();

    virtual std::shared_ptr<http_call> create_http_call(
        _In_ const std::shared_ptr<xbox_live_context_settings>& xboxLiveContextSettings,
        _In_ const string_t& httpMethod,
        _In_ const string_t& serverName,
        _In_ const web::uri& pathQueryFragment,
        _In_ xbox_live_api xboxLiveApi
        );

    virtual std::shared_ptr<http_call_internal> create_http_call_internal(
        _In_ const std::shared_ptr<xbox_live_context_settings>& xboxLiveContextSettings,
        _In_ const string_t& httpMethod,
        _In_ const string_t& serverName,
        _In_ const web::uri& pathQueryFragment
        );

    virtual std::shared_ptr<user_impl> create_user_impl(user_creation_context userCreationContext);

    virtual std::shared_ptr<xbox_web_socket_client> create_web_socket_client();

    virtual std::shared_ptr<multiplayer::multiplayer_subscription> create_multiplayer_subscription(
        _In_ const std::function<void(const multiplayer::multiplayer_session_change_event_args&)>& multiplayerSessionChangeHandler,
        _In_ const std::function<void()>& multiplayerSubscriptionLostHandler,
        _In_ const std::function<void(const XBOX_LIVE_NAMESPACE::real_time_activity::real_time_activity_subscription_error_event_args&)>& subscriptionErrorHandler
        );

    static std::shared_ptr<xbox_system_factory> get_factory();
    static void set_factory(_In_ std::shared_ptr<xbox_system_factory> factory);

private:
    static std::mutex m_factoryInstanceLock;
    static std::shared_ptr<xbox_system_factory> m_factoryInstance;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
