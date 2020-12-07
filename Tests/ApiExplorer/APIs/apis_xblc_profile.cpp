// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"

int XblProfileGetUserProfileAsync_Lua(lua_State *L)
{
    CreateQueueIfNeeded();

    // CODE SNIPPET START: XblProfileGetUserProfileAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>(); 
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        XblUserProfile profile = { 0 };
        HRESULT hr = XblProfileGetUserProfileResult(asyncBlock, &profile);
        LogToFile("XblProfileGetUserProfileResult: hr=%s gamertag=%s", ConvertHR(hr).c_str(), profile.gamertag); // CODE SNIP SKIP
        CallLuaFunctionWithHr(hr, "OnXblProfileGetUserProfileAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = XblProfileGetUserProfileAsync(Data()->xboxLiveContext, Data()->xboxUserId, asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END
    LogToFile("XblProfileGetUserProfileAsync: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblProfileGetUserProfilesAsync_Lua(lua_State *L)
{
    CreateQueueIfNeeded();

    // CODE SNIPPET START: XblProfileGetUserProfilesAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>(); 
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        uint32_t profilesCount = 1;
        XblUserProfile profiles[1] = { 0 };
        HRESULT hr = XblProfileGetUserProfilesResult(asyncBlock, profilesCount, profiles);
        LogToFile("XblProfileGetUserProfilesResult: hr=%s gamertag=%s", ConvertHR(hr).c_str(), profiles[0].gamertag); // CODE SNIP SKIP
        CallLuaFunctionWithHr(hr, "OnXblProfileGetUserProfilesAsync"); // CODE SNIP SKIP
    };

    uint64_t xboxUserIds[1];
    xboxUserIds[0] = Data()->xboxUserId;
    size_t xboxUserIdsCount = 1;

    HRESULT hr = XblProfileGetUserProfilesAsync(Data()->xboxLiveContext, xboxUserIds, xboxUserIdsCount, asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END
    LogToFile("XblProfileGetUserProfilesAsync: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblProfileGetUserProfilesForSocialGroupAsync_Lua(lua_State *L)
{
    CreateQueueIfNeeded();
    auto socialGroup = GetStringFromLua(L, 1, "People");

    // CODE SNIPPET START: XblProfileGetUserProfilesForSocialGroupAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>(); 
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        size_t profilesCount = 0;
        HRESULT hr = XblProfileGetUserProfilesForSocialGroupResultCount(asyncBlock, &profilesCount);
        if (SUCCEEDED(hr) && profilesCount > 0)
        {
            std::vector<XblUserProfile> profiles(profilesCount);
            hr = XblProfileGetUserProfilesForSocialGroupResult(asyncBlock, profilesCount, profiles.data());
            LogToFile("XblProfileGetUserProfilesForSocialGroupResult: hr=%s profilesCount=%d gamertag=%s", ConvertHR(hr).c_str(), profilesCount, profiles[0].gamertag); // CODE SNIP SKIP
        }
        else if (hr == HTTP_E_STATUS_429_TOO_MANY_REQUESTS)
        {
            CallLuaFunctionWithHr(S_OK, "OnXblProfileGetUserProfilesForSocialGroupAsyncRetry");
            return;
        }

        CallLuaFunctionWithHr(hr, "OnXblProfileGetUserProfilesForSocialGroupAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = XblProfileGetUserProfilesForSocialGroupAsync(Data()->xboxLiveContext, socialGroup.c_str(), asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END
    LogToFile("XblProfileGetUserProfilesForSocialGroupAsync: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

void SetupAPIs_XblProfile()
{
    lua_register(Data()->L, "XblProfileGetUserProfileAsync", XblProfileGetUserProfileAsync_Lua);
    lua_register(Data()->L, "XblProfileGetUserProfilesAsync", XblProfileGetUserProfilesAsync_Lua);
    lua_register(Data()->L, "XblProfileGetUserProfilesForSocialGroupAsync", XblProfileGetUserProfilesForSocialGroupAsync_Lua);
}
