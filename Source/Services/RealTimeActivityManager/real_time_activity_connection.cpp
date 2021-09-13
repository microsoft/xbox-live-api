// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "real_time_activity_subscription.h"
#include "real_time_activity_connection.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_CPP_BEGIN

constexpr char s_rtaUri[]{ "wss://rta.xboxlive.com/connect" };
constexpr char s_rtaSubprotocol[]{ "rta.xboxlive.com.V2" };

struct Subscription::State
{
    State(uint32_t _clientId) noexcept : clientId{ _clientId } {}

    // Client ID is assigned by XSAPI and used to identify subscriptions.
    // Used as the unique SEQUENCE_N during RTA Sub/Unsub handshakes.
    uint32_t const clientId;

    // Assigned by service when we successfully subscribe. Used to identify subscription in RTA event messages.
    uint32_t serviceId{ 0 };

    // Current status with respect to RTA service
    enum class ServiceStatus : uint32_t
    {
        // RTA service has no knowledge of this subscription
        Inactive,
        // Registered with RTA service
        Active,
        // Client has indicated that they want to subscribe while unsubscribing. Resubscribe will
        // happen as soon as unsubscribe handshake completes.
        PendingSubscribe,
        // We've sent subscribe message to RTA service but has not yet received the handshake response
        Subscribing,
        // Client has indicated they want to unsubscribe while subscribing. Unsubscribe requires
        // we have a serviceID. Unsubscribe will happen as soon as the subscribe handshake is finished.
        PendingUnsubscribe,
        // We've sent unsubscribe message to RTA service but has not yet received the handshake response
        Unsubscribing
    } serviceStatus{ ServiceStatus::Inactive };

    uint32_t subscribeAttempt{ 0 };
};

// RTA message types and error codes define by service here http://xboxwiki/wiki/Real_Time_Activity
enum class MessageType : uint32_t
{
    Subscribe = 1,
    Unsubscribe = 2,
    Event = 3,
    Resync = 4
};

enum class ErrorCode : uint32_t
{
    Success = 0,
    UnknownResource = 1,
    SubscriptionLimitReached = 2,
    NoResourceData = 3,
    Throttled = 1001,
    ServiceUnavailable = 1002
};

HRESULT ConvertRTAErrorCode(ErrorCode rtaErrorCode) noexcept
{
    switch (rtaErrorCode)
    {
    case ErrorCode::Success: return S_OK;
    case ErrorCode::SubscriptionLimitReached: return E_XBL_RTA_SUBSCRIPTION_LIMIT_REACHED;
    case ErrorCode::NoResourceData: return E_XBL_RTA_ACCESS_DENIED;
    default: return E_XBL_RTA_GENERIC_ERROR;
    }
}

Connection::Connection(
    User&& user,
    const TaskQueue& queue,
    ConnectionStateChangedHandler stateChangedHandler,
    real_time_activity::ResyncHandler resyncHandler
) noexcept
    : m_user{ std::move(user) },
    m_queue{ queue.DeriveWorkerQueue() },
    m_stateChangedHandler{ std::move(stateChangedHandler) },
    m_resyncHandler{ std::move(resyncHandler) }
{
    LOGS_DEBUG << __FUNCTION__ << "[" << this << "]";
}

Connection::~Connection() noexcept
{
    LOGS_DEBUG << __FUNCTION__ << "[" << this << "]";

    m_queue.Terminate(false);
#if HC_PLATFORM == HC_PLATFORM_GDK
    auto state{ GlobalState::Get() };
    if (state)
    {
        state->RemoveAppChangeNotificationHandler(m_registrationID);
    }
#endif
}

