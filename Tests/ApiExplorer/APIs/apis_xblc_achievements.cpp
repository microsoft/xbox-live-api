// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"

std::string AchievementProgressToString(XblAchievement achievement)
{
    std::stringstream stream;

    if (achievement.progressState == XblAchievementProgressState::Achieved)
    {
        stream << "Achieved (" << achievement.progression.timeUnlocked << ")";
    }
    else if (achievement.progressState == XblAchievementProgressState::InProgress)
    {
        stream << "Started";
        stream << "(" << achievement.progression.requirements[0].currentProgressValue << "/" << achievement.progression.requirements[0].targetProgressValue << ")";
    }
    else if (achievement.progressState == XblAchievementProgressState::NotStarted)
    {
        stream << "Not Started";
    }
    else if (achievement.progressState == XblAchievementProgressState::Unknown)
    {
        stream << "Unknown";
    }

    return stream.str();
}

XblAchievementType ConvertStringToXblAchievementType(const char* str)
{
    XblAchievementType type = XblAchievementType::Unknown;

    if (pal::stricmp(str, "XblAchievementType::Unknown") == 0) type = XblAchievementType::Unknown;
    else if (pal::stricmp(str, "XblAchievementType::All") == 0) type = XblAchievementType::All;
    else if (pal::stricmp(str, "XblAchievementType::Persistent") == 0) type = XblAchievementType::Persistent;
    else if (pal::stricmp(str, "XblAchievementType::Challenge") == 0) type = XblAchievementType::Challenge;

    return type;
}

XblAchievementOrderBy ConvertStringToXblAchievementOrderBy(const char* str)
{
    XblAchievementOrderBy orderBy = XblAchievementOrderBy::DefaultOrder;

    if (pal::stricmp(str, "XblAchievementOrderBy::DefaultOrder") == 0) orderBy = XblAchievementOrderBy::DefaultOrder;
    else if (pal::stricmp(str, "XblAchievementOrderBy::TitleId") == 0) orderBy = XblAchievementOrderBy::TitleId;
    else if (pal::stricmp(str, "XblAchievementOrderBy::UnlockTime") == 0) orderBy = XblAchievementOrderBy::UnlockTime;

    return orderBy;
}

// commands

int XblAchievementsResultGetAchievements_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblAchievementsResultGetAchievements
    const XblAchievement* achievements;
    size_t achievementsCount;
    HRESULT hr = XblAchievementsResultGetAchievements(Data()->achievementsResult, &achievements, &achievementsCount);
    // CODE SNIPPET END
    LogToFile("XblAchievementsResultGetAchievements: hr=%s", ConvertHR(hr).c_str());

    if (SUCCEEDED(hr))
    {
        std::stringstream stream;
        for (uint32_t i = 0; i < achievementsCount; i++)
        {
            stream << "\t" << achievements[i].name << "(" << AchievementProgressToString(achievements[i]) << ")\n";
        }
        LogToFile(stream.str().c_str());
    }

    return LuaReturnHR(L, hr);
}

int XblAchievementsResultHasNext_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblAchievementsResultHasNext
    HRESULT hr = S_OK;
    bool hasNext = false;
    if (Data()->achievementsResult != nullptr)
    {
        hr = XblAchievementsResultHasNext(Data()->achievementsResult, &hasNext);
    }
    // CODE SNIPPET END
    LogToFile("XblAchievementsResultHasNext: hr=%s hasNext=%s", ConvertHR(hr).c_str(), hasNext ? "true" : "false");

    lua_pushnumber(L, (int)hasNext);
    return LuaReturnHR(L, hr, 1);
}

