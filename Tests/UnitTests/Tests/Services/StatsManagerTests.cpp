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
        DEFINE_TEST_CASE_PROPERTIES(StatisticManagerAddLocalUser);
        auto statsManager = StatisticManager::SingletonInstance;
        auto mockXblContext = GetMockXboxLiveContext_WinRT();
        auto user = mockXblContext->User;
        InitializeStatsManager(statsManager, user);

        auto stat = statsManager->GetStatistic(user, L"headshots");
        VERIFY_IS_TRUE(stat->DataType == StatisticDataType::Number);
        VERIFY_IS_TRUE(stat->AsNumber == 7);
        VERIFY_IS_TRUE(stat->CompareType == StatisticCompareType::Always);

        //statsManager->SetStatisticNumberData(user, )
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END