// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"

#ifndef _countof
#define _countof(array) (sizeof(array) / sizeof(array[0]))
#endif
int XblPrivacyGetAvoidListAsync_Lua(lua_State* L)
{
    CreateQueueIfNeeded();

    // CODE SNIPPET START: XblPrivacyGetAvoidListAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        size_t resultCount{};
        HRESULT hr = XblPrivacyGetAvoidListResultCount(asyncBlock, &resultCount);

        if (SUCCEEDED(hr))
        {
            std::vector<uint64_t> avoidedXuids(resultCount);
            hr = XblPrivacyGetAvoidListResult(asyncBlock, resultCount, avoidedXuids.data());
        }

        LogToFile("XblPrivacyGetAvoidListResult: hr=%s avoided xuids count=%d", ConvertHR(hr).c_str(), resultCount); // CODE SNIP SKIP
        CallLuaFunctionWithHr(hr, "OnXblPrivacyGetAvoidListAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = XblPrivacyGetAvoidListAsync(Data()->xboxLiveContext, asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END
    LogToFile("XblPrivacyGetAvoidListAsync: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblPrivacyCheckPermissionAsync_Lua(lua_State* L)
{
    CreateQueueIfNeeded();

    XblPermission permissionToCheck{ static_cast<XblPermission>(GetUint32FromLua(L, 1, (uint32_t)XblPermission::ViewTargetProfile)) };
    uint64_t targetXuid{ GetUint64FromLua(L, 2, 2743710844428572) };

    LogToFile("XblPrivacyCheckPermissionAsync: permissionToCheck = %d, targetXuid = %llu", permissionToCheck, static_cast<unsigned long long>(targetXuid));

    // CODE SNIPPET START: XblPrivacyCheckPermissionAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        size_t resultSize;
        HRESULT hr = XblPrivacyCheckPermissionResultSize(asyncBlock, &resultSize);

        if (SUCCEEDED(hr))
        {
            std::vector<char> buffer(resultSize, 0);
            XblPermissionCheckResult* result{};

            hr = XblPrivacyCheckPermissionResult(asyncBlock, resultSize, buffer.data(), &result, nullptr);

            if (SUCCEEDED(hr)) // CODE SNIP SKIP
            { // CODE SNIP SKIP
                LogToFile("XblPrivacyCheckPermissionResult: hr=%s isAllowed=%d", ConvertHR(hr).c_str(), result->isAllowed); // CODE SNIP SKIP
            } // CODE SNIP SKIP
        }

        if (FAILED(hr)) // CODE SNIP SKIP
        { // CODE SNIP SKIP
            LogToFile("XblPrivacyCheckPermissionResult: hr=%s", ConvertHR(hr).c_str()); // CODE SNIP SKIP
        } // CODE SNIP SKIP

        CallLuaFunctionWithHr(hr, "OnXblPrivacyCheckPermissionAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = XblPrivacyCheckPermissionAsync(Data()->xboxLiveContext, permissionToCheck, targetXuid, asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }

    // CODE SNIPPET END
    LogToFile("XblPrivacyGetAvoidListAsync: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblPrivacyCheckPermissionForAnonymousUserAsync_Lua(lua_State* L)
{
    CreateQueueIfNeeded();
    XblPermission permissionToCheck{ static_cast<XblPermission>(GetUint32FromLua(L, 1, (uint32_t)XblPermission::CommunicateUsingText)) };
    XblAnonymousUserType userType{ static_cast<XblAnonymousUserType>(GetUint32FromLua(L, 2, (uint32_t)XblAnonymousUserType::CrossNetworkUser)) };

    LogToFile("XblPrivacyCheckPermissionForAnonymousUserAsync: permissionToCheck = %d, target = %d", permissionToCheck, userType);

    // CODE SNIPPET START: XblPrivacyCheckPermissionForAnonymousUserAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        size_t resultSize;
        HRESULT hr = XblPrivacyCheckPermissionForAnonymousUserResultSize(asyncBlock, &resultSize);

        if (SUCCEEDED(hr))
        {
            std::vector<char> buffer(resultSize, 0);
            XblPermissionCheckResult* result{};

            hr = XblPrivacyCheckPermissionForAnonymousUserResult(asyncBlock, resultSize, buffer.data(), &result, nullptr);

            if (SUCCEEDED(hr)) // CODE SNIP SKIP
            { // CODE SNIP SKIP
                LogToFile("XblPrivacyCheckPermissionResult: hr=%s isAllowed=%d", ConvertHR(hr).c_str(), result->isAllowed); // CODE SNIP SKIP
            } // CODE SNIP SKIP
        }

        if (FAILED(hr)) // CODE SNIP SKIP
        { // CODE SNIP SKIP
            LogToFile("XblPrivacyCheckPermissionResult: hr=%s", ConvertHR(hr).c_str()); // CODE SNIP SKIP
        } // CODE SNIP SKIP

        CallLuaFunctionWithHr(hr, "OnXblPrivacyCheckPermissionForAnonymousUserAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = XblPrivacyCheckPermissionForAnonymousUserAsync(Data()->xboxLiveContext, permissionToCheck, userType, asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }

    // CODE SNIPPET END
    LogToFile("XblPrivacyGetAvoidListAsync: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblPrivacyBatchCheckPermissionAsync_Lua(lua_State* L)
{
    CreateQueueIfNeeded();

    // Only using default values for params, very difficult to parse variable arrays from lua
    XblPermission permissionsToCheck[] {
        XblPermission::CommunicateUsingText,
        XblPermission::CommunicateUsingVideo,
        XblPermission::CommunicateUsingVoice,
        XblPermission::ViewTargetProfile,
        XblPermission::ViewTargetGameHistory,
        XblPermission::ViewTargetVideoHistory,
        XblPermission::ViewTargetMusicHistory,
        XblPermission::ViewTargetExerciseInfo,
        XblPermission::ViewTargetPresence,
        XblPermission::ViewTargetVideoStatus,
        XblPermission::ViewTargetMusicStatus,
        XblPermission::PlayMultiplayer,
        XblPermission::ViewTargetUserCreatedContent,
        XblPermission::BroadcastWithTwitch,
        XblPermission::WriteComment,
        XblPermission::ShareItem,
        XblPermission::ShareTargetContentToExternalNetworks
    };
    uint64_t targetXuids[] { 2743710844428572, 2533274819720636 };
    XblAnonymousUserType targetUserTypes[]{ XblAnonymousUserType::CrossNetworkUser, XblAnonymousUserType::CrossNetworkFriend };

    size_t expectedResultCount{ _countof(permissionsToCheck) * (_countof(targetXuids) + _countof(targetUserTypes)) };

    // CODE SNIPPET START: XblPrivacyBatchCheckPermissionAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    auto contextPtr = std::make_unique<size_t>(expectedResultCount);
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = contextPtr.get(); 
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        std::unique_ptr<size_t> expectedCount{ static_cast<size_t*>(asyncBlock->context) };
        size_t resultSize;
        HRESULT hr = XblPrivacyBatchCheckPermissionResultSize(asyncBlock, &resultSize);

        if (SUCCEEDED(hr))
        {
            std::vector<char> buffer(resultSize, 0);
            size_t resultCount{};
            XblPermissionCheckResult* results{};

            hr = XblPrivacyBatchCheckPermissionResult(asyncBlock, resultSize, buffer.data(), &results, &resultCount, nullptr);
            if (SUCCEEDED(hr))
            {
                assert(resultCount == *expectedCount);
            }
        }

        LogToFile("XblPrivacyBatchCheckPermissionResult: hr=%s", ConvertHR(hr).c_str()); // CODE SNIP SKIP
        CallLuaFunctionWithHr(hr, "OnXblPrivacyBatchCheckPermissionAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = XblPrivacyBatchCheckPermissionAsync(
        Data()->xboxLiveContext,
        permissionsToCheck,
        _countof(permissionsToCheck),
        targetXuids,
        _countof(targetXuids),
        targetUserTypes,
        _countof(targetUserTypes),
        asyncBlock.get()
    );
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
        contextPtr.release();
    }

    // CODE SNIPPET END
    LogToFile("XblPrivacyBatchCheckPermissionAsync: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblPrivacyAddMuteListChangedHandler_Lua(lua_State* L)
{
    // CODE SNIPPET START: XblPrivacyAddMuteListChangedHandler
    Data()->muteListChangedHandlerToken = XblPrivacyAddMuteListChangedHandler(
        Data()->xboxLiveContext,
        [](const XblPrivacyMuteListChangeEventArgs* args, void* context)
        {
            UNREFERENCED_PARAMETER(context);
            LogToFile("Mute list changed:");
            std::stringstream ss;
            for (size_t i = 0; i < args->xuidsCount; ++i)
            {
                if (i > 0)
                {
                    ss << ", ";
                }
                ss << args->xuids[i];
            }
            LogToFile("changeType = %u, affectedXuids = %s", args->changeType, ss.str().data());
            CallLuaFunction("OnMuteListChanged"); // CODE SNIP SKIP
        },
        nullptr
    );
    // CODE SNIPPET END

    LogToFile("XblPrivacyAddMuteListChangedHandler");
    return LuaReturnHR(L, S_OK);
}

int XblPrivacyRemoveMuteListChangedHandler_Lua(lua_State* L)
{
    // CODE SNIPPET START: XblPrivacyRemoveMuteListChangedHandler
    HRESULT hr = XblPrivacyRemoveMuteListChangedHandler(Data()->xboxLiveContext, Data()->muteListChangedHandlerToken);

    Data()->muteListChangedHandlerToken = 0;
    // CODE SNIPPET END

    LogToFile("XblPrivacyRemoveMuteListChangedHandler: hr=%s", ConvertHR(hr).data());
    return LuaReturnHR(L, hr);
}

int XblPrivacyAddBlockListChangedHandler_Lua(lua_State* L)
{
    // CODE SNIPPET START: XblPrivacyAddBlockListChangedHandler
    Data()->blockListChangedHandlerToken = XblPrivacyAddBlockListChangedHandler(
        Data()->xboxLiveContext,
        [](const XblPrivacyBlockListChangeEventArgs* args, void* context)
        {
            UNREFERENCED_PARAMETER(context);
            LogToFile("Block list changed:");
            std::stringstream ss;
            for (size_t i = 0; i < args->xuidsCount; ++i)
            {
                if (i > 0)
                {
                    ss << ", ";
                }
                ss << args->xuids[i];
            }
            LogToFile("changeType = %u, affectedXuids = %s", args->changeType, ss.str().data());
            CallLuaFunction("OnBlockListChanged"); // CODE SNIP SKIP
        },
        nullptr
    );
    // CODE SNIPPET END

    LogToFile("XblPrivacyAddBlockListChangedHandler");
    return LuaReturnHR(L, S_OK);
}

int XblPrivacyRemoveBlockListChangedHandler_Lua(lua_State* L)
{
    // CODE SNIPPET START: XblPrivacyRemoveBlockListChangedHandler
    HRESULT hr = XblPrivacyRemoveBlockListChangedHandler(Data()->xboxLiveContext, Data()->blockListChangedHandlerToken);

    Data()->blockListChangedHandlerToken = 0;
    // CODE SNIPPET END

    LogToFile("XblPrivacyRemoveBlockListChangedHandler: hr=%s", ConvertHR(hr).data());
    return LuaReturnHR(L, hr);
}

void SetupAPIs_XblPrivacy()
{
    lua_register(Data()->L, "XblPrivacyGetAvoidListAsync", XblPrivacyGetAvoidListAsync_Lua);
    lua_register(Data()->L, "XblPrivacyCheckPermissionAsync", XblPrivacyCheckPermissionAsync_Lua);
    lua_register(Data()->L, "XblPrivacyCheckPermissionForAnonymousUserAsync", XblPrivacyCheckPermissionForAnonymousUserAsync_Lua);
    lua_register(Data()->L, "XblPrivacyBatchCheckPermissionAsync", XblPrivacyBatchCheckPermissionAsync_Lua);
    lua_register(Data()->L, "XblPrivacyAddMuteListChangedHandler", XblPrivacyAddMuteListChangedHandler_Lua);
    lua_register(Data()->L, "XblPrivacyRemoveMuteListChangedHandler", XblPrivacyRemoveMuteListChangedHandler_Lua);
    lua_register(Data()->L, "XblPrivacyAddBlockListChangedHandler", XblPrivacyAddBlockListChangedHandler_Lua);
    lua_register(Data()->L, "XblPrivacyRemoveBlockListChangedHandler", XblPrivacyRemoveBlockListChangedHandler_Lua);
}
