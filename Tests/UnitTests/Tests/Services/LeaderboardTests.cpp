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
#define TEST_CLASS_AREA L"Leaderboard"
#include "UnitTestIncludes.h"

#include "Utils_WinRT.h"
#include "xsapi/leaderboard.h"
#include "leaderboard_serializers.h"

using namespace Windows::Foundation;
using namespace xbox::services::leaderboard;
using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::System;
using namespace Microsoft::Xbox::Services::Leaderboard;
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

const std::wstring defaultLeaderboardData =
LR"({
    "pagingInfo": {
        "continuationToken": "6",
        "totalItems": 218
    },
    "leaderboardInfo": {
        "totalCount": 218,
        "columnDefinition": {
            "statName": "EnemyDefeats",
            "type": "Integer"
        }
    },
    "userList": [
        {
            "gamertag": "NSC FaceRocker",
            "xuid": "2533275015216241",
            "percentile": 0.9954,
            "rank": 1,
            "globalrank": 1,
            "value": "3660",
            "valuemetadata": "{\"HasSkull\": true, \"Kills\": 11, \"Level\": \"Hardcake\", \"Empty\": null}"
        },
        {
            "gamertag": "isspmarkbou",
            "xuid": "2533275024657260",
            "percentile": 0.9908,
            "rank": 2,
            "globalrank": 2,
            "value": "2208",
            "valuemetadata": "{\"HasSkull\": false, \"Level\": \"Hardcake\", \"Empty\": null}"
        },
        {
            "gamertag": "UnloosedLeech",
            "xuid": "2535449359478292",
            "percentile": 0.9862,
            "rank": 3,
            "globalrank": 3,
            "value": "1064",
            "valuemetadata": "{\"HasSkull\": null, \"Kills\": 11, \"Level\": \"Hardcake\", \"Empty\": null}"
        },
        {
            "gamertag": "MSFT JEFFSHI 00",
            "xuid": "2814662167029838",
            "percentile": 0.9817,
            "rank": 4,
            "globalrank": 4,
            "value": "783",
            "valuemetadata": "{\"HasSkull\": true, \"Kills\": 11, \"Level\": \"Hardcake\", \"Empty\": null}"
        },
        {
            "gamertag": "ProfittMan",
            "xuid": "2533274998970959",
            "percentile": 0.9771,
            "rank": 5,
            "globalrank": 5,
            "value": "535",
            "valuemetadata": "{\"HasSkull\": true, \"Kills\": 11, \"Level\": \"Hardcake\", \"Empty\": null}"
        }
    ]
})";
const std::wstring defaultV1LeaderboardData =
LR"({
    "pagingInfo": {
        "continuationToken": "6",
        "totalItems": 218
    },
    "leaderboardInfo": {
        "totalCount": 218,
        "columnDefinition": {
            "statName": "EnemyDefeats",
            "type": "Integer"
        }
    },
    "userList": [
        {
            "gamertag": "NSC FaceRocker",
            "xuid": "2533275015216241",
            "percentile": 0.9954,
            "rank": 1,
            "globalrank": 1,
            "values": ["3660"]
        },
        {
            "gamertag": "isspmarkbou",
            "xuid": "2533275024657260",
            "percentile": 0.9908,
            "rank": 2,
            "globalrank": 2,
            "values": ["2208"]
        }
    ]
})";

