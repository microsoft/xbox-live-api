// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"

int HCInitialize_Lua(lua_State *L)
{
    // CODE SNIPPET START: HCInitialize
#if HC_PLATFORM == HC_PLATFORM_ANDROID
    HRESULT hr = HCInitialize(&(Data()->initArgs));
#else
    HRESULT hr = HCInitialize(nullptr);
#endif
    // CODE SNIPPET END

    HCSettingsSetTraceLevel(HCTraceLevel::Verbose);
    HCTraceSetTraceToDebugger(true);

    LogToFile("HCInitialize: hr=%s", ConvertHR(hr).c_str()); 
    return LuaReturnHR(L, hr);
}

int HCCleanup_Lua(lua_State *L)
{
    // CODE SNIPPET START: HCCleanup
    HCCleanup();
    // CODE SNIPPET END

    LogToFile("HCCleanup");
    return LuaReturnHR(L, S_OK);
}

int HCCleanupAsync_Lua(lua_State* L)
{
    CreateQueueIfNeeded();

    // CODE SNIPPET START: HCCleanupAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        HRESULT hr = XAsyncGetStatus(asyncBlock, false);
        LogToFile("HCCleanupAsync result: hr=%s", ConvertHR(hr).c_str());
        CallLuaFunctionWithHr(hr, "OnHCCleanupAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = HCCleanupAsync(asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END

    LogToFile("HCCleanupAsync: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int HCGetLibVersion_Lua(lua_State *L)
{
    // CODE SNIPPET START: HCGetLibVersion
    const char* ver = nullptr;
    HRESULT hr = HCGetLibVersion(&ver);
    // CODE SNIPPET END

    LogToFile("HCGetLibVersion: hr=%s %s", ConvertHR(hr).c_str(), ver); 
    return LuaReturnHR(L, hr);
}

int HCHttpCallCreate_Lua(lua_State *L)
{
    // CODE SNIPPET START: HCHttpCallCreate
    HCCallHandle call = nullptr;
    HRESULT hr = HCHttpCallCreate(&call);
    // CODE SNIPPET END
    Data()->httpCall = call;

    LogToFile("HCHttpCallCreate: hr=%s", ConvertHR(hr).c_str()); 
    return LuaReturnHR(L, hr);
}

int HCHttpCallPerformAsync_Lua(lua_State *L)
{
    CreateQueueIfNeeded();

    // CODE SNIPPET START: HCHttpCallPerformAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = Data()->httpCall;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        HRESULT hr = XAsyncGetStatus(asyncBlock, false);
        LogToFile("HCHttpCallPerformAsync result: hr=%s", ConvertHR(hr).c_str()); 
        CallLuaFunctionWithHr(hr, "OnHCHttpCallPerformAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = HCHttpCallPerformAsync(Data()->httpCall, asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END

    LogToFile("HCHttpCallPerformAsync: hr=%s", ConvertHR(hr).c_str()); 
    return LuaReturnHR(L, hr);
}

int HCHttpCallDuplicateHandle_Lua(lua_State *L)
{
    // CODE SNIPPET START: HCHttpCallDuplicateHandle
    HCCallHandle newHttpCall = HCHttpCallDuplicateHandle(Data()->httpCall);
    // CODE SNIPPET END
    HCHttpCallCloseHandle(newHttpCall);

    LogToFile("HCHttpCallDuplicateHandle"); 
    return LuaReturnHR(L, S_OK);
}

int HCHttpCallCloseHandle_Lua(lua_State *L)
{
    // CODE SNIPPET START: HCHttpCallCloseHandle
    HCHttpCallCloseHandle(Data()->httpCall);
    // CODE SNIPPET END

    Data()->httpCall = nullptr;
    LogToFile("HCHttpCallCloseHandle");
    return LuaReturnHR(L, S_OK);
}

int HCHttpCallGetId_Lua(lua_State *L)
{
    // CODE SNIPPET START: HCHttpCallGetId
    uint64_t httpId = HCHttpCallGetId(Data()->httpCall);
    // CODE SNIPPET END
    UNREFERENCED_PARAMETER(httpId);

    LogToFile("HCHttpCallGetId");
    return LuaReturnHR(L, S_OK);
}

int HCHttpCallSetTracing_Lua(lua_State *L)
{
    bool traceCall = GetBoolFromLua(L, 1, true);

    // CODE SNIPPET START: HCHttpCallSetTracing
    HRESULT hr = HCHttpCallSetTracing(Data()->httpCall, traceCall);
    // CODE SNIPPET END

    LogToFile("HCHttpCallSetTracing: hr=%s", ConvertHR(hr).c_str()); 
    return LuaReturnHR(L, hr);
}

int HCHttpCallGetRequestUrl_Lua(lua_State *L)
{
    // CODE SNIPPET START: HCHttpCallGetRequestUrl
    const char* url = nullptr;
    HRESULT hr = HCHttpCallGetRequestUrl(Data()->httpCall, &url);
    // CODE SNIPPET END

    LogToFile("HCHttpCallGetRequestUrl: hr=%s %s", ConvertHR(hr).c_str(), url); 
    return LuaReturnHR(L, hr);
}

int HCHttpCallRequestSetUrl_Lua(lua_State *L)
{
    std::string methodName = GetStringFromLua(L, 1, "GET");
    std::string url = GetStringFromLua(L, 2, "https://www.bing.com");
    // CODE SNIPPET START: HCHttpCallRequestSetUrl
    HRESULT hr = HCHttpCallRequestSetUrl(Data()->httpCall, methodName.c_str(), url.c_str());
    // CODE SNIPPET END

    LogToFile("HCHttpCallRequestSetUrl: hr=%s %s", ConvertHR(hr).c_str(), url.c_str()); 
    return LuaReturnHR(L, hr);
}

int HCHttpCallRequestSetRequestBodyString_Lua(lua_State *L)
{
    std::string requestBodyString = GetStringFromLua(L, 1, "Test");
    // CODE SNIPPET START: HCHttpCallRequestSetRequestBodyString
    HRESULT hr = HCHttpCallRequestSetRequestBodyString(Data()->httpCall, requestBodyString.c_str());
    // CODE SNIPPET END

    LogToFile("HCHttpCallRequestSetRequestBodyString: hr=%s %s", ConvertHR(hr).c_str(), requestBodyString.c_str());
    return LuaReturnHR(L, hr);
}

int HCHttpCallRequestSetHeader_Lua(lua_State *L)
{
    std::string headerName = GetStringFromLua(L, 1, "TestName");
    std::string headerValue = GetStringFromLua(L, 2, "TestValue");
    bool allowTracing = GetBoolFromLua(L, 3, true);
    // CODE SNIPPET START: HCHttpCallRequestSetHeader
    HRESULT hr = HCHttpCallRequestSetHeader(Data()->httpCall, headerName.c_str(), headerValue.c_str(), allowTracing);
    // CODE SNIPPET END

    LogToFile("HCHttpCallRequestSetHeader: hr=%s %s:%s allowTracing=%d", ConvertHR(hr).c_str(), headerName.c_str(), headerValue.c_str(), allowTracing);
    return LuaReturnHR(L, hr);
}

int HCHttpCallRequestSetTimeout_Lua(lua_State *L)
{
    uint32_t timeoutInSeconds = GetUint32FromLua(L, 1, 100);
    // CODE SNIPPET START: HCHttpCallRequestSetTimeout
    HRESULT hr = HCHttpCallRequestSetTimeout(Data()->httpCall, timeoutInSeconds);
    // CODE SNIPPET END

    LogToFile("HCHttpCallRequestSetTimeout: hr=%s %d", ConvertHR(hr).c_str(), timeoutInSeconds); 
    return LuaReturnHR(L, hr);
}

int HCHttpCallRequestSetRetryDelay_Lua(lua_State *L)
{
    uint32_t retryDelayInSeconds = GetUint32FromLua(L, 1, 100);
    // CODE SNIPPET START: HCHttpCallRequestSetRetryDelay
    HRESULT hr = HCHttpCallRequestSetRetryDelay(Data()->httpCall, retryDelayInSeconds);
    // CODE SNIPPET END

    LogToFile("HCHttpCallRequestSetRetryDelay: hr=%s %d", ConvertHR(hr).c_str(), retryDelayInSeconds); 
    return LuaReturnHR(L, hr);
}

int HCHttpCallRequestSetTimeoutWindow_Lua(lua_State *L)
{
    uint32_t timeoutWindowInSeconds = GetUint32FromLua(L, 1, 100);
    // CODE SNIPPET START: HCHttpCallRequestSetTimeoutWindow
    HRESULT hr = HCHttpCallRequestSetTimeoutWindow(Data()->httpCall, timeoutWindowInSeconds);
    // CODE SNIPPET END

    LogToFile("HCHttpCallRequestSetTimeoutWindow: hr=%s %d", ConvertHR(hr).c_str(), timeoutWindowInSeconds); 
    return LuaReturnHR(L, hr);
}

int HCHttpCallRequestSetRetryCacheId_Lua(lua_State *L)
{
    uint32_t retryAfterCacheId = GetUint32FromLua(L, 1, 1);
    // CODE SNIPPET START: HCHttpCallRequestSetRetryCacheId
    HRESULT hr = HCHttpCallRequestSetRetryCacheId(Data()->httpCall, retryAfterCacheId);
    // CODE SNIPPET END

    LogToFile("HCHttpCallRequestSetRetryCacheId: hr=%s %d", ConvertHR(hr).c_str(), retryAfterCacheId); 
    return LuaReturnHR(L, hr);
}

int HCHttpCallRequestSetRetryAllowed_Lua(lua_State *L)
{
    bool retryAllowed = GetBoolFromLua(L, 1, true);
    // CODE SNIPPET START: HCHttpCallRequestSetRetryAllowed
    HRESULT hr = HCHttpCallRequestSetRetryAllowed(Data()->httpCall, retryAllowed);
    // CODE SNIPPET END

    LogToFile("HCHttpCallRequestSetRetryAllowed: hr=%s %d", ConvertHR(hr).c_str(), retryAllowed); 
    return LuaReturnHR(L, hr);
}

int HCHttpCallResponseGetResponseString_Lua(lua_State *L)
{
    // CODE SNIPPET START: HCHttpCallResponseGetResponseString
    const char* response = nullptr;
    HRESULT hr = HCHttpCallResponseGetResponseString(Data()->httpCall, &response);
    // CODE SNIPPET END

    LogToFile("HCHttpCallResponseGetResponseString: hr=%s %s", ConvertHR(hr).c_str(), response); 
    return LuaReturnHR(L, hr);
}

int HCHttpCallResponseGetResponseBodyBytesSize_Lua(lua_State *L)
{
    size_t bufferSize = 0;
    // CODE SNIPPET START: HCHttpCallResponseGetResponseBodyBytesSize
    HRESULT hr = HCHttpCallResponseGetResponseBodyBytesSize(Data()->httpCall, &bufferSize);
    // CODE SNIPPET END

    LogToFile("HCHttpCallResponseGetResponseBodyBytesSize: hr=%s %zu", ConvertHR(hr).c_str(), bufferSize);
    return LuaReturnHR(L, hr);
}

int HCHttpCallResponseGetStatusCode_Lua(lua_State *L)
{
    uint32_t statusCode = 0;
    // CODE SNIPPET START: HCHttpCallResponseGetStatusCode
    HRESULT hr = HCHttpCallResponseGetStatusCode(Data()->httpCall, &statusCode);
    // CODE SNIPPET END

    LogToFile("HCHttpCallResponseGetStatusCode: hr=%s %d", ConvertHR(hr).c_str(), statusCode);
    return LuaReturnHR(L, hr);
}

int HCHttpCallResponseGetNetworkErrorCode_Lua(lua_State *L)
{
    // CODE SNIPPET START: HCHttpCallResponseGetNetworkErrorCode
    HRESULT networkErrorCode = S_OK;
    uint32_t platErrorCode = 0;
    HRESULT hr = HCHttpCallResponseGetNetworkErrorCode(Data()->httpCall, &networkErrorCode, &platErrorCode);
    // CODE SNIPPET END

    LogToFile("HCHttpCallResponseGetNetworkErrorCode: hr=%s networkError=%d platError=%d", ConvertHR(hr).c_str(), networkErrorCode, platErrorCode);
    return LuaReturnHR(L, hr);
}

int HCHttpCallResponseGetHeader_Lua(lua_State *L)
{
    std::string headerName = GetStringFromLua(L, 1, "UserAgent");
    // CODE SNIPPET START: HCHttpCallResponseGetHeader
    const char* headerValue = nullptr;
    HRESULT hr = HCHttpCallResponseGetHeader(Data()->httpCall, headerName.c_str(), &headerValue);
    // CODE SNIPPET END

    LogToFile("HCHttpCallResponseGetHeader: hr=%s %s:%s", ConvertHR(hr).c_str(), headerName.c_str(), headerValue);
    return LuaReturnHR(L, hr);
}

int HCHttpCallResponseGetNumHeaders_Lua(lua_State *L)
{
    uint32_t numHeaders = 0;
    // CODE SNIPPET START: HCHttpCallResponseGetNumHeaders
    HRESULT hr = HCHttpCallResponseGetNumHeaders(Data()->httpCall, &numHeaders);
    // CODE SNIPPET END

    LogToFile("HCHttpCallResponseGetNumHeaders: hr=%s %d", ConvertHR(hr).c_str(), numHeaders);
    return LuaReturnHR(L, hr);
}

int HCHttpCallResponseGetHeaderAtIndex_Lua(lua_State *L)
{
    uint32_t headerIndex = GetUint32FromLua(L, 1, 0);
    // CODE SNIPPET START: HCHttpCallResponseGetHeaderAtIndex
    const char* headerName = nullptr;
    const char* headerValue = nullptr;
    HRESULT hr = HCHttpCallResponseGetHeaderAtIndex(Data()->httpCall, headerIndex, &headerName, &headerValue);
    // CODE SNIPPET END

    LogToFile("HCHttpCallResponseGetHeaderAtIndex: hr=%s %d=%s:%s", ConvertHR(hr).c_str(), headerIndex, headerName, headerValue);
    return LuaReturnHR(L, hr);
}

void CALLBACK ExampleWebsocketMessageReceived(
    _In_ HCWebsocketHandle websocket,
    _In_z_ const char* incomingBodyString,
    _In_ void* functionContext
)
{
    UNREFERENCED_PARAMETER(websocket);
    UNREFERENCED_PARAMETER(functionContext);
    LogToFile("Received message from websocket: %s", incomingBodyString);
    CallLuaFunction("OnHCWebsocketMessageReceived");
}

void CALLBACK ExampleWebsocketClosed(
    _In_ HCWebsocketHandle websocket,
    _In_ HCWebSocketCloseStatus closeStatus,
    _In_ void* functionContext
)
{
    UNREFERENCED_PARAMETER(websocket);
    UNREFERENCED_PARAMETER(functionContext);
    LogToFile("Websocket closed with status %u", closeStatus);
    CallLuaFunction("OnHCWebsocketClosed");
}

int HCWebSocketCreate_Lua(lua_State *L)
{
    // CODE SNIPPET START: HCWebSocketCreate
    HCWebsocketHandle websocket = nullptr;
    HRESULT hr = HCWebSocketCreate(&websocket, ExampleWebsocketMessageReceived, nullptr, ExampleWebsocketClosed, nullptr);
    // CODE SNIPPET END

    Data()->websocket = websocket;
    LogToFile("HCWebSocketCreate: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int HCWebSocketSetProxyUri_Lua(lua_State *L)
{
    std::string proxyUri = GetStringFromLua(L, 1, "Proxy");
    // CODE SNIPPET START: HCWebSocketSetProxyUri
    HRESULT hr = HCWebSocketSetProxyUri(Data()->websocket, proxyUri.c_str());
    // CODE SNIPPET END

    LogToFile("HCWebSocketSetProxyUri: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int HCWebSocketSetHeader_Lua(lua_State *L)
{
    std::string headerName = GetStringFromLua(L, 1, "TestName");
    std::string headerValue = GetStringFromLua(L, 2, "TestValue");
    // CODE SNIPPET START: HCWebSocketSetHeader
    HRESULT hr = HCWebSocketSetHeader(Data()->websocket, headerName.c_str(), headerValue.c_str());
    // CODE SNIPPET END

    LogToFile("HCWebSocketSetHeader: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int HCWebSocketConnectAsync_Lua(lua_State *L)
{
    std::string uri = GetStringFromLua(L, 1, "ws://localhost:9002");
    std::string subProtocol = GetStringFromLua(L, 2, "");

    // CODE SNIPPET START: HCWebSocketConnectAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        WebSocketCompletionResult result{};
        HRESULT hr = HCGetWebSocketConnectResult(asyncBlock, &result);
        LogToFile("HCGetWebSocketConnectResult: hr=%s errorCode=0x%0.8x", ConvertHR(hr).c_str(), result.errorCode); // CODE SNIP SKIP
        CallLuaFunctionWithHr(hr, "OnHCWebSocketConnectAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = HCWebSocketConnectAsync(uri.c_str(), subProtocol.c_str(), Data()->websocket, asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END

    LogToFile("HCWebSocketConnectAsync: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int HCWebSocketSendMessageAsync_Lua(lua_State *L)
{
    std::string message = GetStringFromLua(L, 1, "Test");

    // CODE SNIPPET START: HCWebSocketSendMessageAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>(); 
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        WebSocketCompletionResult result{};
        HRESULT hr = HCGetWebSocketSendMessageResult(asyncBlock, &result);
        LogToFile("HCGetWebSocketSendMessageResult: hr=%s errorCode=0x%0.8x", ConvertHR(hr).c_str(), result.errorCode); // CODE SNIP SKIP
        CallLuaFunctionWithHr(hr, "OnHCWebSocketSendMessageAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = HCWebSocketSendMessageAsync(Data()->websocket, message.c_str(), asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END

    LogToFile("HCWebSocketSendMessageAsync: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int HCWebSocketDisconnect_Lua(lua_State *L)
{
    // CODE SNIPPET START: HCWebSocketDisconnect
    HRESULT hr = HCWebSocketDisconnect(Data()->websocket);
    // CODE SNIPPET END

    LogToFile("HCWebSocketDisconnect: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int HCWebSocketDuplicateHandle_Lua(lua_State *L)
{
    // CODE SNIPPET START: HCWebSocketDuplicateHandle
    HCWebsocketHandle newWebsocket = HCWebSocketDuplicateHandle(Data()->websocket);
    // CODE SNIPPET END
    HCWebSocketCloseHandle(newWebsocket);

    LogToFile("HCWebSocketDuplicateHandle");
    return LuaReturnHR(L, S_OK);
}

int HCWebSocketCloseHandle_Lua(lua_State *L)
{
    // CODE SNIPPET START: HCWebSocketCloseHandle
    HRESULT hr = HCWebSocketCloseHandle(Data()->websocket);
    // CODE SNIPPET END

    LogToFile("HCWebSocketCloseHandle: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int HCTraceSetTraceToDebugger_Lua(lua_State *L)
{
    // CODE SNIPPET START: HCTraceSetTraceToDebugger
    HCSettingsSetTraceLevel(HCTraceLevel::Verbose); // See HCTraceLevel enum for various levels
    // TODO: jasonsa - confirm works
    HCTraceSetTraceToDebugger(true);
    // CODE SNIPPET END

    LogToFile("HCTraceSetTraceToDebugger");
    return LuaReturnHR(L, S_OK);
}

int HCSettingsSetTraceLevel_Lua(lua_State *L)
{
    // CODE SNIPPET START: HCSettingsSetTraceLevel
    HCSettingsSetTraceLevel(HCTraceLevel::Verbose);
    // CODE SNIPPET END

    LogToFile("HCSettingsSetTraceLevel");
    return LuaReturnHR(L, S_OK);
}

int HCMockCallCreate_Lua(lua_State *L)
{
    // CODE SNIPPET START: HCMockCallCreate
    HRESULT hr = HCMockCallCreate(&Data()->mockHttpCall);
    // CODE SNIPPET END

    LogToFile("HCMockCallCreate");
    return LuaReturnHR(L, hr);
}

int HCMockAddMock_Lua(lua_State *L)
{
    auto method = GetStringFromLua(L, 1, std::string{});
    auto url = GetStringFromLua(L, 2, std::string{});

    // CODE SNIPPET START: HCMockAddMock
    HRESULT hr = HCMockAddMock(
        Data()->mockHttpCall,
        method.empty() ? nullptr : method.data(),
        url.empty() ? nullptr : url.data(),
        nullptr,
        0
    );
    // CODE SNIPPET END

    LogToFile("HCMockAddMock");
    return LuaReturnHR(L, hr);
}

int HCMockClearMocks_Lua(lua_State *L)
{
    // CODE SNIPPET START: HCMockClearMocks
    HRESULT hr = HCMockClearMocks();
    // CODE SNIPPET END

    Data()->mockHttpCall = nullptr;
    LogToFile("HCMockClearMocks");
    return LuaReturnHR(L, hr);
}

int HCMockResponseSetNetworkErrorCode_Lua(lua_State *L)
{
    auto platformErrorCode = GetUint32FromLua(L, 1, 0);

    // CODE SNIPPET START: HCMockResponseSetNetworkErrorCode
    HRESULT hr = HCMockResponseSetNetworkErrorCode(Data()->mockHttpCall, E_FAIL, platformErrorCode);
    // CODE SNIPPET END

    LogToFile("HCMockResponseSetNetworkErrorCode");
    return LuaReturnHR(L, hr);
}

#if HC_PLATFORM == HC_PLATFORM_GDK
namespace xbox
{
namespace httpclient
{
extern void HCWinHttpSuspend();
extern void HCWinHttpResume();
}
}

int HCWinHttpSuspend_lua(lua_State *L)
{
    UNREFERENCED_PARAMETER(L);
    xbox::httpclient::HCWinHttpSuspend();
    return 0;
}

int HCWinHttpResume_lua(lua_State *L)
{
    UNREFERENCED_PARAMETER(L);
    xbox::httpclient::HCWinHttpResume();
    return 0;
}
#endif

void SetupAPIs_LibHttp()
{
    //lua_register(Data()->L, "HCMemSetFunctions", HCMemSetFunctions_Lua);
    //lua_register(Data()->L, "HCMemGetFunctions", HCMemGetFunctions_Lua);

    lua_register(Data()->L, "HCInitialize", HCInitialize_Lua);
    lua_register(Data()->L, "HCCleanup", HCCleanup_Lua);
    lua_register(Data()->L, "HCCleanupAsync", HCCleanupAsync_Lua);
    lua_register(Data()->L, "HCGetLibVersion", HCGetLibVersion_Lua);
    //lua_register(Data()->L, "HCAddCallRoutedHandler", HCAddCallRoutedHandler_Lua);
    //lua_register(Data()->L, "HCRemoveCallRoutedHandler", HCRemoveCallRoutedHandler_Lua);
    lua_register(Data()->L, "HCHttpCallCreate", HCHttpCallCreate_Lua);
    lua_register(Data()->L, "HCHttpCallPerformAsync", HCHttpCallPerformAsync_Lua);
    lua_register(Data()->L, "HCHttpCallDuplicateHandle", HCHttpCallDuplicateHandle_Lua);
    lua_register(Data()->L, "HCHttpCallCloseHandle", HCHttpCallCloseHandle_Lua);
    lua_register(Data()->L, "HCHttpCallGetId", HCHttpCallGetId_Lua);
    lua_register(Data()->L, "HCHttpCallSetTracing", HCHttpCallSetTracing_Lua);
    lua_register(Data()->L, "HCHttpCallGetRequestUrl", HCHttpCallGetRequestUrl_Lua);

    lua_register(Data()->L, "HCHttpCallRequestSetUrl", HCHttpCallRequestSetUrl_Lua);
    //lua_register(Data()->L, "HCHttpCallRequestSetRequestBodyBytes", HCHttpCallRequestSetRequestBodyBytes_Lua);
    lua_register(Data()->L, "HCHttpCallRequestSetRequestBodyString", HCHttpCallRequestSetRequestBodyString_Lua);
    lua_register(Data()->L, "HCHttpCallRequestSetHeader", HCHttpCallRequestSetHeader_Lua);
    lua_register(Data()->L, "HCHttpCallRequestSetRetryAllowed", HCHttpCallRequestSetRetryAllowed_Lua);
    lua_register(Data()->L, "HCHttpCallRequestSetRetryCacheId", HCHttpCallRequestSetRetryCacheId_Lua);
    lua_register(Data()->L, "HCHttpCallRequestSetTimeout", HCHttpCallRequestSetTimeout_Lua);
    lua_register(Data()->L, "HCHttpCallRequestSetRetryDelay", HCHttpCallRequestSetRetryDelay_Lua);
    lua_register(Data()->L, "HCHttpCallRequestSetTimeoutWindow", HCHttpCallRequestSetTimeoutWindow_Lua);

    lua_register(Data()->L, "HCHttpCallResponseGetResponseString", HCHttpCallResponseGetResponseString_Lua);
    lua_register(Data()->L, "HCHttpCallResponseGetResponseBodyBytesSize", HCHttpCallResponseGetResponseBodyBytesSize_Lua);
    //lua_register(Data()->L, "HCHttpCallResponseGetResponseBodyBytes", HCHttpCallResponseGetResponseBodyBytes_Lua);
    lua_register(Data()->L, "HCHttpCallResponseGetStatusCode", HCHttpCallResponseGetStatusCode_Lua);
    lua_register(Data()->L, "HCHttpCallResponseGetNetworkErrorCode", HCHttpCallResponseGetNetworkErrorCode_Lua);
    lua_register(Data()->L, "HCHttpCallResponseGetHeader", HCHttpCallResponseGetHeader_Lua);
    lua_register(Data()->L, "HCHttpCallResponseGetNumHeaders", HCHttpCallResponseGetNumHeaders_Lua);
    lua_register(Data()->L, "HCHttpCallResponseGetHeaderAtIndex", HCHttpCallResponseGetHeaderAtIndex_Lua);

    lua_register(Data()->L, "HCWebSocketCreate", HCWebSocketCreate_Lua);
    lua_register(Data()->L, "HCWebSocketSetProxyUri", HCWebSocketSetProxyUri_Lua);
    lua_register(Data()->L, "HCWebSocketSetHeader", HCWebSocketSetHeader_Lua);
    lua_register(Data()->L, "HCWebSocketConnectAsync", HCWebSocketConnectAsync_Lua);
    lua_register(Data()->L, "HCWebSocketSendMessageAsync", HCWebSocketSendMessageAsync_Lua);
    lua_register(Data()->L, "HCWebSocketDisconnect", HCWebSocketDisconnect_Lua);
    lua_register(Data()->L, "HCWebSocketDuplicateHandle", HCWebSocketDuplicateHandle_Lua);
    lua_register(Data()->L, "HCWebSocketCloseHandle", HCWebSocketCloseHandle_Lua);

    lua_register(Data()->L, "HCTraceSetTraceToDebugger", HCTraceSetTraceToDebugger_Lua);
    lua_register(Data()->L, "HCSettingsSetTraceLevel", HCSettingsSetTraceLevel_Lua);

    lua_register(Data()->L, "HCMockCallCreate", HCMockCallCreate_Lua);
    lua_register(Data()->L, "HCMockAddMock", HCMockAddMock_Lua);
    lua_register(Data()->L, "HCMockClearMocks", HCMockClearMocks_Lua);
    lua_register(Data()->L, "HCMockResponseSetNetworkErrorCode", HCMockResponseSetNetworkErrorCode_Lua);

#if HC_PLATFORM == HC_PLATFORM_GDK
    lua_register(Data()->L, "HCWinHttpSuspend", HCWinHttpSuspend_lua);
    lua_register(Data()->L, "HCWinHttpResume", HCWinHttpResume_lua);
#endif
}

