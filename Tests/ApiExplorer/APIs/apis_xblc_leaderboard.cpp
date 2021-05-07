// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"

XblSocialGroupType ConvertStringToXblSocialGroupType(const char* str)
{
    XblSocialGroupType type = XblSocialGroupType::None;

    if (pal::stricmp(str, "XblSocialGroupType::None") == 0) type = XblSocialGroupType::None;
    else if (pal::stricmp(str, "XblSocialGroupType::People") == 0) type = XblSocialGroupType::People;
    else if (pal::stricmp(str, "XblSocialGroupType::Favorites") == 0) type = XblSocialGroupType::Favorites;

    return type;
}

XblLeaderboardSortOrder ConvertStringToXblLeaderboardSortOrder(const char* str)
{
    XblLeaderboardSortOrder type = XblLeaderboardSortOrder::Descending;

    if (pal::stricmp(str, "XblLeaderboardSortOrder::Descending") == 0) type = XblLeaderboardSortOrder::Descending;
    else if (pal::stricmp(str, "XblLeaderboardSortOrder::Ascending") == 0) type = XblLeaderboardSortOrder::Ascending;

    return type;
}

XblLeaderboardQueryType ConvertStringToXblLeaderboardQueryType(const std::string& queryType)
{
    if (queryType == "XblLeaderboardQueryType::TitleManagedStatBackedGlobal")
    {
        return XblLeaderboardQueryType::TitleManagedStatBackedGlobal;
    }
    else if (queryType == "XblLeaderboardQueryType::TitleManagedStatBackedSocial")
    {
        return XblLeaderboardQueryType::TitleManagedStatBackedSocial;
    }
    else
    {
        return XblLeaderboardQueryType::UserStatBacked;
    }
}

int XblLeaderboardGetLeaderboardAsync_Lua(lua_State *L)
{
    CreateQueueIfNeeded();

    auto leaderboardName = GetStringFromLua(L, 1, "TotalPuzzlesSolvedLB");
    auto statName = GetStringFromLua(L, 2, "");
    XblSocialGroupType socialGroup = ConvertStringToXblSocialGroupType(GetStringFromLua(L, 3, "XblSocialGroupType::None").c_str());
    auto queryType = ConvertStringToXblLeaderboardQueryType(GetStringFromLua(L, 4, "XblLeaderboardQueryType::UserStatBacked"));
    XblLeaderboardSortOrder order = ConvertStringToXblLeaderboardSortOrder(GetStringFromLua(L, 5, "XblLeaderboardSortOrder::Descending").c_str());
    auto scid = GetStringFromLua(L, 6, Data()->scid);
    auto xboxUserId = GetUint64FromLua(L, 7, Data()->xboxUserId);
    uint32_t maxItems = GetUint32FromLua(L, 8, 0);
    uint64_t skipToXboxUserId = GetUint64FromLua(L, 9, 0);
    uint32_t skipResultToRank = GetUint32FromLua(L, 10, 0);

    // _Field_z_ const char** additionalColumnleaderboardNames;
    // size_t additionalColumnleaderboardNamesCount;

    LogToFile("XblLeaderboardGetLeaderboardAsync: xboxUserId: %d", xboxUserId);
    LogToFile("XblLeaderboardGetLeaderboardAsync: scid: %s", scid.c_str());
    LogToFile("XblLeaderboardGetLeaderboardAsync: leaderboardName: %s", leaderboardName.c_str());
    LogToFile("XblLeaderboardGetLeaderboardAsync: statName: %s", statName.c_str());
    LogToFile("XblLeaderboardGetLeaderboardAsync: maxItems: %d", maxItems);
    LogToFile("XblLeaderboardGetLeaderboardAsync: skipToXboxUserId: %d", skipToXboxUserId);
    LogToFile("XblLeaderboardGetLeaderboardAsync: skipResultToRank: %d", skipResultToRank);
    LogToFile("XblLeaderboardGetLeaderboardAsync: socialGroup: %d", socialGroup);
    LogToFile("XblLeaderboardGetLeaderboardAsync: order: %d", order);

    // CODE SNIPPET START: XblLeaderboardGetLeaderboardAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        size_t resultSize;
        HRESULT hr = XblLeaderboardGetLeaderboardResultSize(asyncBlock, &resultSize);

        if (SUCCEEDED(hr))
        {
            Data()->leaderboardBuffer.resize(resultSize);
            XblLeaderboardResult* leaderboard{};

            hr = XblLeaderboardGetLeaderboardResult(asyncBlock, resultSize, Data()->leaderboardBuffer.data(), &leaderboard, nullptr);
            LogToFile("XblLeaderboardGetLeaderboardResult: %s columnsCount=%d rowsCount=%d", ConvertHR(hr).c_str(), leaderboard->columnsCount, leaderboard->rowsCount); // CODE SNIP SKIP

            if (SUCCEEDED(hr))
            {
                // Use XblLeaderboardResult in result
                LogToScreen("Got %d rows in leaderboard", leaderboard->rowsCount); // CODE SNIP SKIP
                for (auto row = 0u; row < leaderboard->rowsCount; ++row)
                {
                    std::stringstream rowText;
                    rowText << leaderboard->rows[row].xboxUserId << "\t";

                    for (auto column = 0u; column < leaderboard->rows[row].columnValuesCount; ++column)
                    {
                        // Each column value is in JSON format
                        rowText << leaderboard->rows[row].columnValues[column] << "\t";
                    }
                    LogToFile(rowText.str().data()); // CODE SNIP SKIP
                }
                Data()->leaderboardResult = leaderboard; // CODE SNIP SKIP
            }
        }
        CallLuaFunctionWithHr(hr, "OnXblLeaderboardGetLeaderboardAsync"); // CODE SNIP SKIP
    };

    XblLeaderboardQuery leaderboardQuery = {}; 
    pal::strcpy(leaderboardQuery.scid, sizeof(leaderboardQuery.scid), scid.c_str()); 
    leaderboardQuery.leaderboardName = leaderboardName.empty() ? nullptr : leaderboardName.c_str();
    // See below on more options in XblLeaderboardQuery
    leaderboardQuery.xboxUserId = xboxUserId; // CODE SNIP SKIP
    leaderboardQuery.statName = statName.c_str(); // CODE SNIP SKIP
    leaderboardQuery.maxItems = maxItems; // CODE SNIP SKIP
    leaderboardQuery.skipToXboxUserId = skipToXboxUserId; // CODE SNIP SKIP
    leaderboardQuery.skipResultToRank = skipResultToRank; // CODE SNIP SKIP
    leaderboardQuery.socialGroup = socialGroup; // CODE SNIP SKIP
    leaderboardQuery.order = order; // CODE SNIP SKIP
    leaderboardQuery.queryType = queryType; // CODE SNIP SKIP

    HRESULT hr = XblLeaderboardGetLeaderboardAsync(
        Data()->xboxLiveContext,
        leaderboardQuery,
        asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END
    LogToFile("XblLeaderboardGetLeaderboardAsync: hr=%s", ConvertHR(hr).c_str());

    return LuaReturnHR(L, hr);
}

