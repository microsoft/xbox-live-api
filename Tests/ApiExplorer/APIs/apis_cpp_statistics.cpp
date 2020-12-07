// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"

#pragma warning(disable:4996)

int UserStatisticsServiceGetSingleUserStatistic_Lua(lua_State* L)
{
#if CPP_TESTS_ENABLED
    string_t xboxUserId = xbox::services::Utils::StringTFromUint64(Data()->xboxUserId);
    string_t serviceConfigurationId = xbox::services::Utils::StringTFromUtf8(Data()->scid);
    string_t statisticName = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 1, "TotalPuzzlesSolved").c_str());

    LogToFile("UserStatisticsServiceGetSingleUserStatistic: statisticName: %s", xbox::services::Utils::StringFromStringT(statisticName).c_str());

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    xblc->user_statistics_service().get_single_user_statistics(
        xboxUserId,
        serviceConfigurationId,
        statisticName
    ).then(
        [](xbox::services::xbox_live_result<xbox::services::user_statistics::user_statistics_result> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("UserStatisticsServiceGetSingleUserStatistic: hr=%s", ConvertHR(hr).data());

            if (SUCCEEDED(hr))
            {
                xbox::services::user_statistics::user_statistics_result userStatisticsResult{ result.payload() };

                VERIFY_IS_TRUE(userStatisticsResult.service_configuration_statistics().size() == 1, "service_configuration_statistics size");
                VERIFY_IS_TRUE(userStatisticsResult.service_configuration_statistics()[0].statistics().size() == 1, "statistics size");

                if (userStatisticsResult.service_configuration_statistics().size() > 0 && userStatisticsResult.service_configuration_statistics()[0].statistics().size() > 0)
                {
                    xbox::services::user_statistics::statistic stat = userStatisticsResult.service_configuration_statistics()[0].statistics()[0];
                    //Data()->lastUserStat = stat.value();

                    LogToScreen("%s's stat %s is %s.  Note: With GDK, ensure fiddler isn't running for stat upload to work",
                        Data()->gamertag.c_str(),
                        xbox::services::Utils::StringFromStringT(stat.statistic_name()).c_str(),
                        xbox::services::Utils::StringFromStringT(stat.value()).c_str()
                    );

                }
            }

            CallLuaFunctionWithHr(hr, "OnUserStatisticsServiceGetSingleUserStatistic");
        });
#else
    LogToFile("UserStatisticsServiceGetSingleUserStatistic is disabled for this platform.");
    CallLuaFunctionWithHr(S_OK, "OnUserStatisticsServiceGetSingleUserStatistic");
#endif
    return LuaReturnHR(L, S_OK);
}

int UserStatisticsServiceGetSingleUserStatistics_Lua(lua_State *L)
{
#if CPP_TESTS_ENABLED
    string_t statisticName1 = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 1, "TotalPuzzlesSolved").c_str());
    string_t statisticName2 = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 2, "TotalRoundsStarted").c_str());
    LogToFile("UserStatisticsServiceGetSingleUserStatistics: statisticName1: %s", xbox::services::Utils::StringFromStringT(statisticName1).c_str());
    LogToFile("UserStatisticsServiceGetSingleUserStatistics: statisticName2: %s", xbox::services::Utils::StringFromStringT(statisticName2).c_str());

    string_t xboxUserId = xbox::services::Utils::StringTFromUint64(Data()->xboxUserId);
    string_t serviceConfigurationId = xbox::services::Utils::StringTFromUtf8(Data()->scid);
    std::vector<string_t> statisticNames{ statisticName1, statisticName2 };


    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    xblc->user_statistics_service().get_single_user_statistics(
        xboxUserId,
        serviceConfigurationId,
        statisticNames
    ).then(
        [](xbox::services::xbox_live_result<xbox::services::user_statistics::user_statistics_result> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("UserStatisticsServiceGetSingleUserStatistics: hr=%s", ConvertHR(hr).data());

            if (SUCCEEDED(hr))
            {
                xbox::services::user_statistics::user_statistics_result userStatisticsResult{ result.payload() };

                if (userStatisticsResult.service_configuration_statistics().size() > 0)
                {
                    std::vector<xbox::services::user_statistics::statistic> stats = userStatisticsResult.service_configuration_statistics()[0].statistics();
                    LogToFile("Got %u statistics for %s", stats.size(), Data()->gamertag.c_str());
                    for (auto stat : stats)
                    {
                        LogToScreen("stat %s is %s",
                            xbox::services::Utils::StringFromStringT(stat.statistic_name()).c_str(),
                            xbox::services::Utils::StringFromStringT(stat.value()).c_str()
                        );
                    }

                }


            }

            CallLuaFunctionWithHr(hr, "OnUserStatisticsServiceGetSingleUserStatistics");
        });