Result<std::shared_ptr<Connection>> Connection::Make(
    User&& user,
    const TaskQueue& queue,
    ConnectionStateChangedHandler stateChangedHandler,
    real_time_activity::ResyncHandler resyncHandler
) noexcept
{
    auto rtaConnection = std::shared_ptr<Connection>(
        new (Alloc(sizeof(Connection))) Connection
        {
            std::move(user),
            queue,
            std::move(stateChangedHandler),
            std::move(resyncHandler)
        },
        Deleter<Connection>(),
        Allocator<Connection>()
    );

    auto hr = rtaConnection->InitializeWebsocket();
    if (FAILED(hr))
    {
        return hr;
    }

    rtaConnection->m_stateChangedHandler(rtaConnection->m_state);
    rtaConnection->m_websocket->Connect(s_rtaUri, s_rtaSubprotocol);

#if HC_PLATFORM == HC_PLATFORM_GDK
    auto state{ GlobalState::Get() };
    if (state)
    {
        rtaConnection->m_registrationID = state->AddAppChangeNotificationHandler(
            [weakThis = std::weak_ptr<Connection>{ rtaConnection }](bool isSuspended)
        {
            std::shared_ptr<Connection> connection = weakThis.lock();
            if (connection)
            {
                connection->AppStateChangeNotificationReceived(isSuspended);
            }
        }
        );
    }
#endif

    return rtaConnection;
}

void Connection::Cleanup()
{
    std::unique_lock<std::mutex> lock{ m_lock };
    assert(m_websocket);

    // Clear our disconnect handler to disable auto-reconnect logic
    m_websocket->SetDisconnectHandler(nullptr);
    m_websocket->Disconnect();

    List<AsyncContext<Result<void>>> pendingAsyncContexts;
    for (auto& pair : m_subscribeAsyncContexts)
    {
        pendingAsyncContexts.emplace_back(std::move(pair.second));
    }
    for (auto& pair : m_unsubscribeAsyncContexts)
    {
        pendingAsyncContexts.emplace_back(std::move(pair.second));
    }

    m_subscribeAsyncContexts.clear();
    m_unsubscribeAsyncContexts.clear();
    lock.unlock();

    m_queue.Terminate(
        false,
        [pendingAsyncContexts = std::move(pendingAsyncContexts)]() {
            for (auto& async : pendingAsyncContexts)
            {
                async.Complete(E_ABORT);
            }
        }
    );
}

#if HC_PLATFORM == HC_PLATFORM_GDK
void Connection::AppStateChangeNotificationReceived(
    bool isSuspended
) noexcept
{
   std::unique_lock<std::mutex> lock{ m_lock };
   this->m_isSuspended = isSuspended;

   if (!this->m_isSuspended && this->m_state == XblRealTimeActivityConnectionState::Disconnected)
   {
      Reconnect(std::move(lock));
   }
}
#endif

HRESULT Connection::AddSubscription(
    std::shared_ptr<Subscription> sub,
    AsyncContext<Result<void>> async
) noexcept
{
    assert(sub);
    std::unique_lock<std::mutex> lock{ m_lock };

    if (!sub->m_state)
    {
        // Initialize subscription state
        sub->m_state = MakeShared<Subscription::State>(m_nextSubId++);
    }

    m_subs[sub->m_state->clientId] = sub;

    LOGS_DEBUG << __FUNCTION__ << ": [" << sub->m_state->clientId << "] ServiceStatus=" << EnumName(sub->m_state->serviceStatus);

    switch (sub->m_state->serviceStatus)
    {
    case Subscription::State::ServiceStatus::Inactive:
    {
        m_subscribeAsyncContexts[sub->m_state->clientId] = std::move(async);

        // If our connection is active, immediately register with RTA service
        if (m_state == XblRealTimeActivityConnectionState::Connected)
        {
            return SendSubscribeMessage(sub);
        }
        return S_OK;
    }
    case Subscription::State::ServiceStatus::PendingUnsubscribe:
    {
        // Client previously removed subscription while we were subscribing.
        // Reset the state to subscribing and collapse the existing & new async contexts
        sub->m_state->serviceStatus = Subscription::State::ServiceStatus::Subscribing;

        auto asyncIter{ m_subscribeAsyncContexts.find(sub->m_state->clientId) };
        assert(asyncIter != m_subscribeAsyncContexts.end());
        asyncIter->second = AsyncContext<Result<void>>::Collapse({ std::move(asyncIter->second), async });

        return S_OK;
    }
    case Subscription::State::ServiceStatus::Unsubscribing:
    {
        // Wait for unsubscribe to finish before resubscribing
        sub->m_state->serviceStatus = Subscription::State::ServiceStatus::PendingSubscribe;
        m_subscribeAsyncContexts[sub->m_state->clientId] = std::move(async);
        return S_OK;
    }
    case Subscription::State::ServiceStatus::Active:
    case Subscription::State::ServiceStatus::PendingSubscribe:
    case Subscription::State::ServiceStatus::Subscribing:
    default:
    {
        // These are all technically ok, but they indicate the subscription was added multiple
        // times, which likely indicates a bug in a dependent service
        assert(false);
        return S_OK;
    }
    }
}

