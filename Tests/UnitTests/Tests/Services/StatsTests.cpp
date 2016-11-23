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
#define TEST_CLASS_OWNER L"jicailiu"
#define TEST_CLASS_AREA L"Stats"
#include "UnitTestIncludes.h"

#include "Utils_WinRT.h"
#include "RtaTestHelper.h"

using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::System;
using namespace Microsoft::Xbox::Services::UserStatistics;
using namespace Platform;
using namespace Platform::Collections;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

#define DEFAULT_STAT_NAME L"Stat"
#define DEFAULT_STAT(name, value) \
L"{ \
    \"name\": \"" name "\", \
    \"type\": \"Integer\", \
    \"value\": \"" value "\" \
}"

const std::wstring defaultRTAStat = DEFAULT_STAT(DEFAULT_STAT_NAME, "31");

const std::wstring updatedRTAStat = DEFAULT_STAT(DEFAULT_STAT_NAME, "32");

const std::wstring defaultSingleUserStatsResponse = 
LR"(
{
    "xuid":"2533274792693551",
    "scids":
    [
        {
            "scid":"7492baca-c1b4-440d-a391-b7ef364a8d40",
            "stats":
            [
                {
                    "statname":"OverallReputation",
                    "type":"Integer",
                    "value":"66"
                },
                {
                    "statname":"FairplayReputation",
                    "type":"Integer",
                    "value":"72"
                }
            ]
        },
        {
            "scid":"7492baca-c1b4-440d-a391-b7ef364a8d41",
            "stats":
            [
                {
                    "statname":"CommsReputation",
                    "type":"Integer",
                    "value":"66"
                },
                {
                    "statname":"UserContentReputation",
                    "type":"Integer",
                    "value":"75"
                }
            ]
        }
    ]
}
)";

const std::wstring defaultBatchUsersStatsResponse =
LR"(
{
    "users":
    [
        {
            "xuid":"2533274792693551",
            "scids":
            [
                {
                    "scid":"7492baca-c1b4-440d-a391-b7ef364a8d40",
                    "stats":
                    [
                        {
                            "statname":"OverallReputation",
                            "type":"Integer",
                            "value":"66"
                        },
                        {
                            "statname":"FairplayReputation",
                            "type":"Integer",
                            "value":"72"
                        }
                    ]
                }
            ]
        },
        {
            "xuid":"2533274792693552",
            "scids":
            [
                {
                    "scid":"7492baca-c1b4-440d-a391-b7ef364a8d40",
                    "stats":
                    [
                        {
                            "statname":"OverallReputation",
                            "type":"Integer",
                            "value":"66"
                        },
                        {
                            "statname":"FairplayReputation",
                            "type":"Integer",
                            "value":"72"
                        }
                    ]
                }
            ]
        }
    ]
}
)";

