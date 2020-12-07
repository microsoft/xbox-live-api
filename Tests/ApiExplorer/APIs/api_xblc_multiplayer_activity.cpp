// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"

// Random Xuids from XDKS.1 sandbox
static uint64_t xuids[] =
{
    2814639011617876,
    2814641789541994,
    2814644008675844
};

int XblMultiplayerActivityUpdateRecentPlayers_Lua(lua_State *L)
{
    XblContextHandle xblContext{ Data()->xboxLiveContext };
    uint32_t xuidIndex{ GetUint32FromLua(L, 1, 0) };
    uint64_t xuid{ xuids[xuidIndex] };

    // CODE SNIPPET START: XblMultiplayerActivityUpdateRecentPlayers_C
    XblMultiplayerActivityRecentPlayerUpdate update{ xuid };
    HRESULT hr = XblMultiplayerActivityUpdateRecentPlayers(xblContext, &update, 1);
    // CODE SNIPPET END

    return LuaReturnHR(L, hr);
}

int XblMultiplayerActivityFlushRecentPlayersAsync_Lua(lua_State* L)
{
    XblContextHandle xblContext{ Data()->xboxLiveContext };

    // CODE SNIPPET START: XblMultiplayerActivityFlushRecentPlayersAsync_C
    auto async = std::make_unique<XAsyncBlock>();
    async->queue = Data()->queue;
    async->callback = [](XAsyncBlock* async)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ async }; // take ownership of XAsyncBlock
        HRESULT hr = XAsyncGetStatus(async, false);
        CallLuaFunctionWithHr(hr, "OnXblMultiplayerActivityFlushRecentPlayersAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = XblMultiplayerActivityFlushRecentPlayersAsync(xblContext, async.get());
    if (SUCCEEDED(hr))
    {
        async.release();
    }
    // CODE SNIPPET END

    return LuaReturnHR(L, hr);
}

int XblMultiplayerActivitySetActivityAsync_Lua(lua_State* L)
{
    XblContextHandle xblContext{ Data()->xboxLiveContext };

    // CODE SNIPPET START: XblMultiplayerActivitySetActivityAsync_C
    auto async = std::make_unique<XAsyncBlock>();
    async->queue = Data()->queue;
    async->callback = [](XAsyncBlock* async)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ async }; // take ownership of XAsyncBlock
        HRESULT hr = XAsyncGetStatus(async, false);
        CallLuaFunctionWithHr(hr, "OnXblMultiplayerActivitySetActivityAsync"); // CODE SNIP SKIP
    };

    XblMultiplayerActivityInfo info{};
    info.connectionString = "dummyConnectionString";
    info.joinRestriction = XblMultiplayerActivityJoinRestriction::Followed;
    info.maxPlayers = 10;
    info.currentPlayers = 1;
    info.groupId = "dummyGroupId";

    HRESULT hr = XblMultiplayerActivitySetActivityAsync(
        xblContext,
        &info,
        true,
        async.get()
    );

    if (SUCCEEDED(hr))
    {
        async.release();
    }
    // CODE SNIPPET END

    return LuaReturnHR(L, hr);
}

std::string SerializeActivityInfo(
    const XblMultiplayerActivityInfo* activityInfo,
    size_t activityCount
) noexcept
{
    std::stringstream ss;

    for (size_t i = 0; i < activityCount; ++i, ++activityInfo)
    {
        ss << "{\n";
        ss << "\txuid: " << activityInfo->xuid << "\n";

        ss << "\tconnectionString: ";
        if (activityInfo->connectionString)
        {
            ss << activityInfo->connectionString << "\n";
        }
        ss << "\n";
        
        ss << "\tjoinRestriction: " << static_cast<uint32_t>(activityInfo->joinRestriction) << "\n";
        ss << "\tmaxPlayers: " << activityInfo->maxPlayers << "\n";
        ss << "\tcurrentPlayers: " << activityInfo->currentPlayers << "\n";
        
        ss << "\tgroupId: ";
        if (activityInfo->groupId)
        {
            ss << activityInfo->groupId << "\n";
        }
        ss << "\n";

        ss << "\tplatform: " << static_cast<uint32_t>(activityInfo->platform) << "\n";
        ss << "},\n";
    }

    return ss.str();
}

int XblMultiplayerActivityGetActivityAsync_Lua(lua_State* L)
{
    XblContextHandle xblContext{ Data()->xboxLiveContext };

    // Get our own activity
    uint64_t xuid{ Data()->xboxUserId };

    // CODE SNIPPET START: XblMultiplayerActivityGetActivityAsync_C
    auto async = std::make_unique<XAsyncBlock>();
    async->queue = Data()->queue;
    async->callback = [](XAsyncBlock* async)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ async }; // take ownership of XAsyncBlock

        size_t resultSize{};
        HRESULT hr = XblMultiplayerActivityGetActivityResultSize(async, &resultSize);
        if (SUCCEEDED(hr))
        {
            std::vector<uint8_t> buffer(resultSize);
            XblMultiplayerActivityInfo* activityInfo{};
            size_t resultCount{};
            hr = XblMultiplayerActivityGetActivityResult(async, buffer.size(), buffer.data(), &activityInfo, &resultCount, nullptr);
            if (SUCCEEDED(hr))
            {
                // ...
                // CODE SKIP START
                auto serializedInfo{ SerializeActivityInfo(activityInfo, resultCount) };
                LogToFile("XblMultiplayerActivityGetActivityAsync complete with %u results:\n%s", resultCount, serializedInfo.data());
                // CODE SKIP END
            }
        }
        CallLuaFunctionWithHr(hr, "OnXblMultiplayerActivityGetActivityAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = XblMultiplayerActivityGetActivityAsync(
        xblContext,
        &xuid,
        1,
        async.get()
    );

    if (SUCCEEDED(hr))
    {
        async.release();
    }
    // CODE SNIPPET END

    return LuaReturnHR(L, hr);
}

