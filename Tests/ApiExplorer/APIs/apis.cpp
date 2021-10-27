// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"

int SetCheckHR_Lua(lua_State *L)
{
    Data()->m_checkHR = GetBoolFromLua(L, 1, true);
    return 0;
}

int GetCheckHR_Lua(lua_State *L)
{
    lua_pushboolean(L, Data()->m_checkHR);
    return 1;
}

int Sleep_Lua(lua_State *L)
{
    DWORD time = (DWORD)GetUint32FromLua(L, 1, 0);
    LogToScreen("Sleep(%d)", time);
    pal::Sleep(time);
    return LuaReturnHR(L, S_OK);
}

int StopTestFile_Lua(lua_State *L)
{
    Data()->m_stopTest = true;
    return LuaReturnHR(L, S_OK);
}

int LogHelper(bool logToFile, lua_State *L)
{
    int arg = 1;
    int nargs = lua_gettop(L);
    for (; nargs--; arg++) 
    {
        if (lua_type(L, arg) == LUA_TNUMBER) 
        {
            if (lua_isinteger(L, arg))
            {
                LogCat(logToFile, "%d", lua_tointeger(L, arg));
            }
            else
            {
                LogCat(logToFile, "%f", lua_tonumber(L, arg));
            }
        }
        else if (lua_type(L, arg) == LUA_TBOOLEAN)
        {
            LogCat(logToFile, "%d", lua_toboolean(L, arg));
        }
        else if (lua_type(L, arg) == LUA_TSTRING)
        {
            size_t l;
            const char *s = luaL_checklstring(L, arg, &l);
            LogCat(logToFile, s);
        }
    }
    LogCat(logToFile, "\n");
    return 0;
}

int LogToFile_Lua(lua_State *L)
{
    return LogHelper(true, L);
}

int LogToScreen_Lua(lua_State *L)
{
    return LogHelper(false, L);
}

int IsRunningTests_Lua(lua_State *L)
{
    bool isTestDone = !Data()->m_runningTests;
    lua_pushboolean(L, isTestDone);
    return 1;
}

int GetLastError_Lua(lua_State *L)
{
    HRESULT hr = Data()->m_lastError;
    lua_pushinteger(L, hr);
    return 1;
}

int SetCallUpdate_Lua(lua_State *L)
{
    Data()->m_callUpdate = GetBoolFromLua(L, 1, true);
    return 0;
}

int SetTestWasSkipped_Lua(lua_State *L)
{
    UNREFERENCED_PARAMETER(L);
    Data()->m_wasTestSkipped = true;
    return 0;
}

int SetOnXalTryAddFirstUserSilentlyAsync_Lua(lua_State *L)
{
    Data()->m_onXalTryAddFirstUserSilentlyAsync = GetStringFromLua(L, 1, "");
    return 0;
}

int SetOnTaskQueueTerminateWithAsyncWait_Lua(lua_State *L)
{
    Data()->m_onTaskQueueTerminateWithAsyncWait = GetStringFromLua(L, 1, "");
    return 0;
}

int MultiDeviceGetRemoteState_Lua(lua_State *L)
{
    std::string key = GetStringFromLua(L, 1, "");

    std::string value = Data()->m_multiDeviceManager->GetStateValueFromKey(key);
    lua_pushstring(L, value.c_str());
    return 1;
}

void MultiDeviceWaitTillRemoteStateHelper(const std::string key, const std::string& valueToWaitFor)
{
    LogToScreen("MultiDevice: Waiting for %s key to be %s in cloud DB", key.c_str(), valueToWaitFor.c_str());
    while (true)
    {
        std::string curValue = Data()->m_multiDeviceManager->GetStateValueFromKey(key);
        if (curValue == valueToWaitFor)
        {
            LogToScreen("MultiDevice: Done waiting. Got %s from cloud DB", valueToWaitFor.c_str());
            break;
        }
        pal::Sleep(100);
    }
}