#else
    LogToFile("UserStatisticsServiceGetSingleUserStatistics is disabled for this platform.");
    CallLuaFunctionWithHr(S_OK, "OnUserStatisticsServiceGetSingleUserStatistics");
#endif
    return LuaReturnHR(L, S_OK);
}

int UserStatisticsServiceGetMultipleUserStatistics_Lua(lua_State *L)
{
#if CPP_TESTS_ENABLED
    string_t statisticName1 = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 1, "TotalPuzzlesSolved").c_str());
    string_t statisticName2 = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 2, "TotalRoundsStarted").c_str());
    LogToFile("UserStatisticsServiceGetMultipleUserStatistics: statisticName1: %s", xbox::services::Utils::StringFromStringT(statisticName1).c_str());
    LogToFile("UserStatisticsServiceGetMultipleUserStatistics: statisticName2: %s", xbox::services::Utils::StringFromStringT(statisticName2).c_str());

    string_t xuid1 = xbox::services::Utils::StringTFromUint64(GetUint64FromLua(L, 3, Data()->xboxUserId));
    string_t xuid2 = xbox::services::Utils::StringTFromUint64(GetUint64FromLua(L, 4, 2814634367189975));
    LogToFile("UserStatisticsServiceGetMultipleUserStatistics: xuid1: %ul", xbox::services::Utils::StringFromStringT(xuid1).c_str());
    LogToFile("UserStatisticsServiceGetMultipleUserStatistics: xuid2: %ul", xbox::services::Utils::StringFromStringT(xuid2).c_str());

    std::vector<string_t> xboxUserIds{ xuid1, xuid2 };
    string_t serviceConfigurationId = xbox::services::Utils::StringTFromUtf8(Data()->scid);
    std::vector<string_t> statisticNames{ statisticName1, statisticName2 };


    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    xblc->user_statistics_service().get_multiple_user_statistics(
        xboxUserIds,
        serviceConfigurationId,
        statisticNames
    ).then(
        [](xbox::services::xbox_live_result<std::vector<xbox::services::user_statistics::user_statistics_result>> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("UserStatisticsServiceGetMultipleUserStatistics: hr=%s", ConvertHR(hr).data());

            if (SUCCEEDED(hr))
            {
                std::vector<xbox::services::user_statistics::user_statistics_result> userStatisticsResults = result.payload();

                for (auto statResult : userStatisticsResults)
                {
                    LogToFile("XUID: %d", xbox::services::Utils::StringFromStringT(statResult.xbox_user_id()).c_str());
                    for (auto scidStats : statResult.service_configuration_statistics())
                    {
                        LogToFile("SCID: %s", xbox::services::Utils::StringFromStringT(scidStats.service_configuration_id()).c_str());
                        for (auto stat : scidStats.statistics())
                        {
                            LogToFile("Stat name:%s value:%s type:%s",
                                xbox::services::Utils::StringFromStringT(stat.statistic_name()).c_str(),
                                xbox::services::Utils::StringFromStringT(stat.value()).c_str(),
                                xbox::services::Utils::StringFromStringT(stat.statistic_type()).c_str()
                            );
                        }
                    }
                }
            }

            CallLuaFunctionWithHr(hr, "OnUserStatisticsServiceGetMultipleUserStatistics");
        });
#else
    LogToFile("UserStatisticsServiceGetMultipleUserStatistics is disabled for this platform");
    CallLuaFunctionWithHr(S_OK, "OnUserStatisticsServiceGetMultipleUserStatistics");
#endif
    return LuaReturnHR(L, S_OK);
}

