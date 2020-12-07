// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "web_socket.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

class MockWebsocket : public IWebsocket, public std::enable_shared_from_this<MockWebsocket>
{
public:
    MockWebsocket(
        User user,
        TaskQueue queue
    ) noexcept;

    // IWebsocket
    HRESULT Connect(
        _In_ const String& uri,
        _In_ const String& subProtocol
    ) noexcept override;

    HRESULT Send(_In_ const char* message) noexcept override;

    HRESULT Disconnect() noexcept override;

    // MockWebsocket
    uint64_t Xuid() const noexcept;

    HRESULT Disconnect(
        WebSocketCloseStatus closeStatus
    ) const noexcept;

    void ReceiveMessage(
        _In_ const char* message
    ) const noexcept;

    void ReceiveMessage(
        _In_ const rapidjson::Value& message
    ) const noexcept;

    // By default MockWebsocket::Connect will asyncronously complete successfully.
    // To alter connection behavior, set a custom ConnectHandler
    using ConnectHandler = std::function<WebsocketResult()>;
    static void SetConnectHandler(ConnectHandler handler) noexcept;

private:
    TaskQueue m_queue{};
    User const m_user;

    static ConnectHandler s_connectHandler;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
