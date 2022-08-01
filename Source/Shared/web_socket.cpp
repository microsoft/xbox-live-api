// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "web_socket.h"
#include "xsapi_utils.h"
#if XSAPI_UNIT_TESTS
#include "mock_web_socket.h"
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

using namespace xbox::services::system;

// Context passed to LHC XAsync APIs. Ensures WebSocket is kept alive until those calls complete
struct XAsyncContext
{
	XAsyncContext(std::shared_ptr<Websocket> _websocket) : websocket{ std::move(_websocket) }
	{
	}
	std::shared_ptr<Websocket> websocket;
};

Websocket::Websocket(
    _In_ User&& user,
    _In_ TaskQueue queue
) noexcept :
    m_user{ std::move(user) },
    m_queue{ std::move(queue) }
{
    HCWebSocketCreate(&m_hcWebsocket, ReceiveHandler, BinaryReceiveHandler, CloseHandler, this);
}

Websocket::~Websocket()
{
    if (m_hcWebsocket)
    {
        HCWebSocketCloseHandle(m_hcWebsocket);
    }
}

HRESULT Websocket::Connect(
    _In_ const String& uri,
    _In_ const String& subProtocol
) noexcept
{
    auto state = GlobalState::Get();
    if (!state)
    {
        return E_XBL_NOT_INITIALIZED;
    }

    m_user.GetTokenAndSignature("GET", uri, HttpHeaders{}, nullptr, 0, false, AsyncContext<Result<TokenAndSignature>>{
        m_queue.GetHandle(),
        [
            uri = String{ uri },
            subProtocol = String{ subProtocol },
            locales = state->Locales(),
            weakThis{ std::weak_ptr<Websocket>{ shared_from_this() } }
        ]
        (Result<TokenAndSignature> authResult)
    {
        auto sharedThis{ weakThis.lock() };
        if (!sharedThis)
        {
            LOGS_DEBUG << "Websocket object destroyed before auth call completed";
            return;
        }
        else if (Failed(authResult))
        {
            sharedThis->m_connectCompleteHandler(WebsocketResult{ authResult.Hresult(), 0 });
            return;
        }
        else
        {
            const auto& authPayload = authResult.Payload();

            HCWebSocketSetHeader(sharedThis->m_hcWebsocket, "Authorization", authPayload.token.data());
            HCWebSocketSetHeader(sharedThis->m_hcWebsocket, "Signature", authPayload.signature.data());
            HCWebSocketSetHeader(sharedThis->m_hcWebsocket, "Accept-Language", locales.data());

            xsapi_internal_string userAgent = DEFAULT_USER_AGENT;
            HCWebSocketSetHeader(sharedThis->m_hcWebsocket, "User-Agent", userAgent.data());

			auto asyncContext = MakeUnique<XAsyncContext>(sharedThis);
            auto asyncBlock = MakeUnique<XAsyncBlock>();
            asyncBlock->queue = sharedThis->m_queue.GetHandle();
			asyncBlock->context = asyncContext.get();
            asyncBlock->callback = [](_In_ XAsyncBlock* asyncBlock)
            {
                UniquePtr<XAsyncBlock> asyncUnique{ asyncBlock };
				UniquePtr<XAsyncContext> asyncContext{ static_cast<XAsyncContext*>(asyncBlock->context) };

                WebSocketCompletionResult hcResult{};
                HRESULT hr = HCGetWebSocketConnectResult(asyncBlock, &hcResult);

                WebsocketResult result{ hr };
                if (SUCCEEDED(hr))
                {
                    result.hr = hcResult.errorCode;
                    result.platformErrorCode = hcResult.platformErrorCode;
                }
                asyncContext->websocket->m_connectCompleteHandler(result);
            };

            auto hr = HCWebSocketConnectAsync(uri.data(), subProtocol.data(), sharedThis->m_hcWebsocket, asyncBlock.get());
            if (SUCCEEDED(hr))
            {
                asyncBlock.release();
				asyncContext.release();
            }
            else
            {
                sharedThis->m_connectCompleteHandler(WebsocketResult{ hr, 0 });
            }
        }
    }
    });

    return S_OK;
}

