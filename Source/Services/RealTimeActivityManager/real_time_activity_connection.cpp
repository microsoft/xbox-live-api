// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "real_time_activity_subscription.h"
#include "real_time_activity_connection.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_CPP_BEGIN

constexpr char s_rtaUri[]{ "wss://rta.xboxlive.com/connect" };
constexpr char s_rtaSubprotocol[]{ "rta.xboxlive.com.V2" };

struct ServiceSubscription
{
    ServiceSubscription(String _uri, uint32_t _clientId) noexcept : uri{ std::move(_uri) }, clientId { _clientId } {}

    // Resource uri for the subscription
    String const uri;

    // Client ID is assigned by XSAPI and used to identify subscriptions.
    // Used as the unique SEQUENCE_N during RTA Sub/Unsub handshakes.
    uint32_t const clientId;

    // Assigned by service when we successfully subscribe. Used to identify subscription in RTA event messages.
    uint32_t serviceId{ 0 };

    // Current status with respect to RTA service
    enum class Status : uint32_t
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
    } status{ Status::Inactive };

    uint32_t subscribeAttempt{ 0 };

    Set<std::shared_ptr<Subscription>> clientSubscriptions;
    List<AsyncContext<Result<void>>> subscribeAsyncContexts;
    List<AsyncContext<Result<void>>> unsubscribeAsyncContexts;

    // OnSubscribe data payload
    JsonDocument onSubscribeData{ rapidjson::kNullType };
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

    rtaConnection->m_stateChangedHandler(rtaConnection->m_state);
    rtaConnection->ScheduleConnect();

    return rtaConnection;
}