HRESULT Connection::RemoveSubscription(
    std::shared_ptr<Subscription> sub,
    AsyncContext<Result<void>> async
) noexcept
{
    assert(sub);
    std::unique_lock<std::mutex> lock{ m_lock };

    auto iter{ m_subs.find(sub->m_state->clientId) };
    assert(iter != m_subs.end());
    (void)(iter); // suppress unused warning

    LOGS_DEBUG << __FUNCTION__ << ": [" << sub->m_state->clientId << "] ServiceStatus=" << EnumName(sub->m_state->serviceStatus);

    switch (sub->m_state->serviceStatus)
    {
    case Subscription::State::ServiceStatus::Inactive:
    {
        // RTA service has no knowledge of inactive subs. Just remove from our local
        // state and complete the AsyncContext.
        m_subs.erase(sub->m_state->clientId);

        lock.unlock();

        async.Complete(S_OK);
        return S_OK;
    }
    case Subscription::State::ServiceStatus::Active:
    {
        // Unregister subscription from RTA service
        m_unsubscribeAsyncContexts[sub->m_state->clientId] = std::move(async);
        return SendUnsubscribeMessage(sub);
    }
    case Subscription::State::ServiceStatus::PendingSubscribe:
    {
        // Client previously added the subscription while we were unsubscribing.
        // Reset the state to unsubscribe and collapse the existing & new async contexts
        sub->m_state->serviceStatus = Subscription::State::ServiceStatus::Unsubscribing;

        auto asyncIter{ m_unsubscribeAsyncContexts.find(sub->m_state->clientId) };
        assert(asyncIter != m_unsubscribeAsyncContexts.end());
        asyncIter->second = AsyncContext<Result<void>>::Collapse({ std::move(asyncIter->second), async });

        return S_OK;
    }
    case Subscription::State::ServiceStatus::Subscribing:
    {
        // We are in the process of subscribing. RTA protocol doesn't allow us to unsubscribe
        // until subscription is complete, so just mark the subscription as pending unsubscribe.
        // After the subscription completes, we will unsubscribe and complete the AsyncContext.
        m_unsubscribeAsyncContexts[sub->m_state->clientId] = std::move(async);
        sub->m_state->serviceStatus = Subscription::State::ServiceStatus::PendingUnsubscribe;
        return S_OK;
    }
    case Subscription::State::ServiceStatus::PendingUnsubscribe:
    case Subscription::State::ServiceStatus::Unsubscribing:
    default:
    {
        // These are all technically ok, but they indicate the subscription was removed multiple
        // times, which likely indicates a bug in a dependent service
        assert(false);
        return E_UNEXPECTED;
    }
    }
}

size_t Connection::SubscriptionCount() const noexcept
{
    std::unique_lock<std::mutex> lock{ m_lock };
    return m_subs.size();
}

HRESULT Connection::SendSubscribeMessage(
    std::shared_ptr<Subscription> sub
) const noexcept
{
    // Payload format [<API_ID>, <SEQUENCE_N>, “<RESOURCE_URI>”]

    sub->m_state->serviceStatus = Subscription::State::ServiceStatus::Subscribing;

    JsonDocument request{ rapidjson::kArrayType };
    auto& a{ request.GetAllocator() };

    request.PushBack(static_cast<uint32_t>(MessageType::Subscribe), a);
    request.PushBack(sub->m_state->clientId, a);
    request.PushBack(JsonValue{ sub->m_resourceUri.data(), a }, a);

    String requestString{ JsonUtils::SerializeJson(request) };
    LOGS_DEBUG << __FUNCTION__ << "[" << this << "]: " << requestString;

    return m_websocket->Send(requestString.data());
}