int XblAchievementsResultGetNextAsync_Lua(lua_State *L)
{
    CreateQueueIfNeeded();

    uint32_t maxItems = GetUint32FromLua(L, 1, 100);
    LogToFile("XblAchievementsResultGetNextAsync: MaxItems: %d", maxItems);

    // CODE SNIPPET START: XblAchievementsResultGetNextAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        XblAchievementsResultHandle resultHandle;
        auto hr = XblAchievementsResultGetNextResult(asyncBlock, &resultHandle);
        LogToFile("XblAchievementsResultGetNextResult: hr=%s", ConvertHR(hr).c_str()); // CODE SNIP SKIP

        if (SUCCEEDED(hr))
        {
            // CODE SKIP START
            if (Data()->achievementsResult)
            {
                XblAchievementsResultCloseHandle(Data()->achievementsResult);
            }
            XblAchievementsResultDuplicateHandle(resultHandle, &Data()->achievementsResult);
            // CODE SKIP END            
            const XblAchievement* achievements = nullptr;
            size_t achievementsCount = 0;
            hr = XblAchievementsResultGetAchievements(resultHandle, &achievements, &achievementsCount);

            LogToFile("OnXblAchievementsGetAchievementsForTitleIdAsync: Got achievementsCount: %d", achievementsCount); // CODE SNIP SKIP
            for (size_t i = 0; i < achievementsCount; i++)
            {
                LogToScreen("Achievement %s: %s = %s",
                    achievements[i].id,
                    achievements[i].name,
                    (achievements[i].progressState == XblAchievementProgressState::Achieved) ? "Achieved" : "Not achieved");
            }

            XblAchievementsResultCloseHandle(resultHandle); // when done with handle, close it
            achievements = nullptr; // Clear array after calling XblAchievementsResultCloseHandle to pointer to freed memory
        }
        CallLuaFunctionWithHr(hr, "OnXblAchievementsResultGetNextAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = XblAchievementsResultGetNextAsync(
        Data()->achievementsResult,
        maxItems,
        asyncBlock.get()
    );
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END
    LogToFile("XblAchievementsResultGetNextAsync: hr=%s", ConvertHR(hr).c_str());

    return LuaReturnHR(L, hr);
}

int XblAchievementsGetAchievementsForTitleIdAsync_Lua(lua_State *L)
{
    CreateQueueIfNeeded();

    XblAchievementType achievementType = ConvertStringToXblAchievementType(GetStringFromLua(L, 1, "XblAchievementType::All").c_str());
    bool unlockedOnly = GetBoolFromLua(L, 2, false);
    XblAchievementOrderBy orderBy = ConvertStringToXblAchievementOrderBy(GetStringFromLua(L, 3, "XblAchievementOrderBy::DefaultOrder").c_str());
    uint32_t skipItems = GetUint32FromLua(L, 4, 0);
    uint32_t maxItems = GetUint32FromLua(L, 5, 100);

    LogToFile("XblAchievementsGetAchievementsForTitleIdAsync: AchievementType: %d", achievementType);
    LogToFile("XblAchievementsGetAchievementsForTitleIdAsync: unlockedOnly: %s", unlockedOnly ? "true" : "false");
    LogToFile("XblAchievementsGetAchievementsForTitleIdAsync: OrderBy: %d", orderBy);
    LogToFile("XblAchievementsGetAchievementsForTitleIdAsync: SkipItems: %d", skipItems);
    LogToFile("XblAchievementsGetAchievementsForTitleIdAsync: MaxItems: %d", maxItems);

    // CODE SNIPPET START: XblAchievementsGetAchievementsForTitleIdAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*

        XblAchievementsResultHandle resultHandle;
        auto hr = XblAchievementsGetAchievementsForTitleIdResult(asyncBlock, &resultHandle);
        LogToFile("XblAchievementsGetAchievementsForTitleIdResult: hr=%s", ConvertHR(hr).c_str()); // CODE SNIP SKIP

        if (SUCCEEDED(hr))
        {
            // CODE SKIP START
            if (Data()->achievementsResult)
            {
                XblAchievementsResultCloseHandle(Data()->achievementsResult);
            }
            XblAchievementsResultDuplicateHandle(resultHandle, &Data()->achievementsResult);
            // CODE SKIP END            
            const XblAchievement* achievements = nullptr;
            size_t achievementsCount = 0;
            hr = XblAchievementsResultGetAchievements(resultHandle, &achievements, &achievementsCount);

            LogToFile("OnXblAchievementsGetAchievementsForTitleIdAsync: Got achievementsCount: %d", achievementsCount); // CODE SNIP SKIP
            for (size_t i = 0; i < achievementsCount; i++)
            {
                LogToScreen("Achievement %s: %s = %s", 
                    achievements[i].id, 
                    achievements[i].name,
                    (achievements[i].progressState == XblAchievementProgressState::Achieved) ? "Achieved" : "Not achieved");
            }

            XblAchievementsResultCloseHandle(resultHandle); // when done with handle, close it
            achievements = nullptr; // Clear array after calling XblAchievementsResultCloseHandle to pointer to freed memory
            // Instead you could not close the handle and store it.  Then 
            // if you needed to copy the handle, call XblAchievementsResultDuplicateHandle()
        }
        CallLuaFunctionWithHr(hr, "OnXblAchievementsGetAchievementsForTitleIdAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = XblAchievementsGetAchievementsForTitleIdAsync(
        Data()->xboxLiveContext,
        Data()->xboxUserId,
        Data()->titleId,
        achievementType,
        unlockedOnly,
        orderBy,
        skipItems,
        maxItems,
        asyncBlock.get()
    );
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release(); 
    }
    // CODE SNIPPET END
    LogToFile("XblAchievementsGetAchievementsForTitleIdAsync: hr=%s", ConvertHR(hr).c_str());

    return LuaReturnHR(L, hr);
}

