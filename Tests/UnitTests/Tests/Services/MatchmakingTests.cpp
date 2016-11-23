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
#define TEST_CLASS_OWNER L"adityat"
#define TEST_CLASS_AREA L"Matchmaking"
#include "UnitTestIncludes.h"
#include "matchmakingservice_winrt.h"

using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::Matchmaking;
using namespace Microsoft::Xbox::Services::Multiplayer;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;

#define DEFAULT_SCID L"FEEDFACE-0000-0000-0000-000000000001"
#define DEFAULT_TEMPLATE_NAME L"TestTemplate"
#define DEFAULT_SESSION_ID L"5E55104-0000-0000-0000-000000000001"
#define DEFAULT_HOPPER_NAME L"TestHopper"
#define DEFAULT_TICKET_ID L"0584338f-a2ff-4eb9-b167-c0e8ddecae72"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

DEFINE_TEST_CLASS(MatchmakingTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(MatchmakingTests);
    const string_t filePath = _T("\\TestResponses\\Matchmaking.json");
    web::json::value testResponseJsonFromFile = utils::read_test_response_file(filePath);
    const string_t defaultMatchTicketResponse = testResponseJsonFromFile[L"defaultMatchTicketResponse"].serialize();

    void VerifyTicket(
        CreateMatchTicketResponse^ ticket,
        Platform::String^ ticketIdToCheck,
        TimeSpan waitTimeToCheck
        )
    {
        VERIFY_ARE_EQUAL(ticket->MatchTicketId, ticketIdToCheck);
        VERIFY_ARE_EQUAL_INT(ticket->EstimatedWaitTime.Duration, waitTimeToCheck.Duration);
    }

    void VerifyMultiplayerSessionReference(
        MultiplayerSessionReference^ result,
        web::json::value resultToVerify
        )
    {
        VERIFY_ARE_EQUAL(result->ServiceConfigurationId->Data(), resultToVerify[L"scid"].as_string());
        VERIFY_ARE_EQUAL(result->SessionTemplateName->Data(), resultToVerify[L"templateName"].as_string());
        VERIFY_ARE_EQUAL(result->SessionName->Data(), resultToVerify[L"name"].as_string());
    }

    void VerifyTicketDetails(
        MatchTicketDetailsResponse^ ticket,
        web::json::value json
        )
    {
        VERIFY_ARE_EQUAL_INT(ticket->EstimatedWaitTime.Duration, json[L"waitTime"].as_integer() * TICKS_PER_SECOND);
        VERIFY_ARE_EQUAL(ticket->MatchStatus.ToString()->Data(), json[L"ticketStatus"].as_string().c_str());
        VERIFY_ARE_EQUAL_STR_IGNORE_CASE(ticket->PreserveSession.ToString()->Data(), json[L"preserveSession"].as_string().c_str());

        VerifyMultiplayerSessionReference(ticket->TicketSession, json[L"ticketSessionRef"]);
        VerifyMultiplayerSessionReference(ticket->TargetSession, json[L"targetSessionRef"]);

        auto ticketAttrJson = web::json::value::parse(ticket->TicketAttributes->Data());
        VERIFY_IS_EQUAL_JSON(ticketAttrJson, json[L"ticketAttributes"]);
    }

    DEFINE_TEST_CASE(TestCreateMatchTicketAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestCreateMatchTicketAsync);
        TimeSpan giveUpDuration;
        giveUpDuration.Duration = 10 * TICKS_PER_SECOND; // 10 seconds

        MultiplayerSessionReference^ sessionRef = ref new MultiplayerSessionReference(DEFAULT_SCID, DEFAULT_TEMPLATE_NAME, DEFAULT_SESSION_ID);

        Platform::String^ ticketAttributes = ref new Platform::String(LR"({"desiredMap":"Hang 'em high", "desiredGameType" : "Crazy King"})");

        std::wstring expectedRequest =
            LR"({"giveUpDuration":10,"preserveSession":"never","ticketAttributes":{"desiredGameType":"Crazy King","desiredMap":"Hang 'em high"},"ticketSessionRef":{"name":"5E55104-0000-0000-0000-000000000001","scid":"FEEDFACE-0000-0000-0000-000000000001","templateName":"TestTemplate"}})";

        std::wstring matchmakingUri = L"https://smartmatch.xboxlive.com/serviceconfigs/07617C5B-3423-4505-B6C6-10A16E1E5DDB/hoppers/DeathMatch";

        auto expecteResponse = web::json::value::parse(LR"({"ticketId":"0584338f-a2ff-4eb9-b167-c0e8ddecae72", "waitTime":60 })");

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(expecteResponse);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();

        auto ticket = create_task(xboxLiveContext->MatchmakingService->CreateMatchTicketAsync(
            sessionRef,
            "07617C5B-3423-4505-B6C6-10A16E1E5DDB", // serviceConfigurationId
            "DeathMatch", // hopperName
            giveUpDuration, // GiveUpDuration
            PreserveSessionMode::Never, // preserveSession
            ticketAttributes
            )).get();

        VERIFY_IS_NOT_NULL(ticket);
        VERIFY_ARE_EQUAL_STR(L"POST", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://smartmatch.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/serviceconfigs/07617C5B-3423-4505-B6C6-10A16E1E5DDB/hoppers/DeathMatch", httpCall->PathQueryFragment.to_string());
        VERIFY_ARE_EQUAL(expectedRequest, httpCall->request_body().request_message_string());

        TimeSpan giveUpDurationToVerify;
        giveUpDurationToVerify.Duration = 60 * TICKS_PER_SECOND; // 60 seconds

        VerifyTicket(
            ticket,
            "0584338f-a2ff-4eb9-b167-c0e8ddecae72", // ticketIdToCheck
            giveUpDurationToVerify // waitTimeToCheck
            );
    }

    DEFINE_TEST_CASE(TestCreateMatchTicketAsync_EmptyResult)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestCreateMatchTicketAsync_EmptyResult);
        TimeSpan giveUpDuration;
        giveUpDuration.Duration = 10 * TICKS_PER_SECOND; // 10 seconds

        MultiplayerSessionReference^ sessionRef = ref new MultiplayerSessionReference(DEFAULT_SCID, DEFAULT_TEMPLATE_NAME, DEFAULT_SESSION_ID);

        auto expecteResponse = web::json::value::parse(LR"({})");

        std::wstring expectedRequest =
            LR"({"giveUpDuration":10,"preserveSession":"always","ticketSessionRef":{"name":"5E55104-0000-0000-0000-000000000001","scid":"FEEDFACE-0000-0000-0000-000000000001","templateName":"TestTemplate"}})";

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(expecteResponse);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();

        // should throw WEB_E_INVALID_JSON_STRING
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->MatchmakingService->CreateMatchTicketAsync(
                sessionRef,
                "07617C5B-3423-4505-B6C6-10A16E1E5DDB", // serviceConfigurationId
                "DeathMatch", // hopperName
                giveUpDuration, // GiveUpDuration
                PreserveSessionMode::Always, // preserveSession
                nullptr
                )).get(),
            WEB_E_INVALID_JSON_STRING
        )

        VERIFY_ARE_EQUAL_STR(L"POST", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://smartmatch.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/serviceconfigs/07617C5B-3423-4505-B6C6-10A16E1E5DDB/hoppers/DeathMatch", httpCall->PathQueryFragment.to_string());
        VERIFY_ARE_EQUAL_STR(expectedRequest, httpCall->request_body().request_message_string());
    }

    DEFINE_TEST_CASE(TestDeleteMatchTicketAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestDeleteMatchTicketAsync);
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        create_task(xboxLiveContext->MatchmakingService->DeleteMatchTicketAsync(
            DEFAULT_SCID,
            DEFAULT_HOPPER_NAME,
            DEFAULT_TICKET_ID
            )).get();

        VERIFY_ARE_EQUAL_STR(L"DELETE", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://smartmatch.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(std::wstring(L"/serviceconfigs/") + DEFAULT_SCID + L"/hoppers/" + DEFAULT_HOPPER_NAME + L"/tickets/" + DEFAULT_TICKET_ID, httpCall->PathQueryFragment.to_string());
        VERIFY_ARE_EQUAL_STR(L"", httpCall->request_body().request_message_string());
    }

    DEFINE_TEST_CASE(TestGetMatchTicketAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES_IGNORE(TestGetMatchTicketAsync);
        auto expecteResponse = web::json::value::parse(defaultMatchTicketResponse);
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(expecteResponse);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto ticket = create_task(xboxLiveContext->MatchmakingService->GetMatchTicketDetailsAsync(
            DEFAULT_SCID,
            DEFAULT_HOPPER_NAME,
            DEFAULT_TICKET_ID
            )).get();

        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://smartmatch.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(std::wstring(L"/serviceconfigs/") + DEFAULT_SCID + L"/hoppers/" + DEFAULT_HOPPER_NAME + L"/tickets/" + DEFAULT_TICKET_ID, httpCall->PathQueryFragment.to_string());
        VERIFY_ARE_EQUAL_STR(L"", httpCall->request_body().request_message_string());

        VerifyTicketDetails(ticket, expecteResponse);
    }

    DEFINE_TEST_CASE(TestGetStatsForHopperAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetStatsForHopperAsync);
        const std::wstring response = LR"({"name":"gameawesome2","waitTime":30,"population":1})";

        auto expecteResponse = web::json::value::parse(response);
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(expecteResponse);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto hopper = create_task(xboxLiveContext->MatchmakingService->GetHopperStatisticsAsync(
            DEFAULT_SCID,
            DEFAULT_HOPPER_NAME
            )).get();

        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://smartmatch.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(std::wstring(L"/serviceconfigs/") + DEFAULT_SCID + L"/hoppers/" + DEFAULT_HOPPER_NAME + L"/stats", httpCall->PathQueryFragment.to_string());
        VERIFY_ARE_EQUAL_STR(L"", httpCall->request_body().request_message_string());

        VERIFY_ARE_EQUAL_STR(hopper->HopperName->Data(), L"gameawesome2");
        VERIFY_ARE_EQUAL_INT(hopper->EstimatedWaitTime.Duration, (long long)(30 * TICKS_PER_SECOND)); // 30 seconds
        VERIFY_ARE_EQUAL_INT(hopper->PlayersWaitingToMatch, 1U);
    }

    DEFINE_TEST_CASE(TestInvalidArgument)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestInvalidArgument);
        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        TimeSpan ticketTimeout;
        ticketTimeout.Duration = 0;
        Platform::String^ ticketAttributes = ref new Platform::String();
        MultiplayerSessionReference^ sessionRef = ref new MultiplayerSessionReference("scid", "tempname", "sessionid");

