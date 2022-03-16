// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"

XBL_WARNING_DISABLE_DEPRECATED

static struct SocialState
{
    SocialState() = default;
    ~SocialState()
    {
        // Validate that our tests cleaned up properly
        assert(!socialResultHandle);
        assert(!socialSubscriptionHandle);
        assert(!socialRelationshipChangedHandlerToken);
    }

    XblSocialRelationshipResultHandle socialResultHandle{ nullptr };
    XblRealTimeActivitySubscriptionHandle socialSubscriptionHandle{ nullptr };
    XblFunctionContext socialRelationshipChangedHandlerToken{ 0 };
} state;

XblSocialRelationshipFilter ConvertStringToXblSocialRelationshipFilter(const char* str)
{
    XblSocialRelationshipFilter filter = XblSocialRelationshipFilter::All;

    if (pal::stricmp(str, "XblSocialRelationshipFilter::All") == 0) filter = XblSocialRelationshipFilter::All;
    else if (pal::stricmp(str, "XblSocialRelationshipFilter::Favorite") == 0) filter = XblSocialRelationshipFilter::Favorite;
    else if (pal::stricmp(str, "XblSocialRelationshipFilter::LegacyXboxLiveFriends") == 0) filter = XblSocialRelationshipFilter::LegacyXboxLiveFriends;

    return filter;
}