DEFINE_TEST_CLASS(UserStatsTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(UserStatsTests);

    void VerifyRTAStat(Statistic^ stat, web::json::value statToVerify)
    {
        VERIFY_ARE_EQUAL(stat->StatisticName->Data(), statToVerify[L"name"].as_string());

        string_t statValue = statToVerify[L"value"].as_string();
        VERIFY_ARE_EQUAL(stat->Value->Data(), statValue);
        if (stat->StatisticType == PropertyType::Int64)
        {
            VERIFY_ARE_EQUAL(statToVerify[L"type"].as_string(), L"Integer");
        }
        else
        {
            VERIFY_ARE_EQUAL(statToVerify[L"type"].as_string(), stat->StatisticType.ToString()->Data());
        }
    }

    void VerifyStats(Statistic^ stat, web::json::value statToVerify, bool isSingleUser)
    {
        VERIFY_ARE_EQUAL(stat->StatisticName->Data(), statToVerify[L"statname"].as_string());
        VERIFY_ARE_EQUAL(stat->Value->Data(), statToVerify[L"value"].as_string());
        if (stat->StatisticType == PropertyType::Int64)
        {
            VERIFY_ARE_EQUAL(statToVerify[L"type"].as_string(), L"Integer");
        }
        else
        {
            VERIFY_ARE_EQUAL(statToVerify[L"type"].as_string(), stat->StatisticType.ToString()->Data());
        }
    }

    void VerifyServiceConfigurationStatistic(ServiceConfigurationStatistic^ scid, web::json::value scidToVerify, bool isSingleUser)
    {
        if (!isSingleUser)
        {
            VERIFY_ARE_EQUAL(scid->ServiceConfigurationId->Data(), scidToVerify[L"scid"].as_string());
        }

        auto stats = scid->Statistics;
        auto statsJson = scidToVerify[L"stats"].as_array();
        VERIFY_ARE_EQUAL_INT(stats->Size, statsJson.size());

        // scids/*/stats/*
        int i = 0;
        for (auto stat : stats)
        {
            VerifyStats(stat, statsJson[i++], isSingleUser);
        }
    }

    void VerifyUserStatisticsResult(UserStatisticsResult^ result, web::json::value resultToVerify, bool isSingleUser)
    {
        VERIFY_ARE_EQUAL(result->XboxUserId->Data(), resultToVerify[L"xuid"].as_string());

        // scids
        auto scids = result->ServiceConfigurationStatistics;
        auto scidsJson = resultToVerify[L"scids"].as_array();
        VERIFY_ARE_EQUAL_INT(scids->Size, scidsJson.size());

        int i = 0;
        for (auto scid : scids)
        {
            VerifyServiceConfigurationStatistic(scid, scidsJson[i++], isSingleUser);
        }
    }

    DEFINE_TEST_CASE(TestGetSingleUserStatistics1)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetSingleUserStatistics1);
        auto responseJson = web::json::value::parse(defaultSingleUserStatsResponse);

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto result = create_task(xboxLiveContext->UserStatisticsService->GetSingleUserStatisticsAsync(
            "xboxUserId",
            "serviceConfigurationId",
            "statisticName"
            )).get();
        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://userstats.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/users/xuid(xboxUserId)/scids/serviceConfigurationId/stats/statisticName", httpCall->PathQueryFragment.to_string());

        VERIFY_IS_NOT_NULL(result);

        VerifyUserStatisticsResult(result, responseJson, true);
    }

    DEFINE_TEST_CASE(TestGetSingleUserStatistics2)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetSingleUserStatistics2);
        auto responseJson = web::json::value::parse(defaultSingleUserStatsResponse);

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto statNames = ref new Platform::Collections::Vector<Platform::String^>();
        statNames->Append("namename");
        auto result = create_task(xboxLiveContext->UserStatisticsService->GetSingleUserStatisticsAsync(
            "xboxUserId",
            "serviceConfigId",
            statNames->GetView()
            )).get();
        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://userstats.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/users/xuid(xboxUserId)/scids/serviceConfigId/stats/namename", httpCall->PathQueryFragment.to_string());

        VerifyUserStatisticsResult(result, responseJson, true);
    }

    DEFINE_TEST_CASE(TestGetBatchUserStatistics1)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetBatchUserStatistics1);
        auto responseJson = web::json::value::parse(defaultBatchUsersStatsResponse);

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        Vector<Platform::String^>^ userNames = ref new Vector<Platform::String^>();
        userNames->Append("123");
        auto statNames = ref new Platform::Collections::Vector<Platform::String^>();
        statNames->Append("namename");
        auto result = create_task(xboxLiveContext->UserStatisticsService->GetMultipleUserStatisticsAsync(
            userNames->GetView(),
            "serviceConfigId",
            statNames->GetView()
            )).get();
        VERIFY_ARE_EQUAL_STR(L"POST", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://userstats.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/batch?operation=read", httpCall->PathQueryFragment.to_string());
        VERIFY_ARE_EQUAL_STR(LR"({"requestedscids":[{"requestedstats":["namename"],"scid":"serviceConfigId"}],"requestedusers":["123"]})", httpCall->request_body().request_message_string());

        VERIFY_IS_NOT_NULL(result);

        VERIFY_ARE_EQUAL_INT(result->Size, responseJson[L"users"].size());

        for (uint32_t i = 0; i < result->Size; i++)
        {
            VerifyUserStatisticsResult(result->GetAt(i), responseJson[L"users"][i], false);
        }
    }

    DEFINE_TEST_CASE(TestGetBatchUserStatistics2)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetBatchUserStatistics2);
        auto responseJson = web::json::value::parse(defaultBatchUsersStatsResponse);

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto userNames = ref new Vector<Platform::String^>();
        userNames->Append("123");
        auto statNames = ref new Vector<Platform::String^>();
        statNames->Append("namename");

        Vector<RequestedStatistics^>^ requests = ref new Vector<RequestedStatistics^>();
        auto request = ref new RequestedStatistics(
            "serviceConfigId",
            statNames->GetView()
            );
        requests->Append(request);
        auto result = create_task(xboxLiveContext->UserStatisticsService->GetMultipleUserStatisticsForMultipleServiceConfigurationsAsync(
            userNames->GetView(),
            requests->GetView()
            )).get();
        VERIFY_ARE_EQUAL_STR(L"POST", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://userstats.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/batch?operation=read", httpCall->PathQueryFragment.to_string());
        VERIFY_ARE_EQUAL_STR(LR"({"requestedscids":[{"requestedstats":["namename"],"scid":"serviceConfigId"}],"requestedusers":["123"]})", httpCall->request_body().request_message_string());

        VERIFY_IS_NOT_NULL(result);

        VERIFY_ARE_EQUAL_INT(result->Size, responseJson[L"users"].size());

        for (uint32_t i = 0; i < result->Size; i++)
        {
            VerifyUserStatisticsResult(result->GetAt(i), responseJson[L"users"][i], false);
        }
    }

    DEFINE_TEST_CASE(TestRTAStatistics)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestRTAStatistics);
        const int subId = 321;
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto mockSocket = m_mockXboxSystemFactory->GetMockWebSocketClient();
        SetWebSocketRTAAutoResponser(mockSocket, defaultRTAStat, subId);

        auto helper = SetupStateChangeHelper(xboxLiveContext->RealTimeActivityService);
        xboxLiveContext->RealTimeActivityService->Activate();
        helper->connectedEvent.wait();

        auto expectedValue = web::json::value::parse(defaultRTAStat);
        concurrency::event fireEvent;
        bool didFire = false;
        const string_t testUser = _T("TestUser");
        const string_t scid = _T("12345");
        const string_t statName = DEFAULT_STAT_NAME;

        auto statisticInitializeHandler = xboxLiveContext->UserStatisticsService->StatisticChanged += 
            ref new Windows::Foundation::EventHandler<StatisticChangeEventArgs^>([this, &fireEvent, &didFire, &expectedValue, testUser, scid](Platform::Object^, StatisticChangeEventArgs^ args)
        {
            didFire = true;
            VERIFY_ARE_EQUAL(args->XboxUserId->Data(), testUser);
            VERIFY_ARE_EQUAL(args->ServiceConfigurationId->Data(), scid);
            VerifyRTAStat(args->LatestStatistic, expectedValue);
            fireEvent.set();
        });

        auto subscription = xboxLiveContext->UserStatisticsService->SubscribeToStatisticChange(
            ref new Platform::String(testUser.c_str()),
            ref new Platform::String(scid.c_str()),
            ref new Platform::String(statName.c_str())
            );

        auto msg = FormatString(L"Subscription %s created, name: %s", statName.c_str(), subscription->StatisticName->Data());
        TEST_LOG(msg.c_str());

        string_t statSubUri = subscription->ResourceUri->Data();
        VERIFY_ARE_EQUAL(statSubUri, L"https://userstats.xboxlive.com/users/xuid(TestUser)/scids/12345/stats/Stat");
        TEST_LOG(L"Wait for StatisticChanged event for initial stat");
        fireEvent.wait();
        fireEvent.reset();

        uint32_t testNumber = 32;
        auto updatedValue = web::json::value(testNumber);
        expectedValue = web::json::value::parse(updatedRTAStat);

        mockSocket->receive_rta_event(subId, updatedValue.serialize());
        VERIFY_IS_TRUE(subscription->State == Microsoft::Xbox::Services::RealTimeActivity::RealTimeActivitySubscriptionState::Subscribed);
        VERIFY_ARE_EQUAL_STR(subscription->ResourceUri->Data(), L"https://userstats.xboxlive.com/users/xuid(TestUser)/scids/12345/stats/Stat");
        VERIFY_ARE_EQUAL(subscription->XboxUserId->Data(), testUser);
        VERIFY_ARE_EQUAL(subscription->ServiceConfigurationId->Data(), scid);
        VERIFY_ARE_EQUAL(subscription->StatisticName->Data(), statName);
        VERIFY_ARE_EQUAL_INT(subscription->SubscriptionId, 321);
        TEST_LOG(L"Wait for StatisticChanged event for updated stat");
        fireEvent.wait();
        fireEvent.reset();

        didFire = false;
        xboxLiveContext->UserStatisticsService->UnsubscribeFromStatisticChange(
            subscription
            );
        mockSocket->receive_rta_event(subId, updatedValue.serialize());
        VERIFY_IS_FALSE(didFire);
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

