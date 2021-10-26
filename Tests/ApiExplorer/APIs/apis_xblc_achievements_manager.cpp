// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"
#include <chrono>

static struct AchievementsManagerState
{
    // TODO move doWork logic into this state class
    AchievementsManagerState() = default;
    ~AchievementsManagerState()
    {
        // Validate that our tests cleaned up correctly
        assert(!doWork);
    }

    std::thread doWorkThread{};
    std::atomic<bool> doWork{ false };
    std::atomic<bool> doWorkJoinWhenDone{ false };
    XblAchievementsManagerResultHandle achievementsResultHandle{ nullptr };

    HCMockCallHandle mockHandle{ nullptr };
} achievementsState;

HRESULT AchievementsManagerDoWork()
{
    const XblAchievementsManagerEvent* events{ nullptr };
    size_t eventCount{ 0 };
    HRESULT hr = XblAchievementsManagerDoWork(&events, &eventCount);
    
    if (FAILED(hr))
    {
        return hr;
    }

    for (size_t i = 0; i < eventCount; ++i)
    {
        auto& achievementEvent = events[i];
        // CODE SKIP START
        static std::unordered_map<XblAchievementsManagerEventType, std::string> eventTypesMap
        {
            { XblAchievementsManagerEventType::AchievementProgressUpdated, "AchievementProgressUpdated" },
            { XblAchievementsManagerEventType::AchievementUnlocked, "AchievementUnlocked" },
            { XblAchievementsManagerEventType::LocalUserInitialStateSynced, "LocalUserInitialStateSynced" },
        };
        // CODE SKIP END

        std::stringstream ss;
        ss << "XblAchievementsManagerDoWork: Event of type " << eventTypesMap[achievementEvent.eventType] << std::endl;
        LogToScreen(ss.str().c_str());

        switch (achievementEvent.eventType)
        {
        case XblAchievementsManagerEventType::LocalUserInitialStateSynced:
            LogToScreen("XblAchievementsManagerDoWork: LocalUserInitialStateSynced event");
            CallLuaFunctionWithHr(hr, "OnXblAchievementsManagerDoWork_LocalUserAddedEvent");
            break;
        case XblAchievementsManagerEventType::AchievementProgressUpdated:
            LogToScreen("XblAchievementsManagerDoWork: AchievementProgressUpdated event");
            CallLuaFunctionWithHr(hr, "OnXblAchievementsManagerDoWork_AchievementProgressUpdatedEvent");
            break;
        case XblAchievementsManagerEventType::AchievementUnlocked:
            LogToScreen("XblAchievementsManagerDoWork: AchievementUnlocked event");
            CallLuaFunctionWithHr(hr, "OnXblAchievementsManagerDoWork_AchievementUnlockedEvent");
            break;
        default:
            break;
        }

    }
    return hr;
}

int StartAchievementsManagerDoWorkLoop_Lua(lua_State* L)
{
    achievementsState.doWork = true;
    achievementsState.doWorkJoinWhenDone = true;
    std::lock_guard<std::recursive_mutex> lock(Data()->m_luaLock);
    achievementsState.doWorkThread = std::thread([]()
        {
#if HC_PLATFORM == HC_PLATFORM_ANDROID
            JNIEnv* jniEnv = nullptr;

            // This should be a background thread that MUST attach a new java thread.
            Data()->javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6);
            Data()->javaVM->AttachCurrentThread(&jniEnv, nullptr);

#endif
            Data()->m_achievementsDoWorkDone = false;
            while (achievementsState.doWork && !Data()->m_quit)
            {
                {
                    std::lock_guard<std::recursive_mutex> lock(Data()->m_luaLock);
                    AchievementsManagerDoWork();
                }
                pal::Sleep(10);
            }
            Data()->m_achievementsDoWorkDone = true;
            LogToScreen("Exiting do work thread");

#if HC_PLATFORM == HC_PLATFORM_ANDROID
           Data()->javaVM->DetachCurrentThread();
#endif
        });
    return LuaReturnHR(L, S_OK);
}

int StopAchievementsManagerDoWorkLoop_Lua(lua_State* L)
{
    LogToScreen("StopAchievementsManagerDoWorkLoop_Lua");
    achievementsState.doWorkJoinWhenDone = true;
    achievementsState.doWork = false;
    return LuaReturnHR(L, S_OK);
}

void StopAchievementsManagerDoWorkHelper()
{
    if (achievementsState.doWorkJoinWhenDone)
    {
        achievementsState.doWork = false;
        achievementsState.doWorkJoinWhenDone = false;
        achievementsState.doWorkThread.join();
    }
}