DEFINE_TEST_CLASS(leaderboard_serializer_tests)
{
public:
    DEFINE_TEST_CLASS_PROPS(leaderboard_serializer_tests)

    void VerifyLeaderboardColumn(LeaderboardColumn^ column, web::json::value columnToVerify)
    {
        VERIFY_IS_NOT_NULL(column);

        VERIFY_ARE_EQUAL(column->StatisticName->Data(), columnToVerify[L"statName"].as_string());
        if (column->StatisticType == PropertyType::UInt64)
        {
            VERIFY_ARE_EQUAL_STR(L"Integer", columnToVerify[L"type"].as_string());
        }
        else if (column->StatisticType == PropertyType::OtherType)
        {
            // do nothing, the returned string is not going to match the JSON
        }
        else
        {
            VERIFY_ARE_EQUAL(column->StatisticType.ToString()->Data(), columnToVerify[L"type"].as_string());
        }
    }

    void VerifyLeaderboardRow(LeaderboardRow^ row, web::json::value rowToVerify, const std::vector<string_t>& columns)
    {
        VERIFY_IS_NOT_NULL(row);

        VERIFY_ARE_EQUAL(row->Gamertag->Data(), rowToVerify[L"gamertag"].as_string());
        VERIFY_ARE_EQUAL(row->XboxUserId->Data(), rowToVerify[L"xuid"].as_string());
        VERIFY_ARE_EQUAL(row->Percentile, rowToVerify[L"percentile"].as_double());
        VERIFY_ARE_EQUAL_INT(row->Rank, rowToVerify[L"rank"].as_integer());
        
        if (!rowToVerify[L"values"].is_null())
        {

        }
        else
        {

            web::json::object metadataJson = web::json::value::parse(rowToVerify[L"valuemetadata"].as_string()).as_object();

            for (uint32_t i = 0; i < row->Values->Size; ++i)
            {
                string_t actual;
                string_t expected;
                if (i == 0)
                {
                    expected = rowToVerify[L"value"].as_string();
                    actual = row->Values->GetAt(i)->Data();
                }
                else if (columns.size() > 0)
                {
                    expected = metadataJson[columns[i - 1]].serialize();
                    actual = web::json::value::parse(row->Values->GetAt(i)->Data()).serialize();
                }
                VERIFY_ARE_EQUAL(expected, actual);
            }

        }

    }

    void VerifyLeadershipResult(LeaderboardResult^ result, web::json::value resultToVerify, const std::vector<string_t>& columns = std::vector<string_t>())
    {
        VERIFY_IS_NOT_NULL(result);

        auto leaderboardInfoJson = resultToVerify[L"leaderboardInfo"];
        VERIFY_ARE_EQUAL_INT(result->TotalRowCount, (leaderboardInfoJson[L"totalCount"]).as_integer());

        auto jsonColumn = leaderboardInfoJson[L"columnDefinition"];
        VerifyLeaderboardColumn(result->Columns->GetAt(0), jsonColumn);

        auto jsonRows = resultToVerify[L"userList"].as_array();
        int index = 0;
        for (auto row : jsonRows)
        {
            VerifyLeaderboardRow(result->Rows->GetAt(index++), row, columns);
        }
    }

    DEFINE_TEST_CASE(TestGetLeaderboardAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetLeaderboardAsync);
        auto responseJson = web::json::value::parse(defaultLeaderboardData);
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        std::vector<string_t> columns;

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto result = create_task(xboxLiveContext->LeaderboardService->GetLeaderboardAsync(
            "c4060100-4951-4a51-a630-dce26c15b8c5",
            "lbEncodedRecordHoleId101RecordTypeId1"
            )).get();

        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://leaderboards.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/scids/c4060100-4951-4a51-a630-dce26c15b8c5/leaderboards/lbEncodedRecordHoleId101RecordTypeId1", httpCall->PathQueryFragment.to_string());
        VerifyLeadershipResult(result, responseJson, columns);

        VERIFY_IS_TRUE( result->HasNext );
        auto nextResult = create_task(result->GetNextAsync(100)).get();
        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://leaderboards.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/scids/c4060100-4951-4a51-a630-dce26c15b8c5/leaderboards/lbEncodedRecordHoleId101RecordTypeId1?maxItems=100&continuationToken=6", httpCall->PathQueryFragment.to_string());
        VerifyLeadershipResult(nextResult, responseJson, columns);

        columns.push_back(_T("HasSkull"));
        columns.push_back(_T("Level"));

        result = create_task(xboxLiveContext->LeaderboardService->GetLeaderboardWithAdditionalColumnsAsync(
            "c4060100-4951-4a51-a630-dce26c15b8c5",
            "lbEncodedRecordHoleId101RecordTypeId1",
            UtilsWinRT::CreatePlatformVectorFromStdVectorString(columns)->GetView()
            )).get();

        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://leaderboards.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/scids/c4060100-4951-4a51-a630-dce26c15b8c5/leaderboards/lbEncodedRecordHoleId101RecordTypeId1?include=valuemetadata", httpCall->PathQueryFragment.to_string());
        VerifyLeadershipResult(result, responseJson, columns);

        columns.push_back(_T("Kills"));
        columns.push_back(_T("Empty"));
        VERIFY_IS_TRUE(result->HasNext);
        nextResult = create_task(result->GetNextAsync(100)).get();
        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://leaderboards.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/scids/c4060100-4951-4a51-a630-dce26c15b8c5/leaderboards/lbEncodedRecordHoleId101RecordTypeId1?include=valuemetadata&maxItems=100&continuationToken=6", httpCall->PathQueryFragment.to_string());
        VerifyLeadershipResult(nextResult, responseJson, columns);
    }

    DEFINE_TEST_CASE(TestGetLeaderboardWitSkipToRankAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetLeaderboardWitSkipToRankAsync);
        auto responseJson = web::json::value::parse(defaultLeaderboardData);
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        std::vector<string_t> columns;

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto result = create_task(xboxLiveContext->LeaderboardService->GetLeaderboardAsync(
            "c4060100-4951-4a51-a630-dce26c15b8c5",
            "lbEncodedRecordHoleId101RecordTypeId1",
            100,
            10
            )).get();

        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://leaderboards.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/scids/c4060100-4951-4a51-a630-dce26c15b8c5/leaderboards/lbEncodedRecordHoleId101RecordTypeId1?maxItems=10&skipToRank=100", httpCall->PathQueryFragment.to_string());
        VerifyLeadershipResult(result, responseJson, columns);

        columns.push_back(_T("HasSkull"));
        columns.push_back(_T("Kills"));
        columns.push_back(_T("Level"));
        columns.push_back(_T("Empty"));

        result = create_task(xboxLiveContext->LeaderboardService->GetLeaderboardWithAdditionalColumnsAsync(
            "c4060100-4951-4a51-a630-dce26c15b8c5",
            "lbEncodedRecordHoleId101RecordTypeId1",
            100,
            UtilsWinRT::CreatePlatformVectorFromStdVectorString(columns)->GetView(),
            10
            )).get();

        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://leaderboards.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/scids/c4060100-4951-4a51-a630-dce26c15b8c5/leaderboards/lbEncodedRecordHoleId101RecordTypeId1?include=valuemetadata&maxItems=10&skipToRank=100", httpCall->PathQueryFragment.to_string());
        VerifyLeadershipResult(result, responseJson, columns);
    }

    DEFINE_TEST_CASE(TestGetLearderboardSkipToUserAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetLearderboardSkipToUserAsync);
        auto responseJson = web::json::value::parse(defaultLeaderboardData);
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        std::vector<string_t> columns;

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto result = create_task(xboxLiveContext->LeaderboardService->GetLeaderboardWithSkipToUserAsync(
            "c4060100-4951-4a51-a630-dce26c15b8c5",
            "lbEncodedRecordHoleId101RecordTypeId1",
            "2533274896500838",
            20
            )).get();

        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://leaderboards.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/scids/c4060100-4951-4a51-a630-dce26c15b8c5/leaderboards/lbEncodedRecordHoleId101RecordTypeId1?maxItems=20&skipToUser=2533274896500838", httpCall->PathQueryFragment.to_string());
        VerifyLeadershipResult(result, responseJson, columns);

        columns.push_back(_T("HasSkull"));
        columns.push_back(_T("Kills"));
        columns.push_back(_T("Level"));
        columns.push_back(_T("Empty"));

        result = create_task(xboxLiveContext->LeaderboardService->GetLeaderboardWithSkipToUserWithAdditionalColumnsAsync(
            "c4060100-4951-4a51-a630-dce26c15b8c5",
            "lbEncodedRecordHoleId101RecordTypeId1",
            "2533274896500838",
            UtilsWinRT::CreatePlatformVectorFromStdVectorString(columns)->GetView(),
            20
            )).get();

        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://leaderboards.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/scids/c4060100-4951-4a51-a630-dce26c15b8c5/leaderboards/lbEncodedRecordHoleId101RecordTypeId1?include=valuemetadata&maxItems=20&skipToUser=2533274896500838", httpCall->PathQueryFragment.to_string());
        VerifyLeadershipResult(result, responseJson, columns);
    }

    DEFINE_TEST_CASE(TestGetLeaderboardForSocialGroupAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetLeaderboardForSocialGroupAsync);
        auto responseJson = web::json::value::parse(defaultLeaderboardData);
        auto responseV1Json = web::json::value::parse(defaultV1LeaderboardData);
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseV1Json);

        std::vector<string_t> columns;

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto result = create_task(xboxLiveContext->LeaderboardService->GetLeaderboardForSocialGroupAsync(
            "98052",
            "c4060100-4951-4a51-a630-dce26c15b8c5",
            "EncodedRecordMax.HoleId.108.RecordTypeId.3",
            "All",
            20
            )).get();

        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://leaderboards.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/users/xuid(98052)/scids/c4060100-4951-4a51-a630-dce26c15b8c5/stats/EncodedRecordMax.HoleId.108.RecordTypeId.3/people/All?maxItems=20", httpCall->PathQueryFragment.to_string());
        VerifyLeadershipResult(result, responseV1Json);

        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        result = create_task(xboxLiveContext->LeaderboardService->GetLeaderboardAsync(
            "c4060100-4951-4a51-a630-dce26c15b8c5",
            "lbEncodedRecordHoleId101RecordTypeId1",
            "98052",
            "All",
            20
            )).get();

        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://leaderboards.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/scids/c4060100-4951-4a51-a630-dce26c15b8c5/leaderboards/lbEncodedRecordHoleId101RecordTypeId1?xuid=98052&maxItems=20&view=People&viewTarget=All", httpCall->PathQueryFragment.to_string());
        VerifyLeadershipResult(result, responseJson);

        columns.push_back(_T("HasSkull"));
        columns.push_back(_T("Kills"));
        columns.push_back(_T("Level"));
        columns.push_back(_T("Empty"));

        result = create_task(xboxLiveContext->LeaderboardService->GetLeaderboardWithAdditionalColumnsAsync(
            "c4060100-4951-4a51-a630-dce26c15b8c5",
            "lbEncodedRecordHoleId101RecordTypeId1",
            "98052",
            "All",
            UtilsWinRT::CreatePlatformVectorFromStdVectorString(columns)->GetView(),
            20
            )).get();

        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://leaderboards.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/scids/c4060100-4951-4a51-a630-dce26c15b8c5/leaderboards/lbEncodedRecordHoleId101RecordTypeId1?include=valuemetadata&xuid=98052&maxItems=20&view=People&viewTarget=All", httpCall->PathQueryFragment.to_string());
        VerifyLeadershipResult(result, responseJson);
    }

    DEFINE_TEST_CASE(TestGetLeaderboardForSocialGroupWithSortAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetLeaderboardForSocialGroupWithSortAsync);
        auto responseJson = web::json::value::parse(defaultLeaderboardData);
        auto responseV1Json = web::json::value::parse(defaultV1LeaderboardData);
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseV1Json);

        std::vector<string_t> columns;

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto result = create_task(xboxLiveContext->LeaderboardService->GetLeaderboardForSocialGroupAsync(
            "98052",
            "c4060100-4951-4a51-a630-dce26c15b8c5",
            "EncodedRecordMax.HoleId.108.RecordTypeId.3",
            "All",
            "descending",
            20
            )).get();
        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://leaderboards.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/users/xuid(98052)/scids/c4060100-4951-4a51-a630-dce26c15b8c5/stats/EncodedRecordMax.HoleId.108.RecordTypeId.3/people/All?sort=descending&maxItems=20", httpCall->PathQueryFragment.to_string());
        VerifyLeadershipResult(result, responseV1Json);
    }

    DEFINE_TEST_CASE(TestGetLeaderboardForSocialGroupWithSkipToRankAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetLeaderboardForSocialGroupWithSkipToRankAsync);
        auto responseJson = web::json::value::parse(defaultLeaderboardData);
        auto responseV1Json = web::json::value::parse(defaultV1LeaderboardData);
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseV1Json);

        std::vector<string_t> columns;

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto result = create_task(xboxLiveContext->LeaderboardService->GetLeaderboardForSocialGroupWithSkipToRankAsync(
            "98052",
            "c4060100-4951-4a51-a630-dce26c15b8c5",
            "EncodedRecordMax.HoleId.108.RecordTypeId.3",
            "All",
            2,
            "descending",
            20
            )).get();

        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://leaderboards.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/users/xuid(98052)/scids/c4060100-4951-4a51-a630-dce26c15b8c5/stats/EncodedRecordMax.HoleId.108.RecordTypeId.3/people/All?sort=descending&maxItems=20&skipToRank=2", httpCall->PathQueryFragment.to_string());
        VerifyLeadershipResult(result, responseV1Json);

        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        result = create_task(xboxLiveContext->LeaderboardService->GetLeaderboardAsync(
            "c4060100-4951-4a51-a630-dce26c15b8c5",
            "lbEncodedRecordHoleId101RecordTypeId1",
            "98052",
            "All",
            2,
            20
            )).get();

        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://leaderboards.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/scids/c4060100-4951-4a51-a630-dce26c15b8c5/leaderboards/lbEncodedRecordHoleId101RecordTypeId1?xuid=98052&maxItems=20&skipToRank=2&view=People&viewTarget=All", httpCall->PathQueryFragment.to_string());
        VerifyLeadershipResult(result, responseJson);

        columns.push_back(_T("HasSkull"));
        columns.push_back(_T("Kills"));
        columns.push_back(_T("Level"));
        columns.push_back(_T("Empty"));

        result = create_task(xboxLiveContext->LeaderboardService->GetLeaderboardWithAdditionalColumnsAsync(
            "c4060100-4951-4a51-a630-dce26c15b8c5",
            "lbEncodedRecordHoleId101RecordTypeId1",
            "98052",
            "All",
            2,
            UtilsWinRT::CreatePlatformVectorFromStdVectorString(columns)->GetView(),
            20
            )).get();

        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://leaderboards.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/scids/c4060100-4951-4a51-a630-dce26c15b8c5/leaderboards/lbEncodedRecordHoleId101RecordTypeId1?include=valuemetadata&xuid=98052&maxItems=20&skipToRank=2&view=People&viewTarget=All", httpCall->PathQueryFragment.to_string());
        VerifyLeadershipResult(result, responseJson);
    }

    DEFINE_TEST_CASE(TestGetLeaderboardForSocialGroupWithSkipToUserAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetLeaderboardForSocialGroupWithSkipToUserAsync);
        auto responseJson = web::json::value::parse(defaultLeaderboardData);
        auto responseV1Json = web::json::value::parse(defaultV1LeaderboardData);
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseV1Json);

        std::vector<string_t> columns;

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto result = create_task(xboxLiveContext->LeaderboardService->GetLeaderboardForSocialGroupWithSkipToUserAsync(
            "98052",
            "c4060100-4951-4a51-a630-dce26c15b8c5",
            "EncodedRecordMax.HoleId.108.RecordTypeId.3",
            "All",
            "2533274896500838",
            "ascending",
            20
            )).get();

        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://leaderboards.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/users/xuid(98052)/scids/c4060100-4951-4a51-a630-dce26c15b8c5/stats/EncodedRecordMax.HoleId.108.RecordTypeId.3/people/All?sort=ascending&maxItems=20&skipToUser=2533274896500838", httpCall->PathQueryFragment.to_string());
        VerifyLeadershipResult(result, responseV1Json);

        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        result = create_task(xboxLiveContext->LeaderboardService->GetLeaderboardWithSkipToUserAsync(
            "c4060100-4951-4a51-a630-dce26c15b8c5",
            "lbEncodedRecordHoleId101RecordTypeId1",
            "98052",
            "All",
            "2533274896500838",
            20
            )).get();

        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://leaderboards.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/scids/c4060100-4951-4a51-a630-dce26c15b8c5/leaderboards/lbEncodedRecordHoleId101RecordTypeId1?xuid=98052&maxItems=20&skipToUser=2533274896500838&view=People&viewTarget=All", httpCall->PathQueryFragment.to_string());
        VerifyLeadershipResult(result, responseJson);
        columns.push_back(_T("HasSkull"));
        columns.push_back(_T("Kills"));
        columns.push_back(_T("Level"));
        columns.push_back(_T("Empty"));

        result = create_task(xboxLiveContext->LeaderboardService->GetLeaderboardWithSkipToUserWithAdditionalColumnsAsync(
            "c4060100-4951-4a51-a630-dce26c15b8c5",
            "lbEncodedRecordHoleId101RecordTypeId1",
            "98052",
            "All",
            "2533274896500838",
            UtilsWinRT::CreatePlatformVectorFromStdVectorString(columns)->GetView(),
            20
            )).get();

        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://leaderboards.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/scids/c4060100-4951-4a51-a630-dce26c15b8c5/leaderboards/lbEncodedRecordHoleId101RecordTypeId1?include=valuemetadata&xuid=98052&maxItems=20&skipToUser=2533274896500838&view=People&viewTarget=All", httpCall->PathQueryFragment.to_string());
        VerifyLeadershipResult(result, responseJson);
    }

    DEFINE_TEST_CASE(TestGetLeaderboardAsyncInvalidArgs)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetLeaderboardAsyncInvalidArgs);
        auto responseJson = web::json::value::parse(defaultLeaderboardData);
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        
#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->LeaderboardService->GetLeaderboardAsync(
                nullptr,    // invalid arg
                "lbEncodedRecordHoleId101RecordTypeId1"
            )).get(), 
            E_INVALIDARG
        )

