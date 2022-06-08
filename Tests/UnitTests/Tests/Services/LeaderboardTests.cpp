// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "UnitTestIncludes.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

const char* scid{ "c4060100-4951-4a51-a630-dce26c15b8c5" };
const char* leaderboardName{ "lbEncodedRecordHoleId101RecordTypeId1" };
const char* statName{ "EncodedRecordMax.HoleId.108.RecordTypeId.3" };
const char* server{ "https://leaderboards.xboxlive.com/" };
const uint64_t xuid{ 2814613569642996 };

const char* defaultLeaderboardData =
R"({
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
            "moderngamertag": "Modern FaceRocker",
            "moderngamertagsuffix": "1234",
            "uniquemoderngamertag": "Modern FaceRocker#1234",
            "xuid": "2533275015216241",
            "percentile": 0.9954,
            "rank": 1,
            "globalrank": 1,
            "value": "3660",
            "valuemetadata": "{\"HasSkull\": true, \"Kills\": 11, \"Level\": \"Hardcake\", \"Empty\": null}"
        },
        {
            "gamertag": "isspmarkbou",
            "moderngamertag": "Modern isspmarkbou",
            "moderngamertagsuffix": "2345",
            "uniquemoderngamertag": "Modern isspmarkbou#2345",
            "xuid": "2533275024657260",
            "percentile": 0.9908,
            "rank": 2,
            "globalrank": 2,
            "value": "2208",
            "valuemetadata": "{\"HasSkull\": false, \"Kills\": 11, \"Level\": \"Hardcake\", \"Empty\": null}"
        },
        {
            "gamertag": "UnloosedLeech",
            "moderngamertag": "Modern UnloosedLeech",
            "moderngamertagsuffix": "3456",
            "uniquemoderngamertag": "Modern UnloosedLeech#3456",
            "xuid": "2535449359478292",
            "percentile": 0.9862,
            "rank": 3,
            "globalrank": 3,
            "value": "1064",
            "valuemetadata": "{\"HasSkull\": null, \"Kills\": 11, \"Level\": \"Hardcake\", \"Empty\": null}"
        },
        {
            "gamertag": "MSFT JEFFSHI 00",
            "moderngamertag": "Modern JEFFSHI",
            "moderngamertagsuffix": "4567",
            "uniquemoderngamertag": "Modern JEFFSHI#4567",
            "xuid": "2814662167029838",
            "percentile": 0.9817,
            "rank": 4,
            "globalrank": 4,
            "value": "783",
            "valuemetadata": "{\"HasSkull\": true, \"Kills\": 11, \"Level\": \"Hardcake\", \"Empty\": null}"
        },
        {
            "gamertag": "ProfittMan",
            "moderngamertag": "Modern ProfittMan",
            "moderngamertagsuffix": "5678",
            "uniquemoderngamertag": "Modern ProfittMan#5678",
            "xuid": "2533274998970959",
            "percentile": 0.9771,
            "rank": 5,
            "globalrank": 5,
            "value": "535",
            "valuemetadata": "{\"HasSkull\": true, \"Kills\": 11, \"Level\": \"Hardcake\", \"Empty\": null}"
        }
    ]
})";

const char* defaultV1LeaderboardData =
R"({
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
            "moderngamertag": "Modern FaceRocker",
            "moderngamertagsuffix": "1234",
            "uniquemoderngamertag": "Modern FaceRocker#1234",
            "xuid": "2533275015216241",
            "percentile": 0.9954,
            "rank": 1,
            "globalrank": 1,
            "values": ["3660"]
        },
        {
            "gamertag": "isspmarkbou",
            "moderngamertag": "Modern isspmarkbou",
            "moderngamertagsuffix": "2345",
            "uniquemoderngamertag": "Modern isspmarkbou#2345",
            "xuid": "2533275024657260",
            "percentile": 0.9908,
            "rank": 2,
            "globalrank": 2,
            "values": ["2208"]
        }
    ]
})";