int XblSocialGetSocialRelationshipsAsync_Lua(lua_State *L)
{
    XblSocialRelationshipFilter socialRelationshipFilter = ConvertStringToXblSocialRelationshipFilter(GetStringFromLua(L, 1, "XblSocialRelationshipFilter::All").c_str());

    // CODE SNIPPET START: XblSocialGetSocialRelationshipsAsync_C
    auto asyncBlock = std::make_unique<XAsyncBlock>(); 
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        HRESULT hr = XblSocialGetSocialRelationshipsResult(asyncBlock, &state.socialResultHandle);

        // Be sure to call XblSocialRelationshipResultCloseHandle when the result object is no longer needed
        LogToFile("XblSocialGetSocialRelationshipsResult: hr=%s", ConvertHR(hr).c_str()); // CODE SNIP SKIP
        CallLuaFunctionWithHr(hr, "OnXblSocialGetSocialRelationshipsAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = XblSocialGetSocialRelationshipsAsync(
        Data()->xboxLiveContext,
        Data()->xboxUserId,
        socialRelationshipFilter,
        0,
        0,
        asyncBlock.get()
    );

    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END

    LogToFile("XblSocialGetSocialRelationshipsAsync: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblSocialRelationshipResultGetRelationships_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblSocialRelationshipResultGetRelationships_C
    const XblSocialRelationship* relationships = nullptr;
    size_t relationshipsCount = 0;
    HRESULT hr = XblSocialRelationshipResultGetRelationships(state.socialResultHandle, &relationships, &relationshipsCount);

    LogToFile("Got %u SocialRelationships:", relationshipsCount);
    for (size_t i = 0; i < relationshipsCount; ++i)
    {
        LogToFile("Xuid = %u, isFollowingCaller = %u", relationships[i].xboxUserId, relationships[i].isFollowingCaller);
    }
    // CODE SNIPPET END

    LogToFile("XblSocialRelationshipResultGetRelationships: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblSocialRelationshipResultHasNext_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblSocialRelationshipResultHasNext_C
    bool hasNext{ false };
    HRESULT hr = XblSocialRelationshipResultHasNext(state.socialResultHandle, &hasNext);
    // CODE SNIPPET END

    LogToFile("XblSocialRelationshipResultHasNext: hr=%s hasNext=%s", ConvertHR(hr).c_str(), hasNext ? "true" : "false");
    lua_pushnumber(L, (int)hasNext);
    return LuaReturnHR(L, hr, 1);
}

int XblSocialRelationshipResultGetNextAsync_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblSocialRelationshipResultGetNextAsync_C
    auto asyncBlock = std::make_unique<XAsyncBlock>(); 
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        // Close handle to previous page of results
        if (state.socialResultHandle)
        {
            XblSocialRelationshipResultCloseHandle(state.socialResultHandle);
        }
        HRESULT hr = XblSocialRelationshipResultGetNextResult(asyncBlock, &state.socialResultHandle);
        LogToFile("XblSocialRelationshipResultGetNextResult: hr=%s", ConvertHR(hr).c_str()); // CODE SNIP SKIP
        CallLuaFunctionWithHr(hr, "OnXblSocialRelationshipResultGetNextAsync"); // CODE SNIP SKIP
    };

    uint32_t maxItems = 100;
    HRESULT hr = XblSocialRelationshipResultGetNextAsync(Data()->xboxLiveContext, state.socialResultHandle, maxItems, asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END
    LogToFile("XblSocialRelationshipResultGetNextAsync: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblSocialRelationshipResultDuplicateHandle_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblSocialRelationshipResultDuplicateHandle_C
    XblSocialRelationshipResultHandle handle{};
    XblSocialRelationshipResultDuplicateHandle(state.socialResultHandle, &handle);
    // CODE SNIPPET END

    XblSocialRelationshipResultCloseHandle(handle);
    LogToFile("XblSocialRelationshipResultDuplicateHandle");
    return LuaReturnHR(L, S_OK);
}

int XblSocialRelationshipResultCloseHandle_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblSocialRelationshipResultCloseHandle_C
    XblSocialRelationshipResultCloseHandle(state.socialResultHandle);
    state.socialResultHandle = nullptr;
    // CODE SNIPPET END
    LogToFile("XblSocialRelationshipResultCloseHandle");
    return LuaReturnHR(L, S_OK);
}

int XblSocialSubscribeToSocialRelationshipChange_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblSocialSubscribeToSocialRelationshipChange_C
#if XSAPI_BUILT_FROM_SOURCE // CODE SNIP SKIP // TODO Remove when new GDK bins are generated
    HRESULT hr = XblSocialSubscribeToSocialRelationshipChange(
        Data()->xboxLiveContext, 
        Data()->xboxUserId, 
        &state.socialSubscriptionHandle
    );
    // CODE SNIPPET END
    LogToFile("XblSocialSubscribeToSocialRelationshipChange: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
#else
    return LuaReturnHR(L, S_OK);
#endif
}

int XblSocialUnsubscribeFromSocialRelationshipChange_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblSocialUnsubscribeFromSocialRelationshipChange_C
#if XSAPI_BUILT_FROM_SOURCE // CODE SNIP SKIP // TODO Remove when new GDK bins are generated
    HRESULT hr = XblSocialUnsubscribeFromSocialRelationshipChange(
        Data()->xboxLiveContext,
        state.socialSubscriptionHandle
    );

    state.socialSubscriptionHandle = nullptr;
    // CODE SNIPPET END

    LogToFile("XblSocialUnsubscribeFromSocialRelationshipChange: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
#else
    return LuaReturnHR(L, S_OK);
#endif
}

int XblSocialAddSocialRelationshipChangedHandler_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblSocialAddSocialRelationshipChangedHandler_C
    state.socialRelationshipChangedHandlerToken = XblSocialAddSocialRelationshipChangedHandler(
        Data()->xboxLiveContext,
        [](const XblSocialRelationshipChangeEventArgs* args, void* context)
        {
            UNREFERENCED_PARAMETER(context);
            LogToFile("Social relationship changed:");
            std::stringstream ss;
            for (size_t i = 0; i < args->xboxUserIdsCount; ++i)
            {
                if (i > 0) 
                {
                    ss << ", ";
                }
                ss << args->xboxUserIds[i];
            }
            LogToFile("socialNotification = %u, affectedXuids = %s", args->socialNotification, ss.str().data());
            CallLuaFunction("OnSocialRelationshipChanged"); // CODE SNIP SKIP
        },
        nullptr
    );
    // CODE SNIPPET END

    LogToFile("XblSocialAddSocialRelationshipChangedHandler");
    return LuaReturnHR(L, S_OK);
}

int XblSocialRemoveSocialRelationshipChangedHandler_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblSocialRemoveSocialRelationshipChangedHandler_C
    HRESULT hr = XblSocialRemoveSocialRelationshipChangedHandler(Data()->xboxLiveContext, state.socialRelationshipChangedHandlerToken);

    state.socialRelationshipChangedHandlerToken = 0;
    // CODE SNIPPET END

    LogToFile("XblSocialRemoveSocialRelationshipChangedHandler: hr=%s", ConvertHR(hr).data());
    return LuaReturnHR(L, hr);
}