int XblMultiplayerActivityDeleteActivityAsync_Lua(lua_State* L)
{
    XblContextHandle xblContext{ Data()->xboxLiveContext };

    // CODE SNIPPET START: XblMultiplayerActivityDeleteActivityAsync_C
    auto async = std::make_unique<XAsyncBlock>();
    async->queue = Data()->queue;
    async->callback = [](XAsyncBlock* async)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ async }; // take ownership of XAsyncBlock
        HRESULT hr = XAsyncGetStatus(async, false);
        CallLuaFunctionWithHr(hr, "OnXblMultiplayerActivityDeleteActivityAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = XblMultiplayerActivityDeleteActivityAsync(xblContext, async.get());

    if (SUCCEEDED(hr))
    {
        async.release();
    }
    // CODE SNIPPET END

    return LuaReturnHR(L, hr);
}

int XblMultiplayerActivitySendInvitesAsync_Lua(lua_State* L)
{
    XblContextHandle xblContext{ Data()->xboxLiveContext };
    uint64_t xuid{ GetUint64FromLua(L, 1, xuids[0]) };

    // CODE SNIPPET START: XblMultiplayerActivitySendInvitesAsync_C
    auto async = std::make_unique<XAsyncBlock>();
    async->queue = Data()->queue;
    async->callback = [](XAsyncBlock* async)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ async }; // take ownership of XAsyncBlock
        HRESULT hr = XAsyncGetStatus(async, false);
        CallLuaFunctionWithHr(hr, "OnXblMultiplayerActivitySendInvitesAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = XblMultiplayerActivitySendInvitesAsync(
        xblContext,
        &xuid,
        1,
        true,
        "dummyConnectionString",
        async.get()
    );

    if (SUCCEEDED(hr))
    {
        async.release();
    }
    // CODE SNIPPET END

    return LuaReturnHR(L, hr);
}

#if HC_PLATFORM == HC_PLATFORM_WIN32 || HC_PLATFORM_IS_EXTERNAL

static struct MultiplayerActivityState
{
    MultiplayerActivityState() = default;
    ~MultiplayerActivityState()
    {
        assert(!gameInviteHandlerToken);
    }

    XblFunctionContext gameInviteHandlerToken{ 0 };
} state;

int XblMultiplayerActivityAddInviteHandler_Lua(lua_State* L)
{
    XblContextHandle xblContext{ Data()->xboxLiveContext };

    // CODE SNIPPET START: XblMultiplayerActivityAddInviteHandler_C
    state.gameInviteHandlerToken = XblMultiplayerActivityAddInviteHandler(
        xblContext,
        [](_In_ const XblMultiplayerActivityInviteData* data, _In_opt_ void*)
        {
            // DOTS
            // CODE SKIP START
            std::stringstream ss;
            ss << "{\n";
            ss << "\tinvitedXuid: " << data->invitedXuid << "\n";
            ss << "\tsenderXuid: " << data->senderXuid << "\n";
            ss << "\tsenderGamertag: " << data->senderGamertag << "\n";
            ss << "\tsenderUniqueModernGamertag: " << data->senderUniqueModernGamertag << "\n";
            ss << "\ttitleName: " << data->titleName << "\n";
            ss << "\texpirationTime: " << data->expirationTime << "\n";
            ss << "}\n";

            LogToScreen("MultiplayerActivity invite received: \n%s", ss.str().data());
            CallLuaFunctionWithHr(S_OK, "OnMultiplayerActivityGameInvite"); 
            // CODE SKIP END
        },
        nullptr
    );
    // CODE SNIPPET END

    LogToFile("XblMultiplayerActivityAddInviteHandler");
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerActivityRemoveInviteHandler_Lua(lua_State* L)
{
    XblContextHandle xblContext{ Data()->xboxLiveContext };

    // CODE SNIPPET START: XblMultiplayerActivityRemoveInviteHandler_C
    HRESULT hr = XblMultiplayerActivityRemoveInviteHandler(
        xblContext,
        state.gameInviteHandlerToken
    );

    state.gameInviteHandlerToken = 0;
    // CODE SNIPPET END

    LogToFile("XblMultiplayerActivityRemoveInviteHandler: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}
#endif

void SetupAPIs_XblMultiplayerActivity()
{
    lua_register(Data()->L, "XblMultiplayerActivityUpdateRecentPlayers", XblMultiplayerActivityUpdateRecentPlayers_Lua);
    lua_register(Data()->L, "XblMultiplayerActivityFlushRecentPlayersAsync", XblMultiplayerActivityFlushRecentPlayersAsync_Lua);
    lua_register(Data()->L, "XblMultiplayerActivitySetActivityAsync", XblMultiplayerActivitySetActivityAsync_Lua);
    lua_register(Data()->L, "XblMultiplayerActivityGetActivityAsync", XblMultiplayerActivityGetActivityAsync_Lua);
    lua_register(Data()->L, "XblMultiplayerActivityDeleteActivityAsync", XblMultiplayerActivityDeleteActivityAsync_Lua);
    lua_register(Data()->L, "XblMultiplayerActivitySendInvitesAsync", XblMultiplayerActivitySendInvitesAsync_Lua);
#if HC_PLATFORM == HC_PLATFORM_WIN32 || HC_PLATFORM_IS_EXTERNAL
    lua_register(Data()->L, "XblMultiplayerActivityAddInviteHandler", XblMultiplayerActivityAddInviteHandler_Lua);
    lua_register(Data()->L, "XblMultiplayerActivityRemoveInviteHandler", XblMultiplayerActivityRemoveInviteHandler_Lua);
#endif
}