#pragma warning(suppress: 6387)
        VERIFY_THROWS_CX(create_task(xboxLiveContext->MatchmakingService->CreateMatchTicketAsync(nullptr, "configId", "hopperName", ticketTimeout, PreserveSessionMode::Always, ticketAttributes)).get(), InvalidArgumentException);
#pragma warning(suppress: 6387)
        VERIFY_THROWS_CX(create_task(xboxLiveContext->MatchmakingService->CreateMatchTicketAsync(sessionRef, nullptr, "hopperName", ticketTimeout, PreserveSessionMode::Always, ticketAttributes)).get(), InvalidArgumentException);
        VERIFY_THROWS_CX(create_task(xboxLiveContext->MatchmakingService->CreateMatchTicketAsync(sessionRef, "", "hopperName", ticketTimeout, PreserveSessionMode::Always, ticketAttributes)).get(), InvalidArgumentException);
#pragma warning(suppress: 6387)
        VERIFY_THROWS_CX(create_task(xboxLiveContext->MatchmakingService->CreateMatchTicketAsync(sessionRef, "configId", nullptr, ticketTimeout, PreserveSessionMode::Always, ticketAttributes)).get(), InvalidArgumentException);
        VERIFY_THROWS_CX(create_task(xboxLiveContext->MatchmakingService->CreateMatchTicketAsync(sessionRef, "configId", "", ticketTimeout, PreserveSessionMode::Always, ticketAttributes)).get(), InvalidArgumentException);