void Connection::Cleanup()
{
    std::unique_lock<std::mutex> lock{ m_lock };

    if (m_websocket)
    {
        // Clear our disconnect handler to disable auto-reconnect logic
        m_websocket->SetDisconnectHandler(nullptr);
        m_websocket->Disconnect();
    }

    List<AsyncContext<Result<void>>> pendingAsyncContexts;
    for (auto& subPair : m_subsByClientId)
    {
        auto& sub{ subPair.second };
        pendingAsyncContexts.insert(pendingAsyncContexts.end(), sub->subscribeAsyncContexts.begin(), sub->subscribeAsyncContexts.end());
        sub->subscribeAsyncContexts.clear();
        pendingAsyncContexts.insert(pendingAsyncContexts.end(), sub->unsubscribeAsyncContexts.begin(), sub->unsubscribeAsyncContexts.end());      
        sub->unsubscribeAsyncContexts.clear();
    }

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

HRESULT Connection::AddSubscription(
    std::shared_ptr<Subscription> sub,
    AsyncContext<Result<void>> async
) noexcept
{
    assert(sub);
    std::unique_lock<std::mutex> lock{ m_lock };

    std::shared_ptr<ServiceSubscription> serviceSub{ nullptr };
    auto serviceSubIter = m_subsByUri.find(sub->ResourceUri());
    if (serviceSubIter != m_subsByUri.end())
    {
        serviceSub = serviceSubIter->second;
    }
    else
    {
        serviceSub = MakeShared<ServiceSubscription>(sub->ResourceUri(), m_nextSubId++);
        assert(m_subsByClientId.find(serviceSub->clientId) == m_subsByClientId.end());
        m_subsByClientId[serviceSub->clientId] = serviceSub;
        m_subsByUri[serviceSub->uri] = serviceSub;
    }

    serviceSub->clientSubscriptions.emplace(sub);

    LOGS_DEBUG << __FUNCTION__ << ": [" << serviceSub->clientId << "] Uri=" << serviceSub->uri << ", ServiceStatus=" << EnumName(serviceSub->status);

    switch (serviceSub->status)
    {
    case ServiceSubscription::Status::Inactive:
    {
        serviceSub->subscribeAsyncContexts.push_back(std::move(async));

        // If our connection is active, immediately register with RTA service
        if (m_state == XblRealTimeActivityConnectionState::Connected)
        {
            return SendSubscribeMessage(serviceSub, std::move(lock));
        }
        return S_OK;
    }
    case ServiceSubscription::Status::PendingUnsubscribe:
    {
        // Client previously removed subscription while we were subscribing. Reset the state to subscribing and complete unsubscribe
        // operations with E_ABORT

        serviceSub->status = ServiceSubscription::Status::Subscribing;
        serviceSub->subscribeAsyncContexts.push_back(std::move(async));

        List<AsyncContext<Result<void>>> unsubscribeAsyncContexts{ std::move(serviceSub->unsubscribeAsyncContexts) };

        lock.unlock();

        for (auto& asyncContext : unsubscribeAsyncContexts)
        {
            asyncContext.Complete(E_ABORT);
        }

        return S_OK;
    }
    case ServiceSubscription::Status::Unsubscribing:
    {
        // Wait for unsubscribe to finish before resubscribing
        serviceSub->status = ServiceSubscription::Status::PendingSubscribe;
        serviceSub->subscribeAsyncContexts.push_back(std::move(async));
        return S_OK;
    }
    case ServiceSubscription::Status::Active:
    {
        // Subscription is already active, trivially complete
        lock.unlock();
        // Pass along original OnSubscribe payload for this subscription
        sub->OnSubscribe(serviceSub->onSubscribeData);
        async.Complete(S_OK);
        return S_OK;
    }
    case ServiceSubscription::Status::PendingSubscribe:
    case ServiceSubscription::Status::Subscribing:
    {
        serviceSub->subscribeAsyncContexts.push_back(std::move(async));
        return S_OK;
    }
    default:
    {
        assert(false);
        return E_UNEXPECTED;
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

    std::shared_ptr<ServiceSubscription> serviceSub{ nullptr };
    auto serviceSubIter = m_subsByUri.find(sub->ResourceUri());
    if (serviceSubIter != m_subsByUri.end())
    {
        serviceSub = serviceSubIter->second;
    }
    else
    {
        return S_OK;
    }

    serviceSub->clientSubscriptions.erase(sub);

    LOGS_DEBUG << __FUNCTION__ << ": [" << serviceSub->clientId << "] Uri=" << serviceSub->uri << ", ServiceStatus=" << EnumName(serviceSub->status);

    if (!serviceSub->clientSubscriptions.empty())
    {
        // Service subscription still needed by other clients. Complete asyncContext but don't unsubscribe from service
        lock.unlock();

        async.Complete(S_OK);
        return S_OK;
    }

    switch (serviceSub->status)
    {
    case ServiceSubscription::Status::Inactive:
    {
        // RTA service has no knowledge of inactive subs. Just remove from our local state and complete the AsyncContext.
        assert(serviceSub->serviceId == 0);
        m_subsByClientId.erase(serviceSub->clientId);
        m_subsByUri.erase(serviceSub->uri);

        lock.unlock();

        async.Complete(S_OK);
        return S_OK;
    }
    case ServiceSubscription::Status::Active:
    {
        // Unregister subscription from RTA service
        serviceSub->unsubscribeAsyncContexts.push_back(std::move(async));
        return SendUnsubscribeMessage(serviceSub, std::move(lock));
    }
    case ServiceSubscription::Status::PendingSubscribe:
    {
        // Client previously added the subscription while we were unsubscribing. Reset the state to unsubscribe and complete
        // subscribe operations with E_ABORT

        serviceSub->status = ServiceSubscription::Status::Unsubscribing;
        serviceSub->unsubscribeAsyncContexts.push_back(std::move(async));
        List<AsyncContext<Result<void>>> subscribeAsyncContexts{ std::move(serviceSub->subscribeAsyncContexts) };
        
        lock.unlock();

        for (auto& asyncContext : subscribeAsyncContexts)
        {
            asyncContext.Complete(E_ABORT);
        }

        return S_OK;
    }
    case ServiceSubscription::Status::Subscribing:
    {
        // We are in the process of subscribing. RTA protocol doesn't allow us to unsubscribe
        // until subscription is complete, so just mark the subscription as pending unsubscribe.
        // After the subscription completes, we will unsubscribe and complete the AsyncContext.
        serviceSub->status = ServiceSubscription::Status::PendingUnsubscribe;
        serviceSub->unsubscribeAsyncContexts.push_back(std::move(async));

        return S_OK;
    }
    case ServiceSubscription::Status::PendingUnsubscribe:
    case ServiceSubscription::Status::Unsubscribing:
    {
        serviceSub->unsubscribeAsyncContexts.push_back(std::move(async));

        return S_OK;
    }
    default:
    {
        assert(false);
        return E_UNEXPECTED;
    }
    }
}

size_t Connection::SubscriptionCount() const noexcept
{
    std::unique_lock<std::mutex> lock{ m_lock };
    return m_subsByClientId.size();
}

JsonDocument Connection::AssembleSubscribeMessage(std::shared_ptr<ServiceSubscription> sub) const noexcept
{
    // Payload format [<API_ID>, <SEQUENCE_N>, “<RESOURCE_URI>”]

    sub->status = ServiceSubscription::Status::Subscribing;

    JsonDocument request{ rapidjson::kArrayType };
    auto& a{ request.GetAllocator() };

    request.PushBack(static_cast<uint32_t>(MessageType::Subscribe), a);
    request.PushBack(sub->clientId, a);
    request.PushBack(JsonValue{ sub->uri.data(), a }, a);

    return request;
}

HRESULT Connection::SendSubscribeMessage(
    std::shared_ptr<ServiceSubscription> sub,
    std::unique_lock<std::mutex>&& lock
) const noexcept
{
    JsonDocument request = AssembleSubscribeMessage(sub);

    lock.unlock();

    return SendAssembledMessage(request);
}

HRESULT Connection::SendAssembledMessage(_In_ const JsonValue& request) const noexcept
{
    String requestString{ JsonUtils::SerializeJson(request) };
    LOGS_DEBUG << __FUNCTION__ << "[" << this << "]: " << requestString;

    return m_websocket->Send(requestString.data());
}

HRESULT Connection::SendUnsubscribeMessage(
    std::shared_ptr<ServiceSubscription> sub,
    std::unique_lock<std::mutex>&& lock
) const noexcept
{
    // Payload format [<API_ID>, <SEQUENCE_N>, <SUB_ID>]

    sub->status = ServiceSubscription::Status::Unsubscribing;

    JsonDocument request{ rapidjson::kArrayType };
    auto& a{ request.GetAllocator() };

    request.PushBack(static_cast<uint32_t>(MessageType::Unsubscribe), a);
    request.PushBack(sub->clientId, a);
    request.PushBack(sub->serviceId, a);

    lock.unlock();

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

    auto subIter{ m_subsByClientId.find(clientId) };
    if (subIter == m_subsByClientId.end())
    {
        // Ignore unexpected message
        assert(false);
        LOGS_DEBUG << "__FUNCTION__" << ": [" << clientId << "] Ignoring unexpected message";
        return;
    }
    auto serviceSub{ subIter->second };

    switch (errorCode)
    {
    case ErrorCode::Success:
    {
        serviceSub->serviceId = message[3].GetInt();
        serviceSub->onSubscribeData.CopyFrom(message[4], serviceSub->onSubscribeData.GetAllocator());

        m_subsByServiceId[serviceSub->serviceId] = serviceSub;
        List<AsyncContext<Result<void>>> subscribeAsyncContexts{ std::move(serviceSub->subscribeAsyncContexts) };
        List<std::shared_ptr<Subscription>> clientSubs{ serviceSub->clientSubscriptions.begin(), serviceSub->clientSubscriptions.end() };

        switch (serviceSub->status)
        {
        case ServiceSubscription::Status::Subscribing:
        {
            serviceSub->status = ServiceSubscription::Status::Active;
            break;
        }
        case ServiceSubscription::Status::PendingUnsubscribe:
        {
            // Client has removed the subscription while subscribe handshake was happening,
            // so immediately begin unsubscribing.
            SendUnsubscribeMessage(serviceSub, std::move(lock));
            break;
        }
        default:
        {
            // Any other Status indicates a XSAPI bug
            assert(false);
            break;
        }
        }

        if (lock)
        {
            lock.unlock();
        }

        for (auto& asyncContext : subscribeAsyncContexts)
        {
            asyncContext.Complete(ConvertRTAErrorCode(errorCode));
        }
        for (auto& clientSub : clientSubs)
        {
            clientSub->OnSubscribe(serviceSub->onSubscribeData);
        }

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
        auto serviceStatus{ serviceSub->status };
        serviceSub->status = ServiceSubscription::Status::Inactive;

        switch (serviceStatus)
        {
        case ServiceSubscription::Status::Subscribing:
        {
            uint64_t backoff = __min(std::pow(serviceSub->subscribeAttempt++, 2), 60) * 1000;
            m_queue.RunWork([weakSub = std::weak_ptr<ServiceSubscription>{ serviceSub }, weakThis = std::weak_ptr<Connection>{ shared_from_this() }]
                {
                    auto sharedThis{ weakThis.lock() };
                    if (sharedThis)
                    {
                        std::unique_lock<std::mutex> lock{ sharedThis->m_lock };

                        auto serviceSub{ weakSub.lock() };
                        if (serviceSub && serviceSub->status == ServiceSubscription::Status::Inactive)
                        {
                            sharedThis->SendSubscribeMessage(serviceSub, std::move(lock));
                        }
                    }
                },
                backoff
            );

            return;
        }
        case ServiceSubscription::Status::PendingUnsubscribe:
        {
            m_subsByClientId.erase(serviceSub->clientId);
            m_subsByUri.erase(serviceSub->uri);
            
            // Complete subscribe operations with error, but don't retry since the client has since removed the subscription
            List<AsyncContext<Result<void>>> subscribeAsyncContexts{ std::move(serviceSub->subscribeAsyncContexts) };               

            // Unsubscribe operations are also trivially done at this point
            List<AsyncContext<Result<void>>> unsubscribeAsyncContexts{ std::move(serviceSub->unsubscribeAsyncContexts) };

            lock.unlock();

            for (auto& asyncContext : subscribeAsyncContexts)
            {
                asyncContext.Complete(ConvertRTAErrorCode(errorCode));
            }

            for (auto& asyncContext : unsubscribeAsyncContexts)
            {
                asyncContext.Complete(S_OK);
            }

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

    auto subIter{ m_subsByClientId.find(clientId) };
    if (subIter == m_subsByClientId.end())
    {
        // Ignore unexpected message
        LOGS_DEBUG << "__FUNCTION__" << ": [" << clientId << "] Ignoring unexpected message";
        return;
    }
    auto serviceSub{ subIter->second };
    m_subsByServiceId.erase(serviceSub->serviceId);
    serviceSub->serviceId = 0;

    LOGS_DEBUG << __FUNCTION__ << ": [" << serviceSub->clientId <<"] ServiceStatus=" << EnumName(serviceSub->status);

    List<AsyncContext<Result<void>>> unsubscribeAsyncContexts{ std::move(serviceSub->unsubscribeAsyncContexts) };

    switch (serviceSub->status)
    {
    case ServiceSubscription::Status::Unsubscribing:
    {
        // We can now remove the subscription from our state entirely
        m_subsByClientId.erase(serviceSub->clientId);
        m_subsByUri.erase(serviceSub->uri);
        serviceSub->status = ServiceSubscription::Status::Inactive;
        break;
    }
    case ServiceSubscription::Status::PendingSubscribe:
    {
        // Client has re-added the subscription while unsubscibe handshake was happening,
        // so immediately begin subscribing.
        SendSubscribeMessage(serviceSub, std::move(lock));
        break;
    }
    default:
    {
        assert(false);
        break;
    }
    }

    if (lock)
    {
        lock.unlock();
    }

    for (auto& asyncContext : unsubscribeAsyncContexts)
    {
        asyncContext.Complete(ConvertRTAErrorCode(errorCode));
    }
}

void Connection::EventHandler(_In_ const JsonValue& message) const noexcept
{
    // Payload format [<API_ID>, <SUB_ID>, <DATA>]

    std::unique_lock<std::mutex> lock{ m_lock };

    auto serviceId = message[1].GetInt();
    const auto& data = message[2];

    auto subIter{ m_subsByServiceId.find(serviceId) };
    assert(subIter != m_subsByServiceId.end());
    auto serviceSub = subIter->second;

    lock.unlock();

    for (auto& clientSub : serviceSub->clientSubscriptions)
    {
        clientSub->OnEvent(data);
    }
}

void Connection::ConnectCompleteHandler(WebsocketResult result) noexcept
{
    LOGS_DEBUG << __FUNCTION__ << ": WebsocketResult [" << result.hr << "," << result.platformErrorCode << "]";

    std::unique_lock<std::mutex> lock{ m_lock };

    if (SUCCEEDED(result.hr))
    {
        m_state = XblRealTimeActivityConnectionState::Connected;
        m_connectTime = std::chrono::system_clock::now();
        m_connectAttempt = 0;

        assert(m_subsByServiceId.empty());

        List<JsonDocument> subMessages{};
        for (auto& pair : m_subsByClientId)
        {
            assert(pair.second->status == ServiceSubscription::Status::Inactive);
            subMessages.push_back(AssembleSubscribeMessage(pair.second));
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

        lock.unlock();

        for (auto& request : subMessages)
        {
            SendAssembledMessage(request);
        }
    }
    else
    {
        if (m_connectAttempt > 3)
        {
            m_state = XblRealTimeActivityConnectionState::Disconnected;
        }
        ScheduleConnect();
    }

    if (lock)
    {
        lock.unlock();
    }

    m_stateChangedHandler(m_state);
}

void Connection::ScheduleConnect() noexcept
{
    LOGS_DEBUG << __FUNCTION__;

    // Backoff and attempt to connect again. 
    uint64_t backoff = __min(std::pow(m_connectAttempt++, 2), 60) * 1000;

    m_queue.RunWork([weakThis = std::weak_ptr<Connection>{ shared_from_this() }, this]
    {
        auto sharedThis{ weakThis.lock() };
        if (sharedThis)
        {
            std::unique_lock<std::mutex> lock{ m_lock };

            LOGS_DEBUG << "RTA::Connection Initializing WebSocket and attempting connect. Subcount=" << m_subsByClientId.size();

            auto hr = InitializeWebsocket();
            if (FAILED(hr))
            {
                ScheduleConnect();
            }
            else
            {
                m_state = XblRealTimeActivityConnectionState::Connecting;
                lock.unlock();

                sharedThis->m_stateChangedHandler(sharedThis->m_state);
                sharedThis->m_websocket->Connect(s_rtaUri, s_rtaSubprotocol); // Do synchronous failures need to be handled here?
            }
        }
    },
    backoff
    );
}


void Connection::DisconnectHandler(WebSocketCloseStatus status) noexcept
{
    LOGS_DEBUG << __FUNCTION__ << ": WebocketCloseStatus [" << static_cast<uint32_t>(status) << "]";

    List<AsyncContext<Result<void>>> unsubscribeAsyncContexts;
    List<AsyncContext<Result<void>>> subscribeAsyncContexts;

    std::unique_lock<std::mutex> lock{ m_lock };

    // All subs are inactive if we are disconnected
    m_subsByServiceId.clear();

    // Update state of our subs
    for (auto subsIter = m_subsByClientId.begin(); subsIter != m_subsByClientId.end();)
    {
        auto serviceSub{ subsIter->second };
        switch (serviceSub->status)
        {
        case ServiceSubscription::Status::Inactive:
        case ServiceSubscription::Status::Active:
        case ServiceSubscription::Status::Subscribing:
        {
            ++subsIter;
            break;
        }
        case ServiceSubscription::Status::PendingSubscribe:
        {
            // Complete the Unsubscribe AsyncContext, but since the client re-added the subscription,
            // don't remove it from our state
            unsubscribeAsyncContexts.insert(unsubscribeAsyncContexts.end(), serviceSub->unsubscribeAsyncContexts.begin(), serviceSub->unsubscribeAsyncContexts.end());
            serviceSub->unsubscribeAsyncContexts.clear();

            ++subsIter;
            break;
        }
        // For subscriptions which removed by clients, complete the relevant AsyncContexts and erase the
        // subscription from our state
        case ServiceSubscription::Status::PendingUnsubscribe:
        {
            subscribeAsyncContexts.insert(subscribeAsyncContexts.end(), serviceSub->subscribeAsyncContexts.begin(), serviceSub->subscribeAsyncContexts.end());
            serviceSub->subscribeAsyncContexts.clear();

            // Intentional fallthrough
        }
        case ServiceSubscription::Status::Unsubscribing:
        {
            unsubscribeAsyncContexts.insert(unsubscribeAsyncContexts.end(), serviceSub->unsubscribeAsyncContexts.begin(), serviceSub->unsubscribeAsyncContexts.end());
            serviceSub->unsubscribeAsyncContexts.clear();

            m_subsByUri.erase(serviceSub->uri);
            subsIter = m_subsByClientId.erase(subsIter);
            break;
        }
        default:
        {
            assert(false);
            break;
        }
        }

        serviceSub->serviceId = 0;
        serviceSub->subscribeAttempt = 0;
        serviceSub->status = ServiceSubscription::Status::Inactive;
    }

    ScheduleConnect();
    lock.unlock();

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
        m_resyncHandler();
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
    LOGS_DEBUG << __FUNCTION__;

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