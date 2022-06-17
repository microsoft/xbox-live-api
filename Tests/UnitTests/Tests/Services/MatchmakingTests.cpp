// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"
#include "UnitTestIncludes.h"

#define DEFAULT_SCID "FEEDFACE-0000-0000-0000-000000000001"
#define DEFAULT_TEMPLATE_NAME "TestTemplate"
#define DEFAULT_SESSION_ID "5E55104-0000-0000-0000-000000000001"
#define DEFAULT_HOPPER_NAME "TestHopper"
#define DEFAULT_TICKET_ID "0584338f-a2ff-4eb9-b167-c0e8ddecae72"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

DEFINE_TEST_CLASS(MatchmakingTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(MatchmakingTests);
    static const JsonDocument testResponseJsonFromFile;
    const xsapi_internal_string defaultMatchTicketResponse = testResponseJsonFromFile.IsObject() && testResponseJsonFromFile.HasMember("defaultMatchTicketResponse") ? JsonUtils::SerializeJson(testResponseJsonFromFile["defaultMatchTicketResponse"]) : "";

    void VerifyTicket(
        XblCreateMatchTicketResponse* ticket,
        xsapi_internal_string ticketIdToCheck,
        int64_t waitTimeToCheck
        )
    {
        VERIFY_ARE_EQUAL_STR(ticket->matchTicketId, ticketIdToCheck.c_str());
        VERIFY_ARE_EQUAL_INT(ticket->estimatedWaitTime, waitTimeToCheck);
    }

    void VerifyMultiplayerSessionReference(
        XblMultiplayerSessionReference* result,
        const JsonValue& resultToVerify
        )
    {
        VERIFY_ARE_EQUAL_STR(result->Scid, resultToVerify["scid"].GetString());
        VERIFY_ARE_EQUAL_STR(result->SessionTemplateName, resultToVerify["templateName"].GetString());
        VERIFY_ARE_EQUAL_STR(result->SessionName, resultToVerify["name"].GetString());
    }

    char* ConvertMatchStatusToString(
        XblTicketStatus ticketStatus
        )
    {
        switch(ticketStatus)
        {
        case XblTicketStatus::Canceled:
            return "Canceled";
        case XblTicketStatus::Expired:
            return "Expired";
        case XblTicketStatus::Found:
            return "Found";
        case XblTicketStatus::Searching:
            return "Searching";
        case XblTicketStatus::Unknown:
            return "Unknown";
        default:
            return "";
        }
    }

    char* ConvertPerserveSessionModeToString(
        XblPreserveSessionMode preserveSessionMode
        )
    {
        switch (preserveSessionMode)
        {
        case XblPreserveSessionMode::Always:
            return "always";
        case XblPreserveSessionMode::Never:
            return "never";
        case XblPreserveSessionMode::Unknown:
            return "unknown";
        default:
            return "";
        }
    }

    void VerifyTicketDetails(
        XblMatchTicketDetailsResponse* ticket,
        const JsonValue& json
        )
    {
        VERIFY_ARE_EQUAL_INT(ticket->estimatedWaitTime, json["waitTime"].GetInt());
        VERIFY_ARE_EQUAL_STR(ConvertMatchStatusToString(ticket->matchStatus),  json["ticketStatus"].GetString());
        VERIFY_ARE_EQUAL_STR(ConvertPerserveSessionModeToString(ticket->preserveSession), json["preserveSession"].GetString());

        VerifyMultiplayerSessionReference(&ticket->ticketSession, json["ticketSessionRef"]);
        VerifyMultiplayerSessionReference(&ticket->targetSession, json["targetSessionRef"]);

        JsonDocument ticketAttrJson;
        ticketAttrJson.Parse(ticket->ticketAttributes);
        VERIFY_IS_EQUAL_JSON(ticketAttrJson, json["ticketAttributes"]);
    }

    DEFINE_TEST_CASE(TestCreateMatchTicketAsync)
    {
        TEST_LOG(L"Test starting: TestCreateMatchTicketAsync");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        int64_t timeoutInSeconds = 10;

        XblMultiplayerSessionReference sessionRef{ DEFAULT_SCID, DEFAULT_TEMPLATE_NAME, DEFAULT_SESSION_ID };

       char ticketAttributes[] = R"({"desiredMap":"Hang 'em high", "desiredGameType":"Crazy King"})";

        char expectedRequest[] =
            R"({"giveUpDuration":10,"preserveSession":"never","ticketAttributes":{"desiredGameType":"Crazy King","desiredMap":"Hang 'em high"},"ticketSessionRef":{"name":"5E55104-0000-0000-0000-000000000001","scid":"FEEDFACE-0000-0000-0000-000000000001","templateName":"TestTemplate"}})";

        xsapi_internal_string matchmakingUri = "https://smartmatch.xboxlive.com/serviceconfigs/07617C5B-3423-4505-B6C6-10A16E1E5DDB/hoppers/DeathMatch";

        JsonDocument expectedResponse; 
        expectedResponse.Parse(R"({"ticketId":"0584338f-a2ff-4eb9-b167-c0e8ddecae72", "waitTime":60 })");

        auto mock = std::make_shared<HttpMock>( "POST", matchmakingUri );
        mock->SetResponseBody(expectedResponse);

        bool requestWellFormed { true };
        mock->SetMockMatchedCallback(
            [&](HttpMock* mock, xsapi_internal_string uri, xsapi_internal_string body)
            {
                UNREFERENCED_PARAMETER(mock);
                UNREFERENCED_PARAMETER(uri);
                requestWellFormed &= VerifyJson(expectedRequest, body.data());
            });

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblMatchmakingCreateMatchTicketAsync(
            xboxLiveContext.get(),
            sessionRef,
            "07617C5B-3423-4505-B6C6-10A16E1E5DDB", // serviceConfigurationId
            "DeathMatch", // hopperName
            timeoutInSeconds,
            XblPreserveSessionMode::Never,
            ticketAttributes,
            &async
        ));

        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_IS_TRUE(requestWellFormed);

        XblCreateMatchTicketResponse ticket;
        VERIFY_SUCCEEDED(XblMatchmakingCreateMatchTicketResult(&async, &ticket));

        int64_t giveUpDurationToVerify = 60; // 60 seconds

        VerifyTicket(
            &ticket,
            "0584338f-a2ff-4eb9-b167-c0e8ddecae72", // ticketIdToCheck
            giveUpDurationToVerify // waitTimeToCheck
            );
    }

    DEFINE_TEST_CASE(TestCreateMatchTicketAsync_EmptyResult)
    {
        TEST_LOG(L"Test starting: TestCreateMatchTicketAsync_EmptyResult");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        int64_t timeoutInSeconds = 10;

        XblMultiplayerSessionReference sessionRef{ DEFAULT_SCID, DEFAULT_TEMPLATE_NAME, DEFAULT_SESSION_ID };

        JsonDocument expectedResponse;
        expectedResponse.Parse(R"({})");

        char expectedRequest[] =
            R"({"giveUpDuration":10,"preserveSession":"always","ticketSessionRef":{"name":"5E55104-0000-0000-0000-000000000001","scid":"FEEDFACE-0000-0000-0000-000000000001","templateName":"TestTemplate"}})";

        xsapi_internal_string matchmakingUri = "https://smartmatch.xboxlive.com/serviceconfigs/07617C5B-3423-4505-B6C6-10A16E1E5DDB/hoppers/DeathMatch";

        auto mock = std::make_shared<HttpMock>( "POST", matchmakingUri );
        mock->SetResponseBody(expectedResponse);

        bool requestWellFormed{ true };
        mock->SetMockMatchedCallback(
            [&](HttpMock* mock, xsapi_internal_string uri, xsapi_internal_string body)
            {
                UNREFERENCED_PARAMETER(mock);
                UNREFERENCED_PARAMETER(uri);
                requestWellFormed &= VerifyJson(expectedRequest, body.data());
            });

        //should return E_INVALIDARG
        XAsyncBlock async{};
        HRESULT hr = XblMatchmakingCreateMatchTicketAsync(
            xboxLiveContext.get(),
            sessionRef,
            "07617C5B-3423-4505-B6C6-10A16E1E5DDB", // serviceConfigurationId
            "DeathMatch", // hopperName
            timeoutInSeconds,
            XblPreserveSessionMode::Always,
            nullptr,
            &async
        );

        VERIFY_ARE_EQUAL(hr, E_INVALIDARG);
        VERIFY_IS_TRUE(requestWellFormed);
    }

    DEFINE_TEST_CASE(TestDeleteMatchTicketAsync)
    {
        TEST_LOG(L"Test starting: TestDeleteMatchTicketAsync");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        xsapi_internal_string matchmakingUri = "https://smartmatch.xboxlive.com/serviceconfigs/FEEDFACE-0000-0000-0000-000000000001/hoppers/TestHopper/tickets/0584338f-a2ff-4eb9-b167-c0e8ddecae72";

        HttpMock mock{ "DELETE", matchmakingUri };

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblMatchmakingDeleteMatchTicketAsync(
            xboxLiveContext.get(),
            DEFAULT_SCID,
            DEFAULT_HOPPER_NAME,
            DEFAULT_TICKET_ID,
            &async
        ));

        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
    }

    DEFINE_TEST_CASE(TestGetMatchTicketAsync)
    {
        TEST_LOG(L"Test starting: TestGetMatchTicketAsync");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        JsonDocument expectedResponse;
        expectedResponse.Parse(defaultMatchTicketResponse.c_str());

        xsapi_internal_string matchmakingUri = "https://smartmatch.xboxlive.com/serviceconfigs/FEEDFACE-0000-0000-0000-000000000001/hoppers/TestHopper/tickets/0584338f-a2ff-4eb9-b167-c0e8ddecae72";

        HttpMock mock{ "GET", matchmakingUri };
        mock.SetResponseBody(expectedResponse);

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblMatchmakingGetMatchTicketDetailsAsync(
            xboxLiveContext.get(),
            DEFAULT_SCID,
            DEFAULT_HOPPER_NAME,
            DEFAULT_TICKET_ID,
            &async
        ));

        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));

        size_t resultSize = 0;
        VERIFY_SUCCEEDED(XblMatchmakingGetMatchTicketDetailsResultSize(&async, &resultSize));
        VERIFY_IS_TRUE(resultSize > 0);
        std::vector<char> buffer(resultSize, 0);
        XblMatchTicketDetailsResponse* ticketPtr;
        VERIFY_SUCCEEDED(XblMatchmakingGetMatchTicketDetailsResult(&async, resultSize, buffer.data(), &ticketPtr, nullptr));

        VerifyTicketDetails(ticketPtr, expectedResponse);
    }

    DEFINE_TEST_CASE(TestGetMatchTicketWithLargeBufferAsync)
    {
        TEST_LOG(L"Test starting: TestGetMatchTicketWithLargeBufferAsync");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        JsonDocument expectedResponse;
        expectedResponse.Parse(defaultMatchTicketResponse.c_str());

        xsapi_internal_string matchmakingUri = "https://smartmatch.xboxlive.com/serviceconfigs/FEEDFACE-0000-0000-0000-000000000001/hoppers/TestHopper/tickets/0584338f-a2ff-4eb9-b167-c0e8ddecae72";

        HttpMock mock{ "GET", matchmakingUri };
        mock.SetResponseBody(expectedResponse);

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblMatchmakingGetMatchTicketDetailsAsync(
            xboxLiveContext.get(),
            DEFAULT_SCID,
            DEFAULT_HOPPER_NAME,
            DEFAULT_TICKET_ID,
            &async
        ));

        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));

        size_t resultSize{};
        size_t bufferUsed{};
        VERIFY_SUCCEEDED(XblMatchmakingGetMatchTicketDetailsResultSize(&async, &resultSize));
        VERIFY_IS_TRUE(resultSize > 0);
        std::vector<char> buffer(resultSize * 2, 0);
        XblMatchTicketDetailsResponse* ticketPtr;
        VERIFY_SUCCEEDED(XblMatchmakingGetMatchTicketDetailsResult(&async, resultSize * 2, buffer.data(), &ticketPtr, &bufferUsed));
        VERIFY_ARE_EQUAL_UINT(resultSize, bufferUsed);

        VerifyTicketDetails(ticketPtr, expectedResponse);
    }

    DEFINE_TEST_CASE(TestGetStatsForHopperAsync)
    {
        TEST_LOG(L"Test starting: TestGetStatsForHopperAsync");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        const xsapi_internal_string response = R"({"name":"gameawesome2","waitTime":30,"population":1})";

        JsonDocument expectedResponse;
        expectedResponse.Parse(response.c_str());

        xsapi_internal_string matchmakingUri = "https://smartmatch.xboxlive.com/serviceconfigs/FEEDFACE-0000-0000-0000-000000000001/hoppers/TestHopper/stats";

        HttpMock mock{ "GET", matchmakingUri };
        mock.SetResponseBody(expectedResponse);

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblMatchmakingGetHopperStatisticsAsync(
            xboxLiveContext.get(),
            DEFAULT_SCID,
            DEFAULT_HOPPER_NAME,
            &async
        ));

        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));

        size_t resultSize = 0;
        VERIFY_SUCCEEDED(XblMatchmakingGetHopperStatisticsResultSize(&async, &resultSize));
        VERIFY_IS_TRUE(resultSize > 0);
        std::vector<char> buffer(resultSize, 0);
        XblHopperStatisticsResponse* hopper{};
        VERIFY_SUCCEEDED(XblMatchmakingGetHopperStatisticsResult(&async, resultSize, buffer.data(), &hopper, nullptr));

        VERIFY_ARE_EQUAL_STR(hopper->hopperName, "gameawesome2");
        VERIFY_ARE_EQUAL_INT(hopper->estimatedWaitTime, 30); // 30 seconds
        VERIFY_ARE_EQUAL_INT(hopper->playersWaitingToMatch, 1U);
    }

    DEFINE_TEST_CASE(TestGetStatsForHopperWithLargeBufferAsync)
    {
        TEST_LOG(L"Test starting: TestGetStatsForHopperWithLargeBufferAsync");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        const xsapi_internal_string response = R"({"name":"gameawesome2","waitTime":30,"population":1})";

        JsonDocument expectedResponse;
        expectedResponse.Parse(response.c_str());

        xsapi_internal_string matchmakingUri = "https://smartmatch.xboxlive.com/serviceconfigs/FEEDFACE-0000-0000-0000-000000000001/hoppers/TestHopper/stats";

        HttpMock mock{ "GET", matchmakingUri };
        mock.SetResponseBody(expectedResponse);

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblMatchmakingGetHopperStatisticsAsync(
            xboxLiveContext.get(),
            DEFAULT_SCID,
            DEFAULT_HOPPER_NAME,
            &async
        ));

        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));

        size_t resultSize{};
        size_t bufferUsed{};
        VERIFY_SUCCEEDED(XblMatchmakingGetHopperStatisticsResultSize(&async, &resultSize));
        VERIFY_IS_TRUE(resultSize > 0);
        std::vector<char> buffer(resultSize * 2, 0);
        XblHopperStatisticsResponse* hopper{};
        VERIFY_SUCCEEDED(XblMatchmakingGetHopperStatisticsResult(&async, resultSize * 2, buffer.data(), &hopper, &bufferUsed));
        VERIFY_ARE_EQUAL_UINT(resultSize, bufferUsed); 

        VERIFY_ARE_EQUAL_STR(hopper->hopperName, "gameawesome2");
        VERIFY_ARE_EQUAL_INT(hopper->estimatedWaitTime, 30); // 30 seconds
        VERIFY_ARE_EQUAL_INT(hopper->playersWaitingToMatch, 1U);
    }

    DEFINE_TEST_CASE(TestInvalidArgument)
    {
        TEST_LOG(L"Test starting: TestInvalidArgument");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        int64_t ticketTimeout = 0;

        char ticketAttributes[] = "";

        XblMultiplayerSessionReference sessionRef{ "scid", "tempname", "sessionid" };

        XAsyncBlock async{};

        VERIFY_ARE_EQUAL(XblMatchmakingCreateMatchTicketAsync(nullptr, sessionRef, "configId", "hopperName", ticketTimeout, XblPreserveSessionMode::Always, ticketAttributes, &async), E_INVALIDARG);
        VERIFY_ARE_EQUAL(XblMatchmakingCreateMatchTicketAsync(xboxLiveContext.get(), sessionRef, nullptr, "hopperName", ticketTimeout, XblPreserveSessionMode::Always, ticketAttributes, &async), E_INVALIDARG);
        VERIFY_ARE_EQUAL(XblMatchmakingCreateMatchTicketAsync(xboxLiveContext.get(), sessionRef, "configId", nullptr, ticketTimeout, XblPreserveSessionMode::Always, ticketAttributes, &async), E_INVALIDARG);
        VERIFY_ARE_EQUAL(XblMatchmakingCreateMatchTicketAsync(xboxLiveContext.get(), sessionRef, "configId", "hopperName", ticketTimeout, XblPreserveSessionMode::Always, nullptr, &async), E_INVALIDARG);
        VERIFY_ARE_EQUAL(XblMatchmakingCreateMatchTicketAsync(xboxLiveContext.get(), sessionRef, "configId", "hopperName", ticketTimeout, XblPreserveSessionMode::Always, ticketAttributes, nullptr), E_INVALIDARG);

        VERIFY_ARE_EQUAL(XblMatchmakingDeleteMatchTicketAsync(nullptr, "configId", "hopperName", "ticketId", &async), E_INVALIDARG);
        VERIFY_ARE_EQUAL(XblMatchmakingDeleteMatchTicketAsync(xboxLiveContext.get(), nullptr, "hopperName", "ticketId", &async), E_INVALIDARG);
        VERIFY_ARE_EQUAL(XblMatchmakingDeleteMatchTicketAsync(xboxLiveContext.get(), "configId", nullptr, "ticketId", &async), E_INVALIDARG);
        VERIFY_ARE_EQUAL(XblMatchmakingDeleteMatchTicketAsync(xboxLiveContext.get(), "configId", "hopperName", nullptr, &async), E_INVALIDARG);
        VERIFY_ARE_EQUAL(XblMatchmakingDeleteMatchTicketAsync(xboxLiveContext.get(), "configId", "hopperName", "ticketId", nullptr), E_INVALIDARG);

        VERIFY_ARE_EQUAL(XblMatchmakingGetMatchTicketDetailsAsync(nullptr, "configId", "hopperName", "ticketId", &async), E_INVALIDARG);
        VERIFY_ARE_EQUAL(XblMatchmakingGetMatchTicketDetailsAsync(xboxLiveContext.get(), nullptr, "hopperName", "ticketId", &async), E_INVALIDARG);
        VERIFY_ARE_EQUAL(XblMatchmakingGetMatchTicketDetailsAsync(xboxLiveContext.get(), "configId", nullptr, "ticketId", &async), E_INVALIDARG);
        VERIFY_ARE_EQUAL(XblMatchmakingGetMatchTicketDetailsAsync(xboxLiveContext.get(), "configId", "hopperName", nullptr, &async), E_INVALIDARG);
        VERIFY_ARE_EQUAL(XblMatchmakingGetMatchTicketDetailsAsync(xboxLiveContext.get(), "configId", "hopperName", "ticketId", nullptr), E_INVALIDARG);

        VERIFY_ARE_EQUAL(XblMatchmakingGetHopperStatisticsAsync(nullptr, "configId", "hopperName", &async), E_INVALIDARG);
        VERIFY_ARE_EQUAL(XblMatchmakingGetHopperStatisticsAsync(xboxLiveContext.get(), nullptr, "hopperName", &async), E_INVALIDARG);
        VERIFY_ARE_EQUAL(XblMatchmakingGetHopperStatisticsAsync(xboxLiveContext.get(), "configId", nullptr, &async), E_INVALIDARG);
        VERIFY_ARE_EQUAL(XblMatchmakingGetHopperStatisticsAsync(xboxLiveContext.get(), "configId", "hopperName", nullptr), E_INVALIDARG);
    }
};

const JsonDocument MatchmakingTests::testResponseJsonFromFile{ GetTestResponses("TestResponses\\Matchmaking.json") };

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