int XblSocialSubmitReputationFeedbackAsync_Lua(lua_State* L)
{
    // CODE SNIPPET START: XblSocialSubmitReputationFeedbackAsync_C
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        HRESULT hr = XAsyncGetStatus(asyncBlock, false);
        LogToFile("XblSocialSubmitReputationFeedbackAsync: hr=%s", ConvertHR(hr).c_str()); // CODE SNIP SKIP
        CallLuaFunctionWithHr(hr, "OnXblSocialSubmitReputationFeedbackAsync"); // CODE SNIP SKIP
    };

    uint64_t xuid{ 2814639011617876 };
    HRESULT hr = XblSocialSubmitReputationFeedbackAsync(
        Data()->xboxLiveContext,
        xuid,
        XblReputationFeedbackType::PositiveHelpfulPlayer,
        nullptr,
        "Helpful player",
        nullptr,
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

int XblSocialSubmitBatchReputationFeedbackAsync_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblSocialSubmitBatchReputationFeedbackAsync_C
    std::vector<XblReputationFeedbackItem> feedbackItems;
    feedbackItems.push_back(XblReputationFeedbackItem
        {
            2814639011617876,
            XblReputationFeedbackType::PositiveHelpfulPlayer,
            nullptr,
            "Helpful player",
            nullptr
        });
    // Add any additional feedback items here

    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        HRESULT hr = XAsyncGetStatus(asyncBlock, false);
        LogToFile("XblSocialSubmitBatchReputationFeedbackAsync: hr=%s", ConvertHR(hr).c_str()); // CODE SNIP SKIP
        CallLuaFunctionWithHr(hr, "OnXblSocialSubmitBatchReputationFeedbackAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = XblSocialSubmitBatchReputationFeedbackAsync(
        Data()->xboxLiveContext,
        feedbackItems.data(),
        feedbackItems.size(),
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


int profile_service_get_user_profile_Lua(lua_State *L)
{
    HRESULT hr = S_OK;

#if XSAPI_BUILT_FROM_SOURCE && HC_PLATFORM != HC_PLATFORM_UWP && HC_PLATFORM != HC_PLATFORM_GDK
    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xalUser);
    xblc->profile_service().get_user_profile(L"1234")
        .then([](xbox::services::xbox_live_result<xbox::services::social::xbox_user_profile> result)
    {
        HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
        LogToFile("get_user_profile: hr=%s", ConvertHR(hr).c_str()); 
        CallLuaFunctionWithHr(S_OK, "On_profile_service_get_user_profile"); 
    });
#endif

    return LuaReturnHR(L, hr);
}

int UnsubscribeToTitleAndDevicePresenceChangeForFriends_Lua(lua_State *L)
{
    HRESULT hr = S_OK;

    for (XblRealTimeActivitySubscriptionHandle subscriptionHandleDevice : Data()->subscriptionHandleDeviceList)
    {
        hr = XblPresenceUnsubscribeFromDevicePresenceChange(
            Data()->xboxLiveContext,
            subscriptionHandleDevice
        );
        LuaStopTestIfFailed(hr);
        assert(SUCCEEDED(hr));
    }

    for (XblRealTimeActivitySubscriptionHandle subscriptionHandleTitle : Data()->subscriptionHandleTitleList)
    {
        hr = XblPresenceUnsubscribeFromTitlePresenceChange(
            Data()->xboxLiveContext,
            subscriptionHandleTitle
        );
    }

    Data()->subscriptionHandleTitleList.clear();
    Data()->subscriptionHandleDeviceList.clear();

    LogToScreen("UnsubscribeToTitleAndDevicePresenceChangeForFriends: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int SubscribeToTitleAndDevicePresenceChangeForFriends_Lua(lua_State *L)
{
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        HRESULT hr = XblSocialGetSocialRelationshipsResult(asyncBlock, &state.socialResultHandle);

        const XblSocialRelationship* relationships = nullptr;
        size_t relationshipsCount = 0;
        hr = XblSocialRelationshipResultGetRelationships(state.socialResultHandle, &relationships, &relationshipsCount);

        Data()->subscriptionHandleTitleList.clear();
        Data()->subscriptionHandleDeviceList.clear();

        XblRealTimeActivitySubscriptionHandle subscriptionHandleDevice;
        XblRealTimeActivitySubscriptionHandle subscriptionHandleTitle;
        LogToScreen("Got %u SocialRelationships:", relationshipsCount);
        for (size_t i = 0; i < relationshipsCount; ++i)
        {
            if (i % 100 == 0)
            {
                LogToScreen("Sub'ing to friend %d", i);
            }

            hr = XblPresenceSubscribeToDevicePresenceChange(
                Data()->xboxLiveContext,
                relationships[i].xboxUserId,
                &subscriptionHandleDevice);
            LuaStopTestIfFailed(hr);
            assert(SUCCEEDED(hr));

            hr = XblPresenceSubscribeToTitlePresenceChange(
                Data()->xboxLiveContext,
                relationships[i].xboxUserId,
                Data()->titleId,
                &subscriptionHandleTitle);
            LuaStopTestIfFailed(hr);
            assert(SUCCEEDED(hr));

            Data()->subscriptionHandleDeviceList.push_back(subscriptionHandleDevice);
            Data()->subscriptionHandleTitleList.push_back(subscriptionHandleTitle);
        }

        LogToScreen("SubscribeToTitleAndDevicePresenceChangeForFriends: hr=%s", ConvertHR(hr).c_str());
        CallLuaFunctionWithHr(hr, "OnSubscribeToTitleAndDevicePresenceChangeForFriends"); 
    };

    HRESULT hr = XblSocialGetSocialRelationshipsAsync(
        Data()->xboxLiveContext,
        Data()->xboxUserId,
        XblSocialRelationshipFilter::All,
        0,
        0,
        asyncBlock.get()
    );

    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }

    LogToFile("SubscribeToTitleAndDevicePresenceChangeForFriends: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

void SetupAPIs_XblSocial()
{
    lua_register(Data()->L, "XblSocialGetSocialRelationshipsAsync", XblSocialGetSocialRelationshipsAsync_Lua);
    lua_register(Data()->L, "XblSocialRelationshipResultGetRelationships", XblSocialRelationshipResultGetRelationships_Lua);
    lua_register(Data()->L, "XblSocialRelationshipResultHasNext", XblSocialRelationshipResultHasNext_Lua);
    lua_register(Data()->L, "XblSocialRelationshipResultGetNextAsync", XblSocialRelationshipResultGetNextAsync_Lua);
    lua_register(Data()->L, "XblSocialRelationshipResultDuplicateHandle", XblSocialRelationshipResultDuplicateHandle_Lua);
    lua_register(Data()->L, "XblSocialRelationshipResultCloseHandle", XblSocialRelationshipResultCloseHandle_Lua);
    
    lua_register(Data()->L, "XblSocialSubscribeToSocialRelationshipChange", XblSocialSubscribeToSocialRelationshipChange_Lua);
    lua_register(Data()->L, "XblSocialUnsubscribeFromSocialRelationshipChange", XblSocialUnsubscribeFromSocialRelationshipChange_Lua);
    lua_register(Data()->L, "XblSocialAddSocialRelationshipChangedHandler", XblSocialAddSocialRelationshipChangedHandler_Lua);
    lua_register(Data()->L, "XblSocialRemoveSocialRelationshipChangedHandler", XblSocialRemoveSocialRelationshipChangedHandler_Lua);

    lua_register(Data()->L, "XblSocialSubmitReputationFeedbackAsync", XblSocialSubmitReputationFeedbackAsync_Lua);
    lua_register(Data()->L, "XblSocialSubmitBatchReputationFeedbackAsync", XblSocialSubmitBatchReputationFeedbackAsync_Lua);

    lua_register(Data()->L, "SubscribeToTitleAndDevicePresenceChangeForFriends", SubscribeToTitleAndDevicePresenceChangeForFriends_Lua);
    lua_register(Data()->L, "UnsubscribeToTitleAndDevicePresenceChangeForFriends", UnsubscribeToTitleAndDevicePresenceChangeForFriends_Lua);

    lua_register(Data()->L, "profile_service_get_user_profile", profile_service_get_user_profile_Lua);
}
