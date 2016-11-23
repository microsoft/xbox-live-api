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
#define TEST_CLASS_AREA L"Tournaments"
#include "UnitTestIncludes.h"

using namespace Microsoft::Xbox::Services;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;
using namespace Microsoft::Xbox::Services::System;
using namespace xbox::services::system;

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

DEFINE_TEST_CLASS(TournamentsTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(TournamentsTests)

const string_t defaultGetTournamentInstancesResponse =
LR"(
{
   "instances":[
    {
        "id":"aea18a61-d0e6-492b-8c57-5e87f97ed834",
        "definitionName":"TestMaxOne",
        "status":"Registering",
        "start":"2016-04-22T16:26:18.5455046Z",
        "properties":{
            "displayName":"TESTMAXONE_NAME",
            "description":"TESTMAXONE_DESCRIPTION",
            "organizer":{
                "name":"XboxLive"
            }
        },
        "configuration":{
            "registration":{
                "start":"2015-06-16T19:02:10.8765706Z",
                "end":"2016-04-22T16:31:18.5455046Z",
                "maxTeams":8,
                "minTeams":2
            },
            "teams":{
                "sessionTemplate":"TeamSessionMaxOne",
                "minMembers":1,
                "maxMembers":1
            },
            "stages":[
            {
                "start":"2016-04-22T16:31:18.5455046Z",
                "minDuration":"00:01:00",
                "mode":{
                    "style":"SingleElimination"
                },
                "sessionTemplate":"TournamentGameSessionMaxTwo",
                "rounds":{
                    "minLength":"00:01:00",
                    "runInParallel":false
                }
            }]
        }
    }]
}
)";

