// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"

XBL_WARNING_DISABLE_DEPRECATED

static struct PresenceState
{
    PresenceState() = default;
    ~PresenceState()
    {
        assert(!presenceRecord);
        assert(!devicePresenceChangeSubscription);
        assert(!titlePresenceChangeSubscription);
        assert(!devicePresenceChangedHandlerToken);
        assert(!titlePresenceChangedHandlerToken);
    }

    XblPresenceRecordHandle presenceRecord{ nullptr };
    XblRealTimeActivitySubscriptionHandle devicePresenceChangeSubscription{ nullptr };
    XblRealTimeActivitySubscriptionHandle titlePresenceChangeSubscription{ nullptr };
    XblFunctionContext devicePresenceChangedHandlerToken{ 0 };
    XblFunctionContext titlePresenceChangedHandlerToken{ 0 };
} state;

int XblPresenceRecordGetXuid_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblPresenceRecordGetXuid
    uint64_t xuid;
    HRESULT hr = XblPresenceRecordGetXuid(state.presenceRecord, &xuid);
    // CODE SNIPPET END

    LogToScreen("XblPresenceRecordGetXuid: hr=%s, xuid=%llu", ConvertHR(hr).c_str(), static_cast<unsigned long long>(xuid));

    return LuaReturnHR(L, hr);
}

int XblPresenceRecordGetUserState_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblPresenceRecordGetXuid
    XblPresenceUserState userState{ XblPresenceUserState::Unknown };
    HRESULT hr = XblPresenceRecordGetUserState(state.presenceRecord, &userState);
    // CODE SNIPPET END

    LogToScreen("XblPresenceRecordGetUserState: hr=%s, state=%u", ConvertHR(hr).c_str(), userState);

    return LuaReturnHR(L, hr);
}

int XblPresenceRecordGetDeviceRecords_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblPresenceRecordGetDeviceRecords_C
    const XblPresenceDeviceRecord* deviceRecords{ nullptr };
    size_t deviceRecordsCount{ 0 };
    HRESULT hr = XblPresenceRecordGetDeviceRecords(state.presenceRecord, &deviceRecords, &deviceRecordsCount);

    for (auto i = 0u; i < deviceRecordsCount; ++i)
    {
        auto& deviceRecord{ deviceRecords[i] };
        LogToScreen("Got XblDeviceRecord with device type %u and %u title records", deviceRecord.deviceType, deviceRecord.titleRecordsCount);

        for (auto j = 0u; j < deviceRecord.titleRecordsCount; ++j)
        {
            auto& titleRecord{ deviceRecord.titleRecords[j] };
            // Display rich presence string
            LogToScreen("Rich presence string for titleId %u: %s", titleRecord.titleId, titleRecord.richPresenceString);
        }
    }
    // CODE SNIPPET END

    LogToFile("XblPresenceRecordGetDeviceRecords: hr=%s", ConvertHR(hr).data());

    return LuaReturnHR(L, hr);
}

int XblPresenceRecordCloseHandle_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblPresenceRecordCloseHandle_C
    XblPresenceRecordCloseHandle(state.presenceRecord);
    state.presenceRecord = nullptr;
    // CODE SNIPPET END

    return LuaReturnHR(L, S_OK);
}

int XblPresenceSetPresenceAsync_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblPresenceSetPresenceAsync_C
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        HRESULT hr = XAsyncGetStatus(asyncBlock, false);
        // CODE SKIP START
        if (hr == HTTP_E_STATUS_429_TOO_MANY_REQUESTS)
        {
            LogToFile("XblPresenceSetPresence returned 429.  Ignoring failure");
            hr = S_OK;
        }
        CallLuaFunctionWithHr(hr, "OnXblPresenceSetPresenceAsync");
        // CODE SKIP END
    };

    std::vector<const char*> tokens{ "0" };
    XblPresenceRichPresenceIds ids
    {
        {},
        "PuzzlesPresence",
        tokens.data(),
        tokens.size()
    };
    pal::strcpy(ids.scid, sizeof(ids.scid), Data()->scid);

    HRESULT hr = XblPresenceSetPresenceAsync(Data()->xboxLiveContext, true, &ids, asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END

    LogToFile("XblPresenceSetPresenceAsync: hr=%s", ConvertHR(hr).data());

    return LuaReturnHR(L, hr);
}

