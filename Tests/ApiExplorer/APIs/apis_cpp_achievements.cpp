// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"

#if CPP_TESTS_ENABLED

xbox::services::achievements::achievement_type ConvertStringToCppAchievementType(const char* str)
{
    xbox::services::achievements::achievement_type type = xbox::services::achievements::achievement_type::unknown;

    if (pal::stricmp(str, "XblAchievementType::Unknown") == 0) type = xbox::services::achievements::achievement_type::unknown;
    else if (pal::stricmp(str, "XblAchievementType::All") == 0) type = xbox::services::achievements::achievement_type::all;
    else if (pal::stricmp(str, "XblAchievementType::Persistent") == 0) type = xbox::services::achievements::achievement_type::persistent;
    else if (pal::stricmp(str, "XblAchievementType::Challenge") == 0) type = xbox::services::achievements::achievement_type::challenge;

    return type;
}

xbox::services::achievements::achievement_order_by ConvertStringToCppAchievementOrderBy(const char* str)
{
    xbox::services::achievements::achievement_order_by orderBy = xbox::services::achievements::achievement_order_by::default_order;

    if (pal::stricmp(str, "XblAchievementOrderBy::DefaultOrder") == 0) orderBy = xbox::services::achievements::achievement_order_by::default_order;
    else if (pal::stricmp(str, "XblAchievementOrderBy::TitleId") == 0) orderBy = xbox::services::achievements::achievement_order_by::title_id;
    else if (pal::stricmp(str, "XblAchievementOrderBy::UnlockTime") == 0) orderBy = xbox::services::achievements::achievement_order_by::unlock_time;

    return orderBy;
}

#endif

int AchievementsResultHasNextCpp_Lua(lua_State* L)
{
    bool hasNext = false;
#if CPP_TESTS_ENABLED
    hasNext = Data()->achievementsResultCpp.has_next();
    LogToFile("AchievementsResultHasNextCpp: hasNext=%s", hasNext ? "true" : "false");
#else
    LogToFile("AchievementsResultHasNextCpp disabled for this platform.");
#endif
    lua_pushnumber(L, (int)hasNext);
    return LuaReturnHR(L, S_OK, 1);
}

int AchievementsResultGetNextCpp_Lua(lua_State* L)
{
#if CPP_TESTS_ENABLED
    uint32_t maxItems = GetUint32FromLua(L, 1, 100);
    LogToFile("XblAchievementsResultGetNextAsync: MaxItems: %d", maxItems);

    Data()->achievementsResultCpp.get_next(maxItems).then(
        [](xbox::services::xbox_live_result<xbox::services::achievements::achievements_result> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            if (SUCCEEDED(hr))
            {
                xbox::services::achievements::achievements_result achievementsResult = result.payload();
                Data()->achievementsResultCpp = xbox::services::achievements::achievements_result(achievementsResult);

                auto achievements = achievementsResult.items();
                size_t achievementsCount = achievements.size();

                LogToFile("AchievementsServiceGetAchievementsForTitleId: Got achievementsCount: %d", achievementsCount);

                for (size_t i = 0; i < achievementsCount; i++)
                {
                    LogToScreen("Achievement %s: %s = %s",
                        xbox::services::Utils::StringFromStringT(achievements[i].id()).c_str(),
                        xbox::services::Utils::StringFromStringT(achievements[i].name()).c_str(),
                        (achievements[i].progress_state() == xbox::services::achievements::achievement_progress_state::achieved) ? "Achieved" : "Not achieved");
                }
            }

            CallLuaFunctionWithHr(hr, "OnAchievementsResultGetNextCpp");
        }
    );
#else
    CallLuaFunctionWithHr(S_OK, "OnAchievementsResultGetNextCpp");
    LogToFile("AchievementsResultGetNextCpp disabled for this platform.");
#endif

    return LuaReturnHR(L, S_OK);
}

