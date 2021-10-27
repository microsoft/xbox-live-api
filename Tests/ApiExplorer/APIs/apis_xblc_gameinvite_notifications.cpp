// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"

XBL_WARNING_DISABLE_DEPRECATED

#if HC_PLATFORM == HC_PLATFORM_WIN32

int XblGameInviteAddNotificationHandler_Lua(lua_State *L)
{
    CreateQueueIfNeeded();

    // CODE SNIPPET START: XblGameInviteAddNotificationHandler
    void* context = nullptr;
    auto t = Data()->xboxLiveContext;
    XblFunctionContext gameinviteFunctionContext = XblGameInviteAddNotificationHandler(
        t,
        [](_In_ const XblGameInviteNotificationEventArgs* args, _In_opt_ void*)
        {
            LogToScreen("XblGameInviteAddNotificationHandler");
            LogToScreen("Invite Handle ID:");
            LogToScreen(args->inviteHandleId);
            LogToScreen("Invite Protocol:");
            LogToScreen(args->inviteProtocol);
            LogToScreen("Invite Context:");
            LogToScreen(args->inviteContext);
            LogToScreen("Sender Gamertag:");
            LogToScreen(args->senderGamertag);
            LogToScreen("Modern Gamertag:");
            LogToScreen(args->senderModernGamertag);
            LogToScreen("Modern Gamertag Suffix:");
            LogToScreen(args->senderModernGamertagSuffix);
            LogToScreen("Unique Modern Gamertag:");
            LogToScreen(args->senderUniqueModernGamertag);
            LogToScreen("Sender Gamertag:");
            LogToScreen(args->senderGamertag);
            LogToScreen("Sender Image URL:");
            LogToScreen(args->senderImageUrl);
            std::string s2 = std::to_string(args->senderXboxUserId);
            LogToScreen("Sender Xbox User ID:");
            LogToScreen(s2.c_str());
            CallLuaFunctionWithHr(S_OK, "OnXblGameInviteAddNotificationHandler"); // CODE SNIP SKIP
        },
        context
    );
    // CODE SNIPPET END
    
    Data()->gameInviteNotificationFunctionContext = gameinviteFunctionContext;

    LogToScreen("XblGameInviteAddNotificationHandler: done");
    return LuaReturnHR(L, S_OK);
}

int XblGameInviteRemoveNotificationHandler_Lua(lua_State *L)
{
    CreateQueueIfNeeded();

    // CODE SNIPPET START: XblGameInviteRemoveNotificationHandler
    XblGameInviteRemoveNotificationHandler(
        Data()->xboxLiveContext,
        Data()->gameInviteNotificationFunctionContext
    );
    Data()->gameInviteNotificationFunctionContext = 0;
    // CODE SNIPPET END

    LogToScreen("XblGameInviteRemoveNotificationHandler: done");

    return LuaReturnHR(L, S_OK);
}

int XblGameInviteRegisterForEventAsync_Lua(lua_State *L)
{
    CreateQueueIfNeeded();

    // CODE SNIPPET START: XblGameInviteRegisterForEventAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        XblRealTimeActivitySubscriptionHandle subscriptionHandle{ nullptr };
        HRESULT hr = XblGameInviteRegisterForEventResult(asyncBlock, &subscriptionHandle);
        Data()->gameInviteNotificationSubscriptionHandle = subscriptionHandle; // CODE SNIP SKIP
        
        LogToFile("XblGameInviteRegisterForEventResult: hr=%s", ConvertHR(hr).c_str()); // CODE SNIP SKIP
        CallLuaFunctionWithHr(hr, "OnXblGameInviteRegisterForEventAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = XblGameInviteRegisterForEventAsync(
        Data()->xboxLiveContext,
        asyncBlock.get()
    );

    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END

    LogToScreen("XblGameInviteRegisterForEventAsync: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, S_OK);
}

int XblGameInviteUnregisterForEventAsync_Lua(lua_State *L)
{
    XblRealTimeActivitySubscriptionHandle subscriptionHandle{ nullptr };
    if (Data()->gameInviteNotificationSubscriptionHandle)
    {
        subscriptionHandle = Data()->gameInviteNotificationSubscriptionHandle;
    }

    if (subscriptionHandle)
    {
        // CODE SNIPPET START: XblGameInviteUnregisterForEventAsync
        auto asyncBlock = std::make_unique<XAsyncBlock>();
        asyncBlock->queue = Data()->queue;
        asyncBlock->callback = [](XAsyncBlock* asyncBlock)
        {
            std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
            HRESULT hr = XAsyncGetStatus(asyncBlock, true);
            Data()->gameInviteNotificationSubscriptionHandle = nullptr; // CODE SNIP SKIP

            LogToFile("XblGameInviteUnregisterForEventAsync Result: hr=%s", ConvertHR(hr).c_str()); // CODE SNIP SKIP
            CallLuaFunctionWithHr(hr, "OnXblGameInviteUnregisterForEventAsync"); // CODE SNIP SKIP
        };

        HRESULT hr = XblGameInviteUnregisterForEventAsync(Data()->xboxLiveContext, subscriptionHandle, asyncBlock.get());

        if (SUCCEEDED(hr))
        {
            // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
            // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
            asyncBlock.release();
        }
        // CODE SNIPPET END
        LogToScreen("XblGameInviteUnregisterForEventAsync: hr=%s", ConvertHR(hr).c_str());
    }

    LogToScreen("XblGameInviteUnregisterForEventAsync: done");
    return LuaReturnHR(L, S_OK);
}

void SetupupAPIs_XblGameInviteNotifications()
{
    lua_register(Data()->L, "XblGameInviteAddNotificationHandler", XblGameInviteAddNotificationHandler_Lua);
    lua_register(Data()->L, "XblGameInviteRemoveNotificationHandler", XblGameInviteRemoveNotificationHandler_Lua);
    lua_register(Data()->L, "XblGameInviteRegisterForEventAsync", XblGameInviteRegisterForEventAsync_Lua);
    lua_register(Data()->L, "XblGameInviteUnregisterForEventAsync", XblGameInviteUnregisterForEventAsync_Lua);
}
#endif

