// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "real_time_activity_manager.h"
#include "real_time_activity_connection.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_CPP_BEGIN

RealTimeActivityManager::RealTimeActivityManager(
    const TaskQueue& queue
) noexcept
    : m_queue{ queue.DeriveWorkerQueue() }
{
}

RealTimeActivityManager::~RealTimeActivityManager() noexcept
{
    HC_TRACE_MESSAGE(XSAPI, HCTraceLevel::Verbose, __FUNCTION__);
}

void RealTimeActivityManager::Cleanup()
{
    std::unique_lock<std::recursive_mutex> lock{ m_lock };

    // Don't invoke disconnect handlers during cleanup
    m_stateChangedHandlers.clear();

    Map<uint64_t, std::shared_ptr<Connection>> connections{ std::move(m_rtaConnections) };
    m_rtaConnections.clear();
    lock.unlock();

    for (auto& pair : connections)
    {
        pair.second->Cleanup();
    }
}

HRESULT RealTimeActivityManager::AddSubscription(
    const User& user,
    std::shared_ptr<Subscription> subscription
) noexcept
{
    assert(subscription);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(subscription);

    std::lock_guard<std::recursive_mutex> lock{ m_lock };
    auto connectionResult{ GetConnection(user) };
    RETURN_HR_IF_FAILED(connectionResult.Hresult());
    return connectionResult.ExtractPayload()->AddSubscription(subscription, AsyncContext<Result<void>>{ m_queue });
}

HRESULT RealTimeActivityManager::RemoveSubscription(
    const User& user,
    std::shared_ptr<Subscription> subscription
) noexcept
{
    assert(subscription);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(subscription);

    std::lock_guard<std::recursive_mutex> lock{ m_lock };

    auto iter{ m_rtaConnections.find(user.Xuid()) };
    if (iter == m_rtaConnections.end())
    {
        return S_OK;
    }

    return iter->second->RemoveSubscription(subscription, AsyncContext<Result<void>>{ m_queue,
        [
            xuid{ user.Xuid() },
            weakThis{ std::weak_ptr<RealTimeActivityManager>{ shared_from_this() } }
        ]
    (Result<void> result)
    {
        if (auto sharedThis{ weakThis.lock() })
        {
            std::unique_lock<std::recursive_mutex> lock{ sharedThis->m_lock };
            // If that was the last remaining subscription and there are no remaining legacy activations, close the connection
            auto iter{ sharedThis->m_rtaConnections.find(xuid) };
            if (iter != sharedThis->m_rtaConnections.end() && iter->second->SubscriptionCount() == 0 && sharedThis->m_legacyActivations[xuid] == 0)
            {
                LOGS_DEBUG << __FUNCTION__ << ": No remaining activations or subscriptions, tearing down connection";
                iter->second->Cleanup();
                sharedThis->m_rtaConnections.erase(iter);

                // Maintain legacy behavior and raise Disconnected event even on intentional shutdown
                auto handlers{ sharedThis->m_stateChangedHandlers[xuid] };
                lock.unlock();

                for (auto& handler : handlers)
                {
                    handler.second(XblRealTimeActivityConnectionState::Disconnected);
                }
            }
        }
    }
    });
}

XblFunctionContext RealTimeActivityManager::AddStateChangedHandler(
    const User& user,
    ConnectionStateChangedHandler handler
) noexcept
{
    std::lock_guard<std::recursive_mutex> lock{ m_lock };
    m_stateChangedHandlers[user.Xuid()][m_nextToken] = std::move(handler);
    return m_nextToken++;
}

void RealTimeActivityManager::RemoveStateChangedHandler(
    const User& user,
    XblFunctionContext token
) noexcept
{
    std::lock_guard<std::recursive_mutex> lock{ m_lock };
    m_stateChangedHandlers[user.Xuid()].erase(token);
}

XblFunctionContext RealTimeActivityManager::AddResyncHandler(
    const User& user,
    ResyncHandler handler
) noexcept
{
    std::lock_guard<std::recursive_mutex> lock{ m_lock };
    m_resyncHandlers[user.Xuid()][m_nextToken] = std::move(handler);
    return m_nextToken++;
}

void RealTimeActivityManager::RemoveResyncHandler(
    const User& user,
    XblFunctionContext token
) noexcept
{
    std::lock_guard<std::recursive_mutex> lock{ m_lock };
    m_resyncHandlers[user.Xuid()].erase(token);
}

