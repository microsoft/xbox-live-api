// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"

XBL_WARNING_DISABLE_DEPRECATED

int GetLastStat_Lua(lua_State *L)
{
    long long n = Data()->lastUserStat;
    lua_pushinteger(L, static_cast<lua_Integer>(n));
    return 1;
}

int XblUserStatisticsGetSingleUserStatisticAsync_Lua(lua_State *L)
{
    CreateQueueIfNeeded();
    if (Data()->xboxUserId == 0)
    {
        return LuaReturnHR(L, E_FAIL);
    }
    
    auto statisticName = GetStringFromLua(L, 1, "TotalPuzzlesSolved"); 
    LogToFile("XblUserStatisticsGetSingleUserStatisticAsync: statisticName: %s", statisticName.c_str());
    
    // CODE SNIPPET START: XblUserStatisticsGetSingleUserStatisticAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        size_t resultSize;
        HRESULT hr = XblUserStatisticsGetSingleUserStatisticResultSize(asyncBlock, &resultSize);

        if (SUCCEEDED(hr))
        {
            std::vector<char> buffer(resultSize, 0);
            XblUserStatisticsResult* result{};

            hr = XblUserStatisticsGetSingleUserStatisticResult(asyncBlock, resultSize, buffer.data(), &result, nullptr);
            VERIFY_IS_TRUE(result != nullptr, "null");// CODE SNIP SKIP
            VERIFY_IS_TRUE(result->xboxUserId != 0, "xuid");// CODE SNIP SKIP
            VERIFY_IS_TRUE(result->serviceConfigStatisticsCount == 1, "serviceConfigStatisticsCount");// CODE SNIP SKIP
            VERIFY_IS_TRUE(result->serviceConfigStatistics[0].statisticsCount == 1, "statisticsCount");// CODE SNIP SKIP
            VERIFY_IS_TRUE(strcmp(result->serviceConfigStatistics[0].statistics[0].statisticType, "Integer")==0, "statisticType");// CODE SNIP SKIP

            if (SUCCEEDED(hr) && result->serviceConfigStatisticsCount > 0 && result->serviceConfigStatistics->statisticsCount > 0)
            {
                int64_t userStatValue = atoll(result->serviceConfigStatistics[0].statistics[0].value);
                // Now you can show or store userStatValue
                Data()->lastUserStat = userStatValue; // CODE SNIP SKIP
                LogToScreen("%s's stat %s is %lld.  Note: With GDK, ensure fiddler isn't running for stat upload to work", // CODE SNIP SKIP
                    Data()->gamertag.c_str(), // CODE SNIP SKIP
                    result->serviceConfigStatistics[0].statistics[0].statisticName, // CODE SNIP SKIP
                    Data()->lastUserStat); // CODE SNIP SKIP
            }
        }

        CallLuaFunctionWithHr(hr, "OnXblUserStatisticsGetSingleUserStatisticAsync"); // CODE SNIP SKIP
    };
    
    HRESULT hr = XblUserStatisticsGetSingleUserStatisticAsync(
        Data()->xboxLiveContext,
        Data()->xboxUserId,
        Data()->scid,
        statisticName.c_str(),
        asyncBlock.get()
    );
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }

    // CODE SNIPPET END
    LogToFile("XblUserStatisticsGetSingleUserStatisticAsync: hr=%s", ConvertHR(hr).c_str());
    
    return LuaReturnHR(L, hr);
}