int SetupAchievementsManagerPerformanceTestMock_Lua(lua_State *L)
{
#if HC_PLATFORM == HC_PLATFORM_ANDROID
    std::string addUserMockResponse = ReadFile("PerformanceTestMockResponse.json");
#else
    std::string addUserMockResponse = ReadFile("achievements\\PerformanceTestMockResponse.json");
#endif
    assert(!addUserMockResponse.empty());
    
    auto hr = HCMockCallCreate(&achievementsState.mockHandle);
    assert(SUCCEEDED(hr));

    hr = HCMockAddMock(achievementsState.mockHandle, "GET", "https://achievements.xboxlive.com", nullptr, 0);
    assert(SUCCEEDED(hr));

    hr = HCMockResponseSetStatusCode(achievementsState.mockHandle, 200);
    assert(SUCCEEDED(hr));

    std::vector<uint8_t> bodyBytes{ addUserMockResponse.begin(), addUserMockResponse.end() };
    hr = HCMockResponseSetResponseBodyBytes(achievementsState.mockHandle, bodyBytes.data(), static_cast<uint32_t>(bodyBytes.size()));
    assert(SUCCEEDED(hr));

    return LuaReturnHR(L, hr);
}

// commands

// handles

int XblAchievementsManagerResultGetAchievements_Lua(lua_State *L)
{
    auto resultHandle{ reinterpret_cast<XblAchievementsManagerResultHandle>(GetUint64FromLua(L, 1, reinterpret_cast<uint64_t>(achievementsState.achievementsResultHandle))) };
    if (resultHandle != nullptr) // might be null if original call fails
    {
        // CODE SNIPPET START: XblAchievementsManagerResultGetAchievements_C
        const XblAchievement* achievements = nullptr;
        uint64_t achievementsCount = 0;
        HRESULT hr = XblAchievementsManagerResultGetAchievements(resultHandle, &achievements, &achievementsCount);

        LogToScreen("Got %u Achievements:", achievementsCount);
        // CODE SNIPPET END
        lua_pushinteger(L, static_cast<lua_Integer>(achievementsCount));
        LogToScreen("XblAchievementsManagerResultGetAchievements: hr=%s", ConvertHR(hr).c_str());
        return LuaReturnHR(L, hr, 1);
    }
    else
    {
        HRESULT hr = S_OK;
        lua_pushinteger(L, static_cast<lua_Integer>(0));
        LogToScreen("XblAchievementsManagerResultGetAchievements: hr=%s", ConvertHR(hr).c_str());
        return LuaReturnHR(L, hr, 1);
    }

}

int XblAchievementsManagerResultDuplicateHandle_Lua(lua_State *L)
{
    auto resultHandle{ reinterpret_cast<XblAchievementsManagerResultHandle>(GetUint64FromLua(L, 1, reinterpret_cast<uint64_t>(achievementsState.achievementsResultHandle))) };
    // CODE SNIPPET START: XblAchievementsRelationshipResultDuplicateHandle_C
    XblAchievementsManagerResultHandle handle{};
    XblAchievementsManagerResultDuplicateHandle(resultHandle, &handle);
    // CODE SNIPPET END

    XblAchievementsManagerResultCloseHandle(handle);
    LogToScreen("XblAchievementsManagerResultDuplicateHandle");
    return LuaReturnHR(L, S_OK);
}

int XblAchievementsManagerResultCloseHandle_Lua(lua_State *L)
{
    auto resultHandle{ reinterpret_cast<XblAchievementsManagerResultHandle>(GetUint64FromLua(L, 1, reinterpret_cast<uint64_t>(achievementsState.achievementsResultHandle))) };
    // CODE SNIPPET START: XblAchievementsRelationshipResultCloseHandle_C
    if (resultHandle == achievementsState.achievementsResultHandle)
    {
        achievementsState.achievementsResultHandle = nullptr;
    }
    XblAchievementsManagerResultCloseHandle(resultHandle);
    // CODE SNIPPET END
    LogToScreen("XblAchievementsManagerResultCloseHandle");
    return LuaReturnHR(L, S_OK);
}

// manager 

int XblAchievementsManagerAddLocalUser_Lua(lua_State *L)
{
    XalUserHandle user = Data()->xalUser;
    
    HRESULT hr = XblAchievementsManagerAddLocalUser(user, nullptr);

    LogToScreen("XblAchievementsManagerAddLocalUser: %s", ConvertHR(hr).c_str(), nullptr);
    return LuaReturnHR(L, hr);
}

int XblAchievementsManagerRemoveLocalUser_Lua(lua_State *L)
{
    XalUserHandle user = Data()->xalUser;

    HRESULT hr = XblAchievementsManagerRemoveLocalUser(user);

    LogToScreen("XblAchievementsManagerAddLocalUser: %s", ConvertHR(hr).c_str(), nullptr);
    return LuaReturnHR(L, hr);
}

