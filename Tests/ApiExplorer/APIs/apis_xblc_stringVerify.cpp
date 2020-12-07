// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"

int XblStringVerifyStringAsync_Lua(lua_State *L)
{
    CreateQueueIfNeeded();
    char testString[] = { 'F', 'u', 'c', 'k', 's', 't', 'r', 'i', 'n', 'g', '\0' }; 

    // CODE SNIPPET START: XblStringVerifyStringAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        size_t resultSize;
        HRESULT hr = XblStringVerifyStringResultSize(asyncBlock, &resultSize);
        if (SUCCEEDED(hr))
        {
            std::vector<char> buffer(resultSize, 0);
            XblVerifyStringResult* result{};

            hr = XblStringVerifyStringResult(asyncBlock, resultSize, buffer.data(), &result, nullptr);
            LogToFile(
                "Result: Result Code: %d - First Offending String: %s",
                result->resultCode,
                (result->resultCode == XblVerifyStringResultCode::Offensive) ? result->firstOffendingSubstring : ""); // CODE SNIP SKIP
        }

        CallLuaFunctionWithHr(hr, "OnTestStringVerify"); // CODE SNIP SKIP
    };

    HRESULT hr = XblStringVerifyStringAsync(
        Data()->xboxLiveContext,
        testString,
        asyncBlock.get()
    );
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }

    // CODE SNIPPET END
    return LuaReturnHR(L, hr);
}

int XblStringVerifyStringsAsync_Lua(lua_State *L)
{
    CreateQueueIfNeeded();
    char testString1[] = { 'S', 'h', 'i', 't', 's', 't', 'r', 'i', 'n', 'g', '\0' }; // CODE SNIP SKIP
    char testString2[] = { 'G', 'o', 'o', 'd', 's', 't', 'r', 'i', 'n', 'g', '\0' }; // CODE SNIP SKIP
    char testString3[] = { 'S', 'h', 'i', 't', 's', 't', 'r', 'i', 'n', 'g', '2', '\0' }; // CODE SNIP SKIP
    const char* testStrings[] = { testString1, testString2, testString3 };

    // CODE SNIPPET START: XblStringVerifyStringsAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        size_t resultSize;
        HRESULT hr = XblStringVerifyStringsResultSize(asyncBlock, &resultSize);
        if (SUCCEEDED(hr))
        {
            std::vector<char> buffer(resultSize, 0);
            XblVerifyStringResult* result{};
            size_t resultCount = 0;
            hr = XblStringVerifyStringsResult(asyncBlock, resultSize, buffer.data(), &result, &resultCount, nullptr);
            LogToScreen("XblStringVerifyStringsResult: %s count=%d", ConvertHR(hr).c_str(), resultCount); // CODE SNIP SKIP
            if (SUCCEEDED(hr)) // CODE SNIP SKIP
            { // CODE SNIP SKIP
                assert(resultCount == 3); // CODE SNIP SKIP
            }
            
            for (uint64_t i = 0; i < resultCount; i++) // CODE SNIP SKIP
            { // CODE SNIP SKIP
                LogToFile(
                    "Result [%d]: Result Code: %d - First Offending String: %s", 
                    i, 
                    result->resultCode, 
                    (result->resultCode == XblVerifyStringResultCode::Offensive)? result->firstOffendingSubstring: "" ); // CODE SNIP SKIP
                result++; // CODE SNIP SKIP 
            } // CODE SNIP SKIP
        }
    };
    HRESULT hr = XblStringVerifyStringsAsync(
        Data()->xboxLiveContext,
        testStrings,
        3,
        asyncBlock.get()
    );
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }

    // CODE SNIPPET END
    return LuaReturnHR(L, hr);
}

void SetupAPIs_XblStringVerify()
{
    lua_register(Data()->L, "XblStringVerifyStringAsync", XblStringVerifyStringAsync_Lua);
    lua_register(Data()->L, "XblStringVerifyStringsAsync", XblStringVerifyStringsAsync_Lua);
}