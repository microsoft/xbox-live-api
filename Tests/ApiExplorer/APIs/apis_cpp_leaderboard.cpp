// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"

int LeaderboardServiceGetLeaderboard_Lua(lua_State* L)
{
#if CPP_TESTS_ENABLED
    string_t scid = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 1, "00000000-0000-0000-0000-000076029b4d").c_str());
    string_t leaderboardName = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 2, "TotalPuzzlesSolvedLB").c_str());
    string_t xboxUserId = xbox::services::Utils::StringTFromUint64(GetUint64FromLua(L, 3, Data()->xboxUserId));
    string_t socialGroup = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 8, "XblSocialGroupType::None").c_str());
    uint32_t maxItems = GetUint32FromLua(L, 5, 0);

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xalUser);
    xblc->leaderboard_service().get_leaderboard(
        scid,
        leaderboardName,
        xboxUserId,
        socialGroup,
        maxItems
    ).then(
        [xblc](xbox::services::xbox_live_result<xbox::services::leaderboard::leaderboard_result> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("LeaderboardServiceGetLeaderboard: hr=%s", ConvertHR(hr).c_str());

            if (SUCCEEDED(hr))
            {
                xbox::services::leaderboard::leaderboard_result leaderboardResult{ result.payload() };
                Data()->leaderboardResultCpp = leaderboardResult;

                LogToScreen("Got %d rows in leaderboard", leaderboardResult.rows().size());
                for (size_t row = 0; row < leaderboardResult.rows().size(); ++row)
                {
                    std::stringstream rowText;
                    rowText << xbox::services::Utils::Uint64FromStringT(leaderboardResult.rows()[row].xbox_user_id()) << "\t";

                    for (size_t column = 0; column < leaderboardResult.rows()[row].column_values().size(); ++column)
                    {
                        rowText << xbox::services::Utils::StringFromStringT(leaderboardResult.rows()[row].column_values()[column]) << "\t";
                    }
                    LogToFile(rowText.str().data());
                }
            }

            CallLuaFunctionWithHr(hr, "OnLeaderboardServiceGetLeaderboard");
        });
#else
    CallLuaFunctionWithHr(S_OK, "OnLeaderboardServiceGetLeaderboard");
    LogToFile("LeaderboardServiceGetLeaderboard disabled for this platform.");
#endif
    return LuaReturnHR(L, S_OK);
}

int LeaderboardResultHasNextCpp_Lua(lua_State* L)
{
    bool hasNext = false;
#if CPP_TESTS_ENABLED
    hasNext = Data()->leaderboardResultCpp.has_next();
    LogToFile("LeaderboardResultHasNextCpp: hasNext=%s", hasNext ? "true" : "false");
#else
    LogToFile("LeaderboardResultHasNextCpp disabled for this platform.");
#endif
    lua_pushnumber(L, (int)hasNext);
    return LuaReturnHR(L, S_OK, 1);
}

int LeaderboardResultGetNextCpp_Lua(lua_State* L)
{
#if CPP_TESTS_ENABLED
    uint32_t maxItems = GetUint32FromLua(L, 1, 0);
    LogToFile("LeaderboardResultGetNextCpp: maxItems: %d", maxItems);

    Data()->leaderboardResultCpp.get_next(maxItems).then(
        [](xbox::services::xbox_live_result<xbox::services::leaderboard::leaderboard_result> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("LeaderboardResultGetNextCpp: hr=%s", ConvertHR(hr).c_str());

            if (SUCCEEDED(hr))
            {
                xbox::services::leaderboard::leaderboard_result leaderboardResult{ result.payload() };
                Data()->leaderboardResultCpp = leaderboardResult;

                LogToScreen("Got %d rows in leaderboard", leaderboardResult.rows().size());
                for (size_t row = 0; row < leaderboardResult.rows().size(); ++row)
                {
                    std::stringstream rowText;
                    rowText << xbox::services::Utils::Uint64FromStringT(leaderboardResult.rows()[row].xbox_user_id()) << "\t";

                    for (size_t column = 0; column < leaderboardResult.rows()[row].column_values().size(); ++column)
                    {
                        rowText << xbox::services::Utils::StringFromStringT(leaderboardResult.rows()[row].column_values()[column]) << "\t";
                    }
                    LogToFile(rowText.str().data());
                }
            }

            CallLuaFunctionWithHr(hr, "OnLeaderboardResultGetNextCpp");
        }
    );

#else
    CallLuaFunctionWithHr(S_OK, "OnLeaderboardResultGetNextCpp");
    LogToFile("LeaderboardResultGetNextCpp disabled for this platform.");
#endif
    return LuaReturnHR(L, S_OK);
}


void SetupAPIs_CppLeaderboard()
{
    lua_register(Data()->L, "LeaderboardServiceGetLeaderboard", LeaderboardServiceGetLeaderboard_Lua);
    lua_register(Data()->L, "LeaderboardResultGetNextCpp", LeaderboardResultGetNextCpp_Lua);
    lua_register(Data()->L, "LeaderboardResultHasNextCpp", LeaderboardResultHasNextCpp_Lua);
}