#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->LeaderboardService->GetLeaderboardAsync(
                "c4060100 - 4951 - 4a51 - a630 - dce26c15b8c5",
                nullptr     // invalid arg
            )).get(),
            E_INVALIDARG
        )
    }

    DEFINE_TEST_CASE(TestGetLeaderboardForSocialGroupAsyncWithInvalidArgs)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetLeaderboardForSocialGroupAsyncWithInvalidArgs);
        auto responseJson = web::json::value::parse(defaultLeaderboardData);
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();

#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->LeaderboardService->GetLeaderboardForSocialGroupAsync(
                nullptr,
                "c4060100-4951-4a51-a630-dce26c15b8c5",
                "EncodedRecordMax.HoleId.108.RecordTypeId.3",
                "All",
                20
            )).get(),
            E_INVALIDARG
        )

#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->LeaderboardService->GetLeaderboardForSocialGroupAsync(
                "98052",
                nullptr,
                "EncodedRecordMax.HoleId.108.RecordTypeId.3",
                "All",
                20
            )).get(),
            E_INVALIDARG
        )

#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->LeaderboardService->GetLeaderboardForSocialGroupAsync(
                "98052",
                "c4060100-4951-4a51-a630-dce26c15b8c5",
                nullptr,
                "All",
                20
            )).get(),
            E_INVALIDARG
        )

#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->LeaderboardService->GetLeaderboardForSocialGroupAsync(
                "98052",
                "c4060100-4951-4a51-a630-dce26c15b8c5",
                "EncodedRecordMax.HoleId.108.RecordTypeId.3",
                nullptr,
                20
            )).get(),
            E_INVALIDARG
        )
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