int XblAchievementsManagerGetAchievement_Lua(lua_State *L)
{
    auto achievementId{ GetStringFromLua(L, 1, "1") };
    
    HRESULT hr = XblAchievementsManagerGetAchievement(Data()->xboxUserId, achievementId.c_str(), &achievementsState.achievementsResultHandle);
    if (FAILED(hr))
    {
        lua_pushinteger(L, 0);
        return LuaReturnHR(L, hr, 1);
    }
    lua_pushinteger(L, reinterpret_cast<lua_Integer>(achievementsState.achievementsResultHandle));

    LogToScreen("XblAchievementsManagerGetAchievement: %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr, 1);
}

int XblAchievementsManagerGetAchievements_Lua(lua_State *L)
{
    HRESULT hr = XblAchievementsManagerGetAchievements(
        Data()->xboxUserId,
        XblAchievementOrderBy::DefaultOrder,
        XblAchievementsManagerSortOrder::Unsorted,
        &achievementsState.achievementsResultHandle);
    
    if (FAILED(hr))
    {
        lua_pushinteger(L, 0);
        return LuaReturnHR(L, hr, 1);
    }
    lua_pushinteger(L, reinterpret_cast<lua_Integer>(achievementsState.achievementsResultHandle));
    return LuaReturnHR(L, hr, 1);
}

int XblAchievementsManagerGetAchievementsPerfTest_Lua(lua_State *L)
{
    auto startTime = std::chrono::high_resolution_clock::now();
    HRESULT hr = XblAchievementsManagerGetAchievements(
        Data()->xboxUserId, 
        XblAchievementOrderBy::DefaultOrder,
        XblAchievementsManagerSortOrder::Unsorted,
        &achievementsState.achievementsResultHandle);
    auto endTime = std::chrono::high_resolution_clock::now();
    
    if (FAILED(hr))
    {
        lua_pushinteger(L, 0);
        return LuaReturnHR(L, hr, 1);
    }
    lua_pushinteger(L, reinterpret_cast<lua_Integer>(achievementsState.achievementsResultHandle));
    
    std::chrono::duration<double> duration = endTime - startTime;
    LogToScreen("XblAchievementsManagerGetAchievements: %s", ConvertHR(hr).c_str());
    LogToScreen("XblAchievementsManagerGetAchievements Performance Test Duration: %f milliseconds", duration.count() * 1000);
    return LuaReturnHR(L, hr, 1);
}

int XblAchievementsManagerGetAchievementsByState_Lua(lua_State *L)
{
    HRESULT hr = XblAchievementsManagerGetAchievementsByState(
        Data()->xboxUserId,
        XblAchievementOrderBy::DefaultOrder,
        XblAchievementsManagerSortOrder::Unsorted,
        XblAchievementProgressState::Achieved,
        &achievementsState.achievementsResultHandle
    );
    if (FAILED(hr))
    {
        return LuaReturnHR(L, hr, 1);
    }
    lua_pushinteger(L, reinterpret_cast<lua_Integer>(achievementsState.achievementsResultHandle));

    LogToScreen("XblAchievementsManagerGetAchievementsByState: %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr, 1);
}

int XblAchievementsManagerUpdateAchievement_Lua(lua_State *L)
{
    auto achievementId{ GetStringFromLua(L, 1, "1") };
    uint8_t newProgress{ static_cast<uint8_t>(GetUint32FromLua(L, 2, 100)) };

    HRESULT hr = XblAchievementsManagerUpdateAchievement(Data()->xboxUserId, achievementId.c_str(), newProgress);
    LogToScreen("XblAchievementsManagerUpdateAchievement: %s", ConvertHR(hr).c_str());

    return LuaReturnHR(L, hr);
}

void SetupAPIs_XblAchievementsManager()
{
    // Non XSAPI APIs
    lua_register(Data()->L, "StartAchievementsManagerDoWorkLoop", StartAchievementsManagerDoWorkLoop_Lua);
    lua_register(Data()->L, "StopAchievementsManagerDoWorkLoop", StopAchievementsManagerDoWorkLoop_Lua);
    lua_register(Data()->L, "SetupAchievementsManagerPerformanceTestMock", SetupAchievementsManagerPerformanceTestMock_Lua);
    lua_register(Data()->L, "XblAchievementsManagerGetAchievementsPerfTest", XblAchievementsManagerGetAchievementsPerfTest_Lua);

    // XSAPI APIs
    lua_register(Data()->L, "XblAchievementsManagerAddLocalUser", XblAchievementsManagerAddLocalUser_Lua);
    lua_register(Data()->L, "XblAchievementsManagerRemoveLocalUser", XblAchievementsManagerRemoveLocalUser_Lua);
    lua_register(Data()->L, "XblAchievementsManagerGetAchievement", XblAchievementsManagerGetAchievement_Lua);
    lua_register(Data()->L, "XblAchievementsManagerGetAchievements", XblAchievementsManagerGetAchievements_Lua);
    lua_register(Data()->L, "XblAchievementsManagerGetAchievementsByState", XblAchievementsManagerGetAchievementsByState_Lua);
    
    lua_register(Data()->L, "XblAchievementsManagerResultGetAchievements", XblAchievementsManagerResultGetAchievements_Lua);
    lua_register(Data()->L, "XblAchievementsManagerResultDuplicateHandle", XblAchievementsManagerResultDuplicateHandle_Lua);
    lua_register(Data()->L, "XblAchievementsManagerResultCloseHandle", XblAchievementsManagerResultCloseHandle_Lua);

    lua_register(Data()->L, "XblAchievementsManagerUpdateAchievement", XblAchievementsManagerUpdateAchievement_Lua);
}