int XblAchievementsResultCloseHandle_Lua(lua_State *L)
{
    if (Data()->achievementsResult)
    {
        XblAchievementsResultCloseHandle(Data()->achievementsResult);
        Data()->achievementsResult = nullptr;
    }

    return LuaReturnHR(L, S_OK);
}

int XblAchievementsGetAchievementAsync_Lua(lua_State *L)
{
    CreateQueueIfNeeded();

    auto achievementId = GetStringFromLua(L, 1, "1");
    LogToFile("XblAchievementsGetAchievementAsync: AchievementId: %s", achievementId.c_str());

    // CODE SNIPPET START: XblAchievementsGetAchievementAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        XblAchievementsResultHandle resultHandle;
        auto hr = XblAchievementsGetAchievementResult(asyncBlock, &resultHandle);

        LogToFile("XblAchievementsGetAchievementResult: hr=%s", ConvertHR(hr).c_str()); // CODE SNIP SKIP
        if (SUCCEEDED(hr))
        {
            // CODE SKIP START
            if (Data()->achievementsResult)
            {
                XblAchievementsResultCloseHandle(Data()->achievementsResult);
            }
            XblAchievementsResultDuplicateHandle(resultHandle, &Data()->achievementsResult);
            // CODE SKIP END
            const XblAchievement* achievements = nullptr;
            size_t achievementsCount = 0;
            hr = XblAchievementsResultGetAchievements( resultHandle, &achievements, &achievementsCount );

            for (size_t i = 0; i < achievementsCount; i++)
            {
                LogToScreen("Achievement %s: %s = %s",
                    achievements[i].id,
                    achievements[i].name,
                    (achievements[i].progressState == XblAchievementProgressState::Achieved) ? "Achieved" : "Not achieved");
            }

            XblAchievementsResultCloseHandle(resultHandle); // when done with handle, close it
            achievements = nullptr; // Clear array after calling XblAchievementsResultCloseHandle to pointer to freed memory
        }
        CallLuaFunctionWithHr(hr, "OnXblAchievementsGetAchievementAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = XblAchievementsGetAchievementAsync(
        Data()->xboxLiveContext,
        Data()->xboxUserId,
        Data()->scid,
        achievementId.c_str(),
        asyncBlock.get()
    );
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END
    LogToFile("XblAchievementsGetAchievementAsync: hr=%s", ConvertHR(hr).c_str());

    return LuaReturnHR(L, hr);
}

