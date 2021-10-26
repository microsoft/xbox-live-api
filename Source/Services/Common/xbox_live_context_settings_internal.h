// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include <mutex>
#include <unordered_map>
#include "xsapi-c/xbox_live_context_settings_c.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

#define DEFAULT_HTTP_TIMEOUT_SECONDS (30)
#if XSAPI_UNIT_TESTS
#define MIN_HTTP_TIMEOUT_SECONDS (0) // speed up unit tests
#else
#define MIN_HTTP_TIMEOUT_SECONDS (5)
#endif
#define MIN_HTTP_TIMEOUT_MILLISECONDS (MIN_HTTP_TIMEOUT_SECONDS * 1000)
#define DEFAULT_LONG_HTTP_TIMEOUT_SECONDS (5 * 60)
#if XSAPI_UNIT_TESTS
#define DEFAULT_WEBSOCKET_TIMEOUT_SECONDS (1)
#else
#define DEFAULT_WEBSOCKET_TIMEOUT_SECONDS (60)
#endif
#define MAXIMUM_WEBSOCKETS_ACTIVATIONS_ALLOWED_PER_USER (5)
#define DEFAULT_HTTP_RETRY_WINDOW_SECONDS (20)
#define DEFAULT_RETRY_DELAY_SECONDS (2)
#define MIN_RETRY_DELAY_SECONDS (2)

enum class HttpCallAgent : uint32_t
{
    Title,
    MultiplayerManager,
    SocialManager,
    MultiplayerActivity,
    AchievementsManager
};

class XboxLiveContextSettings
{
public:
    XboxLiveContextSettings() = default;

    uint32_t LongHttpTimeout() const;
    void SetLongHttpTimeout(_In_ uint32_t timeoutInSeconds);

    uint32_t HttpRetryDelay() const;
    void SetHttpRetryDelay(_In_ uint32_t delayInSeconds);

    uint32_t HttpTimeoutWindow() const;
    void SetHttpTimeoutWindow(_In_ uint32_t timeoutWindowInSeconds);

    uint32_t WebsocketTimeoutWindow() const;
    void SetWebsocketTimeoutWindow(_In_ uint32_t timeoutInSeconds);

    HttpCallAgent HttpUserAgent() const;
    void SetHttpUserAgent(_In_ HttpCallAgent userAgent);

#if XSAPI_WINRT // WinRT only
    bool UseCoreDispatcherForEventRouting() const;

    void SetUseCoreDispatcherForEventRouting(_In_ bool value);
#endif

    bool UseCrossplatformQosServers() const;
    void SetUseCrossplatformQosServers(_In_ bool value);

public:

#if __cplusplus_winrt
    static Windows::UI::Core::CoreDispatcher^ _s_dispatcher;
#endif

private:
    uint32_t m_httpTimeoutInSeconds{ DEFAULT_HTTP_TIMEOUT_SECONDS };
    uint32_t m_longHttpTimeoutInSeconds{ DEFAULT_LONG_HTTP_TIMEOUT_SECONDS };
    uint32_t m_httpRetryDelayInSeconds{ DEFAULT_RETRY_DELAY_SECONDS };
    uint32_t m_httpTimeoutWindowInSeconds{ DEFAULT_HTTP_RETRY_WINDOW_SECONDS };
    uint32_t m_websocketTimeoutWindowInSeconds{ DEFAULT_WEBSOCKET_TIMEOUT_SECONDS };
    HttpCallAgent m_userAgent{ HttpCallAgent::Title };
#if XSAPI_WINRT
    bool m_useCoreDispatcherForEventRouting{ false };
#endif
    bool m_useXplatQosServer{ HC_PLATFORM == HC_PLATFORM_XDK };
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END