HRESULT Websocket::Send(_In_ const char* message) noexcept
{
	auto asyncContext = MakeUnique<XAsyncContext>(shared_from_this());
    auto asyncBlock = MakeUnique<XAsyncBlock>();
    asyncBlock->queue = m_queue.GetHandle();
    asyncBlock->context = asyncContext.get();
    asyncBlock->callback = [](_In_ XAsyncBlock* asyncBlock)
    {
        UniquePtr<XAsyncBlock> asyncUnique{ asyncBlock };
		UniquePtr<XAsyncContext> asyncContext{ static_cast<XAsyncContext*>(asyncBlock->context) };

        WebSocketCompletionResult hcResult{};
        HRESULT hr = HCGetWebSocketSendMessageResult(asyncBlock, &hcResult);

        WebsocketResult result{ hr };
        if (SUCCEEDED(hr))
        {
            result.hr = hcResult.errorCode;
            result.platformErrorCode = hcResult.platformErrorCode;
        }
        asyncContext->websocket->m_sendCompleteHandler(result);
    };

    HRESULT hr = HCWebSocketSendMessageAsync(m_hcWebsocket, message, asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        asyncBlock.release();
		asyncContext.release();
    }
    return hr;
}

HRESULT Websocket::Disconnect() noexcept
{
    return HCWebSocketDisconnect(m_hcWebsocket);
}

void Websocket::ReceiveHandler(
    _In_ HCWebsocketHandle /*websocket*/,
    _In_z_ const char* incomingBodyString,
    _In_ void* functionContext
)
{
    auto thisPtr{ static_cast<Websocket*>(functionContext) };
    thisPtr->m_receiveHandler(incomingBodyString);
}

void Websocket::BinaryReceiveHandler(
    _In_ HCWebsocketHandle /*websocket*/,
    _In_reads_bytes_(payloadSize) const uint8_t* payloadBytes,
    _In_ uint32_t payloadSize,
    _In_ void* functionContext
)
{
    auto thisPtr{ static_cast<Websocket*>(functionContext) };
    thisPtr->m_receiveHandler(xsapi_internal_string{ reinterpret_cast<const char*>(payloadBytes), payloadSize });
}

void Websocket::CloseHandler(
    _In_ HCWebsocketHandle /*websocket*/,
    _In_ HCWebSocketCloseStatus closeStatus,
    _In_ void* functionContext
)
{
    auto thisPtr{ static_cast<Websocket*>(functionContext) };
    thisPtr->m_disconnectHandler(closeStatus);
}

std::shared_ptr<IWebsocket> IWebsocket::Make(
    User&& user,
    TaskQueue queue
) noexcept
{

#if XSAPI_UNIT_TESTS
    auto webSocket = MakeShared<MockWebsocket>(std::move(user), std::move(queue));
#else
    auto webSocket = MakeShared<Websocket>(std::move(user), std::move(queue));
#endif

    return webSocket;
}

void IWebsocket::SetConnectCompleteHandler(_In_ Callback<WebsocketResult> connectCompleteHandler) noexcept
{
    std::lock_guard<std::recursive_mutex> lock{ m_mutex };
    m_connectCompleteHandler = std::move(connectCompleteHandler);
}

void IWebsocket::SetDisconnectHandler(_In_ Callback<WebSocketCloseStatus> disconnectHandler) noexcept
{
    std::lock_guard<std::recursive_mutex> lock{ m_mutex };
    m_disconnectHandler = std::move(disconnectHandler);
}

void IWebsocket::SetSendCompleteHandler(_In_ Callback<WebsocketResult> sendCompleteHandler) noexcept
{
    std::lock_guard<std::recursive_mutex> lock{ m_mutex };
    m_sendCompleteHandler = std::move(sendCompleteHandler);
}

void IWebsocket::SetReceiveHandler(_In_ Callback<xsapi_internal_string> receiveHandler) noexcept
{
    std::lock_guard<std::recursive_mutex> lock{ m_mutex };
    m_receiveHandler = std::move(receiveHandler);

}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