int XblLeaderboardResultHasNext_Lua(lua_State *L)
{
    HRESULT hr = S_OK;
    bool hasNext = false;
    if (Data()->leaderboardResult != nullptr)
    {
        hasNext = Data()->leaderboardResult->hasNext;
    }

    lua_pushnumber(L, (int)hasNext);
    return LuaReturnHR(L, hr, 1);
}

int XblLeaderboardResultGetNextAsync_Lua(lua_State *L)
{
    CreateQueueIfNeeded();

    uint32_t maxItems = GetUint32FromLua(L, 1, 0);
    LogToFile("XblLeaderboardGetLeaderboardAsync: maxItems: %d", maxItems);

    // CODE SNIPPET START: XblLeaderboardResultGetNextAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        size_t resultSize;
        HRESULT hr = XblLeaderboardResultGetNextResultSize(asyncBlock, &resultSize);

        if (SUCCEEDED(hr))
        {
            Data()->leaderboardBuffer.resize(resultSize);
            XblLeaderboardResult* result{};

            hr = XblLeaderboardResultGetNextResult(asyncBlock, resultSize, Data()->leaderboardBuffer.data(), &result, nullptr);
            // Use result to read the leaderboard results
            // CODE SKIP START
            LogToFile("XblLeaderboardResultGetNextResult: %s columnsCount=%d rowsCount=%d", ConvertHR(hr).c_str(), result->columnsCount, result->rowsCount);
            LogToScreen("LeaderboardName = %s", result->nextQuery.leaderboardName);

            Data()->leaderboardResult = result; 
            // CODE SKIP END
        }
        CallLuaFunctionWithHr(hr, "OnXblLeaderboardResultGetNextAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = XblLeaderboardResultGetNextAsync(
        Data()->xboxLiveContext,
        Data()->leaderboardResult,
        maxItems,
        asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END
    LogToFile("XblLeaderboardResultGetNextAsync: hr=%s", ConvertHR(hr).c_str());

    return LuaReturnHR(L, hr);
}

void SetupAPIs_XblLeaderboard()
{
    lua_register(Data()->L, "XblLeaderboardGetLeaderboardAsync", XblLeaderboardGetLeaderboardAsync_Lua);
    lua_register(Data()->L, "XblLeaderboardResultGetNextAsync", XblLeaderboardResultGetNextAsync_Lua);
    lua_register(Data()->L, "XblLeaderboardResultHasNext", XblLeaderboardResultHasNext_Lua);
}