DEFINE_TEST_CLASS(LeaderboardTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(LeaderboardTests);

    void VerifyLeaderboardColumn(XblLeaderboardColumn* column, JsonValue columnToVerify)
    {
        VERIFY_IS_NOT_NULL(column);
        VERIFY_ARE_EQUAL_STR(column->statName, columnToVerify["statName"].GetString());

        if (column->statType == XblLeaderboardStatType::Uint64)
        {
            VERIFY_ARE_EQUAL_STR("Integer", columnToVerify["type"].GetString());
        }
        else if (column->statType == XblLeaderboardStatType::Other)
        {
            // do nothing, the returned string is not going to match the JSON
        }
        else
        {
            const char* typeStr{};
            switch (column->statType)
            {
                case XblLeaderboardStatType::Boolean:
                    typeStr = "Boolean";
                    break;
                case XblLeaderboardStatType::Double:
                    typeStr = "Double";
                    break;
                case XblLeaderboardStatType::String:
                    typeStr = "String";
                    break;
            }

            VERIFY_ARE_EQUAL_STR(typeStr, columnToVerify["type"].GetString());
        }
    }

    void VerifyLeaderboardRow(XblLeaderboardRow* row, JsonValue rowToVerify, const std::vector<char*>& columns)
    {
        VERIFY_IS_NOT_NULL(row);
        VERIFY_ARE_EQUAL_STR(row->gamertag, rowToVerify["gamertag"].GetString());
        VERIFY_ARE_EQUAL_INT(row->xboxUserId, strtoull(rowToVerify["xuid"].GetString(), nullptr, 0));
        VERIFY_ARE_EQUAL_DOUBLE(row->percentile, rowToVerify["percentile"].GetDouble());
        VERIFY_ARE_EQUAL_INT(row->rank, rowToVerify["rank"].GetUint());
        VERIFY_ARE_EQUAL_INT(row->globalRank, rowToVerify["globalrank"].GetUint());
        
        if (!rowToVerify.HasMember("values"))
        {
            JsonDocument metadataJson;
            metadataJson.Parse(rowToVerify["valuemetadata"].GetString());

            for (uint32_t i = 0; i < row->columnValuesCount; ++i)
            {
                std::string actual;
                std::string expected;
                if (i == 0)
                {
                    expected = rowToVerify["value"].GetString();
                    actual = row->columnValues[i];
                }
                else if (columns.size() > 0)
                {
                    xsapi_internal_stringstream stream;
                    rapidjson::StringBuffer json;
                    rapidjson::Writer<rapidjson::StringBuffer> writer(json);
                    metadataJson[columns[i - 1]].Accept(writer);
                    expected = json.GetString();
                    actual = row->columnValues[i];
                }

                VERIFY_ARE_EQUAL_STR(expected, actual);
            }
        }
    }

    void VerifyLeadershipResult(XblLeaderboardResult* result, JsonValue& resultToVerify, const std::vector<char*>& columns = std::vector<char*>())
    {
        VERIFY_IS_NOT_NULL(result);

        auto leaderboardInfoJson = resultToVerify["leaderboardInfo"].GetObjectW();
        VERIFY_ARE_EQUAL_INT(result->totalRowCount, leaderboardInfoJson["totalCount"].GetUint());

        VerifyLeaderboardColumn(&result->columns[0], leaderboardInfoJson["columnDefinition"].GetObjectW());

        int index{ 0 };
        for (auto& row : resultToVerify["userList"].GetArray())
        {
            VerifyLeaderboardRow(&result->rows[index], row.GetObjectW(), columns);
            ++index;
        }
    }

    XblLeaderboardResult* TestAndGetLeaderboardResult(XblContextHandle xblContextHandle, XblLeaderboardQuery query, const char* responseStr, uint32_t bufferSizeMultiplier, const std::vector<char*>& columns = std::vector<char*>())
    {
        XAsyncBlock async{};
        size_t resultSize{};
        VERIFY_SUCCEEDED(XblLeaderboardGetLeaderboardAsync(xblContextHandle, query, &async));
        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_SUCCEEDED(XblLeaderboardGetLeaderboardResultSize(&async, &resultSize));

        size_t bufferUsed{};
        XblLeaderboardResult* result{};
        std::shared_ptr<char> buffer(new char[resultSize * bufferSizeMultiplier], std::default_delete<char[]>());
        VERIFY_SUCCEEDED(XblLeaderboardGetLeaderboardResult(&async, resultSize * bufferSizeMultiplier, buffer.get(), &result, &bufferUsed));
        VERIFY_ARE_EQUAL_UINT(resultSize, bufferUsed);

        JsonDocument responseJson;
        responseJson.Parse(responseStr);
        VerifyLeadershipResult(result, responseJson, columns);

        return result;
    }

    XblLeaderboardQuery MakeDefaultQuery()
    {
        XblLeaderboardQuery query{};
        strcpy_s(query.scid, scid);
        query.xboxUserId = 0;
        query.leaderboardName = leaderboardName;
        query.statName = "";
        query.maxItems = 0;
        query.skipToXboxUserId = 0;
        query.skipResultToRank = 0;
        query.socialGroup = XblSocialGroupType::None;
        query.order = XblLeaderboardSortOrder::Descending;

        return query;
    }

    DEFINE_TEST_CASE(TestGetLeaderboardAsync)
    {
        TEST_LOG(L"Test starting: TestGetLeaderboardAsync");

        TestEnvironment env{};

        const char* token = "6";
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        xsapi_internal_stringstream url;
        url << server << "scids/" << scid << "/leaderboards/" << leaderboardName;
        HttpMock mock0("GET", url.str(), 200);
        mock0.SetResponseBody(defaultLeaderboardData);

        XAsyncBlock async{};
        size_t resultSize{};
        XblLeaderboardQuery query{ MakeDefaultQuery() };
        VERIFY_SUCCEEDED(XblLeaderboardGetLeaderboardAsync(xboxLiveContext.get(), query, &async));
        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_SUCCEEDED(XblLeaderboardGetLeaderboardResultSize(&async, &resultSize));

        XblLeaderboardResult* result{};
        std::shared_ptr<char> buffer(new char[resultSize], std::default_delete<char[]>());
        VERIFY_SUCCEEDED(XblLeaderboardGetLeaderboardResult(&async, resultSize, buffer.get(), &result, nullptr));

        JsonDocument responseJson;
        responseJson.Parse(defaultLeaderboardData);
        VerifyLeadershipResult(result, responseJson);

        VERIFY_IS_TRUE(result->hasNext);

        const uint32_t maxItems{ 100 };
        url.str("");
        url << server << "scids/" << scid << "/leaderboards/" << leaderboardName << "?maxItems=" << maxItems << "&continuationToken=" << token;
        HCMockClearMocks();
        HttpMock mock1("GET", url.str(), 200);
        mock1.SetResponseBody(defaultLeaderboardData);

        XblLeaderboardQuery nextQuery{ MakeDefaultQuery() };
        nextQuery.maxItems = maxItems;
        nextQuery.continuationToken = token;
        result->nextQuery = nextQuery;
        VERIFY_SUCCEEDED(XblLeaderboardResultGetNextAsync(xboxLiveContext.get(), result, maxItems, &async));
        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_SUCCEEDED(XblLeaderboardResultGetNextResultSize(&async, &resultSize));

        XblLeaderboardResult* nextResult{};
        std::shared_ptr<char> nextBuffer(new char[resultSize], std::default_delete<char[]>());
        VERIFY_SUCCEEDED(XblLeaderboardResultGetNextResult(&async, resultSize, nextBuffer.get(), &nextResult, nullptr));

        responseJson.Parse(defaultLeaderboardData);
        VerifyLeadershipResult(nextResult, responseJson);

        url.str("");
        url << server << "scids/" << scid << "/leaderboards/" << leaderboardName << "?include=valuemetadata";
        HCMockClearMocks();
        HttpMock mock2("GET", url.str(), 200);
        mock2.SetResponseBody(defaultLeaderboardData);

        std::vector<char*> vecColumns{ "HasSkull", "Level" };
        const char* columns2[2] = { vecColumns[0], vecColumns[1] };
        query.additionalColumnleaderboardNamesCount = 2;
        query.additionalColumnleaderboardNames = columns2;
        TestAndGetLeaderboardResult(xboxLiveContext.get(), query, defaultLeaderboardData, 1, vecColumns);

        url.str("");
        url << server << "scids/" << scid << "/leaderboards/" << leaderboardName << "?include=valuemetadata&maxItems=" << maxItems << "&continuationToken=6";
        HCMockClearMocks();
        HttpMock mock3("GET", url.str(), 200);
        mock3.SetResponseBody(defaultLeaderboardData);

        vecColumns.push_back("Kills");
        vecColumns.push_back("Empty");
        const char* columns4[4] = { vecColumns[0], vecColumns[1], vecColumns[2], vecColumns[3] };
        nextQuery.additionalColumnleaderboardNamesCount = 4;
        nextQuery.additionalColumnleaderboardNames = columns4;
        TestAndGetLeaderboardResult(xboxLiveContext.get(), nextQuery, defaultLeaderboardData, 1, vecColumns);
    }

    DEFINE_TEST_CASE(TestGetLeaderboardWithLargeBufferAsync)
    {
        TEST_LOG(L"Test starting: TestGetLeaderboardWithLargeBufferAsync");

        TestEnvironment env{};

        const char* token = "6";
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        xsapi_internal_stringstream url;
        url << server << "scids/" << scid << "/leaderboards/" << leaderboardName;
        HttpMock mock0("GET", url.str(), 200);
        mock0.SetResponseBody(defaultLeaderboardData);

        XAsyncBlock async{};
        size_t resultSize{};
        XblLeaderboardQuery query{ MakeDefaultQuery() };
        VERIFY_SUCCEEDED(XblLeaderboardGetLeaderboardAsync(xboxLiveContext.get(), query, &async));
        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_SUCCEEDED(XblLeaderboardGetLeaderboardResultSize(&async, &resultSize));

        size_t bufferUsed{};
        XblLeaderboardResult* result{};
        std::shared_ptr<char> buffer(new char[resultSize * 2], std::default_delete<char[]>());
        VERIFY_SUCCEEDED(XblLeaderboardGetLeaderboardResult(&async, resultSize * 2, buffer.get(), &result, &bufferUsed));
        VERIFY_ARE_EQUAL_UINT(resultSize, bufferUsed);

        JsonDocument responseJson;
        responseJson.Parse(defaultLeaderboardData);
        VerifyLeadershipResult(result, responseJson);

        VERIFY_IS_TRUE(result->hasNext);

        const uint32_t maxItems{ 100 };
        url.str("");
        url << server << "scids/" << scid << "/leaderboards/" << leaderboardName << "?maxItems=" << maxItems << "&continuationToken=" << token;
        HCMockClearMocks();
        HttpMock mock1("GET", url.str(), 200);
        mock1.SetResponseBody(defaultLeaderboardData);

        XblLeaderboardQuery nextQuery{ MakeDefaultQuery() };
        nextQuery.maxItems = maxItems;
        nextQuery.continuationToken = token;
        result->nextQuery = nextQuery;
        VERIFY_SUCCEEDED(XblLeaderboardResultGetNextAsync(xboxLiveContext.get(), result, maxItems, &async));
        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_SUCCEEDED(XblLeaderboardResultGetNextResultSize(&async, &resultSize));

        XblLeaderboardResult* nextResult{};
        std::shared_ptr<char> nextBuffer(new char[resultSize * 2], std::default_delete<char[]>());
        VERIFY_SUCCEEDED(XblLeaderboardResultGetNextResult(&async, resultSize * 2, nextBuffer.get(), &nextResult, &bufferUsed));
        VERIFY_ARE_EQUAL_UINT(resultSize, bufferUsed);

        responseJson.Parse(defaultLeaderboardData);
        VerifyLeadershipResult(nextResult, responseJson);

        url.str("");
        url << server << "scids/" << scid << "/leaderboards/" << leaderboardName << "?include=valuemetadata";
        HCMockClearMocks();
        HttpMock mock2("GET", url.str(), 200);
        mock2.SetResponseBody(defaultLeaderboardData);

        std::vector<char*> vecColumns{ "HasSkull", "Level" };
        const char* columns2[2] = { vecColumns[0], vecColumns[1] };
        query.additionalColumnleaderboardNamesCount = 2;
        query.additionalColumnleaderboardNames = columns2;
        TestAndGetLeaderboardResult(xboxLiveContext.get(), query, defaultLeaderboardData, 2, vecColumns);

        url.str("");
        url << server << "scids/" << scid << "/leaderboards/" << leaderboardName << "?include=valuemetadata&maxItems=" << maxItems << "&continuationToken=6";
        HCMockClearMocks();
        HttpMock mock3("GET", url.str(), 200);
        mock3.SetResponseBody(defaultLeaderboardData);

        vecColumns.push_back("Kills");
        vecColumns.push_back("Empty");
        const char* columns4[4] = { vecColumns[0], vecColumns[1], vecColumns[2], vecColumns[3] };
        nextQuery.additionalColumnleaderboardNamesCount = 4;
        nextQuery.additionalColumnleaderboardNames = columns4;
        TestAndGetLeaderboardResult(xboxLiveContext.get(), nextQuery, defaultLeaderboardData, 2, vecColumns);
    }

    DEFINE_TEST_CASE(TestGetLeaderboardWitSkipToRankAsync)
    {
        TEST_LOG(L"Test starting: TestGetLeaderboardWitSkipToRankAsync");

        TestEnvironment env{};

        const int rank{ 100 };
        const int maxItems{ 10 };
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        xsapi_internal_stringstream url;
        url << server << "scids/" << scid << "/leaderboards/" << leaderboardName << "?maxItems=" << maxItems << "&skipToRank=" << rank;
        HttpMock mock0("GET", url.str(), 200);
        mock0.SetResponseBody(defaultLeaderboardData);

        XblLeaderboardQuery query{ MakeDefaultQuery() };
        query.maxItems = maxItems;
        query.skipResultToRank = rank;
        TestAndGetLeaderboardResult(xboxLiveContext.get(), query, defaultLeaderboardData, 1);

        url.str("");
        url << server << "scids/" << scid << "/leaderboards/" << leaderboardName << "?include=valuemetadata&maxItems=" << maxItems << "&skipToRank=" << rank;
        HCMockClearMocks();
        HttpMock mock1("GET", url.str(), 200);
        mock1.SetResponseBody(defaultLeaderboardData);
        
        std::vector<char*> vecColumns{ "HasSkull", "Kills", "Level", "Empty" };
        const char* columns[4] = { vecColumns[0], vecColumns[1], vecColumns[2], vecColumns[3] };
        query.additionalColumnleaderboardNamesCount = 4;
        query.additionalColumnleaderboardNames = columns;
        TestAndGetLeaderboardResult(xboxLiveContext.get(), query, defaultLeaderboardData, 1, vecColumns);
    }

    DEFINE_TEST_CASE(TestGetLearderboardSkipToUserAsync)
    {
        TEST_LOG(L"Test starting: TestGetLearderboardSkipToUserAsync");

        TestEnvironment env{};

        const uint64_t user{ 2533274896500838 };
        const uint32_t maxItems{ 20 };
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        xsapi_internal_stringstream url;
        url << server << "scids/" << scid << "/leaderboards/" << leaderboardName << "?maxItems=" << maxItems << "&skipToUser=" << user;
        HttpMock mock0("GET", url.str(), 200);
        mock0.SetResponseBody(defaultLeaderboardData);

        XblLeaderboardQuery query{ MakeDefaultQuery() };
        query.maxItems = maxItems;
        query.skipToXboxUserId = user;
        TestAndGetLeaderboardResult(xboxLiveContext.get(), query, defaultLeaderboardData, 1);

        url.str("");
        url << server << "scids/" << scid << "/leaderboards/" << leaderboardName << "?include=valuemetadata&maxItems=" << maxItems << "&skipToUser=" << user;
        HCMockClearMocks();
        HttpMock mock1("GET", url.str(), 200);
        mock1.SetResponseBody(defaultLeaderboardData);

        std::vector<char*> vecColumns{ "HasSkull", "Kills", "Level", "Empty" };
        const char* columns4[4] = { vecColumns[0], vecColumns[1], vecColumns[2], vecColumns[3] };
        query.additionalColumnleaderboardNamesCount = 4;
        query.additionalColumnleaderboardNames = columns4;
        TestAndGetLeaderboardResult(xboxLiveContext.get(), query, defaultLeaderboardData, 1, vecColumns);
    }

    DEFINE_TEST_CASE(TestGetLeaderboardForSocialGroupAsync)
    {
        TEST_LOG(L"Test starting: TestGetLeaderboardForSocialGroupAsync");

        TestEnvironment env{};

        const uint32_t maxItems{ 20 };
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        xsapi_internal_stringstream url;
        url << server << "users/xuid(" << xuid << ")/scids/" << scid << "/stats/" << statName << "/people/all?sort=descending&maxItems=" << maxItems;
        HttpMock mock0("GET", url.str(), 200);
        mock0.SetResponseBody(defaultV1LeaderboardData);

        XblLeaderboardQuery query{ MakeDefaultQuery() };
        query.xboxUserId = xuid;
        query.leaderboardName = nullptr;
        query.statName = statName;
        query.maxItems = maxItems;
        query.socialGroup = XblSocialGroupType::People;
        TestAndGetLeaderboardResult(xboxLiveContext.get(), query, defaultV1LeaderboardData, 1);

        mock0.SetResponseBody(defaultLeaderboardData);

        TestAndGetLeaderboardResult(xboxLiveContext.get(), query, defaultLeaderboardData, 1);

        url.str("");
        url << server << "scids/" << scid << "/leaderboards/" << leaderboardName << "?xuid=" << xuid << "&maxItems=" << maxItems << "&view=People&viewTarget=People";
        HCMockClearMocks();
        HttpMock mock1("GET", url.str(), 200);
        mock1.SetResponseBody(defaultLeaderboardData);

        query.statName = "";
        query.leaderboardName = leaderboardName;
        TestAndGetLeaderboardResult(xboxLiveContext.get(), query, defaultLeaderboardData, 1);

        url.str("");
        url << server << "scids/" << scid << "/leaderboards/" << leaderboardName << "?include=valuemetadata&xuid=" << xuid << "&maxItems=" << maxItems << "&view=People&viewTarget=People";
        HCMockClearMocks();
        HttpMock mock2("GET", url.str(), 200);
        mock2.SetResponseBody(defaultLeaderboardData);

        std::vector<char*> vecColumns{ "HasSkull", "Kills", "Level", "Empty" };
        const char* columns[4] = { vecColumns[0], vecColumns[1], vecColumns[2], vecColumns[3] };
        query.additionalColumnleaderboardNamesCount = 4;
        query.additionalColumnleaderboardNames = columns;
        TestAndGetLeaderboardResult(xboxLiveContext.get(), query, defaultLeaderboardData, 1);
    }

    DEFINE_TEST_CASE(TestGetLeaderboardForSocialGroupWithSortAsync)
    {
        TEST_LOG(L"Test starting: TestGetLeaderboardForSocialGroupWithSortAsync");

        TestEnvironment env{};

        const uint32_t maxItems{ 20 };
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        xsapi_internal_stringstream url;
        url << server << "users/xuid(" << xuid << ")/scids/" << scid << "/stats/" << statName << "/people/all?sort=descending&maxItems=" << maxItems;
        HttpMock mock0("GET", url.str(), 200);
        mock0.SetResponseBody(defaultV1LeaderboardData);
        
        XblLeaderboardQuery query{ MakeDefaultQuery() };
        query.xboxUserId = xuid;
        query.leaderboardName = nullptr;
        query.statName = statName;
        query.maxItems = maxItems;
        query.socialGroup = XblSocialGroupType::People;
        TestAndGetLeaderboardResult(xboxLiveContext.get(), query, defaultV1LeaderboardData, 1);
    }

    DEFINE_TEST_CASE(TestGetLeaderboardForSocialGroupWithSkipToRankAsync)
    {
        TEST_LOG(L"Test starting: TestGetLeaderboardForSocialGroupWithSkipToRankAsync");

        TestEnvironment env{};

        const uint32_t rank{ 2 };
        const uint32_t maxItems{ 20 };
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        xsapi_internal_stringstream url;
        url << server << "users/xuid(" << xuid << ")/scids/" << scid << "/stats/" << statName << "/people/all?sort=descending&maxItems=" << maxItems << "&skipToRank=" << rank;
        HttpMock mock0("GET", url.str(), 200);
        mock0.SetResponseBody(defaultV1LeaderboardData);

        XblLeaderboardQuery query{ MakeDefaultQuery() };
        query.xboxUserId = xuid;
        query.leaderboardName = nullptr;
        query.statName = statName;
        query.maxItems = maxItems;
        query.skipResultToRank = rank;
        query.socialGroup = XblSocialGroupType::People;
        TestAndGetLeaderboardResult(xboxLiveContext.get(), query, defaultV1LeaderboardData, 1);

        url.str("");
        url << server << "scids/" << scid << "/leaderboards/" << leaderboardName << "?xuid=" << xuid << "&maxItems=" << maxItems << "&skipToRank=" << rank << "&view=People&viewTarget=People";
        HCMockClearMocks();
        HttpMock mock1("GET", url.str(), 200);
        mock1.SetResponseBody(defaultLeaderboardData);

        query.statName = "";
        query.leaderboardName = leaderboardName;
        TestAndGetLeaderboardResult(xboxLiveContext.get(), query, defaultLeaderboardData, 1);

        url.str("");
        url << server << "scids/" << scid << "/leaderboards/" << leaderboardName << "?include=valuemetadata&xuid=" << xuid << "&maxItems=" << maxItems << "&skipToRank=" << rank << "&view=People&viewTarget=People";
        HCMockClearMocks();
        HttpMock mock2("GET", url.str(), 200);
        mock2.SetResponseBody(defaultLeaderboardData);
        
        std::vector<char*> vecColumns{ "HasSkull", "Kills", "Level", "Empty" };
        const char* columns4[4] = { vecColumns[0], vecColumns[1], vecColumns[2], vecColumns[3] };
        query.additionalColumnleaderboardNamesCount = 4;
        query.additionalColumnleaderboardNames = columns4;
        TestAndGetLeaderboardResult(xboxLiveContext.get(), query, defaultLeaderboardData, 1, vecColumns);
    }

    DEFINE_TEST_CASE(TestGetLeaderboardForSocialGroupWithSkipToUserAsync)
    {
        TEST_LOG(L"Test starting: TestGetLeaderboardForSocialGroupWithSkipToUserAsync");

        TestEnvironment env{};

        const uint64_t user{ 2533274896500838 };
        const uint32_t maxItems{ 20 };
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        xsapi_internal_stringstream url;
        url << server << "users/xuid(" << xuid << ")/scids/" << scid << "/stats/" << statName << "/people/all?sort=ascending&maxItems=" << maxItems << "&skipToUser=" << user;
        HttpMock mock0("GET", url.str(), 200);
        mock0.SetResponseBody(defaultV1LeaderboardData);

        XblLeaderboardQuery query{ MakeDefaultQuery() };
        query.xboxUserId = xuid;
        query.leaderboardName = nullptr;
        query.statName = statName;
        query.maxItems = maxItems;
        query.skipToXboxUserId = user;
        query.socialGroup = XblSocialGroupType::People;
        query.order = XblLeaderboardSortOrder::Ascending;
        TestAndGetLeaderboardResult(xboxLiveContext.get(), query, defaultV1LeaderboardData, 1);

        mock0.SetResponseBody(defaultLeaderboardData);

        TestAndGetLeaderboardResult(xboxLiveContext.get(), query, defaultV1LeaderboardData, 1);

        url.str("");
        url << server << "scids/" << scid << "/leaderboards/" << leaderboardName << "?xuid=" << xuid << "&maxItems=" << maxItems << "&skipToUser=" << user << "&view=People&viewTarget=People";
        HCMockClearMocks();
        HttpMock mock1("GET", url.str(), 200);
        mock1.SetResponseBody(defaultLeaderboardData);

        query.statName = "";
        query.leaderboardName = leaderboardName;
        TestAndGetLeaderboardResult(xboxLiveContext.get(), query, defaultLeaderboardData, 1);
        
        url.str("");
        url << server << "scids/" << scid << "/leaderboards/" << leaderboardName << "?include=valuemetadata&xuid=" << xuid << "&maxItems=" << maxItems << "&skipToUser=" << user << "&view=People&viewTarget=People";
        HCMockClearMocks();
        HttpMock mock2("GET", url.str(), 200);
        mock2.SetResponseBody(defaultLeaderboardData);

        std::vector<char*> vecColumns{ "HasSkull", "Kills", "Level", "Empty" };
        const char* columns4[4] = { vecColumns[0], vecColumns[1], vecColumns[2], vecColumns[3] };
        query.additionalColumnleaderboardNamesCount = 4;
        query.additionalColumnleaderboardNames = columns4;
        TestAndGetLeaderboardResult(xboxLiveContext.get(), query, defaultLeaderboardData, 1, vecColumns);
    }

    DEFINE_TEST_CASE(TestGetLeaderboardAsyncInvalidArgs)
    {
        TEST_LOG(L"Test starting: TestGetLeaderboardAsyncInvalidArgs");

        TestEnvironment env{};
        
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        XAsyncBlock async{};
        XblLeaderboardQuery query{};

#pragma warning(suppress: 6387)
        VERIFY_ARE_EQUAL_INT(XblLeaderboardGetLeaderboardAsync(nullptr, query, &async), E_INVALIDARG);
        VERIFY_ARE_EQUAL_INT(XblLeaderboardGetLeaderboardAsync(xboxLiveContext.get(), query, nullptr), E_INVALIDARG);
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