int MultiDeviceWaitTillRemoteState_Lua(lua_State *L)
{
    std::string key = GetStringFromLua(L, 1, "");
    std::string valueToWaitFor = GetStringFromLua(L, 2, "");

    MultiDeviceWaitTillRemoteStateHelper(key, valueToWaitFor);
    return 0;
}

int MultiDeviceSyncAndWait_Lua(lua_State *L)
{
    std::string key = GetStringFromLua(L, 1, "");

    if (Data()->m_multiDeviceManager->IsHost())
    {
        // Set to READY, and wait for peer to ACK, then delete ACK
        Data()->m_multiDeviceManager->SetSessionState(key, "READY", [](HRESULT) {});
        MultiDeviceWaitTillRemoteStateHelper(key, "ACK");
        Data()->m_multiDeviceManager->SetSessionState(key, "", [](HRESULT) {});
    }
    else
    {
        // Wait for state to be READY, then ACK
        MultiDeviceWaitTillRemoteStateHelper(key, "READY");
        Data()->m_multiDeviceManager->SetSessionState(key, "ACK", [](HRESULT) {});
    }

    return 0;
}

int MultiDeviceSetLocalState_Lua(lua_State *L)
{
    std::string key = GetStringFromLua(L, 1, "");
    std::string value = GetStringFromLua(L, 2, "");

    Data()->m_multiDeviceManager->SetSessionState(key, value, [](HRESULT) {});
    return 0;
}

int MultiDeviceIsHost_Lua(lua_State *L)
{
    int value = Data()->m_multiDeviceManager->IsHost() ? 1 : 0;
    lua_pushnumber(L, value);
    return 1;
}

int MultiDeviceGetRemoteXuid_Lua(lua_State *L)
{
    if (Data()->m_multiDeviceManager->IsHost())
    {
        std::string value = Data()->m_multiDeviceManager->GetSessionState().client2xuid;
        lua_pushstring(L, value.c_str());
    }
    else
    {
        std::string value = Data()->m_multiDeviceManager->GetSessionState().client1xuid;
        lua_pushstring(L, value.c_str());
    }
    return 1;
}

int APIRunner_AssertOnAllocOfId_Lua(lua_State *L)
{
    auto id = GetUint64FromLua(L, 1, 0);
    auto memHook = GetApiRunnerMemHook();
    memHook->AssertOnAllocOfId(id);
    return LuaReturnHR(L, S_OK);
}

int APIRunner_MemStartTracking_Lua(lua_State *L)
{
    auto memHook = GetApiRunnerMemHook();
    memHook->StartMemTracking();
    return LuaReturnHR(L, S_OK);
}

int APIRunner_LogStats_Lua(lua_State *L)
{
    auto memHook = GetApiRunnerMemHook();
    memHook->LogStats("MemCheck");
    return LuaReturnHR(L, S_OK);
}

int APIRunner_MemLogUnhookedStats_Lua(lua_State *L)
{
    auto memHook = GetApiRunnerMemHook();
    memHook->LogUnhookedStats();
    return LuaReturnHR(L, S_OK);
}

int IsGDKPlatform_Lua(lua_State *L)
{
#if HC_PLATFORM == HC_PLATFORM_GDK
    lua_pushboolean(L, true);
#else 
    lua_pushboolean(L, false);
#endif
    return 1;
}

