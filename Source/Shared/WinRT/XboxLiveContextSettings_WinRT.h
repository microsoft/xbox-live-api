// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "shared_macros.h"
#include "Macros_WinRT.h"
#include "xsapi/system.h"
#include "Utils_WinRT.h"
#include "XboxServiceCallRoutedEventArgs_WinRT.h"
#include "XboxServicesDiagnosticsTraceLevel_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN

ref class XboxLiveContextSettings;

class XboxLiveContextSettingsEventBind : public std::enable_shared_from_this<XboxLiveContextSettingsEventBind>
{
public:
    XboxLiveContextSettingsEventBind(
        _In_ Platform::WeakReference setting,
        _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> cppObj
        );

    void AddXboxLiveContextSettingsEvent();

    void RemoveServiceCallRoutedEvent(_In_ std::shared_ptr<xbox::services::xbox_live_context_settings> cppObj);

    void ServiceCallRoutedHandler(_In_ const xbox::services::xbox_service_call_routed_event_args& callRoutedEventArgs);

private:
    Platform::WeakReference m_setting;
    std::shared_ptr<xbox::services::xbox_live_context_settings> m_cppObj;
    function_context m_serviceCallHandlerContext;
};

/// <summary>
/// Enum used with DisableAsserts APIs
/// </summary>
public enum class XboxLiveContextRecommendedSetting
{
    /// <summary>
    /// Passed to xboxLiveContext->Settings->DisableAsserts APIs
    /// to warn code reviewers that there's an outstanding calling pattern issue that needs to be addressed.
    /// </summary>
    ThisCodeNeedsToBeChangedToFollowBestPractices
};

/// <summary>
/// Enum used with disable_asserts_for_xbox_live_throttling_in_dev_sandboxes()
/// </summary>
public enum class XboxLiveContextThrottleSetting
{
    /// <summary>
    /// Passed to xboxLiveContext->Settings->DisableAssertsForXboxLiveThrottlingInDevSandboxes()
    /// to warn code reviewers that there's an outstanding Xbox Live calling pattern issue that needs to be addressed.
    /// </summary>
    ThisCodeNeedsToBeChangedToAvoidThrottling
};