int XblUserStatisticsGetSingleUserStatisticsAsync_Lua(lua_State *L)
{
    CreateQueueIfNeeded();

    auto statisticName1 = GetStringFromLua(L, 1, "TotalPuzzlesSolved");
    auto statisticName2 = GetStringFromLua(L, 2, "TotalRoundsStarted");
    LogToFile("XblUserStatisticsGetSingleUserStatisticsAsync: statisticName1: %s", statisticName1.c_str());
    LogToFile("XblUserStatisticsGetSingleUserStatisticsAsync: statisticName2: %s", statisticName2.c_str());

    // CODE SNIPPET START: XblUserStatisticsGetSingleUserStatisticsAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        size_t resultSize;
        HRESULT hr = XblUserStatisticsGetSingleUserStatisticsResultSize(asyncBlock, &resultSize);

        if (SUCCEEDED(hr))
        {
            std::vector<char> buffer(resultSize, 0);
            XblUserStatisticsResult* results{};

            hr = XblUserStatisticsGetSingleUserStatisticsResult(asyncBlock, resultSize, buffer.data(), &results, nullptr);
            if (SUCCEEDED(hr))
            {
                // Now you can use the XblUserStatisticsResult array in results
            }
        }

        CallLuaFunctionWithHr(hr, "OnXblUserStatisticsGetSingleUserStatisticsAsync"); // CODE SNIP SKIP
    };

    const char* statisticNames[2] = {};
    statisticNames[0] = statisticName1.c_str();
    statisticNames[1] = statisticName2.c_str();

    HRESULT hr = XblUserStatisticsGetSingleUserStatisticsAsync(
        Data()->xboxLiveContext,
        Data()->xboxUserId,
        Data()->scid,
        statisticNames, 2, 
        asyncBlock.get()
    );
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }

    // CODE SNIPPET END
    LogToFile("XblUserStatisticsGetSingleUserStatisticsAsync: hr=%s", ConvertHR(hr).c_str());

    return LuaReturnHR(L, hr);
}

int XblUserStatisticsGetMultipleUserStatisticsAsync_Lua(lua_State *L)
{
    CreateQueueIfNeeded();

    auto statisticName1 = GetStringFromLua(L, 1, "TotalPuzzlesSolved");
    auto statisticName2 = GetStringFromLua(L, 2, "TotalRoundsStarted");
    LogToFile("XblUserStatisticsGetMultipleUserStatisticsAsync: statisticName1: %s", statisticName1.c_str());
    LogToFile("XblUserStatisticsGetMultipleUserStatisticsAsync: statisticName2: %s", statisticName2.c_str());

    auto xuid1 = GetUint64FromLua(L, 3, Data()->xboxUserId);
    auto xuid2 = GetUint64FromLua(L, 4, 2814634367189975);
    LogToFile("XblUserStatisticsGetMultipleUserStatisticsAsync: xuid1: %ul", xuid1);
    LogToFile("XblUserStatisticsGetMultipleUserStatisticsAsync: xuid2: %ul", xuid2);

    // CODE SNIPPET START: XblUserStatisticsGetMultipleUserStatisticsAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        size_t resultSize;
        HRESULT hr = XblUserStatisticsGetMultipleUserStatisticsResultSize(asyncBlock, &resultSize);

        if (SUCCEEDED(hr))
        {
            std::vector<char> buffer(resultSize, 0);
            XblUserStatisticsResult* results{};
            size_t resultsCount = 0;

            hr = XblUserStatisticsGetMultipleUserStatisticsResult(asyncBlock, resultSize, buffer.data(), &results, &resultsCount, nullptr);

            // Process results array to read the user stats data
            for (size_t iResult = 0; iResult < resultsCount; iResult++)
            {
                LogToFile("%d", results[iResult].xboxUserId);
                for (size_t iScid = 0; iScid < results[iResult].serviceConfigStatisticsCount; iScid++)
                {
                    LogToFile("SCID: %s", results[iResult].serviceConfigStatistics[iScid].serviceConfigurationId);
                    for (size_t iStat = 0; iStat < results[iResult].serviceConfigStatistics[iScid].statisticsCount; iStat++)
                    {
                        LogToFile("Stat %d: name:%s value:%s type:%s", iResult,
                            results[iResult].serviceConfigStatistics[iScid].statistics[iStat].statisticName,
                            results[iResult].serviceConfigStatistics[iScid].statistics[iStat].value,
                            results[iResult].serviceConfigStatistics[iScid].statistics[iStat].statisticType );
                    }
                }
            }
        }

        CallLuaFunctionWithHr(hr, "OnXblUserStatisticsGetMultipleUserStatisticsAsync"); // CODE SNIP SKIP
    };

    const char* statisticNames[2] = {};
    statisticNames[0] = statisticName1.c_str();
    statisticNames[1] = statisticName2.c_str();

    uint64_t xuids[2] = {};
    xuids[0] = xuid1;
    xuids[1] = xuid2;

    HRESULT hr = XblUserStatisticsGetMultipleUserStatisticsAsync(
        Data()->xboxLiveContext,
        xuids, 2,
        Data()->scid,
        statisticNames, 2,
        asyncBlock.get()
    );
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }

    // CODE SNIPPET END
    LogToFile("XblUserStatisticsGetMultipleUserStatisticsAsync: hr=%s", ConvertHR(hr).c_str());

    return LuaReturnHR(L, hr);
}