HRESULT Connection::SendUnsubscribeMessage(
    std::shared_ptr<Subscription> sub
) const noexcept
{
    // Payload format [<API_ID>, <SEQUENCE_N>, <SUB_ID>]

    sub->m_state->serviceStatus = Subscription::State::ServiceStatus::Unsubscribing;

    JsonDocument request{ rapidjson::kArrayType };
    auto& a{ request.GetAllocator() };

    request.PushBack(static_cast<uint32_t>(MessageType::Unsubscribe), a);
    request.PushBack(sub->m_state->clientId, a);
    request.PushBack(sub->m_state->serviceId, a);

    String requestString{ JsonUtils::SerializeJson(request) };
    LOGS_DEBUG << __FUNCTION__ << "[" << this << "]: " << requestString;

    return m_websocket->Send(requestString.data());
}

void Connection::SubscribeResponseHandler(_In_ const JsonValue& message) noexcept
{
    // Payload format [<API_ID>, <SEQUENCE_N>, <CODE_N>, <SUB_ID>, <DATA>]

    std::unique_lock<std::mutex> lock{ m_lock };

    auto clientId = message[1].GetUint();
    auto errorCode = static_cast<ErrorCode>(message[2].GetUint());

    auto subIter{ m_subs.find(clientId) };
    if (subIter == m_subs.end())
    {
        // Ignore unexpected message
        LOGS_DEBUG << "__FUNCTION__" << ": [" << clientId << "] Ignoring unexpected message";
        return;
    }
    auto sub{ subIter->second };

    switch (errorCode)
    {
    case ErrorCode::Success:
    {
        sub->m_state->serviceId = message[3].GetInt();
        const auto& data = message[4];

        m_activeSubs[sub->m_state->serviceId] = sub;

        switch (sub->m_state->serviceStatus)
        {
        case Subscription::State::ServiceStatus::Subscribing:
        {
            sub->m_state->serviceStatus = Subscription::State::ServiceStatus::Active;
            break;
        }
        case Subscription::State::ServiceStatus::PendingUnsubscribe:
        {
            // Client has removed the subscription while subscribe handshake was happening,
            // so immediately begin unsubscribing.
            SendUnsubscribeMessage(sub);
            break;
        }
        default:
        {
            // Any other Status indicates a XSAPI bug
            assert(false);
            break;
        }
        }

        AsyncContext<Result<void>> asyncContext{ std::move(m_subscribeAsyncContexts[sub->m_state->clientId]) };
        m_subscribeAsyncContexts.erase(sub->m_state->clientId);

        lock.unlock();

        asyncContext.Complete(ConvertRTAErrorCode(errorCode));
        sub->OnSubscribe(data);

        return;
    }
    case ErrorCode::UnknownResource:
    case ErrorCode::SubscriptionLimitReached:
    case ErrorCode::NoResourceData:
    {
        // With the possible exception of SubscriptionLimitReached, these are all indicative of a bug in XSAPI
        LOGS_ERROR << __FUNCTION__ << ": Failed with [" << static_cast<uint32_t>(errorCode) << "]";
        return;
    }
    case ErrorCode::Throttled:
    case ErrorCode::ServiceUnavailable:
    {
        auto serviceStatus{ sub->m_state->serviceStatus };
        sub->m_state->serviceStatus = Subscription::State::ServiceStatus::Inactive;

        switch (serviceStatus)
        {
        case Subscription::State::ServiceStatus::Subscribing:
        {
            uint64_t backoff = __min(std::pow(sub->m_state->subscribeAttempt++, 2), 60) * 1000;
            m_queue.RunWork([sub, weakThis = std::weak_ptr<Connection>{ shared_from_this() }]
                {
                    if (auto sharedThis{ weakThis.lock() })
                    {
                        sharedThis->AddSubscription(sub, AsyncContext<Result<void>>{ sharedThis->m_queue });
                    }
                },
                backoff
            );

            return;
        }
        case Subscription::State::ServiceStatus::PendingUnsubscribe:
        {
            // Don't retry subscription in this case because client has already removed it.
            AsyncContext<Result<void>> asyncContext{ std::move(m_subscribeAsyncContexts[sub->m_state->clientId]) };
            m_subscribeAsyncContexts.erase(sub->m_state->clientId);
            m_subs.erase(sub->m_state->clientId);

            lock.unlock();

            asyncContext.Complete(ConvertRTAErrorCode(errorCode));
            return;
        }
        default:
        {
            assert(false);
            return;
        }
        }
    }
    default:
    {
        LOGS_ERROR << __FUNCTION__ << ": Failed with unrecognized error code [" << static_cast<uint32_t>(errorCode) << "]";
        return;
    }
    }
}