int UserStatisticsServiceGetMultipleUserStatisticsForMultipleServiceConfigurations_Lua(lua_State *L)
{
#if CPP_TESTS_ENABLED
    string_t statisticName1 = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 1, "TotalPuzzlesSolved").c_str());
    string_t statisticName2 = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 2, "TotalRoundsStarted").c_str());
    LogToFile("UserStatisticsServiceGetMultipleUserStatisticsForMultipleServiceConfigurations: statisticName1: %s", xbox::services::Utils::StringFromStringT(statisticName1).c_str());
    LogToFile("UserStatisticsServiceGetMultipleUserStatisticsForMultipleServiceConfigurations: statisticName2: %s", xbox::services::Utils::StringFromStringT(statisticName2).c_str());

    std::vector<string_t> statisticNames{ statisticName1, statisticName2 };
    string_t serviceConfigurationId = xbox::services::Utils::StringTFromUtf8(Data()->scid);

    xbox::services::user_statistics::requested_statistics requestedStatistic1{ serviceConfigurationId, statisticNames };

    string_t xuid1 = xbox::services::Utils::StringTFromUint64(GetUint64FromLua(L, 3, Data()->xboxUserId));
    string_t xuid2 = xbox::services::Utils::StringTFromUint64(GetUint64FromLua(L, 4, 2814634367189975));
    LogToFile("UserStatisticsServiceGetMultipleUserStatisticsForMultipleServiceConfigurations: xuid1: %ul", xbox::services::Utils::StringFromStringT(xuid1).c_str());
    LogToFile("UserStatisticsServiceGetMultipleUserStatisticsForMultipleServiceConfigurations: xuid2: %ul", xbox::services::Utils::StringFromStringT(xuid2).c_str());

    std::vector<string_t> xboxUserIds{ xuid1, xuid2 };
    std::vector<xbox::services::user_statistics::requested_statistics> requestedStatistics{ requestedStatistic1 };

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    xblc->user_statistics_service().get_multiple_user_statistics_for_multiple_service_configurations(
        xboxUserIds,
        requestedStatistics
    ).then(
        [](xbox::services::xbox_live_result<std::vector<xbox::services::user_statistics::user_statistics_result>> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("UserStatisticsServiceGetMultipleUserStatisticsForMultipleServiceConfigurations: hr=%s", ConvertHR(hr).data());

            if (SUCCEEDED(hr))
            {
                std::vector<xbox::services::user_statistics::user_statistics_result> userStatisticsResults = result.payload();

                for (auto statResult : userStatisticsResults)
                {
                    LogToFile("Xbox User ID: %d", xbox::services::Utils::StringFromStringT(statResult.xbox_user_id()).c_str());
                    for (auto scidStats : statResult.service_configuration_statistics())
                    {
                        LogToFile("SCID: %s", xbox::services::Utils::StringFromStringT(scidStats.service_configuration_id()).c_str());
                        for (auto stat : scidStats.statistics())
                        {
                            LogToFile("Stat name:%s value:%s type:%s",
                                xbox::services::Utils::StringFromStringT(stat.statistic_name()).c_str(),
                                xbox::services::Utils::StringFromStringT(stat.value()).c_str(),
                                xbox::services::Utils::StringFromStringT(stat.statistic_type()).c_str()
                            );
                        }
                    }
                }
            }

            CallLuaFunctionWithHr(hr, "OnUserStatisticsServiceGetMultipleUserStatisticsForMultipleServiceConfigurations");
        });
#else
    LogToFile("UserStatisticsServiceGetMultipleUserStatisticsForMultipleServiceConfigurations is disabled for this platform");
    CallLuaFunctionWithHr(S_OK, "OnUserStatisticsServiceGetMultipleUserStatisticsForMultipleServiceConfigurations");
#endif
    return LuaReturnHR(L, S_OK);
}

int UserStatisticsServiceSubscribeToStatisticChange_Lua(lua_State *L)
{
#if CPP_TESTS_ENABLED
    if (Data()->statisticChangeSubscriptionCpp != nullptr)
    {
        return LuaReturnHR(L, E_FAIL);
    }

    string_t xboxUserId = xbox::services::Utils::StringTFromUint64(Data()->xboxUserId);
    string_t serviceConfigurationId = xbox::services::Utils::StringTFromUtf8(Data()->scid);
    string_t statisticName = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 1, "TotalPuzzlesSolved").c_str());
    
    LogToFile("UserStatisticsServiceSubscribeToStatisticChange: statisticName: %s", xbox::services::Utils::StringFromStringT(statisticName).c_str());

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    auto result = xblc->user_statistics_service().subscribe_to_statistic_change(xboxUserId, serviceConfigurationId, statisticName);

    HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
    if (SUCCEEDED(hr))
    {
        Data()->statisticChangeSubscriptionCpp = result.payload();
    }
    LogToFile("UserStatisticsServiceSubscribeToStatisticChange: hr=%s", ConvertHR(hr).data());
    return LuaReturnHR(L, hr);
#else
    LogToFile("UserStatisticsServiceSubscribeToStatisticChange is disabled for this platform.");
    return LuaReturnHR(L, S_OK);
#endif
}

int UserStatisticsServiceUnsubscribeFromStatisticChange_Lua(lua_State *L)
{
#if CPP_TESTS_ENABLED
    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    auto result = xblc->user_statistics_service().unsubscribe_from_statistic_change(Data()->statisticChangeSubscriptionCpp);

    HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
    if (SUCCEEDED(hr))
    {
        Data()->statisticChangeSubscriptionCpp = nullptr;
    }
    LogToFile("UserStatisticsServiceUnsubscribeFromStatisticChange: hr=%s", ConvertHR(hr).data());
    return LuaReturnHR(L, hr);
#else
    LogToFile("UserStatisticsServiceUnsubscribeFromStatisticChange is disabled for this platform");
    return LuaReturnHR(L, S_OK);
#endif
}