int XblPresenceGetPresenceAsync_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblPresenceGetPresenceAsync_C
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        assert(state.presenceRecord == nullptr); // CODE SNIP SKIP
        HRESULT hr = XblPresenceGetPresenceResult(asyncBlock, &state.presenceRecord);
        
        // Be sure to call XblPresenceRecordCloseHandle when the presence record is no longer needed.
        LogToFile("XblPresenceGetPresenceResult hr=%s", ConvertHR(hr).data()); // CODE SNIP SKIP
        CallLuaFunctionWithHr(hr, "OnXblPresenceGetPresenceAsync"); // CODE SNIP SKIP
    };

    uint64_t xuid = Data()->xboxUserId;
    if (Data()->m_multiDeviceManager->GetRemoteXuid() != 0)
    {
        xuid = Data()->m_multiDeviceManager->GetRemoteXuid();
    }

    HRESULT hr = XblPresenceGetPresenceAsync(Data()->xboxLiveContext, xuid, asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }

    // CODE SNIPPET END
    LogToFile("XblPresenceGetPresenceAsync: hr=%s", ConvertHR(hr).c_str());

    return LuaReturnHR(L, hr);
}

int XblPresenceGetPresenceForMultipleUsersAsync_Lua(lua_State* L)
{
    // CODE SNIPPET START: XblPresenceGetPresenceForMultipleUsersAsync_C
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        size_t resultCount{ 0 };
        HRESULT hr = XblPresenceGetPresenceForMultipleUsersResultCount(asyncBlock, &resultCount);

        if (SUCCEEDED(hr) && resultCount > 0)
        {
            std::vector<XblPresenceRecordHandle> handles(resultCount, nullptr);
            hr = XblPresenceGetPresenceForMultipleUsersResult(asyncBlock, handles.data(), resultCount);

            // Be sure to call XblPresenceRecordCloseHandle for each presence record when they are
            // no longer needed.
            // CODE SKIP START
            LogToFile("XblPresenceGetPresenceForMultipleUsersResult hr=%s", ConvertHR(hr).data());

            if (SUCCEEDED(hr))
            {
                for (auto i = 0u; i < resultCount; ++i)
                {
                    XblPresenceRecordCloseHandle(handles[i]);
                }
            }
            // CODE SKIP END
        }

        CallLuaFunctionWithHr(hr, "OnXblPresenceGetPresenceForMultipleUsersAsync"); // CODE SNIP SKIP
    };

    std::vector<uint64_t> xuids{ Data()->xboxUserId };

    // Filter results to only online users
    XblPresenceQueryFilters filters{};
    filters.onlineOnly = true;
    filters.detailLevel = XblPresenceDetailLevel::All;

    HRESULT hr = XblPresenceGetPresenceForMultipleUsersAsync(Data()->xboxLiveContext, xuids.data(), xuids.size(), &filters, asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }

    // CODE SNIPPET END
    LogToFile("XblPresenceGetPresenceForMultipleUsersAsync: hr=%s", ConvertHR(hr).c_str());

    return LuaReturnHR(L, hr);
}

int XblPresenceGetPresenceForSocialGroupAsync_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblPresenceGetPresenceForSocialGroupAsync_C
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        size_t resultCount{ 0 };
        HRESULT hr = XblPresenceGetPresenceForSocialGroupResultCount(asyncBlock, &resultCount);

        if (SUCCEEDED(hr) && resultCount > 0)
        {
            std::vector<XblPresenceRecordHandle> handles(resultCount, nullptr);
            hr = XblPresenceGetPresenceForSocialGroupResult(asyncBlock, handles.data(), resultCount);

            // Be sure to call XblPresenceRecordCloseHandle for each presence record when they are
            // no longer needed.
            // CODE SKIP START
            LogToFile("XblPresenceGetPresenceForSocialGroupResult hr=%s", ConvertHR(hr).data());

            if (SUCCEEDED(hr))
            {
                for (auto i = 0u; i < resultCount; ++i)
                {
                    XblPresenceRecordCloseHandle(handles[i]);
                }
            }
            // CODE SKIP END
        }

        CallLuaFunctionWithHr(hr, "OnXblPresenceGetPresenceForSocialGroupAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = XblPresenceGetPresenceForSocialGroupAsync(Data()->xboxLiveContext, "Favorites", nullptr, nullptr, asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }

    // CODE SNIPPET END
    LogToFile("XblPresenceGetPresenceForSocialGroupAsync: hr=%s", ConvertHR(hr).c_str());

    return LuaReturnHR(L, hr);
}