void Connection::UnsubscribeResponseHandler(_In_ const JsonValue& message) noexcept
{
    // Payload format [<API_ID>, <SEQUENCE_N>, <CODE_N>]

    std::unique_lock<std::mutex> lock{ m_lock };

    auto clientId = message[1].GetUint();
    auto errorCode = static_cast<ErrorCode>(message[2].GetUint());

    if (errorCode != ErrorCode::Success)
    {
        // Not sure why unsubscribing would ever fail
        LOGS_ERROR << __FUNCTION__ << ": Failed with error code [" << static_cast<uint32_t>(errorCode) << "]";
    }

    auto subIter{ m_subs.find(clientId) };
    if (subIter == m_subs.end())
    {
        // Ignore unexpected message
        LOGS_DEBUG << "__FUNCTION__" << ": [" << clientId << "] Ignoring unexpected message";
        return;
    }
    auto sub{ subIter->second };
    m_activeSubs.erase(sub->m_state->serviceId);

    LOGS_DEBUG << __FUNCTION__ << ": [" << sub->m_state->clientId <<"] ServiceStatus=" << EnumName(sub->m_state->serviceStatus);

    switch (sub->m_state->serviceStatus)
    {
    case Subscription::State::ServiceStatus::Unsubscribing:
    {
        // We can now remove the subscription from our state entirely
        sub->m_state->serviceStatus = Subscription::State::ServiceStatus::Inactive;
        m_subs.erase(subIter);
        break;
    }
    case Subscription::State::ServiceStatus::PendingSubscribe:
    {
        // Client has re-added the subscription while unsubscibe handshake was happening,
        // so immediately begin subscribing.
        SendSubscribeMessage(sub);
        break;
    }
    default:
    {
        assert(false);
        break;
    }
    }

    AsyncContext<Result<void>> asyncContext{ std::move(m_unsubscribeAsyncContexts[clientId]) };
    m_unsubscribeAsyncContexts.erase(clientId);

    lock.unlock();

    asyncContext.Complete(ConvertRTAErrorCode(errorCode));
}

void Connection::EventHandler(_In_ const JsonValue& message) const noexcept
{
    // Payload format [<API_ID>, <SUB_ID>, <DATA>]

    std::unique_lock<std::mutex> lock{ m_lock };

    auto serviceId = message[1].GetInt();
    const auto& data = message[2];

    auto subIter{ m_activeSubs.find(serviceId) };
    assert(subIter != m_activeSubs.end());
    auto sub = subIter->second;

    lock.unlock();

    sub->OnEvent(data);
}

void Connection::ResyncHandler() const noexcept
{
    List<std::shared_ptr<Subscription>> subs;

    std::unique_lock<std::mutex> lock{ m_lock };
    for (auto& pair : m_subs)
    {
        subs.push_back(pair.second);
    }
    lock.unlock();

    // In some cases, subscriptions have enough context to handle a resync internally. In other cases,
    // there is some client context required, so we also will raise the resync to the client.
    for (auto& sub : subs)
    {
        sub->OnResync();
    }

    m_resyncHandler();
}