int XblUserStatisticsGetMultipleUserStatisticsForMultipleServiceConfigurationsAsync_Lua(lua_State *L)
{
    CreateQueueIfNeeded();

    auto statisticName1 = GetStringFromLua(L, 1, "TotalPuzzlesSolved");
    auto statisticName2 = GetStringFromLua(L, 2, "TotalRoundsStarted");
    LogToFile("XblUserStatisticsGetMultipleUserStatisticsForMultipleServiceConfigurationsAsync: statisticName1: %s", statisticName1.c_str());
    LogToFile("XblUserStatisticsGetMultipleUserStatisticsForMultipleServiceConfigurationsAsync: statisticName2: %s", statisticName2.c_str());

    auto xuid1 = GetUint64FromLua(L, 3, Data()->xboxUserId);
    auto xuid2 = GetUint64FromLua(L, 4, 2814634367189975);
    LogToFile("XblUserStatisticsGetMultipleUserStatisticsForMultipleServiceConfigurationsAsync: xuid1: %ul", xuid1);
    LogToFile("XblUserStatisticsGetMultipleUserStatisticsForMultipleServiceConfigurationsAsync: xuid2: %ul", xuid2);

    // CODE SNIPPET START: XblUserStatisticsGetMultipleUserStatisticsForMultipleServiceConfigurationsAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        size_t resultSize;
        HRESULT hr = XblUserStatisticsGetMultipleUserStatisticsForMultipleServiceConfigurationsResultSize(asyncBlock, &resultSize);

        if (SUCCEEDED(hr))
        {
            std::vector<char> buffer(resultSize, 0);
            XblUserStatisticsResult* results{};
            size_t resultsCount = 0;

            hr = XblUserStatisticsGetMultipleUserStatisticsForMultipleServiceConfigurationsResult(asyncBlock, resultSize, buffer.data(), &results, &resultsCount, nullptr);
            LogToFile("XblUserStatisticsGetMultipleUserStatisticsForMultipleServiceConfigurationsResult: hr=%s count=%d", ConvertHR(hr).c_str(), resultsCount); // CODE SNIP SKIP

            if (SUCCEEDED(hr))
            {
                for (size_t i = 0; i < resultsCount; i++)
                {
                    // Log results
                    std::stringstream stream;
                    stream << "XUID: " << results[i].xboxUserId << std::endl;;

                    for (size_t j = 0; j < results[i].serviceConfigStatisticsCount; j++)
                    {
                        stream << " " << results[i].serviceConfigStatistics[j].serviceConfigurationId << ": " << std::endl;
                        for (size_t k = 0; k < results[i].serviceConfigStatistics[j].statisticsCount; k++)
                        {
                            stream << " " << results[i].serviceConfigStatistics[j].statistics[k].statisticName << "=" << results[i].serviceConfigStatistics[j].statistics[k].value << std::endl;
                        }
                    }
                    LogToScreen(stream.str().c_str()); // CODE SNIP SKIP
                }
            }
        }

        CallLuaFunctionWithHr(hr, "OnXblUserStatisticsGetMultipleUserStatisticsForMultipleServiceConfigurationsAsync"); // CODE SNIP SKIP
    };

    const char* requestedStatsNames1[2] = {};
    requestedStatsNames1[0] = statisticName1.c_str();
    requestedStatsNames1[1] = statisticName2.c_str();

    XblRequestedStatistics requestedStats[1] = {};
    pal::strcpy(requestedStats[0].serviceConfigurationId, sizeof(XblRequestedStatistics::serviceConfigurationId), Data()->scid);
    requestedStats[0].statistics = requestedStatsNames1;
    requestedStats[0].statisticsCount = 2;

    uint64_t xuids[2] = {};
    xuids[0] = xuid1;
    xuids[1] = xuid2;
        
    HRESULT hr = XblUserStatisticsGetMultipleUserStatisticsForMultipleServiceConfigurationsAsync(
        Data()->xboxLiveContext,
        xuids, 2,
        requestedStats, 1,
        asyncBlock.get()
    );
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }

    // CODE SNIPPET END
    LogToFile("XblUserStatisticsGetMultipleUserStatisticsForMultipleServiceConfigurationsAsync: hr=%s", ConvertHR(hr).c_str());

    return LuaReturnHR(L, hr);
}

