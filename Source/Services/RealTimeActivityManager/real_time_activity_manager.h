// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi-c/real_time_activity_c.h"
#include "real_time_activity_subscription.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_CPP_BEGIN

typedef Callback<XblRealTimeActivityConnectionState> ConnectionStateChangedHandler;
typedef Function<void()> ResyncHandler;

class RealTimeActivityManager : public std::enable_shared_from_this<RealTimeActivityManager>
{
public:
    RealTimeActivityManager(const TaskQueue& queue) noexcept;
    ~RealTimeActivityManager() noexcept;

    // Finalize RTA communication and disconnect all WebSockets
    void Cleanup();

    HRESULT AddSubscription(
        const User& user,
        std::shared_ptr<Subscription> subscription
    ) noexcept;

    HRESULT RemoveSubscription(
        const User& user,
        std::shared_ptr<Subscription> subscription
    ) noexcept;

    XblFunctionContext AddStateChangedHandler(
        const User& user,
        ConnectionStateChangedHandler handler
    ) noexcept;

    void RemoveStateChangedHandler(
        const User& user,
        XblFunctionContext token
    ) noexcept;

    XblFunctionContext AddResyncHandler(
        const User& user,
        ResyncHandler handler
    ) noexcept;

    void RemoveResyncHandler(
        const User& user,
        XblFunctionContext token
    ) noexcept;

    // Legacy API support. Allow titles to explicitly activate and deactivate the RTA connection.
    // Internal components will also maintain the activation count to maintain existing behavior.
    void Activate(
        const User& user,
        bool titleActivation = false
    ) noexcept;

    void Deactivate(
        const User& user
    ) noexcept;

    // Test Hook
    void TriggerResync() const noexcept;

private:
    Result<std::shared_ptr<class Connection>> GetConnection(
        const User& user
    ) noexcept;

    Map<uint64_t, std::shared_ptr<class Connection>> m_rtaConnections;
    TaskQueue const m_queue;

    XblFunctionContext m_nextToken{ 1 };
    Map<uint64_t, Map<XblFunctionContext, ConnectionStateChangedHandler>> m_stateChangedHandlers;
    Map<uint64_t, Map<XblFunctionContext, ResyncHandler>> m_resyncHandlers;

    uint32_t m_pendingOperations{ 0 };

    // Legacy API support
    Map<uint64_t, size_t> m_legacyActivations;
    bool m_titleActivated{ false };

    mutable std::recursive_mutex m_lock;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_CPP_END