void RealTimeActivityManager::Activate(
    const User& user,
    bool titleActivation
) noexcept
{
    std::lock_guard<std::recursive_mutex> lock{ m_lock };
    m_legacyActivations[user.Xuid()]++;

    // If the title manually activated, establish RTA connection if its not already set up
    if (titleActivation)
    {
        m_titleActivated = true;
        (void)GetConnection(user); 
    }
}

void RealTimeActivityManager::Deactivate(
    const User& user
) noexcept
{
    std::unique_lock<std::recursive_mutex> lock{ m_lock };

    auto& activationCount{ m_legacyActivations[user.Xuid()] };
    auto connectionIter{ m_rtaConnections.find(user.Xuid()) };
    if (connectionIter == m_rtaConnections.end())
    {
        return;
    }

    assert(activationCount > 0);
    if (activationCount > 0 && --activationCount == 0)
    {
        // When the activation count reaches 0, tear down the WebSocket connection if the title
        // manually activated/deactivated RTA or if there are no remaining subscriptions.
        // The second case is important due to a race condition between RemoveSubscription and Deactivate.
        if (m_titleActivated || connectionIter->second->SubscriptionCount() == 0)
        {
            LOGS_DEBUG << __FUNCTION__ << ": No remaining activations tearing down connection";
            connectionIter->second->Cleanup();
            m_rtaConnections.erase(connectionIter);

            // Maintain legacy behavior and raise Disconnected event even on intentional shutdown
            auto handlers{ m_stateChangedHandlers[user.Xuid()] };
            lock.unlock();

            for (auto& handler : handlers)
            {
                handler.second(XblRealTimeActivityConnectionState::Disconnected);
            }
        }
    }
}

void RealTimeActivityManager::TriggerResync() const noexcept
{
    std::unique_lock<std::recursive_mutex> lock{ m_lock };
    auto handlers{ m_resyncHandlers };
    lock.unlock();

    for (auto& userPair : handlers)
    {
        for (auto& handlerPair : userPair.second)
        {
            handlerPair.second();
        }
    }
}

Result<std::shared_ptr<Connection>> RealTimeActivityManager::GetConnection(
    const User& user
) noexcept
{
    std::shared_ptr<Connection> connection;

    auto iter{ m_rtaConnections.find(user.Xuid()) };
    if (iter != m_rtaConnections.end())
    {
        connection = iter->second;
    }
    else
    {
        ConnectionStateChangedHandler stateChangedHandler =
            [
                xuid{ user.Xuid() },
                weakThis{ std::weak_ptr<RealTimeActivityManager>{ shared_from_this() } }
            ]
        (XblRealTimeActivityConnectionState state)
        {
            if (auto sharedThis{ weakThis.lock() })
            {
                std::unique_lock<std::recursive_mutex> lock{ sharedThis->m_lock };
                auto handlers{ sharedThis->m_stateChangedHandlers[xuid] };
                lock.unlock();

                for (auto& handler : handlers)
                {
                    handler.second(state);
                }
            }
        };

        ResyncHandler resyncHandler =
            [
                xuid{ user.Xuid() },
                weakThis{ std::weak_ptr<RealTimeActivityManager>{ shared_from_this() } }
            ]
        (void)
        {
            if (auto sharedThis{ weakThis.lock() })
            {
                std::unique_lock<std::recursive_mutex> lock{ sharedThis->m_lock };
                auto handlers{ sharedThis->m_resyncHandlers[xuid] };
                lock.unlock();

                for (auto& handler : handlers)
                {
                    handler.second();
                }
            }
        };

        auto copyUserResult{ user.Copy() };
        if (Failed(copyUserResult))
        {
            return copyUserResult.Hresult();
        }

        auto connectionResult = Connection::Make(copyUserResult.ExtractPayload(), m_queue, std::move(stateChangedHandler), std::move(resyncHandler));
        if (Failed(connectionResult))
        {
            return connectionResult.Hresult();
        }
        
        connection = connectionResult.ExtractPayload();
        m_rtaConnections[user.Xuid()] = connection;
    }

    return connection;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_CPP_END

// Test Hook
HRESULT XblTestHooksTriggerRTAResync()
{
    auto state = GlobalState::Get();
    if (!state)
    {
        return E_XBL_NOT_INITIALIZED;
    }
    state->RTAManager()->TriggerResync();

    return S_OK;
}