/// <summary>
/// Represents settings for an HTTP call.
/// </summary>
public ref class XboxLiveContextSettings sealed
{
public:
    XboxLiveContextSettings();

    /// <summary>
    /// Registers for all service call notifications.  Event handlers will receive an XboxServiceCallRoutedEventArgs object.
    /// </summary>
    event Windows::Foundation::EventHandler<XboxServiceCallRoutedEventArgs^>^ ServiceCallRouted; 

    /// <summary>
    /// Indicates if events should be generated for ServiceCallRouted.
    /// </summary>
    DEFINE_PTR_PROP_GETSET(EnableServiceCallRoutedEvents, enable_service_call_routed_events, bool);

    /// <summary>
    /// Indicates the level of debug messages to send to the debugger's Output window.
    /// </summary>
#if _MSC_VER >= 1800
    [Windows::Foundation::Metadata::Deprecated("Call XboxLiveServicesSettings::SingletonInstance->DiagnosticsTraceLevel instead", Windows::Foundation::Metadata::DeprecationType::Deprecate, 0x0)]
#endif
    property XboxServicesDiagnosticsTraceLevel DiagnosticsTraceLevel 
    { 
        XboxServicesDiagnosticsTraceLevel get();
        void set(_In_ XboxServicesDiagnosticsTraceLevel value);
    }

    /// <summary>
    /// DEPRECATED.  This value is ignored and is now automatically calculated to ensure the http_timeout_window() is obeyed
    /// This means that calls will be more deterministic about the time window they return in.
    /// </summary>
    DEFINE_PROP_GETSET_TIMESPAN_IN_SEC(HttpTimeout, http_timeout);

    /// <summary>
    /// Controls the connect, send, and receive timeouts for HTTP socket operations of long calls such as Title Storage calls.
    /// Default is 5 minutes.  Calls that take longer than this are aborted.
    /// Take care when setting this to smaller values as some calls like Title Storage may take a few minutes to complete.
    /// </summary>
    DEFINE_PROP_GETSET_TIMESPAN_IN_SEC(LongHttpTimeout, long_http_timeout);

    /// <summary>
    /// Controls the HTTP retry delay in seconds.
    ///
    /// Retries are delayed using a exponential back off.  By default, it will delay 2 seconds then the 
    /// next retry will delay 4 seconds, then 8 seconds, and so on up to a max of 1 min until either
    /// the call succeeds or the http_timeout_window is reached, at which point the call will fail.
    /// The delay is also jittered between the current and next delay to spread out service load.
    /// The default for http_timeout_window is 20 seconds and can be changed using set_http_timeout_window()
    /// 
    /// If the service returns an an HTTP error with a "Retry-After" header, then all future calls to that API 
    /// will immediately fail with the original error without contacting the service until the "Retry-After" 
    /// time has been reached.
    ///
    /// Idempotent service calls are retried when a network error occurs or the server responds with one of these HTTP status codes:
    /// 408 (Request Timeout)
    /// 429 (Too Many Requests)
    /// 500 (Internal Server Error)
    /// 502 (Bad Gateway)
    /// 503 (Service Unavailable)
    /// 504 (Gateway Timeout)
    /// </summary>
    DEFINE_PROP_GETSET_TIMESPAN_IN_SEC(HttpRetryDelay, http_retry_delay);

    /// <summary>
    /// Controls the HTTP timeout window in seconds.
    ///
    /// This controls how long to spend attempting to retry idempotent service calls before failing.
    /// The default is 20 seconds
    ///
    /// Idempotent service calls are retried when a network error occurs or the server responds with one of these HTTP status codes:
    /// 408 (Request Timeout)
    /// 429 (Too Many Requests)
    /// 500 (Internal Server Error)
    /// 502 (Bad Gateway)
    /// 503 (Service Unavailable)
    /// 504 (Gateway Timeout)
    /// </summary>
    DEFINE_PROP_GETSET_TIMESPAN_IN_SEC(HttpTimeoutWindow, http_timeout_window);

    /// <summary>
    /// Controls how long to spend attempting to retry establishing a websocket connection before failing. 
    /// Default is 300 seconds.  Set to 0 to turn off retry.
    /// </summary>
    DEFINE_PROP_GETSET_TIMESPAN_IN_SEC(WebsocketTimeoutWindow, websocket_timeout_window);

    /// <summary>
    /// Controls whether to use the CoreDispatcher from the User object to route events through. 
    /// This is required to be false if using events with JavaScript.
    /// </summary>
    DEFINE_PTR_PROP_GETSET_OBJ(UseCoreDispatcherForEventRouting, use_core_dispatcher_for_event_routing, bool);

    /// <summary>
    /// Disables asserts for Xbox Live throttling in dev sandboxes.
    /// The asserts will not fire in RETAIL sandbox, and this setting has has no affect in RETAIL sandboxes.
    /// It is best practice to not call this API, and instead adjust the calling pattern but this is provided
    /// as a temporary way to get unblocked while in early stages of game development.
    /// </summary>
    void DisableAssertsForXboxLiveThrottlingInDevSandboxes( _In_ XboxLiveContextThrottleSetting setting );

    /// <summary>
    /// Disables asserts for having maximum number of websockets being activated.
    /// It is best practice to not call this API, and instead adjust the calling pattern but this is provided
    /// as a temporary way to get unblocked while in early stages of game development.
    /// </summary>
    void DisableAssertsForMaximumNumberOfWebsocketsActivated(_In_ XboxLiveContextRecommendedSetting setting);

internal:
    XboxLiveContextSettings(
        _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> cppObj
        );

    void RaiseServiceCallRouted(_In_ XboxServiceCallRoutedEventArgs^ args);

    std::shared_ptr<xbox::services::xbox_live_context_settings> GetCppObj() { return m_cppObj; }

    static property Windows::UI::Core::CoreDispatcher^ Dispatcher { Windows::UI::Core::CoreDispatcher^ get(); };

private:
    ~XboxLiveContextSettings();
    std::shared_ptr<xbox::services::xbox_live_context_settings> m_cppObj;
    std::shared_ptr<XboxLiveContextSettingsEventBind> m_xboxLiveContextSettingsEventBind;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_END