int XblAchievementsUpdateAchievementAsync_Lua(lua_State *L)
{
    CreateQueueIfNeeded();

    auto achievementId = GetStringFromLua(L, 1, "1");
    uint32_t percentComplete = GetUint32FromLua(L, 2, 100);
    LogToFile("XblAchievementsUpdateAchievementAsync: AchievementId: %s", achievementId.c_str());
    LogToFile("XblAchievementsUpdateAchievementAsync: PercentComplete: %d", percentComplete);

    // CODE SNIPPET START: XblAchievementsUpdateAchievementAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        auto result = XAsyncGetStatus(asyncBlock, false);
        LogToFile("XAsyncGetStatus: hr=%s", ConvertHR(result).c_str()); // CODE SNIP SKIP
        if (SUCCEEDED(result))
        {
            // Achievement updated
        }
        else if (result == HTTP_E_STATUS_NOT_MODIFIED)
        {
            // Achievement not modified
        }
        else
        {
            // Achievement failed to update
        }
        CallLuaFunctionWithHr(result, "OnXblAchievementsUpdateAchievementAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = XblAchievementsUpdateAchievementAsync(
        Data()->xboxLiveContext,
        Data()->xboxUserId,
        achievementId.c_str(),
        percentComplete,
        asyncBlock.get()
    );
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END
    LogToFile("XblAchievementsUpdateAchievementAsync: hr=%s", ConvertHR(hr).c_str());

    return LuaReturnHR(L, hr);
}

int XblAchievementsUpdateAchievementForTitleIdAsync_Lua(lua_State *L)
{
    CreateQueueIfNeeded();

    auto achievementId = GetStringFromLua(L, 1, "1");
    uint32_t percentComplete = GetUint32FromLua(L, 2, 100);
    LogToFile("XblAchievementsGetAchievementAsync: AchievementId: %s", achievementId.c_str());
    LogToFile("XblAchievementsGetAchievementAsync: PercentComplete: %d", percentComplete);

    // CODE SNIPPET START: XblAchievementsUpdateAchievementForTitleIdAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        auto result = XAsyncGetStatus(asyncBlock, false);
        LogToFile("XAsyncGetStatus: hr=%s", ConvertHR(result).c_str()); // CODE SNIP SKIP
        if (SUCCEEDED(result))
        {
            // Achievement updated
        }
        else if (result == HTTP_E_STATUS_NOT_MODIFIED)
        {
            // Achievement not modified
        }
        else
        {
            // Achievement failed to update
        }
        CallLuaFunctionWithHr(result, "OnXblAchievementsUpdateAchievementForTitleIdAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = XblAchievementsUpdateAchievementForTitleIdAsync(
        Data()->xboxLiveContext,
        Data()->xboxUserId,
        Data()->titleId,
        Data()->scid,
        achievementId.c_str(),
        percentComplete,
        asyncBlock.get()
    );
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END
    LogToFile("XblAchievementsUpdateAchievementForTitleIdAsync: hr=%s", ConvertHR(hr).c_str());

    return LuaReturnHR(L, hr);
}

void SetupAPIs_XblAchievements()
{
    lua_register(Data()->L, "XblAchievementsResultGetAchievements", XblAchievementsResultGetAchievements_Lua);
    lua_register(Data()->L, "XblAchievementsResultGetNextAsync", XblAchievementsResultGetNextAsync_Lua);
    lua_register(Data()->L, "XblAchievementsResultHasNext", XblAchievementsResultHasNext_Lua);
    lua_register(Data()->L, "XblAchievementsResultCloseHandle", XblAchievementsResultCloseHandle_Lua);
    lua_register(Data()->L, "XblAchievementsGetAchievementAsync", XblAchievementsGetAchievementAsync_Lua);
    lua_register(Data()->L, "XblAchievementsGetAchievementsForTitleIdAsync", XblAchievementsGetAchievementsForTitleIdAsync_Lua);
    lua_register(Data()->L, "XblAchievementsUpdateAchievementAsync", XblAchievementsUpdateAchievementAsync_Lua);
    lua_register(Data()->L, "XblAchievementsUpdateAchievementForTitleIdAsync", XblAchievementsUpdateAchievementForTitleIdAsync_Lua);    
}
