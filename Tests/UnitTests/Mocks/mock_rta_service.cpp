// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "mock_rta_service.h"
#include "mock_web_socket.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

// RTA Protocol desribed http://xboxwiki/wiki/Real_Time_Activity

enum class MessageType : uint32_t
{
    Subscribe = 1,
    Unsubscribe = 2,
    Event = 3,
    Resync = 4
};

MockRealTimeActivityService& MockRealTimeActivityService::Instance() noexcept
{
    static MockRealTimeActivityService s_instance{};
    return s_instance;
}

void MockRealTimeActivityService::HandleClientMessage(
    std::shared_ptr<MockWebsocket> socket,
    const char* message
) noexcept
{
    std::unique_lock<std::mutex> lock{ m_mutex };

    rapidjson::Document rtaMessage{ rapidjson::kArrayType };
    rtaMessage.Parse(message);
    assert(!rtaMessage.HasParseError());

    auto messageType{ static_cast<MessageType>(rtaMessage[0].GetUint()) };
    switch (messageType)
    {
    case MessageType::Subscribe:
    {
        // Subscribe messages will not be automatically be acknowledged since they require
        // a service specific response payload.
        auto sequenceNumber{ rtaMessage[1].GetUint() };
        auto uri{ rtaMessage[2].GetString() };
        auto subId{ m_nextSubId++ };

        m_clients[socket][subId] = Subscription{ uri, sequenceNumber };

        auto handler{ m_subscribeHandler };
        lock.unlock();

        if (handler)
        {
            handler(subId, uri);
        }
        return;
    }
    case MessageType::Unsubscribe:
    {
        // Unsubscribe messages will be automatically acknowledged
        auto sequenceNumber{ rtaMessage[1].GetUint() };
        auto subId{ rtaMessage[2].GetUint() };

        rapidjson::Document response{ rapidjson::kArrayType };
        auto& a{ response.GetAllocator() };

        response.PushBack(static_cast<uint32_t>(messageType), a);
        response.PushBack(sequenceNumber, a);

        auto& subs{ m_clients[socket] };
        auto subIter{ subs.find(subId) };
        if (subIter != subs.end())
        {
            assert(subIter->second.active);
            response.PushBack(static_cast<uint32_t>(ErrorCode::Success), a);
            subs.erase(subIter);
        }
        else
        {
            // TODO figure out what is actually returned by service in this case
            response.PushBack(5u, a);
        }

        lock.unlock();
        socket->ReceiveMessage(response);
        return;
    }
    default:
    {
        assert(false);
        return;
    }
    }
}

void MockRealTimeActivityService::SetSubscribeHandler(
    SubscribeHandler handler
) noexcept
{
    std::unique_lock<std::mutex> lock{ m_mutex };
    m_subscribeHandler = std::move(handler);
}

void MockRealTimeActivityService::CompleteSubscribeHandshake(
    uint32_t subId,
    const rapidjson::Value& payload,
    ErrorCode errorCode
) noexcept
{
    std::unique_lock<std::mutex> lock{ m_mutex };
    for (auto& clientPair : m_clients)
    {
        auto& clientSubs{ clientPair.second };
        auto subIter{ clientSubs.find(subId) };
        if (subIter != clientSubs.end())
        {
            if (auto socket{ clientPair.first.lock() })
            {
                rapidjson::Document response{ rapidjson::kArrayType };
                auto& a{ response.GetAllocator() };

                response.PushBack(static_cast<uint32_t>(MessageType::Subscribe), a);
                response.PushBack(subIter->second.clientSequenceNumber, a);
                response.PushBack(static_cast<uint32_t>(errorCode), a);
                response.PushBack(subId, a);
                response.PushBack(rapidjson::Value{}.CopyFrom(payload, a), a);

                assert(!subIter->second.active);
                subIter->second.active = true;
                socket->ReceiveMessage(response);
            }
            else
            {
                // Client Socket has been destroyed, clean up state for that client
                m_clients.erase(clientPair.first);
            }
            break;
        }
    }
}

void MockRealTimeActivityService::CompleteSubscribeHandshake(
    uint32_t subId,
    const char* payload,
    ErrorCode errorCode
) noexcept
{
    rapidjson::Document d;
    d.Parse(payload);
    assert(!d.HasParseError());
    CompleteSubscribeHandshake(subId, d, errorCode);
}

void MockRealTimeActivityService::RaiseEvent(
    const xsapi_internal_string& uri,
    const rapidjson::Value& payload
) noexcept
{
    std::unique_lock<std::mutex> lock{ m_mutex };

    rapidjson::Document eventMessage{ rapidjson::kArrayType };
    auto& a{ eventMessage.GetAllocator() };

    eventMessage.PushBack(static_cast<uint32_t>(MessageType::Event), a);
    eventMessage.PushBack(0u, a);
    eventMessage.PushBack(rapidjson::Value{}.CopyFrom(payload, a).Move(), a);

    for (auto clientIter = m_clients.begin(); clientIter != m_clients.end();)
    {
        if (auto socket{ clientIter->first.lock() })
        {
            for (auto& subPair : clientIter->second)
            {
                if (subPair.second.active && subPair.second.uri == uri)
                {
                    eventMessage[1] = subPair.first;
                    socket->ReceiveMessage(eventMessage);
                }
            }
            clientIter++;
        }
        else
        {
            // Client Socket has been destroyed, clean up state for that client
            clientIter = m_clients.erase(clientIter);
        }
    }
}

void MockRealTimeActivityService::RaiseEvent(
    const xsapi_internal_string& uri,
    const char* payload
) noexcept
{
    rapidjson::Document d;
    d.Parse(payload);
    assert(!d.HasParseError());
    RaiseEvent(uri, d);
}

void MockRealTimeActivityService::RaiseResync() noexcept
{
    std::unique_lock<std::mutex> lock{ m_mutex };

    const char resyncMessage[]{ "[4]" };
    for (auto clientIter = m_clients.begin(); clientIter != m_clients.end();)
    {
        if (auto socket{ clientIter->first.lock() })
        {
            socket->ReceiveMessage(resyncMessage);
            clientIter++;
        }
        else
        {
            // Client Socket has been destroyed, clean up state for that client
            clientIter = m_clients.erase(clientIter);
        }
    }
}

void MockRealTimeActivityService::DisconnectClient(
    uint64_t xuid
) noexcept
{
    std::unique_lock<std::mutex> lock{ m_mutex };

    for (auto clientIter = m_clients.begin(); clientIter != m_clients.end();)
    {
        if (auto socket{ clientIter->first.lock() })
        {
            if (socket->Xuid() == xuid)
            {
                socket->Disconnect(WebSocketCloseStatus::GoingAway);
                clientIter = m_clients.erase(clientIter);
            }
            else
            {
                clientIter++;
            }
        }
        else
        {
            clientIter = m_clients.erase(clientIter);
        }
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END