int AchievementsServiceGetAchievementsForTitleId_Lua(lua_State* L)
{
#if CPP_TESTS_ENABLED
    xbox::services::achievements::achievement_type achievementType = ConvertStringToCppAchievementType(GetStringFromLua(L, 1, "XblAchievementType::All").c_str());
    bool unlockedOnly = GetBoolFromLua(L, 2, false);
    xbox::services::achievements::achievement_order_by orderBy = ConvertStringToCppAchievementOrderBy(GetStringFromLua(L, 3, "XblAchievementOrderBy::DefaultOrder").c_str());
    uint32_t skipItems = GetUint32FromLua(L, 4, 0);
    uint32_t maxItems = GetUint32FromLua(L, 5, 100);
    string_t xboxUserId = xbox::services::Utils::StringTFromUint64(Data()->xboxUserId);

    LogToFile("AchievementsServiceGetAchievementsForTitleId: AchievementType: %d", achievementType);
    LogToFile("AchievementsServiceGetAchievementsForTitleId: unlockedOnly: %s", unlockedOnly ? "true" : "false");
    LogToFile("AchievementsServiceGetAchievementsForTitleId: OrderBy: %d", orderBy);
    LogToFile("AchievementsServiceGetAchievementsForTitleId: SkipItems: %d", skipItems);
    LogToFile("AchievementsServiceGetAchievementsForTitleId: MaxItems: %d", maxItems);

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xalUser);
    xblc->achievement_service().get_achievements_for_title_id(
        xboxUserId,
        Data()->titleId,
        achievementType,
        unlockedOnly,
        orderBy,
        skipItems,
        maxItems
    ).then([](xbox::services::xbox_live_result < xbox::services::achievements::achievements_result> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("AchievementsServiceGetAchievementsForTitleId: hr=%s", ConvertHR(hr).c_str());

            if (SUCCEEDED(hr))
            {
                xbox::services::achievements::achievements_result achievementsResult = result.payload();
                Data()->achievementsResultCpp = xbox::services::achievements::achievements_result(achievementsResult);

                auto achievements = achievementsResult.items();
                size_t achievementsCount = achievements.size();

                LogToFile("AchievementsServiceGetAchievementsForTitleId: Got achievementsCount: %d", achievementsCount);

                for (size_t i = 0; i < achievementsCount; i++)
                {
                    LogToScreen("Achievement %s: %s = %s",
                        xbox::services::Utils::StringFromStringT(achievements[i].id()).c_str(),
                        xbox::services::Utils::StringFromStringT(achievements[i].name()).c_str(),
                        (achievements[i].progress_state() == xbox::services::achievements::achievement_progress_state::achieved) ? "Achieved" : "Not achieved");
                }
            }

            CallLuaFunctionWithHr(hr, "OnAchievementsServiceGetAchievementsForTitleId");
        });
#else
    CallLuaFunctionWithHr(S_OK, "OnAchievementsServiceGetAchievementsForTitleId");
    LogToFile("AchievementsServiceGetAchievementsForTitleId disabled for this platform.");
#endif
    return LuaReturnHR(L, S_OK);
}

int AchievementsServiceGetAchievement_Lua(lua_State* L)
{
#if CPP_TESTS_ENABLED
    auto achievementId = GetStringFromLua(L, 1, "1");
    LogToFile("XblAchievementsGetAchievementAsync: AchievementId: %s", achievementId.c_str());
    string_t xboxUserId = xbox::services::Utils::StringTFromUint64(Data()->xboxUserId);
    string_t serviceConfigId = xbox::services::Utils::StringTFromUtf8(Data()->scid);
    string_t achievementIdString = xbox::services::Utils::StringTFromUtf8(achievementId.c_str());

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xalUser);
    xblc->achievement_service().get_achievement(
        xboxUserId,
        serviceConfigId,
        achievementIdString
    ).then(
        [](xbox::services::xbox_live_result<xbox::services::achievements::achievement> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("AchievementsServiceGetAchievement: hr=%s", ConvertHR(hr).c_str());

            if (SUCCEEDED(hr))
            {
                xbox::services::achievements::achievement achievement = result.payload();

                LogToScreen("Achievement %s: %s = %s",
                    xbox::services::Utils::StringFromStringT(achievement.id()).c_str(),
                    xbox::services::Utils::StringFromStringT(achievement.name()).c_str(),
                    (achievement.progress_state() == xbox::services::achievements::achievement_progress_state::achieved) ? "Achieved" : "Not achieved");
            }

            CallLuaFunctionWithHr(hr, "OnAchievementsServiceGetAchievement");
        });
#else
    CallLuaFunctionWithHr(S_OK, "OnAchievemementsServiceGetAchievement");
    LogToFile("AchievemementsServiceGetAchievement disabled for this platform.");