void RegisterLuaAPIs()
{
    SetupAPIs_Xal();
    SetupAPIs_Xbl();
    SetupAPIs_Async();
    SetupAPIs_XblAchievements();
    SetupAPIs_XblAchievementsManager();
    SetupAPIs_XblAchievementsProgressNotifications();
    SetupAPIs_XblSocial();
    SetupAPIs_XblSocialManager();
    SetupAPIs_XblProfile();
    SetupAPIS_Platform();
    SetupAPIs_LibHttp();
    SetupAPIs_XblMultiplayer();
    SetupAPIs_XblMultiplayerManager();
    SetupAPIs_XblPrivacy();
    SetupAPIs_XblEvents();
    SetupAPIs_XblStatistics();
    SetupAPIs_XblLeaderboard();
    SetupAPIs_GrtsGameInvite();
    SetupAPIs_XblRta();
    SetupAPIs_XblPresence();
    SetupAPIs_XblHttp();
    SetupAPIs_XblTitleStorage();
    SetupAPIs_XblTitleManagedStats();
    SetupAPIs_XblStringVerify();
    SetupAPIs_XblMultiplayerActivity();

    SetupAPIs_CppAchievements();
    SetupAPIs_CppLeaderboard();
    SetupAPIs_CppProfile();
    SetupAPIs_CppPrivacy();
    SetupAPIs_CppStringVerify();
    SetupAPIs_CppTitleStorage();
    SetupAPIs_CppEvents();
    SetupAPIs_CppSocial();
    SetupAPIs_CppSocialManager();
    SetupAPIs_CppPresence();
    SetupAPIs_CppStatistics();
    SetupAPIs_CppRta();
    SetupAPIs_CppMultiplayer();

    
#if HC_PLATFORM == HC_PLATFORM_WIN32
    SetupupAPIs_XblGameInviteNotifications();
    SetupAPIs_XblAchievementUnlockNotification();
#endif
    SetupAPIs_GRTS();

    lua_register(Data()->L, "SetCheckHR", SetCheckHR_Lua);
    lua_register(Data()->L, "GetCheckHR", GetCheckHR_Lua);
    lua_register(Data()->L, "StopTestFile", StopTestFile_Lua);
    lua_register(Data()->L, "Sleep", Sleep_Lua);
    lua_register(Data()->L, "LogToScreen", LogToScreen_Lua);
    lua_register(Data()->L, "LogToFile", LogToFile_Lua);
    lua_register(Data()->L, "IsRunningTests", IsRunningTests_Lua);
    lua_register(Data()->L, "GetLastError", GetLastError_Lua);
    lua_register(Data()->L, "SetCallUpdate", SetCallUpdate_Lua);
    lua_register(Data()->L, "SetTestWasSkipped", SetTestWasSkipped_Lua);
    lua_register(Data()->L, "SetOnXalTryAddFirstUserSilentlyAsync", SetOnXalTryAddFirstUserSilentlyAsync_Lua);
    lua_register(Data()->L, "SetOnTaskQueueTerminateWithAsyncWait", SetOnTaskQueueTerminateWithAsyncWait_Lua);

    lua_register(Data()->L, "MultiDeviceGetRemoteState", MultiDeviceGetRemoteState_Lua);
    lua_register(Data()->L, "MultiDeviceSetLocalState", MultiDeviceSetLocalState_Lua);
    lua_register(Data()->L, "MultiDeviceSyncAndWait", MultiDeviceSyncAndWait_Lua);
    lua_register(Data()->L, "MultiDeviceIsHost", MultiDeviceIsHost_Lua);
    lua_register(Data()->L, "MultiDeviceGetRemoteXuid", MultiDeviceGetRemoteXuid_Lua);
    lua_register(Data()->L, "MultiDeviceWaitTillRemoteState", MultiDeviceWaitTillRemoteState_Lua);

    lua_register(Data()->L, "APIRunner_MemStartTracking", APIRunner_MemStartTracking_Lua);
    lua_register(Data()->L, "APIRunner_LogStats", APIRunner_LogStats_Lua);
    lua_register(Data()->L, "APIRunner_AssertOnAllocOfId", APIRunner_AssertOnAllocOfId_Lua);
    lua_register(Data()->L, "APIRunner_MemLogUnhookedStats", APIRunner_MemLogUnhookedStats_Lua);
    lua_register(Data()->L, "IsGDKPlatform", IsGDKPlatform_Lua);
}

void SetupAPIS_Platform()
{
}
