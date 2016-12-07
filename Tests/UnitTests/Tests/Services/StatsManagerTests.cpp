//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#define TEST_CLASS_OWNER L"blgross"
#define TEST_CLASS_AREA L"SimplfiedStatService"
#include "UnitTestIncludes.h"
#include "xsapi/simple_stats.h"
#include "xbox_live_context_impl.h"
#include "StatisticManager_WinRT.h"
#include "StatsManagerHelper.h"

using namespace Microsoft::Xbox::Services::Experimental::Statistics::Manager;

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
        VERIFY_IS_TRUE(stat->CompareType == StatisticCompareType::Always);

        stat = statsManager->GetStatistic(user, L"fastestRound");
        VERIFY_IS_TRUE(stat->DataType == StatisticDataType::Number);
        VERIFY_IS_TRUE(stat->AsNumber == 7);
        VERIFY_IS_TRUE(stat->CompareType == StatisticCompareType::Min);

        stat = statsManager->GetStatistic(user, L"longestJump");
        VERIFY_IS_TRUE(stat->DataType == StatisticDataType::Number);
        VERIFY_IS_TRUE(stat->AsNumber == 9.5);
        VERIFY_IS_TRUE(stat->CompareType == StatisticCompareType::Max);

        stat = statsManager->GetStatistic(user, L"strangeStat");
        VERIFY_IS_TRUE(stat->DataType == StatisticDataType::String);
        VERIFY_ARE_EQUAL_STR(stat->AsString, L"foo");
        VERIFY_IS_TRUE(stat->CompareType == StatisticCompareType::Always);

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
        VERIFY_IS_TRUE(numericStat->CompareType == StatisticCompareType::Always);

        statsManager->SetStatisticNumberData(user, statName, 20.f);
        numericStat = statsManager->GetStatistic(user, statName);
        VERIFY_IS_TRUE(numericStat->DataType == StatisticDataType::Number);
        VERIFY_IS_TRUE(numericStat->AsNumber == 7);
        VERIFY_IS_TRUE(numericStat->CompareType == StatisticCompareType::Always);

        statsManager->SetStatisticStringData(user, L"hello", L"goodbye");
        VERIFY_THROWS_HR_CX(statsManager->GetStatistic(user, L"hello"), E_INVALIDARG);

        statsManager->DoWork();

        VERIFY_IS_TRUE(numericStat->DataType == StatisticDataType::Number);
        VERIFY_IS_TRUE(numericStat->AsNumber == 20.f);
        VERIFY_IS_TRUE(numericStat->CompareType == StatisticCompareType::Always);

        auto stringStat = statsManager->GetStatistic(user, L"hello");
        VERIFY_IS_TRUE(stringStat->DataType == StatisticDataType::String);
        VERIFY_IS_TRUE(stringStat->AsString == L"goodbye");
        VERIFY_IS_TRUE(stringStat->CompareType == StatisticCompareType::Always);

        Platform::Collections::Vector<StatisticContext^>^ vec = ref new Platform::Collections::Vector<StatisticContext ^>();
        vec->Append(ref new StatisticContext(L"what", L"up"));
        statsManager->SetStatisticContexts(user, vec->GetView());

        auto statContexts = statsManager->GetStatisticContexts(user);
        VERIFY_IS_TRUE(statContexts->Size == 0);
        statsManager->DoWork();
        statContexts = statsManager->GetStatisticContexts(user);
        VERIFY_IS_TRUE(statContexts->Size == 1);
        auto statContext = statContexts->GetAt(0);
        VERIFY_IS_TRUE(statContext->Name == L"what");
        VERIFY_IS_TRUE(statContext->Value == L"up");

        statsManager->ClearStatisticContexts(user);
        VERIFY_IS_TRUE(statContexts->Size == 1);
        statsManager->DoWork();
        statContexts = statsManager->GetStatisticContexts(user);
        VERIFY_IS_TRUE(statContexts->Size == 0);

        VERIFY_IS_TRUE(numericStat->DataType == StatisticDataType::Number);
        VERIFY_IS_TRUE(numericStat->AsNumber == 20.f);
        VERIFY_IS_TRUE(numericStat->CompareType == StatisticCompareType::Always);

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
        VERIFY_IS_TRUE(fastestRoundStat->CompareType == StatisticCompareType::Min);

        statsManager->SetStatisticIntegerData(user, L"fastestRound", 3);
        statsManager->RequestFlushToService(user);
        VERIFY_IS_TRUE(fastestRoundStat->AsInteger == 3);
        VERIFY_IS_TRUE(fastestRoundStat->DataType == StatisticDataType::Number);
        VERIFY_IS_TRUE(fastestRoundStat->CompareType == StatisticCompareType::Min);

        Cleanup(statsManager, user);
    }

    DEFINE_TEST_CASE(StatisticManagerStatisticCompareTypes)
    {
        DEFINE_TEST_CASE_PROPERTIES(StatisticManagerStatisticCompareTypes);
        auto statsManager = StatisticManager::SingletonInstance;
        auto mockXblContext = GetMockXboxLiveContext_WinRT();
        auto user = mockXblContext->User;
        InitializeStatsManager(statsManager, user);
        auto fastestRoundStat = statsManager->GetStatistic(user, L"fastestRound");
        VERIFY_IS_TRUE(fastestRoundStat->AsInteger == 7);
        VERIFY_IS_TRUE(fastestRoundStat->DataType == StatisticDataType::Number);
        VERIFY_IS_TRUE(fastestRoundStat->CompareType == StatisticCompareType::Min);

        statsManager->SetStatisticIntegerData(user, L"fastestRound", 16);
        statsManager->RequestFlushToService(user);
        VERIFY_IS_TRUE(fastestRoundStat->AsInteger == 7);
        VERIFY_IS_TRUE(fastestRoundStat->DataType == StatisticDataType::Number);
        VERIFY_IS_TRUE(fastestRoundStat->CompareType == StatisticCompareType::Min);

        //statsManager->
        Cleanup(statsManager, user);
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END