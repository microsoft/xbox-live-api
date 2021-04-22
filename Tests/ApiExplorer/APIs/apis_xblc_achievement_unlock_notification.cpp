// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"
#if HC_PLATFORM == HC_PLATFORM_WIN32
namespace xbl
{
namespace apirunner
{

enum status
{
    OK = 0,
    ERROR_NO_MSG
};

static status errorCode;


HRESULT GetAchievementLockStatus(const std::string& id, bool& isLocked)
{
    LogToScreen("GetAchievement [%s]", id.c_str());

    CreateQueueIfNeeded();

    std::unique_ptr<XAsyncBlock>    asyncBlock = std::make_unique<XAsyncBlock>();
    
    HRESULT hr = XblAchievementsGetAchievementAsync( Data()->xboxLiveContext,
                                                     Data()->xboxUserId,
                                                     Data()->scid,
                                                     id.c_str(),
                                                     asyncBlock.get());

    // wait for get achievement to complete
    XAsyncGetStatus(asyncBlock.get(), true);

    XblAchievementsResultHandle resultHandle;

    hr = XblAchievementsGetAchievementResult(asyncBlock.get(), &resultHandle);

    if (hr < 0) 
    {
        return hr;
    }

    const XblAchievement*   achievements = nullptr;
    size_t                  achievementsCount = 0;

    hr = XblAchievementsResultGetAchievements(resultHandle, &achievements, &achievementsCount);

    LogToFile("Got achievementsCount: %d", achievementsCount);

    if (achievementsCount != 1)
    {
        return hr;
    }

    if (achievements[0].progressState == XblAchievementProgressState::Achieved)
    {
        isLocked = false;
    }
    else
    {
        isLocked = true;
    }
    
    const char * state = (isLocked) ? "Not achieved" : "Achieved";
    LogToScreen( "Achievement id=[%s] name=[%s] state[%s]",
                 achievements[0].id,
                 achievements[0].name,
                 state);

    return hr;
}


HRESULT UnlockAchievement(std::string id)
{
    LogToScreen("UnlockAchievement [%s]", id.c_str());

    CreateQueueIfNeeded();

    std::unique_ptr<XAsyncBlock>    asyncBlock = std::make_unique<XAsyncBlock>();

    
    uint32_t percent = 100;
    HRESULT hr =  XblAchievementsUpdateAchievementAsync( Data()->xboxLiveContext,
                                                         Data()->xboxUserId,
                                                         id.c_str(),
                                                         percent,
                                                         asyncBlock.get());

    XAsyncGetStatus(asyncBlock.get(), true);

    XblAchievementsResultHandle resultHandle;
    hr = XblAchievementsGetAchievementResult(asyncBlock.get(), &resultHandle);

    return hr;
}


void CALLBACK AchievementUnlockHandler(const XblAchievementUnlockEvent* args, void* context)
{
    UNREFERENCED_PARAMETER(context);
    LogToScreen("Achievement Unlock Notification");
    LogToScreen("titleId: %u", args->titleId);
    LogToScreen("achievementName: %s", args->achievementName);
    LogToScreen("achievementIcon: %s", args->achievementIcon);
    LogToScreen("gamerscore: %u", args->gamerscore);
    LogToScreen("rarityPercentage: %f", args->rarityPercentage);
    LogToScreen("rarityCategory: %u", static_cast<uint32_t>(args->rarityCategory));

    errorCode = status::OK;
}


XblFunctionContext SetHandler()
{
    XblFunctionContext id = XblAchievementUnlockAddNotificationHandler(Data()->xboxLiveContext,
        AchievementUnlockHandler,
        nullptr);
    return id;
}


void UnsetHandler(XblFunctionContext id)
{
    XblAchievementUnlockRemoveNotificationHandler(Data()->xboxLiveContext, id);
}


namespace lua
{
    int XblAchievementUnlockAddNotificationHandler(lua_State *L)
    {
        auto id = SetHandler();
        lua_pushinteger(L, id);
        LuaReturnHR(L, S_OK, 1);
        return 0;
    }

    int XblAchievementUnlockRemoveNotificationHandler(lua_State *L)
    {
        uint32_t id = GetUint32FromLua(L, 1, 0);
        UnsetHandler(id);
        return 0;
    }

    int RunAchievementUnlock(lua_State *L)
    {
        std::string achievementId = luaL_checkstring(L, 1);

        HRESULT hr = 0;
        errorCode = status::ERROR_NO_MSG;

        Sleep(1000);

        hr = UnlockAchievement(achievementId);

        return LuaReturnHR(L, hr);
    }


    int Cleanup(lua_State *L)
    {
        uint32_t id = GetUint32FromLua(L, 1, 0);
        LogToScreen("Got %d", id);
        UnsetHandler(id);
        return 0;
    }


    int CheckStatus(lua_State *L)
    {
        lua_pushinteger(L, errorCode);
        return 1;
    }


    int IsAchievementLocked(lua_State *L)
    {
        std::string achievementId = luaL_checkstring(L, 1);

        bool isLocked = false;

        HRESULT hr = GetAchievementLockStatus(achievementId, isLocked);

        lua_pushboolean(L, isLocked);

        return LuaReturnHR(L, hr, 1);
    }

}
}
}


void SetupAPIs_XblAchievementUnlockNotification()
{
    lua_register(Data()->L, "RunAchievementUnlock", xbl::apirunner::lua::RunAchievementUnlock);
    lua_register(Data()->L, "Cleanup", xbl::apirunner::lua::Cleanup);
    lua_register(Data()->L, "CheckStatus", xbl::apirunner::lua::CheckStatus);
    lua_register(Data()->L, "IsAchievementLocked", xbl::apirunner::lua::IsAchievementLocked);
    lua_register(Data()->L, "XblAchievementUnlockAddNotificationHandler", xbl::apirunner::lua::XblAchievementUnlockAddNotificationHandler);
    lua_register(Data()->L, "XblAchievementUnlockRemoveNotificationHandler", xbl::apirunner::lua::XblAchievementUnlockRemoveNotificationHandler);
}

#endif