void Connection::ConnectCompleteHandler(WebsocketResult result) noexcept
{
    LOGS_DEBUG << __FUNCTION__ << ": WebsocketResult [" << result.hr << "," << result.platformErrorCode << "]";

    std::unique_lock<std::mutex> lock{ m_lock };

    if (SUCCEEDED(result.hr))
    {
        m_state = XblRealTimeActivityConnectionState::Connected;
        m_connectTime = std::chrono::system_clock::now();

        assert(m_activeSubs.empty());
        for (auto& pair : m_subs)
        {
            assert(pair.second->m_state->serviceStatus == Subscription::State::ServiceStatus::Inactive);
            SendSubscribeMessage(pair.second);
        }

        // RTA v2 has a lifetime of 2 hours. After 2 hours RTA service will disconnect the title. On some platforms
        // the websocket stack is able to recognize that the disconnect happened and notify libHttpClient/xsapi,
        // triggering a reconnect attempt, but on others the disconnect goes undetected. To be more defensive against 
        // this, xsapi proactively will disconnect from rta after ~90 minutes and trigger the reconnect flow.
#define CONNECTION_TIMEOUT_MS (90 /*mins*/ * 60 /*seconds/min*/ * 1000 /*ms/second*/)
        m_queue.RunWork([weakThis = std::weak_ptr<Connection>{ shared_from_this() }]
            {
                if (auto sharedThis{ weakThis.lock() })
                {
                    std::unique_lock<std::mutex> lock{ sharedThis->m_lock };
                    if ((std::chrono::system_clock::now() - sharedThis->m_connectTime).count() >= CONNECTION_TIMEOUT_MS)
                    {
                        auto socket = sharedThis->m_websocket;
                        lock.unlock();
                        // Disconnect so that auto-reconnect logic kicks in
                        socket->Disconnect();
                    }
                }
            },
            CONNECTION_TIMEOUT_MS
        );
    }
    else
    {
        m_state = XblRealTimeActivityConnectionState::Disconnected;

        //libHttpClient websocket does not support connecting
        // the same websocket handle multiple times, so create a new one.
        auto hr = InitializeWebsocket();
        if (FAILED(hr))
        {
            return;
        }

        // Backoff and attempt to connect again. 
        m_connectAttempt++;
        uint64_t backoff = __min(std::pow(m_connectAttempt, 2), 60) * 1000;

        m_queue.RunWork([weakThis = std::weak_ptr<Connection>{ shared_from_this() }]
            {
                auto sharedThis{ weakThis.lock() };
                if (sharedThis)
                {
                    {
                        std::unique_lock<std::mutex> lock{ sharedThis->m_lock };
                        sharedThis->m_state = XblRealTimeActivityConnectionState::Connecting;
                    }
                    sharedThis->m_stateChangedHandler(sharedThis->m_state);
                    sharedThis->m_websocket->Connect(s_rtaUri, s_rtaSubprotocol);
                }
            },
            backoff
        );
    }

    lock.unlock();
    m_stateChangedHandler(m_state);
}

void Connection::Reconnect(std::unique_lock<std::mutex>&& lock) noexcept
{
    // Immediately attempt to reconnect. libHttpClient websocket does not support connecting 
    // the same websocket handle multiple times, so create a new one.
    auto hr = InitializeWebsocket();
    if (FAILED(hr))
    {
        return;
    }

    m_connectAttempt = 0;
    m_state = XblRealTimeActivityConnectionState::Connecting;
    lock.unlock();

    m_stateChangedHandler(m_state);
    m_websocket->Connect(s_rtaUri, s_rtaSubprotocol);
}