int UserStatisticsServiceAddStatisticChangedHandler_Lua(lua_State *L)
{
#if CPP_TESTS_ENABLED
    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    Data()->statisticChangedFunctionContextCpp = xblc->user_statistics_service().add_statistic_changed_handler(
        [](xbox::services::user_statistics::statistic_change_event_args args) 
        {
            // Handle stat change 
            LogToScreen("Statistic changed callback: stat changed (%s = %s)",
                xbox::services::Utils::StringFromStringT(args.latest_statistic().statistic_name()).c_str(),
                xbox::services::Utils::StringFromStringT(args.latest_statistic().value()).c_str());
            CallLuaFunctionWithHr(S_OK, "OnStatisticChangedHandlerCpp");
        });

    LogToFile("UserStatisticsServiceAddStatisticChangedHandler");
#else
    LogToFile("UserStatisticsServiceAddStatisticChangedHandler is disabled on this platform.");
#endif
    return LuaReturnHR(L, S_OK);
}

int UserStatisticsServiceRemoveStatisticChangedHandler_Lua(lua_State *L)
{
#if CPP_TESTS_ENABLED
    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    xblc->user_statistics_service().remove_statistic_changed_handler(Data()->statisticChangedFunctionContextCpp);
    Data()->statisticChangedFunctionContextCpp = nullptr;
    LogToFile("UserStatisticsServiceRemoveStatisticChangedHandler");
#else
    LogToFile("UserStatisticsServiceRemoveStatisticChangedHandler is disabled for this platform.");
#endif
    return LuaReturnHR(L, S_OK);
}

int StatisticChangeSubscriptionGetStateCpp_Lua(lua_State* L)
{
#if CPP_TESTS_ENABLED
    if (Data()->statisticChangeSubscriptionCpp)
    {
        xbox::services::real_time_activity::real_time_activity_subscription_state subscriptionState = Data()->statisticChangeSubscriptionCpp->state();
        LogToFile("StatisticChangeSubscriptionGetStateCpp: subscriptionState=%u", subscriptionState);
    }
    else
    {
        LogToFile("StatisticChangeSubscriptionGetStateCpp: No subscription found");
    }
#else
    LogToFile("StatisticChangeSubscriptionGetStateCpp is disabled for this platform.");
#endif
    return LuaReturnHR(L, S_OK);
}

int StatisticChangeSubscriptionGetIdCpp_Lua(lua_State* L)
{
#if CPP_TESTS_ENABLED
    if (Data()->statisticChangeSubscriptionCpp)
    {
        uint32_t subscriptionId = Data()->statisticChangeSubscriptionCpp->subscription_id();
        LogToFile("StatisticChangeSubscriptionGetStateCpp: subscriptionState=%u", subscriptionId);
    }
    else
    {
        LogToFile("StatisticChangeSubscriptionGetIdCpp: No subscription found");
    }
#else
    LogToFile("StatisticChangeSubscriptionGetIdCpp is disabled for this platform.");
#endif
    return LuaReturnHR(L, S_OK);
}

void SetupAPIs_CppStatistics()
{
    lua_register(Data()->L, "UserStatisticsServiceGetSingleUserStatistic", UserStatisticsServiceGetSingleUserStatistic_Lua);
    lua_register(Data()->L, "UserStatisticsServiceGetSingleUserStatistics", UserStatisticsServiceGetSingleUserStatistics_Lua);
    lua_register(Data()->L, "UserStatisticsServiceGetMultipleUserStatistics", UserStatisticsServiceGetMultipleUserStatistics_Lua);
    lua_register(Data()->L, "UserStatisticsServiceGetMultipleUserStatisticsForMultipleServiceConfigurations", UserStatisticsServiceGetMultipleUserStatisticsForMultipleServiceConfigurations_Lua);
    lua_register(Data()->L, "UserStatisticsServiceSubscribeToStatisticChange", UserStatisticsServiceSubscribeToStatisticChange_Lua);
    lua_register(Data()->L, "UserStatisticsServiceUnsubscribeFromStatisticChange", UserStatisticsServiceUnsubscribeFromStatisticChange_Lua);
    lua_register(Data()->L, "UserStatisticsServiceAddStatisticChangedHandler", UserStatisticsServiceAddStatisticChangedHandler_Lua);
    lua_register(Data()->L, "UserStatisticsServiceRemoveStatisticChangedHandler", UserStatisticsServiceRemoveStatisticChangedHandler_Lua);

    lua_register(Data()->L, "StatisticChangeSubscriptionGetStateCpp", StatisticChangeSubscriptionGetStateCpp_Lua);
    lua_register(Data()->L, "StatisticChangeSubscriptionGetIdCpp", StatisticChangeSubscriptionGetIdCpp_Lua);
}