int XblUserStatisticsTrackStatistics_Lua(lua_State *L)
{
    CreateQueueIfNeeded();
    if (Data()->xboxUserId == 0)
    {
        return LuaReturnHR(L, E_FAIL);
    }

    auto statisticName = GetStringFromLua(L, 1, "TotalPuzzlesSolved");
    LogToFile("XblUserStatisticsTrackStatistics: statisticName: %s", statisticName.c_str());
    std::vector<const char*> statisticNames{ statisticName.data() };

    // CODE SNIPPET START: XblUserStatisticsTrackStatistics
    HRESULT hr = XblUserStatisticsTrackStatistics(
        Data()->xboxLiveContext,
        &Data()->xboxUserId,
        1,
        Data()->scid,
        statisticNames.data(),
        statisticNames.size()
    );
    // CODE SNIPPET END

    LogToFile("XblUserStatisticsTrackStatistics: hr=%s", ConvertHR(hr).c_str());

    return LuaReturnHR(L, hr);
}

int XblUserStatisticsStopTrackingStatistics_Lua(lua_State *L)
{
    CreateQueueIfNeeded();
    if (Data()->xboxUserId == 0)
    {
        return LuaReturnHR(L, E_FAIL);
    }

    auto statisticName = GetStringFromLua(L, 1, "TotalPuzzlesSolved");
    LogToFile("XblUserStatisticsTrackStatistics: statisticName: %s", statisticName.c_str());
    std::vector<const char*> statisticNames{ statisticName.data() };

    // CODE SNIPPET START: XblUserStatisticsStopTrackingStatistics
    HRESULT hr = XblUserStatisticsStopTrackingStatistics(
        Data()->xboxLiveContext,
        &Data()->xboxUserId,
        1,
        Data()->scid,
        statisticNames.data(),
        statisticNames.size()
    );
    // CODE SNIPPET END

    LogToFile("XblUserStatisticsStopTrackingStatistics: hr=%s", ConvertHR(hr).c_str());

    return LuaReturnHR(L, hr);
}

int XblUserStatisticsStopTrackingUsers_Lua(lua_State* L)
{
    CreateQueueIfNeeded();
    if (Data()->xboxUserId == 0)
    {
        return LuaReturnHR(L, E_FAIL);
    }

    // CODE SNIPPET START: XblUserStatisticsStopTrackingStatistics
    HRESULT hr = XblUserStatisticsStopTrackingUsers(
        Data()->xboxLiveContext,
        &Data()->xboxUserId,
        1
    );
    // CODE SNIPPET END

    LogToFile("XblUserStatisticsStopTrackingUsers: hr=%s", ConvertHR(hr).c_str());

    return LuaReturnHR(L, hr);
}

int XblUserStatisticsSubscribeToStatisticChange_Lua(lua_State *L)
{
    CreateQueueIfNeeded();
    if (Data()->xboxUserId == 0)
    {
        return LuaReturnHR(L, E_FAIL);
    }
    if (Data()->statisticChangeSubscriptionHandle != nullptr)
    {
        return LuaReturnHR(L, E_FAIL);
    }

    auto statisticName = GetStringFromLua(L, 1, "TotalPuzzlesSolved");
    LogToFile("XblUserStatisticsSubscribeToStatisticChange: statisticName: %s", statisticName.c_str());

    // CODE SNIPPET START: XblUserStatisticsSubscribeToStatisticChange
    XblRealTimeActivitySubscriptionHandle subscriptionHandle{ nullptr };

    HRESULT hr = XblUserStatisticsSubscribeToStatisticChange(
        Data()->xboxLiveContext,
        Data()->xboxUserId,
        Data()->scid,
        statisticName.c_str(),
        &subscriptionHandle
    );
    // CODE SNIPPET END
    Data()->statisticChangeSubscriptionHandle = subscriptionHandle;

    LogToFile("XblUserStatisticsSubscribeToStatisticChange: hr=%s", ConvertHR(hr).c_str());

    return LuaReturnHR(L, hr);
}

int XblUserStatisticsUnsubscribeFromStatisticChange_Lua(lua_State *L)
{
    CreateQueueIfNeeded();
    if (Data()->xboxUserId == 0)
    {
        return LuaReturnHR(L, E_FAIL);
    }

    HRESULT hr = S_OK;
    if (Data()->statisticChangeSubscriptionHandle != nullptr)
    {
        // CODE SNIPPET START: XblUserStatisticsUnsubscribeFromStatisticChange
        hr = XblUserStatisticsUnsubscribeFromStatisticChange(
            Data()->xboxLiveContext,
            Data()->statisticChangeSubscriptionHandle
        );
        // CODE SNIPPET END
        Data()->statisticChangeSubscriptionHandle = nullptr;
    }

    LogToFile("XblUserStatisticsUnsubscribeFromStatisticChange: hr=%s", ConvertHR(hr).c_str());

    return LuaReturnHR(L, hr);
}