#pragma warning(suppress: 6387)
        VERIFY_THROWS_CX(create_task(xboxLiveContext->MatchmakingService->DeleteMatchTicketAsync("configId", "hopperName", nullptr)).get(), InvalidArgumentException);
        VERIFY_THROWS_CX(create_task(xboxLiveContext->MatchmakingService->DeleteMatchTicketAsync("configId", "hopperName", "")).get(), InvalidArgumentException);
#pragma warning(suppress: 6387)
        VERIFY_THROWS_CX(create_task(xboxLiveContext->MatchmakingService->DeleteMatchTicketAsync("configId", nullptr, "ticketId")).get(), InvalidArgumentException);
        VERIFY_THROWS_CX(create_task(xboxLiveContext->MatchmakingService->DeleteMatchTicketAsync("configId", "", "ticketId")).get(), InvalidArgumentException);
#pragma warning(suppress: 6387)
        VERIFY_THROWS_CX(create_task(xboxLiveContext->MatchmakingService->DeleteMatchTicketAsync(nullptr, "hopperName", "ticketId")).get(), InvalidArgumentException);
        VERIFY_THROWS_CX(create_task(xboxLiveContext->MatchmakingService->DeleteMatchTicketAsync("", "hopperName", "ticketId")).get(), InvalidArgumentException);

