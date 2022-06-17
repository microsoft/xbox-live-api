// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi-c/real_time_activity_c.h"
#include "real_time_activity_manager.h"
#include "web_socket.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_CPP_BEGIN

struct ServiceSubscription;

class Connection : public std::enable_shared_from_this<Connection>
{
public:
    static Result<std::shared_ptr<Connection>> Make(
        User&& user,
        const TaskQueue& queue,
        ConnectionStateChangedHandler stateChangedHandler,
        ResyncHandler resyncHandler
    ) noexcept;

    ~Connection() noexcept;

    // Disconnect WebSocket, terminate background queue, and complete any pending async operations
    void Cleanup();

    HRESULT AddSubscription(
        std::shared_ptr<Subscription> subscription,
        AsyncContext<Result<void>> async
    ) noexcept;

    HRESULT RemoveSubscription(
        std::shared_ptr<Subscription> subscription,
        AsyncContext<Result<void>> async
    ) noexcept;

    size_t SubscriptionCount() const noexcept;

#if HC_PLATFORM == HC_PLATFORM_GDK
    void AppStateChangeNotificationReceived(
        bool isSuspended
    ) noexcept;
#endif

private:
    Connection(
        User&& user,
        const TaskQueue& queue,
        ConnectionStateChangedHandler stateChangedHandler,
        ResyncHandler resyncHandler
    ) noexcept;

    JsonDocument AssembleSubscribeMessage(
        std::shared_ptr<ServiceSubscription> sub
    ) const noexcept;

    // RTA protocol implementation
    HRESULT SendSubscribeMessage(
        std::shared_ptr<ServiceSubscription> subscription,
        std::unique_lock<std::mutex>&& lock
    ) const noexcept;

    HRESULT SendUnsubscribeMessage(
        std::shared_ptr<ServiceSubscription> subscription,
        std::unique_lock<std::mutex>&& lock
    ) const noexcept;

    HRESULT SendAssembledMessage(_In_ const JsonValue& message) const noexcept;

    void SubscribeResponseHandler(_In_ const JsonValue& message) noexcept;
    void UnsubscribeResponseHandler(_In_ const JsonValue& message) noexcept;
    void EventHandler(_In_ const JsonValue& message) const noexcept;

    // IWebsocket handlers
    void ConnectCompleteHandler(WebsocketResult result) noexcept;
    void DisconnectHandler(WebSocketCloseStatus result) noexcept;
    void WebsocketMessageReceived(const String& message) noexcept;
    HRESULT InitializeWebsocket() noexcept;
    void ScheduleConnect() noexcept;

    User m_user;
    TaskQueue const m_queue;
    std::shared_ptr<IWebsocket> m_websocket;
    uint32_t m_connectAttempt{ 0 };
    std::chrono::time_point<std::chrono::system_clock> m_connectTime;
    XblRealTimeActivityConnectionState m_state{ XblRealTimeActivityConnectionState::Connecting };
    const ConnectionStateChangedHandler m_stateChangedHandler;
    const real_time_activity::ResyncHandler m_resyncHandler;

    Map<String, std::shared_ptr<ServiceSubscription>> m_subsByUri; // needed to add/remove client subscription
    Map<uint32_t, std::shared_ptr<ServiceSubscription>> m_subsByClientId; // needed for subscribe/unsubscribe handshake
    Map<uint32_t, std::shared_ptr<ServiceSubscription>> m_subsByServiceId; // needed to handle subscription events

    uint32_t m_nextSubId{ 1 };

    mutable std::mutex m_lock;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_CPP_END