const string_t defaultGetTournamentProgressResponse =
LR"({
    "progress":{
    "stages":[{
        "rounds":[{
        "start":"2015-06-16T19:00:00.0000000Z",
        "end":"2015-06-16T19:17:45.0500949Z",
        "games":[{
            "id":"4fa96370-3d2a-407e-ad74-125d7e098f6d",
            "start":"2015-06-16T19:02:10.8765706Z",
            "end":"2015-06-16T19:17:45.0500949Z",
            "state":"inprogress",
            "teams":[{
            "id":"8cb05010-4ca1-4f15-abf3-d686e7ac1588",
            "result":{
                "state":"draw",
                "source":"adjusted"
            }
            }]
        },{
            "id":"7f918194-882b-44ae-8ab7-249f5400b4be",
            "start":"2015-06-16T19:01:57.4314319Z",
            "end":"2015-06-16T19:12:04.4346712Z",
            "state":"completed",
            "teams":[{
            "id":"75c4e11d-dd99-4e00-8b90-93f20286146d",
            "result":{
                "state":"win",
                "source":"arbitration"
            }},{
            "id":"b53aaad7-7c0b-4ec2-8ff7-830dc551955a",
            "result":{
                "state":"loss",
                "source":"server"
            }
            }]
        }]
        }]
    }]
    }
})";

    void VerifyTournament(Tournaments::Tournament^ tournament, web::json::value tournamentToVerify)
    {
        VERIFY_ARE_EQUAL(tournamentToVerify[L"id"].as_string(), tournament->Id->Data());
        VERIFY_ARE_EQUAL(tournamentToVerify[L"definitionName"].as_string(), tournament->Definition->Data());
        VERIFY_ARE_EQUAL(tournamentToVerify[L"properties"][L"displayName"].as_string(), tournament->DisplayName->Data());
        VERIFY_ARE_EQUAL(tournamentToVerify[L"properties"][L"description"].as_string(), tournament->Description->Data());
        VERIFY_ARE_EQUAL_STR_IGNORE_CASE(tournamentToVerify[L"status"].as_string().c_str(), tournament->Status.ToString()->Data());

        //dates
        VERIFY_ARE_EQUAL(DateTimeToString(tournament->StartDate).substr(0, DATETIME_STRING_LENGTH_TO_SECOND).c_str(),
            tournamentToVerify[L"start"].as_string().substr(0, DATETIME_STRING_LENGTH_TO_SECOND));
        if (tournamentToVerify.has_field(L"end"))
        {
            VERIFY_ARE_EQUAL(DateTimeToString(tournament->EndDate).substr(0, DATETIME_STRING_LENGTH_TO_SECOND).c_str(),
                tournamentToVerify[L"end"].as_string().substr(0, DATETIME_STRING_LENGTH_TO_SECOND));
        }
    }

    DEFINE_TEST_CASE(TestGetTournamentInstancesAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetTournamentInstancesAsync);
        auto responseJson = web::json::value::parse(defaultGetTournamentInstancesResponse);
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto task = create_task(xboxLiveContext->TournamentService->GetTournamentInstancesAsync(
            _T("ServiceConfigurationId"),
            DateTime(),
            DateTime(),
            _T("0")));
        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://tournaments.mockenv.xboxlive.com", httpCall->ServerName);

        Tournaments::TournamentInstances^ result = task.get();
        VERIFY_ARE_EQUAL_INT(result->Items->Size, responseJson.as_object()[L"instances"].as_array().size());

        for (uint32_t i = 0; i < result->Items->Size; i++)
        {
            VerifyTournament(result->Items->GetAt(i), responseJson.as_object()[L"instances"].as_array()[i]);
        }
    }

    DEFINE_TEST_CASE(TestGetTournamentProgressAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetTournamentProgressAsync);
        auto responseJson = web::json::value::parse(defaultGetTournamentProgressResponse);
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto task = create_task(xboxLiveContext->TournamentService->GetTournamentProgressAsync(
            _T("ServiceConfigurationId"),
            _T("TournamentId")));
        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://tournaments.mockenv.xboxlive.com", httpCall->ServerName);

        auto stages = task.get();
        VERIFY_ARE_EQUAL_INT(1, stages->Size);

        auto rounds = stages->GetAt(0)->Rounds;
        VERIFY_ARE_EQUAL_INT(1, rounds->Size);

        auto round1 = rounds->GetAt(0);
        VERIFY_IS_TRUE(round1->StartTime.UniversalTime != 0);
        VERIFY_IS_TRUE(round1->EndTime.UniversalTime != 0);

        auto games = round1->Games;
        VERIFY_ARE_EQUAL_INT(2, games->Size);

        auto game1 = games->GetAt(0);
        VERIFY_ARE_EQUAL_STR(game1->Id->Data(), L"4fa96370-3d2a-407e-ad74-125d7e098f6d");
        VERIFY_IS_TRUE(game1->StartTime.UniversalTime != 0);
        VERIFY_IS_TRUE(game1->EndTime.UniversalTime != 0);
        VERIFY_IS_TRUE(game1->State == TournamentGameState::InProgress);

        auto results1 = game1->Results;
        VERIFY_ARE_EQUAL_INT(1, results1->Size);
        auto result11 = results1->GetAt(0);
        VERIFY_IS_TRUE(result11->State == TournamentGameResultState::Draw);
        VERIFY_IS_TRUE(result11->Source == TournamentGameResultSource::Adjusted);

        auto game2 = games->GetAt(1);
        VERIFY_IS_TRUE(game2->State == TournamentGameState::Completed);

        auto results2 = game2->Results;
        VERIFY_ARE_EQUAL_INT(2, results2->Size);

        auto result21 = results2->GetAt(0);
        VERIFY_ARE_EQUAL_STR(result21->TeamId->Data(), L"75c4e11d-dd99-4e00-8b90-93f20286146d");
        VERIFY_IS_TRUE(result21->State == TournamentGameResultState::Win);
        VERIFY_IS_TRUE(result21->Source == TournamentGameResultSource::Arbitration);

        auto result22 = results2->GetAt(1);
        VERIFY_IS_TRUE(result22->State == TournamentGameResultState::Loss);
        VERIFY_IS_TRUE(result22->Source == TournamentGameResultSource::Server);
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END
