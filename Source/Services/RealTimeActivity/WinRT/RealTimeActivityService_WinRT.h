// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/real_time_activity.h"
#include "xsapi/xbox_live_context_settings.h"
#include "RealTimeActivityConnectionState_WinRT.h"
#include "RealTimeActivitySubscriptionErrorEventArgs_WinRT.h"
#include "RealTimeActivityResyncEventArgs_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_BEGIN

class ConnectionStateChangeEventBind : public std::enable_shared_from_this<ConnectionStateChangeEventBind>
{
public:
    ConnectionStateChangeEventBind(
        _In_ Platform::WeakReference setting,
        _In_ std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service> cppObj
        );

    void AddConnectionStateChangeEvent();

    void RemoveConnectionStateChangeEvent();

    void RealTimeActivityConnectionStateChangeRouter(_In_ xbox::services::real_time_activity::real_time_activity_connection_state state);

private:
    function_context m_functionContext;
    Platform::WeakReference m_setting;
    std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service> m_rtaCppObj;
};

class SubscriptionErrorEventBind : public std::enable_shared_from_this<SubscriptionErrorEventBind>
{
public:
    SubscriptionErrorEventBind(
        _In_ Platform::WeakReference setting,
        _In_ std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service> cppObj
        );

    void AddSubscriptionErrorEvent();

    void RemoveSubscriptionErrorEvent();

    void RealTimeActivitySubscriptionErrorRouter(_In_ const xbox::services::real_time_activity::real_time_activity_subscription_error_event_args& args);

private:
    function_context m_functionContext;
    Platform::WeakReference m_setting;
    std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service> m_rtaCppObj;
};

class ResyncEventBind : public std::enable_shared_from_this<ResyncEventBind>
{
public:
    ResyncEventBind(
        _In_ Platform::WeakReference setting,
        _In_ std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service> cppObj
        );

    void AddResyncEvent();

    void RemoveResyncEvent();

    void RealTimeActivityResyncRouter();

private:
    function_context m_functionContext;
    Platform::WeakReference m_setting;
    std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service> m_rtaCppObj;
};

/// <summary>
/// Represents a client side service that handles connections and communications with
/// the Xbox Live real-time activity service.
/// </summary>
public ref class RealTimeActivityService sealed
{
public:
    /// <summary>
    /// Starts a background task that creates and initializes a websocket connection to the
    /// Xbox Live real-time activity service.
    /// </summary>
    void Activate();

    /// <summary>
    /// Cancels all existing subscriptions to the Xbox Live real-time activity service,
    /// unhooks from the websocket connection, and stops the background task.
    /// </summary>
    void Deactivate();

    /// <summary>
    /// A notification that is sent when the client service loses or gains connectivity to the
    /// Xbox Live service.
    /// Event handlers receive a RealTimeActivitySubscriptionState^ object.
    /// </summary>
    event Windows::Foundation::EventHandler<RealTimeActivityConnectionState>^ RealTimeActivityConnectionStateChange;

    /// <summary>
    /// Registers a handler function to receive a notification that is sent when there is an
    /// error in the real time activity service.
    /// Event handlers receive a real_time_activity_subscription_error_event_args&amp; object.
    /// </summary>
    event Windows::Foundation::EventHandler<RealTimeActivitySubscriptionErrorEventArgs^>^ RealTimeActivitySubscriptionError;

    /// <summary>
    /// Registers a handler function to receive a notification that is sent when there is a
    /// resync message from the real time activity service.
    /// This message indicates that data may have been lost and to resync all data by calling
    /// corresponding REST API's
    /// </summary>
    event Windows::Foundation::EventHandler<RealTimeActivityResyncEventArgs^>^ RealTimeActivityResync;

internal:
    RealTimeActivityService(
        _In_ std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service> cppObj
        );

    std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service> GetCppObj() const;

    void RaiseRealTimeActivityConnectedStateChange(_In_ RealTimeActivityConnectionState state);

    void RaiseRealTimeActivitySubscriptionError(_In_ RealTimeActivitySubscriptionErrorEventArgs^ args);

    void RaiseRealTimeActivityResync(_In_ RealTimeActivityResyncEventArgs^ args);

private:
    ~RealTimeActivityService();
    std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service> m_cppObj;
    std::shared_ptr<ConnectionStateChangeEventBind> m_connectionStateChangeEventBind;
    std::shared_ptr<SubscriptionErrorEventBind> m_subscriptionErrorEventBind;
    std::shared_ptr<ResyncEventBind> m_resyncEventBind;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_END