#endif

    return LuaReturnHR(L, S_OK);

}

int AchievementsServiceUpdateAchievement_Lua(lua_State *L)
{
#if CPP_TESTS_ENABLED
    auto achievementId = GetStringFromLua(L, 1, "1");
    uint32_t percentComplete = GetUint32FromLua(L, 2, 100);
    LogToFile("XblAchievementsUpdateAchievementAsync: AchievementId: %s", achievementId.c_str());
    LogToFile("XblAchievementsUpdateAchievementAsync: PercentComplete: %d", percentComplete);
    string_t xboxUserId = xbox::services::Utils::StringTFromUint64(Data()->xboxUserId);
    string_t achievementIdString = xbox::services::Utils::StringTFromUtf8(achievementId.c_str());

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xalUser);
    xblc->achievement_service().update_achievement(
        xboxUserId,
        achievementIdString,
        percentComplete
    ).then(
        [](xbox::services::xbox_live_result <void> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("AchievementsServiceUpdateAchievement: hr=%s", ConvertHR(hr).c_str());
            if (SUCCEEDED(hr))
            {
                //Achievement Updated
            }
            else if (hr == HTTP_E_STATUS_NOT_MODIFIED)
            {
                //Achievement Not Modified
            }
            else
            {
                //Achievement Failed to Update
            }

            CallLuaFunctionWithHr(hr, "OnAchievementsServiceUpdateAchievement");
        });
#else
    CallLuaFunctionWithHr(S_OK, "OnAchievementsServiceUpdateAchievement");
    LogToFile("AchievementsServiceUpdateAchievement disabled for this platform.");
#endif

    return LuaReturnHR(L, S_OK);
}

int AchievementsServiceUpdateAchievementForTitleId_Lua(lua_State* L)
{
#if CPP_TESTS_ENABLED
    auto achievementId = GetStringFromLua(L, 1, "1");
    uint32_t percentComplete = GetUint32FromLua(L, 2, 100);
    LogToFile("XblAchievementsUpdateAchievementAsync: AchievementId: %s", achievementId.c_str());
    LogToFile("XblAchievementsUpdateAchievementAsync: PercentComplete: %d", percentComplete);
    string_t xboxUserId = xbox::services::Utils::StringTFromUint64(Data()->xboxUserId);
    string_t serviceConfigId = xbox::services::Utils::StringTFromUtf8(Data()->scid);
    string_t achievementIdString = xbox::services::Utils::StringTFromUtf8(achievementId.c_str());

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xalUser);
    xblc->achievement_service().update_achievement(
        xboxUserId,
        Data()->titleId,
        serviceConfigId,
        achievementIdString,
        percentComplete
    ).then(
        [](xbox::services::xbox_live_result<void> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("AchievementsServiceUpdateAchievementForTitleId: hr=%s", ConvertHR(hr).c_str());
            if (SUCCEEDED(hr))
            {
                //Achievement Updated
            }
            else if (hr == HTTP_E_STATUS_NOT_MODIFIED)
            {
                //Achievement Not Modified
            }
            else
            {
                //Achievement Failed to Update
            }

            CallLuaFunctionWithHr(hr, "OnAchievementServiceUpdateAchievementForTitleId");
        });
#else
    CallLuaFunctionWithHr(S_OK, "OnAchievementServiceUpdateAchievementForTitleId");
    LogToFile("AchievementServiceUpdateAchievementForTitleId disabled for this platform.");
#endif

    return LuaReturnHR(L, S_OK);
}

void SetupAPIs_CppAchievements()
{
    lua_register(Data()->L, "AchievementsResultGetNextCpp", AchievementsResultGetNextCpp_Lua);
    lua_register(Data()->L, "AchievementsResultHasNextCpp", AchievementsResultHasNextCpp_Lua);
    lua_register(Data()->L, "AchievementsServiceGetAchievement", AchievementsServiceGetAchievement_Lua);
    lua_register(Data()->L, "AchievementsServiceGetAchievementsForTitleId", AchievementsServiceGetAchievementsForTitleId_Lua);
    lua_register(Data()->L, "AchievementsServiceUpdateAchievement", AchievementsServiceUpdateAchievement_Lua);
    lua_register(Data()->L, "AchievementsServiceUpdateAchievementForTitleId", AchievementsServiceUpdateAchievementForTitleId_Lua);
}
