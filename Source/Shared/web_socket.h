// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

typedef HCWebSocketCloseStatus WebSocketCloseStatus;

struct WebsocketResult
{
    HRESULT hr;
    uint32_t platformErrorCode;
};

// Base class for Websocket and MockWebsocket
class IWebsocket
{
public:
    static std::shared_ptr<IWebsocket> Make(
        User&& user,
        TaskQueue queue
    ) noexcept;

    virtual ~IWebsocket() = default;

    virtual void SetConnectCompleteHandler(_In_ Callback<WebsocketResult> connectCompleteHandler) noexcept;

    virtual void SetDisconnectHandler(_In_ Callback<WebSocketCloseStatus> disconnectHandler) noexcept;

    virtual void SetSendCompleteHandler(_In_ Callback<WebsocketResult> sendCompleteHandler) noexcept;

    virtual void SetReceiveHandler(_In_ Callback<String> receiveHandler) noexcept;

    virtual HRESULT Connect(
        _In_ const String& uri,
        _In_ const String& subProtocol
    ) noexcept = 0;

    virtual HRESULT Send(_In_ const char* message) noexcept = 0;

    virtual HRESULT Disconnect() noexcept = 0;

protected:
    std::recursive_mutex m_mutex;
    Callback<WebsocketResult> m_connectCompleteHandler;
    Callback<WebSocketCloseStatus> m_disconnectHandler;
    Callback<String> m_receiveHandler;
    Callback<WebsocketResult> m_sendCompleteHandler;
};

class Websocket : public IWebsocket, public std::enable_shared_from_this<Websocket>
{
public:
    Websocket(
        _In_ User&& user,
        _In_ TaskQueue queue
    ) noexcept;

    ~Websocket();

    HRESULT Connect(
        _In_ const String& uri,
        _In_ const String& subProtocol
    ) noexcept override;

    HRESULT Send(_In_ const char* message) noexcept override;

    HRESULT Disconnect() noexcept override;

private:
    static void CALLBACK ReceiveHandler(
        _In_ HCWebsocketHandle websocket,
        _In_z_ const char* incomingBodyString,
        _In_ void* functionContext
    );

    static void CALLBACK BinaryReceiveHandler(
        _In_ HCWebsocketHandle websocket,
        _In_reads_bytes_(payloadSize) const uint8_t* payloadBytes,
        _In_ uint32_t payloadSize,
        _In_ void* functionContext
    );

    static void CALLBACK CloseHandler(
        _In_ HCWebsocketHandle websocket,
        _In_ HCWebSocketCloseStatus closeStatus,
        _In_ void* functionContext
    );

    HCWebsocketHandle m_hcWebsocket{ nullptr };
    User m_user;
    TaskQueue m_queue;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
