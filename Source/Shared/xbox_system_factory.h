// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "shared_macros.h"
#include "web_socket.h"
#include "multiplayer_internal.h"
#if HC_PLATFORM == HC_PLATFORM_UWP
#include "local_config.h"
#endif
#include <mutex>

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN
namespace legacy
{
    class http_call;
}
NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END


NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN



class xbox_system_factory
{
public:
    // temporary method to support http call initialization in android jni layer without xalUserHandle
    // TODo: remove it in the future
    std::shared_ptr<legacy::http_call> create_http_call(
        _In_ const std::shared_ptr<XboxLiveContextSettings>& xboxLiveContextSettings,
        _In_ const string_t& httpMethod,
        _In_ const string_t& serverName,
        _In_ const xbox::services::uri& pathQueryFragment,
        _In_ xbox_live_api xboxLiveApi
    );

    static std::shared_ptr<xbox_system_factory> get_factory();
    static void set_factory(_In_ std::shared_ptr<xbox_system_factory> factory);

    static void set_http_call_user(xbox_live_user_t user);

    virtual ~xbox_system_factory() = default;

private:
    static xbox_live_user_t httpCallUserHandle;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
