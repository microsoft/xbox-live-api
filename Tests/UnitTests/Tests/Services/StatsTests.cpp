// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "UnitTestIncludes.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

const char* statsServer = "https://userstats.xboxlive.com";
const char* batchUrl = "https://userstats.xboxlive.com/batch?operation=read";
const uint64_t xuid{ 2533274792693551 };

const char* defaultRtaStat =
R"({
        "name": "Stat",
        "type": "Integer",
        "value": "31"
    })";

const char* updatedRtaStat =
R"({
        "name": "Stat",
        "type": "Integer",
        "value": "32"
    })";

const char* defaultSingleUserStatsResponse =
R"({
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
    })";

const char* defaultBatchUsersStatsResponse =
R"({
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
    })";

DEFINE_TEST_CLASS(UserStatsTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(UserStatsTests);

    const char* rtaScid{ "12345" };
    Event rtaMessageReceived{};
    bool rtaMessageWellFormed{ true };
    char rtaResultName[8]{};
    char rtaResultValue[8]{};
    char rtaResultType[8]{};

    void VerifyStat(XblStatistic stat, JsonValue statToVerify, bool isRta)
    {
        const char* name = isRta ? "name" : "statname";
        VERIFY_ARE_EQUAL_STR(stat.statisticName, statToVerify[name].GetString());
        VERIFY_ARE_EQUAL_STR(stat.value, statToVerify["value"].GetString());
        VERIFY_ARE_EQUAL_STR(stat.statisticType, statToVerify["type"].GetString());
    }

    void VerifyServiceConfigurationStatistic(XblServiceConfigurationStatistic scid, JsonValue scidToVerify, bool isSingleUser)
    {   
        if (!isSingleUser)
        {
            VERIFY_ARE_EQUAL_STR(scid.serviceConfigurationId, scidToVerify["scid"].GetString());
        }

        auto statsJson = scidToVerify["stats"].GetArray();
        VERIFY_ARE_EQUAL_UINT(scid.statisticsCount, statsJson.Size());

        // scids/*/stats/*
        uint32_t i{ 0 };
        for (auto& statJson : statsJson)
        {
            VerifyStat(scid.statistics[i], statJson.GetObjectW(), false);
            ++i;
        }
    }

    void VerifyUserStatisticsResult(XblUserStatisticsResult* result, JsonValue resultJson, bool isSingleUser)
    {
        VERIFY_IS_NOT_NULL(result);
        VERIFY_ARE_EQUAL_STR(Utils::StringFromUint64(result->xboxUserId), resultJson["xuid"].GetString());

        // scids
        auto scids = result->serviceConfigStatistics;
        auto scidsJson = resultJson["scids"].GetArray();
        VERIFY_ARE_EQUAL_UINT(result->serviceConfigStatisticsCount, scidsJson.Size());

        uint32_t i{ 0 };
        for (auto& scidJson : scidsJson)
        {
            VerifyServiceConfigurationStatistic(scids[i], scidJson.GetObjectW(), isSingleUser);
            ++i;
        }
    }

    XblRequestedStatistics CreateRequestedStat(char* scid, const char** statNames, uint32_t statCount)
    {
        XblRequestedStatistics requestedStat{};
        requestedStat.statistics = statNames;
        requestedStat.statisticsCount = statCount;
        strcpy_s(requestedStat.serviceConfigurationId, scid);

        return requestedStat;
    }

    void TestSingleUserStat(const char* scid, const char* statName, uint32_t bufferSizeMultiplier)
    {
        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        xsapi_internal_stringstream url;
        url << statsServer << "/users/xuid(" << xuid << ")/scids/" << scid << "/stats/" << statName;
        HttpMock mock("GET", url.str(), 200);
        mock.SetResponseBody(defaultSingleUserStatsResponse);

        XAsyncBlock async{};
        size_t resultSize{};
        VERIFY_SUCCEEDED(XblUserStatisticsGetSingleUserStatisticAsync(xboxLiveContext.get(), xuid, scid, statName, &async));
        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_SUCCEEDED(XblUserStatisticsGetSingleUserStatisticResultSize(&async, &resultSize));

        size_t bufferUsed{};
        XblUserStatisticsResult* result{};
        std::shared_ptr<char> buffer(new char[resultSize * bufferSizeMultiplier], std::default_delete<char[]>());
        VERIFY_SUCCEEDED(XblUserStatisticsGetSingleUserStatisticResult(&async, resultSize * bufferSizeMultiplier, buffer.get(), &result, &bufferUsed));
        VERIFY_ARE_EQUAL_UINT(resultSize, bufferUsed);

        JsonDocument responseJson;
        responseJson.Parse(defaultSingleUserStatsResponse);
        VerifyUserStatisticsResult(result, responseJson.GetObjectW(), true);
    }

    DEFINE_TEST_CASE(TestGetSingleUserStatistics1)
    {
        TEST_LOG(L"Test starting: TestGetSingleUserStatistics1");

        const char* scid{ "7492baca-c1b4-440d-a391-b7ef364a8d40" };
        const char* statName{ "OverallReputation" };

        TestSingleUserStat(scid, statName, 1);
    }

    DEFINE_TEST_CASE(TestGetSingleUserStatistics2)
    {
        TEST_LOG(L"Test starting: TestGetSingleUserStatistics2");

        const char* scid{ "7492baca-c1b4-440d-a391-b7ef364a8d41" };
        const char* statName{ "UserContentReputation" };

        TestSingleUserStat(scid, statName, 1);
    }

    DEFINE_TEST_CASE(TestGetSingleUserStatisticsWithLargeBuffer1)
    {
        TEST_LOG(L"Test starting: TestGetSingleUserStatisticsWithLargeBuffer1");

        const char* scid{ "7492baca-c1b4-440d-a391-b7ef364a8d40" };
        const char* statName{ "OverallReputation" };

        TestSingleUserStat(scid, statName, 2);
    }

    DEFINE_TEST_CASE(TestGetSingleUserStatisticsWithLargeBuffer2)
    {
        TEST_LOG(L"Test starting: TestGetSingleUserStatisticsWithLargeBuffer2");

        const char* scid{ "7492baca-c1b4-440d-a391-b7ef364a8d41" };
        const char* statName{ "UserContentReputation" };

        TestSingleUserStat(scid, statName, 2);
    }

    void TestSingleUserStats(const char* scid, std::vector<const char*> statNames, uint32_t bufferSizeMultiplier)
    {
        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        char delimiter{};
        xsapi_internal_stringstream url;
        url << statsServer << "/users/xuid(" << xuid << ")/scids/" << scid << "/stats/";
        for (auto stat : statNames)
        {
            url << delimiter << stat;
            delimiter = ',';
        }
        HttpMock mock("GET", url.str(), 200);
        mock.SetResponseBody(defaultSingleUserStatsResponse);

        XAsyncBlock async{};
        size_t resultSize{};
        VERIFY_SUCCEEDED(XblUserStatisticsGetSingleUserStatisticsAsync(xboxLiveContext.get(), xuid, scid, statNames.data(), 1, &async));
        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_SUCCEEDED(XblUserStatisticsGetSingleUserStatisticsResultSize(&async, &resultSize));

        size_t bufferUsed{};
        XblUserStatisticsResult* result{};
        std::shared_ptr<char> buffer(new char[resultSize * bufferSizeMultiplier], std::default_delete<char[]>());
        VERIFY_SUCCEEDED(XblUserStatisticsGetSingleUserStatisticsResult(&async, resultSize * bufferSizeMultiplier, buffer.get(), &result, &bufferUsed));
        VERIFY_ARE_EQUAL_UINT(resultSize, bufferUsed);

        JsonDocument responseJson;
        responseJson.Parse(defaultSingleUserStatsResponse);
        VerifyUserStatisticsResult(result, responseJson.GetObjectW(), true);
    }

    DEFINE_TEST_CASE(TestGetSingleUserStatistics)
    {
        TEST_LOG(L"Test starting: TestGetSingleUserStatistics");

        const char* scid{ "7492baca-c1b4-440d-a391-b7ef364a8d40" };
        std::vector<const char*> statNames{ "OverallReputation", "UserContentReputation" };

        TestSingleUserStats(scid, statNames, 1);
    }

    DEFINE_TEST_CASE(TestGetSingleUserStatisticsWithLargeBuffer)
    {
        TEST_LOG(L"Test starting: TestGetSingleUserStatisticsWithLargeBuffer");

        const char* scid{ "7492baca-c1b4-440d-a391-b7ef364a8d40" };
        std::vector<const char*> statNames{ "OverallReputation", "UserContentReputation" };

        TestSingleUserStats(scid, statNames, 2);
    }

    DEFINE_TEST_CASE(TestGetBatchUserStatistics)
    {
        TEST_LOG(L"Test starting: TestGetBatchUserStatistics");

        TestEnvironment env{};

        const uint32_t xuidCount{ 2 };
        const uint32_t nameCount{ 1 };
        const char* scid{ "serviceConfigId" };
        uint64_t xuids[xuidCount]{ 2533274792693551, 2533274792693552 };
        const char* statNames[nameCount]{ "namename" };
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        auto mock = std::make_shared<HttpMock>("POST", batchUrl, 200);
        mock->SetResponseBody(defaultBatchUsersStatsResponse);
        
        bool requestWellFormed{ true };
        mock->SetMockMatchedCallback(
            [&requestWellFormed, xuids, scid, statNames](HttpMock* mock, xsapi_internal_string requestUrl, xsapi_internal_string requestBody)
            {
                UNREFERENCED_PARAMETER(mock);
                UNREFERENCED_PARAMETER(requestUrl);

                JsonDocument requestJson;
                requestJson.Parse(requestBody.c_str());

                int userIndex{ 0 };
                for (auto& requestXuid : requestJson["requestedusers"].GetArray())
                {
                    requestWellFormed &= strcmp(Utils::StringFromUint64(xuids[userIndex]).c_str(), requestXuid.GetString()) == 0;
                    ++userIndex;
                }

                auto& requestScid = requestJson["requestedscids"].GetArray()[0];
                requestWellFormed &= strcmp(scid, requestScid["scid"].GetString()) == 0;

                int nameIndex{ 0 };
                for (auto& requestStatName : requestScid["requestedstats"].GetArray())
                {
                    requestWellFormed &= strcmp(statNames[nameIndex], requestStatName.GetString()) == 0;
                    ++nameIndex;
                }
            }
        );

        XAsyncBlock async{};
        size_t resultSize{};
        VERIFY_SUCCEEDED(XblUserStatisticsGetMultipleUserStatisticsAsync(xboxLiveContext.get(), xuids, xuidCount, scid, statNames, nameCount, &async));
        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_IS_TRUE(requestWellFormed);
        VERIFY_SUCCEEDED(XblUserStatisticsGetMultipleUserStatisticsResultSize(&async, &resultSize));

        size_t resultCount{};
        XblUserStatisticsResult* results{};
        std::shared_ptr<char> buffer(new char[resultSize], std::default_delete<char[]>());
        VERIFY_SUCCEEDED(XblUserStatisticsGetMultipleUserStatisticsResult(&async, resultSize, buffer.get(), &results, &resultCount, nullptr));
        VERIFY_IS_NOT_NULL(results);

        JsonDocument responseJson;
        responseJson.Parse(defaultBatchUsersStatsResponse);
        VERIFY_ARE_EQUAL_INT(resultCount, responseJson["users"].Size());

        uint32_t i{ 0 };
        for (auto& userJson : responseJson["users"].GetArray())
        {
            VerifyUserStatisticsResult(&results[i], userJson.GetObjectW(), false);
            ++i;
        }
    }

    DEFINE_TEST_CASE(TestGetBatchUserStatisticsWithLargeBuffer)
    {
        TEST_LOG(L"Test starting: TestGetBatchUserStatisticsWithLargeBuffer");

        TestEnvironment env{};

        const uint32_t xuidCount{ 2 };
        const uint32_t nameCount{ 1 };
        const char* scid{ "serviceConfigId" };
        uint64_t xuids[xuidCount]{ 2533274792693551, 2533274792693552 };
        const char* statNames[nameCount]{ "namename" };
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        auto mock = std::make_shared<HttpMock>("POST", batchUrl, 200);
        mock->SetResponseBody(defaultBatchUsersStatsResponse);

        bool requestWellFormed{ true };
        mock->SetMockMatchedCallback(
            [&requestWellFormed, xuids, scid, statNames](HttpMock* mock, xsapi_internal_string requestUrl, xsapi_internal_string requestBody)
        {
            UNREFERENCED_PARAMETER(mock);
            UNREFERENCED_PARAMETER(requestUrl);

            JsonDocument requestJson;
            requestJson.Parse(requestBody.c_str());

            int userIndex{ 0 };
            for (auto& requestXuid : requestJson["requestedusers"].GetArray())
            {
                requestWellFormed &= strcmp(Utils::StringFromUint64(xuids[userIndex]).c_str(), requestXuid.GetString()) == 0;
                ++userIndex;
            }

            auto& requestScid = requestJson["requestedscids"].GetArray()[0];
            requestWellFormed &= strcmp(scid, requestScid["scid"].GetString()) == 0;

            int nameIndex{ 0 };
            for (auto& requestStatName : requestScid["requestedstats"].GetArray())
            {
                requestWellFormed &= strcmp(statNames[nameIndex], requestStatName.GetString()) == 0;
                ++nameIndex;
            }
        }
        );

        XAsyncBlock async{};
        size_t resultSize{};
        VERIFY_SUCCEEDED(XblUserStatisticsGetMultipleUserStatisticsAsync(xboxLiveContext.get(), xuids, xuidCount, scid, statNames, nameCount, &async));
        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_IS_TRUE(requestWellFormed);
        VERIFY_SUCCEEDED(XblUserStatisticsGetMultipleUserStatisticsResultSize(&async, &resultSize));

        size_t bufferUsed{};
        size_t resultCount{};
        XblUserStatisticsResult* results{};
        std::shared_ptr<char> buffer(new char[resultSize * 2], std::default_delete<char[]>());
        VERIFY_SUCCEEDED(XblUserStatisticsGetMultipleUserStatisticsResult(&async, resultSize * 2, buffer.get(), &results, &resultCount, &bufferUsed));
        VERIFY_IS_NOT_NULL(results);
        VERIFY_ARE_EQUAL_UINT(resultSize, bufferUsed);

        JsonDocument responseJson;
        responseJson.Parse(defaultBatchUsersStatsResponse);
        VERIFY_ARE_EQUAL_INT(resultCount, responseJson["users"].Size());

        uint32_t i{ 0 };
        for (auto& userJson : responseJson["users"].GetArray())
        {
            VerifyUserStatisticsResult(&results[i], userJson.GetObjectW(), false);
            ++i;
        }
    }

    DEFINE_TEST_CASE(TestGetBatchUserStatisticsForMultipleServiceConfigs)
    {
        TEST_LOG(L"Test starting: TestGetBatchUserStatisticsForMultipleServiceConfigs");

        TestEnvironment env{};

        const uint32_t xuidCount{ 1 };
        const uint32_t nameCount{ 1 };
        const uint32_t requestedStatCount{ 2 };
        uint64_t xuids[xuidCount]{ 2533274792693551 };
        const char* statNames[nameCount]{ "namename" };
        XblRequestedStatistics requestedStats[requestedStatCount]
        {
            CreateRequestedStat("serviceConfigId1", statNames, nameCount),
            CreateRequestedStat("serviceConfigId2", statNames, nameCount)
        };
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        auto mock = std::make_shared<HttpMock>("POST", batchUrl, 200);
        mock->SetResponseBody(defaultBatchUsersStatsResponse);

        bool requestWellFormed{ true };
        mock->SetMockMatchedCallback(
            [&requestWellFormed, xuids, requestedStats](HttpMock* mock, xsapi_internal_string requestUrl, xsapi_internal_string requestBody)
            {
                UNREFERENCED_PARAMETER(mock);
                UNREFERENCED_PARAMETER(requestUrl);

                JsonDocument requestJson;
                requestJson.Parse(requestBody.c_str());

                int userIndex{ 0 };
                for (auto& requestXuid : requestJson["requestedusers"].GetArray())
                {
                    requestWellFormed &= strcmp(Utils::StringFromUint64(xuids[userIndex]).c_str(), requestXuid.GetString()) == 0;
                    ++userIndex;
                }

                int requestedStatIndex{ 0 };
                for (auto& requestScid : requestJson["requestedscids"].GetArray())
                {
                    auto stat = requestedStats[requestedStatIndex];
                    requestWellFormed &= strcmp(stat.serviceConfigurationId, requestScid["scid"].GetString()) == 0;
                    ++requestedStatIndex;

                    int statIndex{ 0 };
                    for (auto& requestStatName : requestScid["requestedstats"].GetArray())
                    {
                        requestWellFormed &= strcmp(stat.statistics[statIndex], requestStatName.GetString()) == 0;
                        ++statIndex;
                    }
                }
            }
        );

        XAsyncBlock async{};
        size_t resultSize{};
        VERIFY_SUCCEEDED(XblUserStatisticsGetMultipleUserStatisticsForMultipleServiceConfigurationsAsync(xboxLiveContext.get(), xuids, xuidCount, requestedStats, requestedStatCount, &async));
        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_IS_TRUE(requestWellFormed);
        VERIFY_SUCCEEDED(XblUserStatisticsGetMultipleUserStatisticsForMultipleServiceConfigurationsResultSize(&async, &resultSize));

        size_t resultCount{};
        XblUserStatisticsResult* results{};
        std::shared_ptr<char> buffer(new char[resultSize], std::default_delete<char[]>());
        VERIFY_SUCCEEDED(XblUserStatisticsGetMultipleUserStatisticsForMultipleServiceConfigurationsResult(&async, resultSize, buffer.get(), &results, &resultCount, nullptr));
        VERIFY_IS_NOT_NULL(results);

        JsonDocument responseJson;
        responseJson.Parse(defaultBatchUsersStatsResponse);
        VERIFY_ARE_EQUAL_INT(resultCount, responseJson["users"].Size());

        uint32_t i{ 0 };
        for (auto& userJson : responseJson["users"].GetArray())
        {
            VerifyUserStatisticsResult(&results[i], userJson.GetObjectW(), false);
            ++i;
        }
    }

    DEFINE_TEST_CASE(TestGetBatchUserStatisticsForMultipleServiceConfigsWithLargeBuffer)
    {
        TEST_LOG(L"Test starting: TestGetBatchUserStatisticsForMultipleServiceConfigsWithLargeBuffer");

        TestEnvironment env{};

        const uint32_t xuidCount{ 1 };
        const uint32_t nameCount{ 1 };
        const uint32_t requestedStatCount{ 2 };
        uint64_t xuids[xuidCount]{ 2533274792693551 };
        const char* statNames[nameCount]{ "namename" };
        XblRequestedStatistics requestedStats[requestedStatCount]
        {
            CreateRequestedStat("serviceConfigId1", statNames, nameCount),
            CreateRequestedStat("serviceConfigId2", statNames, nameCount)
        };
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        auto mock = std::make_shared<HttpMock>("POST", batchUrl, 200);
        mock->SetResponseBody(defaultBatchUsersStatsResponse);

        bool requestWellFormed{ true };
        mock->SetMockMatchedCallback(
            [&requestWellFormed, xuids, requestedStats](HttpMock* mock, xsapi_internal_string requestUrl, xsapi_internal_string requestBody)
        {
            UNREFERENCED_PARAMETER(mock);
            UNREFERENCED_PARAMETER(requestUrl);

            JsonDocument requestJson;
            requestJson.Parse(requestBody.c_str());

            int userIndex{ 0 };
            for (auto& requestXuid : requestJson["requestedusers"].GetArray())
            {
                requestWellFormed &= strcmp(Utils::StringFromUint64(xuids[userIndex]).c_str(), requestXuid.GetString()) == 0;
                ++userIndex;
            }

            int requestedStatIndex{ 0 };
            for (auto& requestScid : requestJson["requestedscids"].GetArray())
            {
                auto stat = requestedStats[requestedStatIndex];
                requestWellFormed &= strcmp(stat.serviceConfigurationId, requestScid["scid"].GetString()) == 0;
                ++requestedStatIndex;

                int statIndex{ 0 };
                for (auto& requestStatName : requestScid["requestedstats"].GetArray())
                {
                    requestWellFormed &= strcmp(stat.statistics[statIndex], requestStatName.GetString()) == 0;
                    ++statIndex;
                }
            }
        }
        );

        XAsyncBlock async{};
        size_t resultSize{};
        VERIFY_SUCCEEDED(XblUserStatisticsGetMultipleUserStatisticsForMultipleServiceConfigurationsAsync(xboxLiveContext.get(), xuids, xuidCount, requestedStats, requestedStatCount, &async));
        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_IS_TRUE(requestWellFormed);
        VERIFY_SUCCEEDED(XblUserStatisticsGetMultipleUserStatisticsForMultipleServiceConfigurationsResultSize(&async, &resultSize));

        size_t bufferUsed{};
        size_t resultCount{};
        XblUserStatisticsResult* results{};
        std::shared_ptr<char> buffer(new char[resultSize * 2], std::default_delete<char[]>());
        VERIFY_SUCCEEDED(XblUserStatisticsGetMultipleUserStatisticsForMultipleServiceConfigurationsResult(&async, resultSize * 2, buffer.get(), &results, &resultCount, &bufferUsed));
        VERIFY_IS_NOT_NULL(results);
        VERIFY_ARE_EQUAL_UINT(resultSize, bufferUsed);

        JsonDocument responseJson;
        responseJson.Parse(defaultBatchUsersStatsResponse);
        VERIFY_ARE_EQUAL_INT(resultCount, responseJson["users"].Size());

        uint32_t i{ 0 };
        for (auto& userJson : responseJson["users"].GetArray())
        {
            VerifyUserStatisticsResult(&results[i], userJson.GetObjectW(), false);
            ++i;
        }
    }

    DEFINE_TEST_CASE(TestRTAStatistics)
    {
        TEST_LOG(L"Test starting: TestRTAStatistics");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext(xuid);
        auto& mockRtaService{ MockRealTimeActivityService::Instance() };

        const uint32_t subId{ 321 };
        const char* statName{ "Stat" };
        xsapi_internal_stringstream statsRtaUri;
        statsRtaUri << statsServer << "/users/xuid(" << xuid << ")/scids/" << rtaScid << "/stats/" << statName;

        mockRtaService.SetSubscribeHandler([&](uint32_t n, xsapi_internal_string uri)
        {
            if (uri == statsRtaUri.str())
            {
                mockRtaService.CompleteSubscribeHandshake(n, defaultRtaStat);
            }
        });

        VERIFY_SUCCEEDED(XblUserStatisticsTrackStatistics(xboxLiveContext.get(), &xuid, 1, rtaScid, &statName, 1));

        auto handlerToken = XblUserStatisticsAddStatisticChangedHandler(xboxLiveContext.get(),
            [](XblStatisticChangeEventArgs args, void* context)
            {
                auto testsContext = static_cast<UserStatsTests*>(context);

                testsContext->rtaMessageWellFormed &= xuid == args.xboxUserId;
                testsContext->rtaMessageWellFormed &= strcmp(testsContext->rtaScid, args.serviceConfigurationId) == 0;

                auto stat = args.latestStatistic;
                strcpy_s(testsContext->rtaResultName, stat.statisticName);
                strcpy_s(testsContext->rtaResultType, stat.statisticType);
                strcpy_s(testsContext->rtaResultValue, stat.value);

                testsContext->rtaMessageReceived.Set();
            }
        , this);

        rtaMessageReceived.Wait();
        VERIFY_IS_TRUE(rtaMessageWellFormed);

        XblStatistic resultStat{};
        resultStat.statisticName = rtaResultName;
        resultStat.statisticType = rtaResultType;
        resultStat.value = rtaResultValue;
        JsonDocument resultJson;
        resultJson.Parse(defaultRtaStat);
        VerifyStat(resultStat, resultJson.GetObjectW(), true);

        mockRtaService.RaiseEvent(statsRtaUri.str(), "32");
        rtaMessageReceived.Wait();
        VERIFY_IS_TRUE(rtaMessageWellFormed);

        XblStatistic updatedStat{};
        updatedStat.statisticName = rtaResultName;
        updatedStat.statisticType = rtaResultType;
        updatedStat.value = rtaResultValue;
        JsonDocument updatedJson;
        updatedJson.Parse(updatedRtaStat);
        VerifyStat(updatedStat, updatedJson.GetObjectW(), true);

        XblUserStatisticsRemoveStatisticChangedHandler(xboxLiveContext.get(), handlerToken);
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END