int XblUserStatisticsAddStatisticChangedHandler_Lua(lua_State *L)
{
    CreateQueueIfNeeded();
    if (Data()->xboxUserId == 0)
    {
        return LuaReturnHR(L, E_FAIL);
    }

    // CODE SNIPPET START: XblUserStatisticsAddStatisticChangedHandler
    void* context{ nullptr };
    XblFunctionContext statisticChangedFunctionContext = XblUserStatisticsAddStatisticChangedHandler(
        Data()->xboxLiveContext,
        [](XblStatisticChangeEventArgs eventArgs, void* context) 
        {
            // Handle stat change 
            LogToScreen("Statistic changed callback: stat changed (%s = %s)",
                eventArgs.latestStatistic.statisticName, 
                eventArgs.latestStatistic.value); 
            CallLuaFunctionWithHr(S_OK, "OnStatisticChangedHandler"); // CODE SNIP SKIP
            UNREFERENCED_PARAMETER(context); // CODE SNIP SKIP
        },
        context
        );
    // CODE SNIPPET END
    Data()->statisticChangedFunctionContext = statisticChangedFunctionContext;

    LogToFile("XblUserStatisticsAddStatisticChangedHandler: done");

    return LuaReturnHR(L, S_OK);
}

int XblUserStatisticsRemoveStatisticChangedHandler_Lua(lua_State *L)
{
    CreateQueueIfNeeded();
    if (Data()->xboxUserId == 0)
    {
        return LuaReturnHR(L, E_FAIL);
    }

    // CODE SNIPPET START: XblUserStatisticsRemoveStatisticChangedHandler
    XblUserStatisticsRemoveStatisticChangedHandler(
            Data()->xboxLiveContext,
            Data()->statisticChangedFunctionContext
        );
    // CODE SNIPPET END
    Data()->statisticChangedFunctionContext = 0;

    LogToFile("XblUserStatisticsRemoveStatisticChangedHandler: done");

    return LuaReturnHR(L, S_OK);
}

void SetupAPIs_XblStatistics()
{
    lua_register(Data()->L, "GetLastStat", GetLastStat_Lua);
    lua_register(Data()->L, "XblUserStatisticsGetSingleUserStatisticAsync", XblUserStatisticsGetSingleUserStatisticAsync_Lua);
    lua_register(Data()->L, "XblUserStatisticsGetSingleUserStatisticsAsync", XblUserStatisticsGetSingleUserStatisticsAsync_Lua);
    lua_register(Data()->L, "XblUserStatisticsGetMultipleUserStatisticsAsync", XblUserStatisticsGetMultipleUserStatisticsAsync_Lua);
    lua_register(Data()->L, "XblUserStatisticsGetMultipleUserStatisticsForMultipleServiceConfigurationsAsync", XblUserStatisticsGetMultipleUserStatisticsForMultipleServiceConfigurationsAsync_Lua);
    lua_register(Data()->L, "XblUserStatisticsTrackStatistics", XblUserStatisticsTrackStatistics_Lua);
    lua_register(Data()->L, "XblUserStatisticsStopTrackingStatistics", XblUserStatisticsStopTrackingStatistics_Lua);
    lua_register(Data()->L, "XblUserStatisticsStopTrackingUsers", XblUserStatisticsStopTrackingUsers_Lua);
    lua_register(Data()->L, "XblUserStatisticsSubscribeToStatisticChange", XblUserStatisticsSubscribeToStatisticChange_Lua);
    lua_register(Data()->L, "XblUserStatisticsUnsubscribeFromStatisticChange", XblUserStatisticsUnsubscribeFromStatisticChange_Lua);
    lua_register(Data()->L, "XblUserStatisticsAddStatisticChangedHandler", XblUserStatisticsAddStatisticChangedHandler_Lua);
    lua_register(Data()->L, "XblUserStatisticsRemoveStatisticChangedHandler", XblUserStatisticsRemoveStatisticChangedHandler_Lua);
}

