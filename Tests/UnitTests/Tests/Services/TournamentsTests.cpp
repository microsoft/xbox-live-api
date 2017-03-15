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

#include "Utils_WinRT.h"
#include "xsapi/tournaments.h"
#include "XboxLiveContext_WinRT.h"

using namespace xbox::services;
using namespace xbox::services::tournaments;

using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::System;
using namespace Microsoft::Xbox::Services::Multiplayer;
using namespace Microsoft::Xbox::Services::Tournaments;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

DEFINE_TEST_CLASS(TournamentsTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(TournamentsTests)

    const string_t filePath = _T("\\TestResponses\\Tournaments.json");
    web::json::value testResponseJsonFromFile = utils::read_test_response_file(filePath);

    void VerifyTournament(Tournaments::Tournament^ tournament, web::json::value tournamentToVerify)
    {
        web::json::value tournamentJson = tournamentToVerify[L"tournament"];
        web::json::value teamJson = tournamentToVerify[L"team"];

        VERIFY_ARE_EQUAL(tournament->Id->Data(), tournamentJson[L"id"].as_string());
        VERIFY_ARE_EQUAL(tournament->OrganizerId->Data(), tournamentJson[L"organizerId"].as_string());
        VERIFY_ARE_EQUAL(tournament->Name->Data(), tournamentJson[L"name"].as_string());
        VERIFY_ARE_EQUAL(tournament->Description->Data(), tournamentJson[L"description"].as_string());
        VERIFY_ARE_EQUAL(tournament->GameMode->Data(), tournamentJson[L"gameMode"].as_string());
        VERIFY_ARE_EQUAL(tournament->TournamentStyle->Data(), tournamentJson[L"tournamentStyle"].as_string());
        VERIFY_ARE_EQUAL(tournament->IsRegistrationOpen, tournamentJson[L"isRegistrationOpen"].as_bool());
        VERIFY_ARE_EQUAL(tournament->IsCheckinOpen, tournamentJson[L"isCheckinOpen"].as_bool());
        VERIFY_ARE_EQUAL(tournament->IsPlayingOpen, tournamentJson[L"isPlayingOpen"].as_bool());
        VERIFY_ARE_EQUAL(tournament->HasPrize, tournamentJson[L"hasPrize"].as_bool());
        VERIFY_ARE_EQUAL(tournament->IsPaused, tournamentJson[L"paused"].as_bool());

        VERIFY_ARE_EQUAL_INT(tournament->MinTeamSize, tournamentJson[L"minTeamSize"].as_integer());
        VERIFY_ARE_EQUAL_INT(tournament->MaxTeamSize, tournamentJson[L"maxTeamSize"].as_integer());
        VERIFY_ARE_EQUAL_INT(tournament->TeamsRegisteredCount, tournamentJson[L"numTeamsRegistered"].as_integer());
        VERIFY_ARE_EQUAL_INT(tournament->MinTeamsRegistered, tournamentJson[L"minRegistrationCount"].as_integer());
        VERIFY_ARE_EQUAL_INT(tournament->MaxTeamsRegistered, tournamentJson[L"maxRegistrationCount"].as_integer());
        VERIFY_ARE_EQUAL_STR_IGNORE_CASE(tournament->TournamentState.ToString()->Data(), tournamentJson[L"state"].as_string().c_str());
        if (!tournamentJson[L"endTime"].is_null())
        {
            VERIFY_ARE_EQUAL(DateTimeToString(
                tournament->EndTime).substr(0, DATETIME_STRING_LENGTH_TO_SECOND).c_str(),
                tournamentJson[L"endTime"].as_string().substr(0, DATETIME_STRING_LENGTH_TO_SECOND));
        }

        web::json::value scheduleJson = tournamentJson[L"schedule"];
        VERIFY_ARE_EQUAL(DateTimeToString(
            tournament->RegistrationStartTime).substr(0, DATETIME_STRING_LENGTH_TO_SECOND).c_str(),
            scheduleJson[L"registrationStart"].as_string().substr(0, DATETIME_STRING_LENGTH_TO_SECOND));
        VERIFY_ARE_EQUAL(DateTimeToString(
            tournament->RegistrationEndTime).substr(0, DATETIME_STRING_LENGTH_TO_SECOND).c_str(),
            scheduleJson[L"registrationEnd"].as_string().substr(0, DATETIME_STRING_LENGTH_TO_SECOND));
        VERIFY_ARE_EQUAL(DateTimeToString(
            tournament->CheckinStartTime).substr(0, DATETIME_STRING_LENGTH_TO_SECOND).c_str(),
            scheduleJson[L"checkinStart"].as_string().substr(0, DATETIME_STRING_LENGTH_TO_SECOND));
        VERIFY_ARE_EQUAL(DateTimeToString(
            tournament->CheckinEndTime).substr(0, DATETIME_STRING_LENGTH_TO_SECOND).c_str(),
            scheduleJson[L"checkinEnd"].as_string().substr(0, DATETIME_STRING_LENGTH_TO_SECOND));
        VERIFY_ARE_EQUAL(DateTimeToString(
            tournament->PlayingStartTime).substr(0, DATETIME_STRING_LENGTH_TO_SECOND).c_str(),
            scheduleJson[L"playingStart"].as_string().substr(0, DATETIME_STRING_LENGTH_TO_SECOND));
        VERIFY_ARE_EQUAL(DateTimeToString(
            tournament->PlayingEndTime).substr(0, DATETIME_STRING_LENGTH_TO_SECOND).c_str(),
            scheduleJson[L"playingEnd"].as_string().substr(0, DATETIME_STRING_LENGTH_TO_SECOND));

        // Team Info
        if (!teamJson.is_null())
        {
            VERIFY_ARE_EQUAL(tournament->TeamId->Data(), teamJson[L"id"].as_string());
            VERIFY_ARE_EQUAL_STR_IGNORE_CASE(tournament->TeamState.ToString()->Data(), teamJson[L"state"].as_string().c_str());
            VERIFY_ARE_EQUAL_UINT(tournament->TeamRanking, teamJson[L"ranking"].as_number().to_uint64());
        }
    }

    DEFINE_TEST_CASE(TestGetTournamentsAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetTournamentsAsync);

        const string_t defaultGetTournamentsResponse = testResponseJsonFromFile[L"defaultGetTournamentsResponse"].serialize();
        auto responseJson = web::json::value::parse(defaultGetTournamentsResponse);
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        TournamentRequest^ request = ref new TournamentRequest(true);
        auto task = create_task(xboxLiveContext->TournamentService->GetTournamentsAsync(request));
        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://tournamentshub.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/tournaments/?titleId=1234&teamForMember=TestXboxUserId&memberId=TestXboxUserId", httpCall->PathQueryFragment.to_string());

        TournamentRequestResult^ result = task.get();
        VERIFY_ARE_EQUAL_INT(result->Items->Size, responseJson.as_object()[L"value"].as_array().size());

        for (uint32_t i = 0; i < result->Items->Size; i++)
        {
            VerifyTournament(result->Items->GetAt(i), responseJson.as_object()[L"value"].as_array()[i]);
        }
    }

    DEFINE_TEST_CASE(TestGetTournamentsAsync2)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetTournamentsAsync2);

        const string_t defaultGetTournamentsResponse = testResponseJsonFromFile[L"defaultGetTournamentsResponse"].serialize();
        auto responseJson = web::json::value::parse(defaultGetTournamentsResponse);
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        TournamentRequest^ request = ref new TournamentRequest(false);
        Platform::Collections::Vector<TournamentState>^ states = ref new Platform::Collections::Vector<TournamentState>();
        states->Append(TournamentState::Active);
        states->Append(TournamentState::Completed);
        request->StateFilter = states->GetView();

        auto task = create_task(xboxLiveContext->TournamentService->GetTournamentsAsync(request));
        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://tournamentshub.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR_IGNORE_CASE(L"/tournaments/?titleId=1234&teamForMember=TestXboxUserId&state=%5B%22active%22,%22completed%22%5D", httpCall->PathQueryFragment.to_string().c_str());

        TournamentRequestResult^ result = task.get();
        VERIFY_ARE_EQUAL_INT(result->Items->Size, responseJson.as_object()[L"value"].as_array().size());

        for (uint32_t i = 0; i < result->Items->Size; i++)
        {
            VerifyTournament(result->Items->GetAt(i), responseJson.as_object()[L"value"].as_array()[i]);
        }
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END