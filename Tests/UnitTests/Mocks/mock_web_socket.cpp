// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "mock_web_socket.h"
#include "mock_rta_service.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

MockWebsocket::MockWebsocket(
    User user,
    TaskQueue queue
) noexcept
    : m_user{ std::move(user) },
    m_queue{ std::move(queue) }
{
}

HRESULT MockWebsocket::Connect(
    _In_ const String& uri,
    _In_ const String& subProtocol
) noexcept
{
    UNREFERENCED_PARAMETER(subProtocol);
    UNREFERENCED_PARAMETER(uri);

    return m_queue.RunWork([sharedThis{ shared_from_this() }]
    {
        std::unique_lock<std::recursive_mutex>{ sharedThis->m_mutex };
        if (s_connectHandler == nullptr)
        {
            sharedThis->m_connectCompleteHandler(WebsocketResult{ S_OK });
        }
        else
        {
            sharedThis->m_connectCompleteHandler(s_connectHandler());
        }
    });
}

HRESULT MockWebsocket::Send(_In_ const char* message) noexcept
{
    return m_queue.RunWork([sharedThis{ shared_from_this() }, message = std::string{ message }]
    {
        MockRealTimeActivityService::Instance().HandleClientMessage(sharedThis, message.data());
    });
}

HRESULT MockWebsocket::Disconnect() noexcept
{
    return m_queue.RunWork([sharedThis{ shared_from_this() }]
    {
        sharedThis->m_disconnectHandler(WebSocketCloseStatus::Normal);
    });
}

uint64_t MockWebsocket::Xuid() const noexcept
{
    return m_user.Xuid();
}

HRESULT MockWebsocket::Disconnect(WebSocketCloseStatus closeStatus) const noexcept
{
    m_disconnectHandler(closeStatus);
    return S_OK;
}

void MockWebsocket::ReceiveMessage(
    _In_ const char* message
) const noexcept
{
    m_receiveHandler(message);
}

void MockWebsocket::ReceiveMessage(
    _In_ const rapidjson::Value& message
) const noexcept
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    message.Accept(writer);

    m_receiveHandler(buffer.GetString());
}

void MockWebsocket::SetConnectHandler(ConnectHandler handler) noexcept
{
    s_connectHandler = std::move(handler);
}

MockWebsocket::ConnectHandler MockWebsocket::s_connectHandler{};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END