int XblPresenceSubscribeToDevicePresenceChange_Lua(lua_State* L)
{
    // CODE SNIPPET START: XblPresenceSubscribeToDevicePresenceChange_C
    uint64_t xuid{ 2814639011617876 };

    HRESULT hr = XblPresenceSubscribeToDevicePresenceChange(
        Data()->xboxLiveContext,
        xuid,
        &state.devicePresenceChangeSubscription
    );
    // CODE SNIPPET END

    LogToFile("XblPresenceSubscribeToDevicePresenceChange: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblPresenceUnsubscribeFromDevicePresenceChange_Lua(lua_State* L)
{
    // CODE SNIPPET START: XblPresenceUnsubscribeFromDevicePresenceChange_C
    HRESULT hr = XblPresenceUnsubscribeFromDevicePresenceChange(
        Data()->xboxLiveContext,
        state.devicePresenceChangeSubscription
    );

    state.devicePresenceChangeSubscription = nullptr;
    // CODE SNIPPET END

    LogToFile("XblPresenceUnsubscribeFromDevicePresenceChange: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblPresenceSubscribeToTitlePresenceChange_Lua(lua_State* L)
{
    // CODE SNIPPET START: XblPresenceSubscribeToTitlePresenceChange_C
    uint64_t xuid{ 2814639011617876 };

    HRESULT hr = XblPresenceSubscribeToTitlePresenceChange(
        Data()->xboxLiveContext,
        xuid,
        Data()->titleId,
        &state.titlePresenceChangeSubscription
    );
    // CODE SNIPPET END

    LogToFile("XblPresenceSubscribeToTitlePresenceChange: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblPresenceUnsubscribeFromTitlePresenceChange_Lua(lua_State* L)
{
    // CODE SNIPPET START: XblPresenceUnsubscribeFromTitlePresenceChange_C
    HRESULT hr = XblPresenceUnsubscribeFromTitlePresenceChange(
        Data()->xboxLiveContext,
        state.titlePresenceChangeSubscription
    );

    state.titlePresenceChangeSubscription = nullptr;
    // CODE SNIPPET END

    LogToFile("XblPresenceUnsubscribeFromTitlePresenceChange: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblPresenceTrackUsers_Lua(lua_State* L)
{
    auto xuid{ GetUint64FromLua(L, 1, 2814639011617876) };

    // CODE SNIPPET START: XblPresenceTrackUsers_C
    HRESULT hr = XblPresenceTrackUsers(
        Data()->xboxLiveContext,
        &xuid,
        1
    );
    // CODE SNIPPET END

    LogToFile("XblPresenceTrackUsers: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblPresenceStopTrackingUsers_Lua(lua_State* L)
{
    auto xuid{ GetUint64FromLua(L, 1, 2814639011617876) };

    // CODE SNIPPET START: XblPresenceStopTrackingUsers_C
    HRESULT hr = XblPresenceStopTrackingUsers(
        Data()->xboxLiveContext,
        &xuid,
        1
    );

    // CODE SNIPPET END

    LogToFile("XblPresenceStopTrackingUsers: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblPresenceTrackAdditionalTitles_Lua(lua_State* L)
{
    auto titleId{ GetUint32FromLua(L, 1, Data()->titleId) };
    // CODE SNIPPET START: XblPresenceTrackTitles_C

    HRESULT hr = XblPresenceTrackAdditionalTitles(
        Data()->xboxLiveContext,
        &titleId,
        1
    );
    // CODE SNIPPET END

    LogToFile("XblPresenceTrackTitles: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblPresenceStopTrackingAdditionalTitles_Lua(lua_State* L)
{
    auto titleId{ GetUint32FromLua(L, 1, Data()->titleId) };

    // CODE SNIPPET START: XblPresenceStopTrackingTitles_C
    HRESULT hr = XblPresenceStopTrackingAdditionalTitles(
        Data()->xboxLiveContext,
        &titleId,
        1
    );
    // CODE SNIPPET END

    LogToFile("XblPresenceStopTrackingTitles: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblPresenceAddDevicePresenceChangedHandler_Lua(lua_State* L)
{
    // CODE SNIPPET START: XblPresenceAddDevicePresenceChangedHandler_C
    state.devicePresenceChangedHandlerToken = XblPresenceAddDevicePresenceChangedHandler(
        Data()->xboxLiveContext,
        [](void* context, uint64_t xuid, XblPresenceDeviceType deviceType, bool isUserLoggedOnDevice)
        {
            UNREFERENCED_PARAMETER(context);
            LogToFile("Device presence change notification received:");
            LogToFile("Xuid = %u, deviceType = %u, isUserLoggedOnDevice = %u", xuid, deviceType, isUserLoggedOnDevice);
            CallLuaFunction("OnDevicePresenceChanged"); // CODE SNIP SKIP
        },
        nullptr
    );
    // CODE SNIPPET END

    LogToFile("XblPresenceAddDevicePresenceChangedHandler");
    return LuaReturnHR(L, S_OK);
}

int XblPresenceRemoveDevicePresenceChangedHandler_Lua(lua_State* L)
{
    // CODE SNIPPET START: XblPresenceRemoveDevicePresenceChangedHandler_C
    HRESULT hr = XblPresenceRemoveDevicePresenceChangedHandler(
        Data()->xboxLiveContext,
        state.devicePresenceChangedHandlerToken
    );

    state.devicePresenceChangedHandlerToken = 0;
    // CODE SNIPPET END

    LogToFile("XblPresenceRemoveDevicePresenceChangedHandler: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblPresenceAddTitlePresenceChangedHandler_Lua(lua_State* L)
{
    // CODE SNIPPET START: XblPresenceAddTitlePresenceChangedHandler_C
    state.titlePresenceChangedHandlerToken =  XblPresenceAddTitlePresenceChangedHandler(
        Data()->xboxLiveContext,
        [](void* context, uint64_t xuid, uint32_t titleId, XblPresenceTitleState titleState)
        {
            UNREFERENCED_PARAMETER(context);
            LogToFile("Title presence change notification received:");
            LogToFile("Xuid = %u, titleId = %u, titleState = %u", xuid, titleId, titleState);
            CallLuaFunction("OnTitlePresenceChanged"); // CODE SNIP SKIP
        },
        nullptr
    );
    // CODE SNIPPET END

    LogToFile("XblPresenceAddTitlePresenceChangedHandler");
    return LuaReturnHR(L, S_OK);
}

int XblPresenceRemoveTitlePresenceChangedHandler_Lua(lua_State* L)
{
    // CODE SNIPPET START: XblPresenceRemoveTitlePresenceChangedHandler_C
    HRESULT hr = XblPresenceRemoveTitlePresenceChangedHandler(
        Data()->xboxLiveContext,
        state.titlePresenceChangedHandlerToken
    );

    state.titlePresenceChangedHandlerToken = 0;
    // CODE SNIPPET END

    LogToFile("XblPresenceRemoveTitlePresenceChangedHandler: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

void SetupAPIs_XblPresence()
{
    lua_register(Data()->L, "XblPresenceRecordGetXuid", XblPresenceRecordGetXuid_Lua);
    lua_register(Data()->L, "XblPresenceRecordGetUserState", XblPresenceRecordGetUserState_Lua);
    lua_register(Data()->L, "XblPresenceRecordGetDeviceRecords", XblPresenceRecordGetDeviceRecords_Lua);
    lua_register(Data()->L, "XblPresenceRecordCloseHandle", XblPresenceRecordCloseHandle_Lua);
    lua_register(Data()->L, "XblPresenceSetPresenceAsync", XblPresenceSetPresenceAsync_Lua);
    lua_register(Data()->L, "XblPresenceGetPresenceAsync", XblPresenceGetPresenceAsync_Lua);
    lua_register(Data()->L, "XblPresenceGetPresenceForSocialGroupAsync", XblPresenceGetPresenceForSocialGroupAsync_Lua);
    lua_register(Data()->L, "XblPresenceGetPresenceForMultipleUsersAsync", XblPresenceGetPresenceForMultipleUsersAsync_Lua);
    lua_register(Data()->L, "XblPresenceSubscribeToDevicePresenceChange", XblPresenceSubscribeToDevicePresenceChange_Lua);
    lua_register(Data()->L, "XblPresenceUnsubscribeFromDevicePresenceChange", XblPresenceUnsubscribeFromDevicePresenceChange_Lua);
    lua_register(Data()->L, "XblPresenceSubscribeToTitlePresenceChange", XblPresenceSubscribeToTitlePresenceChange_Lua);
    lua_register(Data()->L, "XblPresenceUnsubscribeFromTitlePresenceChange", XblPresenceUnsubscribeFromTitlePresenceChange_Lua);
    lua_register(Data()->L, "XblPresenceTrackUsers", XblPresenceTrackUsers_Lua);
    lua_register(Data()->L, "XblPresenceStopTrackingUsers", XblPresenceStopTrackingUsers_Lua);
    lua_register(Data()->L, "XblPresenceTrackAdditionalTitles", XblPresenceTrackAdditionalTitles_Lua);
    lua_register(Data()->L, "XblPresenceStopTrackingAdditionalTitles", XblPresenceStopTrackingAdditionalTitles_Lua);
    lua_register(Data()->L, "XblPresenceAddDevicePresenceChangedHandler", XblPresenceAddDevicePresenceChangedHandler_Lua);
    lua_register(Data()->L, "XblPresenceRemoveDevicePresenceChangedHandler", XblPresenceRemoveDevicePresenceChangedHandler_Lua);
    lua_register(Data()->L, "XblPresenceAddTitlePresenceChangedHandler", XblPresenceAddTitlePresenceChangedHandler_Lua);
    lua_register(Data()->L, "XblPresenceRemoveTitlePresenceChangedHandler", XblPresenceRemoveTitlePresenceChangedHandler_Lua);

}
