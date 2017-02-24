// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#define TEST_CLASS_OWNER L"blgross"
#define TEST_CLASS_AREA L"SimplfiedStatService"
#include "UnitTestIncludes.h"
#include "xsapi/stats_manager.h"
#include "xbox_live_context_impl.h"
#include "StatisticManager_WinRT.h"
#include "StatsManagerHelper.h"

using namespace Microsoft::Xbox::Services::Statistics::Manager;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

DEFINE_TEST_CLASS(StatsManagerTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(StatsManagerTests)

    void InitializeStatsManager(StatisticManager^ statsManager, XboxLiveUser_t user)
    {
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(web::json::value::parse(statValueDocumentResponse));
        statsManager->AddLocalUser(user);
        bool isDone = false;

        while (!isDone)
        {
            auto eventList = statsManager->DoWork();
            for (auto evt : eventList)
            {
                if (evt->EventType == StatisticEventType::LocalUserAdded)
                {
                    isDone = true;
                    break;
                }
            }
        }
    }

    void Cleanup(StatisticManager^ statsManager, XboxLiveUser_t user)
    {
        statsManager->RemoveLocalUser(user);
        bool isDone = false;

        while (!isDone)
        {
            auto eventList = statsManager->DoWork();
            for (auto evt : eventList)
            {
                if (evt->EventType == StatisticEventType::LocalUserRemoved)
                {
                    isDone = true;
                    break;
                }
            }
        }
    }

    DEFINE_TEST_CASE(StatisticManagerAddLocalUser)
    {
        DEFINE_TEST_CASE_PROPERTIES(StatisticManagerAddLocalUser);
        auto statsManager = StatisticManager::SingletonInstance;
        auto mockXblContext = GetMockXboxLiveContext_WinRT();
        auto user = mockXblContext->User;
        InitializeStatsManager(statsManager, user);
        
        auto statNames = statsManager->GetStatisticNames(user);
        VERIFY_IS_TRUE(statNames->Size == 4);

        auto stat = statsManager->GetStatistic(user, L"headshots");
        VERIFY_IS_TRUE(stat->DataType == StatisticDataType::Number);
        VERIFY_IS_TRUE(stat->AsNumber == 7);

        stat = statsManager->GetStatistic(user, L"fastestRound");
        VERIFY_IS_TRUE(stat->DataType == StatisticDataType::Number);
        VERIFY_IS_TRUE(stat->AsNumber == 7);

        stat = statsManager->GetStatistic(user, L"longestJump");
        VERIFY_IS_TRUE(stat->DataType == StatisticDataType::Number);
        VERIFY_IS_TRUE(stat->AsNumber == 9.5);

        stat = statsManager->GetStatistic(user, L"strangeStat");
        VERIFY_IS_TRUE(stat->DataType == StatisticDataType::String);
        VERIFY_ARE_EQUAL_STR(stat->AsString, L"foo");

        Cleanup(statsManager, user);
    }

    DEFINE_TEST_CASE(StatisticManagerSetStat)
    {
        DEFINE_TEST_CASE_PROPERTIES(StatisticManagerSetStat);
        auto statsManager = StatisticManager::SingletonInstance;
        auto mockXblContext = GetMockXboxLiveContext_WinRT();
        auto user = mockXblContext->User;
        InitializeStatsManager(statsManager, user);

        Platform::String^ statName = L"headshots";
        auto numericStat = statsManager->GetStatistic(user, statName);
        VERIFY_IS_TRUE(numericStat->DataType == StatisticDataType::Number);
        VERIFY_IS_TRUE(numericStat->AsNumber == 7);

        statsManager->SetStatisticNumberData(user, statName, 20.f);
        numericStat = statsManager->GetStatistic(user, statName);
        VERIFY_IS_TRUE(numericStat->DataType == StatisticDataType::Number);
        VERIFY_IS_TRUE(numericStat->AsNumber == 7);

        statsManager->SetStatisticStringData(user, L"hello", L"goodbye");
        VERIFY_THROWS_HR_CX(statsManager->GetStatistic(user, L"hello"), E_INVALIDARG);

        statsManager->DoWork();

        numericStat = statsManager->GetStatistic(user, statName);
        VERIFY_IS_TRUE(numericStat->DataType == StatisticDataType::Number);
        VERIFY_IS_TRUE(numericStat->AsNumber == 20.f);

        auto stringStat = statsManager->GetStatistic(user, L"hello");
        VERIFY_IS_TRUE(stringStat->DataType == StatisticDataType::String);
        VERIFY_IS_TRUE(stringStat->AsString == L"goodbye");

        Cleanup(statsManager, user);
    }

    DEFINE_TEST_CASE(StatisticManagerRequestFlushToService)
    {
        DEFINE_TEST_CASE_PROPERTIES(StatisticManagerRequestFlushToService);
        auto statsManager = StatisticManager::SingletonInstance;
        auto mockXblContext = GetMockXboxLiveContext_WinRT();
        auto user = mockXblContext->User;
        InitializeStatsManager(statsManager, user);
        auto fastestRoundStat = statsManager->GetStatistic(user, L"fastestRound");
        VERIFY_IS_TRUE(fastestRoundStat->AsInteger == 7);
        VERIFY_IS_TRUE(fastestRoundStat->DataType == StatisticDataType::Number);

        statsManager->SetStatisticIntegerData(user, L"fastestRound", 3);
        statsManager->RequestFlushToService(user);
        auto eventList = statsManager->DoWork();
        Sleep(500); // fix
        fastestRoundStat = statsManager->GetStatistic(user, L"fastestRound");
        VERIFY_IS_TRUE(fastestRoundStat->AsInteger == 3);
        VERIFY_IS_TRUE(fastestRoundStat->DataType == StatisticDataType::Number);

        Cleanup(statsManager, user);
    }

    DEFINE_TEST_CASE(StatisticManagerDeleteStat)
    {
        DEFINE_TEST_CASE_PROPERTIES(StatisticManagerRequestFlushToService);
        auto statsManager = StatisticManager::SingletonInstance;
        auto mockXblContext = GetMockXboxLiveContext_WinRT();
        auto user = mockXblContext->User;
        InitializeStatsManager(statsManager, user);
        statsManager->DeleteStatistic(user, L"fastestRound");
        auto eventList = statsManager->DoWork();
        VERIFY_THROWS_HR_CX(statsManager->GetStatistic(user, L"fastestRound"), E_INVALIDARG);

        Cleanup(statsManager, user);
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END