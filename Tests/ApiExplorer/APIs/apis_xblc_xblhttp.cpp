// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"
#include <inttypes.h>

#define CHECKHR(hr) if (FAILED(hr)) goto Cleanup;

int XblHttpCallCreate_Lua(lua_State *L)
{
    std::string methodName = GetStringFromLua(L, 1, "GET");
    char url[300];
    sprintf_s(url, "https://achievements.xboxlive.com/users/xuid(%" PRIu64 ")/achievements?titleId=1979882317&maxItems=100", Data()->xboxUserId);

    XblHttpCallHandle output;
    HRESULT hr = XblHttpCallCreate(Data()->xboxLiveContext, methodName.c_str(), url, &output);
    XblHttpCallRequestSetHeader(output, "Content-Type", "application/json; charset=utf-8", true);
    XblHttpCallRequestSetHeader(output, "Accept-Language", "en-US,en", true);
    XblHttpCallRequestSetHeader(output, "x-xbl-contract-version", "2", true);

    Data()->xblHttpCall = output;
    LogToFile("XblHttpCallCreate: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblHttpCallPerform_Lua(lua_State *L)
{
    auto asyncBlock = std::make_unique<XAsyncBlock>(); 
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = Data()->xblHttpCall;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        HRESULT hr = XAsyncGetStatus(asyncBlock, false);
        LogToFile("XblHttpCallPerformAsync result: hr=%s", ConvertHR(hr).c_str());

        if (SUCCEEDED(hr))
        {
            const char* responseString;
            hr = XblHttpCallGetResponseString(Data()->xblHttpCall, &responseString);
            LogToFile("XblHttpCallResponseGetResponseString: length %d, hr=%s", strlen(responseString), ConvertHR(hr).c_str());
            CHECKHR(hr);

            uint32_t statusCode;
            hr = XblHttpCallGetStatusCode(Data()->xblHttpCall, &statusCode);
            LogToFile("XblHttpCallResponseGetStatusCode: %d, hr=%s", statusCode, ConvertHR(hr).c_str());
            CHECKHR(hr);

            HRESULT networkErrorCode;
            uint32_t platformNetworkErrorCode;
            hr = XblHttpCallGetNetworkErrorCode(Data()->xblHttpCall, &networkErrorCode, &platformNetworkErrorCode);
            LogToFile("XblHttpCallResponseGetNetworkErrorCode: codehr=%s, rawcode=%d, hr=%s",
                ConvertHR(networkErrorCode).c_str(),
                platformNetworkErrorCode,
                ConvertHR(hr).c_str());
            CHECKHR(hr);

            const char* errorMsg;
            hr = XblHttpCallGetPlatformNetworkErrorMessage(Data()->xblHttpCall, &errorMsg);
            LogToFile("XblHttpCallResponseGetPlatformNetworkErrorMessage: hr=%s, msg=%s", ConvertHR(hr).c_str(), errorMsg);
            CHECKHR(hr);

            const char* cv;
            hr = XblHttpCallGetHeader(Data()->xblHttpCall, "MS-CV", &cv);
            LogToFile("XblHttpCallResponseGetHeader: cv=%s, hr=%s", cv, ConvertHR(hr).c_str());
            CHECKHR(hr);

            uint32_t numHeaders;
            hr = XblHttpCallGetNumHeaders(Data()->xblHttpCall, &numHeaders);
            LogToFile("XblHttpCallResponseGetNumHeaders: num=%d, hr=%s", numHeaders, ConvertHR(hr).c_str());
            CHECKHR(hr);

            if (numHeaders > 0)
            {
                const char* headerName;
                const char* headerValue;

                hr = XblHttpCallGetHeaderAtIndex(Data()->xblHttpCall, 0, &headerName, &headerValue);
                LogToFile("XblHttpCallResponseGetHeaderAtIndex: name: %s, val: %s, hr: %s", headerName, headerValue, ConvertHR(hr).c_str());
                CHECKHR(hr);
            }
        }

    Cleanup:
        LogToFile("XblHttpCallPerformAsync Completion: hr=%s", ConvertHR(hr).c_str());
        CallLuaFunctionWithHr(hr, "XblHttpCallPerformCompleted");
    };

    HRESULT hr = XblHttpCallPerformAsync(Data()->xblHttpCall, XblHttpCallResponseBodyType::String, asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    LogToFile("XblHttpCallPerformAsync: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblHttpCallDuplicate_Lua(lua_State *L)
{
    LogToFile("Starting duplicate testing...");
    XblHttpCallHandle duplicatedHandle;
    HRESULT hr = XblHttpCallDuplicateHandle(Data()->xblHttpCall, &duplicatedHandle);
    LogToFile("XblHttpCallDuplicateHandle: hr=%s", ConvertHR(hr).c_str());
    LogToFile("  orig: %p dup: %p", Data()->xblHttpCall, duplicatedHandle);
    XblHttpCallCloseHandle(duplicatedHandle);
    LogToFile("Closed duplicate handle", ConvertHR(hr).c_str());
    LogToFile("  orig: %p dup: %p", Data()->xblHttpCall, duplicatedHandle);

    return LuaReturnHR(L, hr);
}

int XblHttpCallValidateSetters_Lua(lua_State *L)
{
    LogToFile("Starting setters testing...");
    uint8_t bodyBytes[] = { 0xD, 0xE, 0xA, 0xD, 0xB, 0xE, 0xE, 0xF };

    HRESULT hr = XblHttpCallSetTracing(Data()->xblHttpCall, false);
    LogToFile("XblHttpCallSetTracing: hr=%s", ConvertHR(hr).c_str());
    CHECKHR(hr);

    hr = XblHttpCallRequestSetRequestBodyBytes(
        Data()->xblHttpCall,
        bodyBytes,
        sizeof(bodyBytes)
    );
    LogToFile("XblHttpCallRequestSetRequestBodyBytes: hr=%s", ConvertHR(hr).c_str());
    CHECKHR(hr);

    hr = XblHttpCallRequestSetRequestBodyString(Data()->xblHttpCall, "requestBodyStringContent");
    LogToFile("XblHttpCallRequestSetRequestBodyString: hr=%s", ConvertHR(hr).c_str());
    CHECKHR(hr);

    hr = XblHttpCallRequestSetRetryAllowed(Data()->xblHttpCall, false);
    LogToFile("XblHttpCallRequestSetRetryAllowed: hr=%s", ConvertHR(hr).c_str());
    CHECKHR(hr);

    hr = XblHttpCallRequestSetRetryCacheId(Data()->xblHttpCall, 12345);
    LogToFile("XblHttpCallRequestSetRetryCacheId: hr=%s", ConvertHR(hr).c_str());
    CHECKHR(hr);

    hr = XblHttpCallRequestSetLongHttpCall(Data()->xblHttpCall, true);
    LogToFile("XblHttpCallRequestSetLongHttpCall: hr=%s", ConvertHR(hr).c_str());
    CHECKHR(hr);

Cleanup:
    return LuaReturnHR(L, hr);
}

int XblHttpCallValidateGetters_Lua(lua_State *L)
{
    LogToFile("Starting getters testing...");

    const char* url;
    HRESULT hr = XblHttpCallGetRequestUrl(Data()->xblHttpCall, &url);
    LogToFile("XblHttpCallGetRequestUrl: hr=%s", ConvertHR(hr).c_str());
    LogToFile("  url length: %d", strlen(url));

    return LuaReturnHR(L, hr);
}

void SetupAPIs_XblHttp()
{
    lua_register(Data()->L, "XblHttpCallCreate", XblHttpCallCreate_Lua);
    lua_register(Data()->L, "XblHttpCallPerform", XblHttpCallPerform_Lua);
    lua_register(Data()->L, "XblHttpCallDuplicate", XblHttpCallDuplicate_Lua);
    lua_register(Data()->L, "XblHttpCallValidateGetters", XblHttpCallValidateGetters_Lua);
    lua_register(Data()->L, "XblHttpCallValidateSetters", XblHttpCallValidateSetters_Lua);
}