void Connection::DisconnectHandler(WebSocketCloseStatus status) noexcept
{
    LOGS_DEBUG << __FUNCTION__ << ": WebocketCloseStatus [" << static_cast<uint32_t>(status) << "]";

    List<AsyncContext<Result<void>>> unsubscribeAsyncContexts;
    List<AsyncContext<Result<void>>> subscribeAsyncContexts;

    std::unique_lock<std::mutex> lock{ m_lock };

    // All subs are inactive if we are disconnected
    m_activeSubs.clear();

    // Update state of our subs
    for (auto subsIter = m_subs.begin(); subsIter != m_subs.end();)
    {
        auto subState{ subsIter->second->m_state };
        switch (subState->serviceStatus)
        {
        case Subscription::State::ServiceStatus::Inactive:
        case Subscription::State::ServiceStatus::Active:
        case Subscription::State::ServiceStatus::Subscribing:
        {
            ++subsIter;
            break;
        }
        case Subscription::State::ServiceStatus::PendingSubscribe:
        {
            // Complete the Unsubscribe AsyncContext, but since the client re-added the subscription,
            // don't remove it from our state
            unsubscribeAsyncContexts.emplace_back(std::move(m_unsubscribeAsyncContexts[subState->clientId]));
            m_unsubscribeAsyncContexts.erase(subState->clientId);

            ++subsIter;
            break;
        }
        // For subscriptions which removed by clients, complete the relevant AsyncContexts and erase the
        // subscription from our state
        case Subscription::State::ServiceStatus::PendingUnsubscribe:
        {
            subscribeAsyncContexts.emplace_back(std::move(m_subscribeAsyncContexts[subState->clientId]));
            m_subscribeAsyncContexts.erase(subState->clientId);

            // Intentional fallthrough
        }
        case Subscription::State::ServiceStatus::Unsubscribing:
        {
            unsubscribeAsyncContexts.emplace_back(std::move(m_unsubscribeAsyncContexts[subState->clientId]));
            m_unsubscribeAsyncContexts.erase(subState->clientId);

            subsIter = m_subs.erase(subsIter);
            break;
        }
        default:
        {
            assert(false);
            break;
        }
        }

        subState->serviceId = 0;
        subState->subscribeAttempt = 0;
        subState->serviceStatus = Subscription::State::ServiceStatus::Inactive;
    }

    m_state = XblRealTimeActivityConnectionState::Disconnected;

    // On GDK, if the cause of the disconnection is that the title went into suspended mode
    // Don't reconnect right away and wait for the title to exit suspended mode first.
    // Otherwise, attempt to reconnect.
#if HC_PLATFORM == HC_PLATFORM_GDK
    if (!this->m_isSuspended) {
#endif
        Reconnect(std::move(lock));

#if HC_PLATFORM == HC_PLATFORM_GDK
    }
    else 
    {
        lock.unlock();
    }
#endif

    for (auto& async : unsubscribeAsyncContexts)
    {
        // Consider any unsubscribes successful. Service will eventually drop our subscriptions
        // since we disconnected anyways
        async.Complete(S_OK);
    }

    for (auto& async : subscribeAsyncContexts)
    {
        async.Complete(E_XBL_RTA_GENERIC_ERROR);
    }
}

void Connection::WebsocketMessageReceived(const String& message) noexcept
{
    // Payload format defined here http://xboxwiki/wiki/Real_Time_Activity

    LOGS_DEBUG << __FUNCTION__ << "[" << this << "]: " << message;

    JsonDocument msgJson;
    msgJson.Parse(message.c_str());
    MessageType messageType = static_cast<MessageType>(msgJson[0].GetInt());

    switch (messageType)
    {
    case MessageType::Subscribe:
    {
        SubscribeResponseHandler(msgJson);
        break;
    }
    case MessageType::Unsubscribe:
    {
        UnsubscribeResponseHandler(msgJson);
        break;
    }
    case MessageType::Event:
    {
        EventHandler(msgJson);
        break;
    }
    case MessageType::Resync:
    {
        ResyncHandler();
        break;
    }
    default:
    {
        LOGS_ERROR << "Received unrecognized RTA payload, ignoring";
        break;
    }
    }
}

HRESULT Connection::InitializeWebsocket() noexcept
{
    if (m_websocket)
    {
        m_websocket->SetConnectCompleteHandler([](WebsocketResult) {});
        m_websocket->SetDisconnectHandler([](WebSocketCloseStatus) {});
        m_websocket->SetReceiveHandler([](String) {});
    }

    auto copyUserResult = m_user.Copy();
    RETURN_HR_IF_FAILED(copyUserResult.Hresult());

    m_websocket = IWebsocket::Make(copyUserResult.ExtractPayload(), m_queue);

    std::weak_ptr<Connection> thisWeakPtr{ shared_from_this() };

    m_websocket->SetConnectCompleteHandler([thisWeakPtr](WebsocketResult result)
    {
        auto sharedThis{ thisWeakPtr.lock() };
        if (sharedThis)
        {
            sharedThis->ConnectCompleteHandler(result);
        }
    });

    m_websocket->SetDisconnectHandler([thisWeakPtr](WebSocketCloseStatus status)
    {
        auto sharedThis{ thisWeakPtr.lock() };
        if (sharedThis)
        {
            sharedThis->DisconnectHandler(status);
        }
    });

    m_websocket->SetReceiveHandler([thisWeakPtr](String message)
    {
        auto sharedThis{ thisWeakPtr.lock() };
        if (sharedThis)
        {
            sharedThis->WebsocketMessageReceived(message);
        }
    });

    return S_OK;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_CPP_END