#pragma warning(suppress: 6387)
        VERIFY_THROWS_CX(create_task(xboxLiveContext->MatchmakingService->GetMatchTicketDetailsAsync("configId", "hopperName", nullptr)).get(), InvalidArgumentException);
        VERIFY_THROWS_CX(create_task(xboxLiveContext->MatchmakingService->GetMatchTicketDetailsAsync("configId", "hopperName", "")).get(), InvalidArgumentException);
#pragma warning(suppress: 6387)
        VERIFY_THROWS_CX(create_task(xboxLiveContext->MatchmakingService->GetMatchTicketDetailsAsync("configId", nullptr, "ticketId")).get(), InvalidArgumentException);
        VERIFY_THROWS_CX(create_task(xboxLiveContext->MatchmakingService->GetMatchTicketDetailsAsync("configId", "", "ticketId")).get(), InvalidArgumentException);
#pragma warning(suppress: 6387)
        VERIFY_THROWS_CX(create_task(xboxLiveContext->MatchmakingService->GetMatchTicketDetailsAsync(nullptr, "hopperName", "ticketId")).get(), InvalidArgumentException);
        VERIFY_THROWS_CX(create_task(xboxLiveContext->MatchmakingService->GetMatchTicketDetailsAsync("", "hopperName", "ticketId")).get(), InvalidArgumentException);

#pragma warning(suppress: 6387)
        VERIFY_THROWS_CX(create_task(xboxLiveContext->MatchmakingService->GetHopperStatisticsAsync(nullptr, "hopperName")).get(), InvalidArgumentException);
        VERIFY_THROWS_CX(create_task(xboxLiveContext->MatchmakingService->GetHopperStatisticsAsync("", "hopperName")).get(), InvalidArgumentException);
#pragma warning(suppress: 6387)
        VERIFY_THROWS_CX(create_task(xboxLiveContext->MatchmakingService->GetHopperStatisticsAsync("configId", nullptr)).get(), InvalidArgumentException);
        VERIFY_THROWS_CX(create_task(xboxLiveContext->MatchmakingService->GetHopperStatisticsAsync("configId", "")).get(), InvalidArgumentException);
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

