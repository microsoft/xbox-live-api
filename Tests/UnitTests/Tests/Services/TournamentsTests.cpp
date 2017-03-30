// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

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

    void VerifyMultiplayerSessionReference(MultiplayerSessionReference^ result, web::json::value resultToVerify)
    {
        VERIFY_ARE_EQUAL(result->ServiceConfigurationId->Data(), resultToVerify[L"scid"].as_string());
        VERIFY_ARE_EQUAL(result->SessionTemplateName->Data(), resultToVerify[L"templateName"].as_string());
        VERIFY_ARE_EQUAL(result->SessionName->Data(), resultToVerify[L"name"].as_string());
    }

    void VerifyMatchMetadata(Tournaments::MatchMetadata^ currentMatchData, web::json::value matchDataToVerify)
    {
        VERIFY_ARE_EQUAL_STR(currentMatchData->Description->Data(), L"Final Match");
        VERIFY_ARE_EQUAL(DateTimeToString(
            currentMatchData->StartTIme).substr(0, DATETIME_STRING_LENGTH_TO_SECOND).c_str(),
            matchDataToVerify[L"startTime"].as_string().substr(0, DATETIME_STRING_LENGTH_TO_SECOND));
        VERIFY_ARE_EQUAL(currentMatchData->IsBye, matchDataToVerify[L"bye"].as_bool());

        web::json::array opposingTeamIdsArray = matchDataToVerify[L"opposingTeamIds"].as_array();
        uint32 counter = 0;
        for (auto oppTeamId : currentMatchData->OpposingTeamIds)
        {
            VERIFY_ARE_EQUAL(oppTeamId->Data(), opposingTeamIdsArray[counter].as_string());
            ++counter;
        }
    }

    void VerifyCurrentMatchMetadata(Tournaments::CurrentMatchMetadata^ currentMatchData, web::json::value matchDataToVerify)
    {
        if (currentMatchData == nullptr) return;

        VerifyMultiplayerSessionReference(currentMatchData->GameSessionReference, matchDataToVerify[L"gameSessionRef"]);
        VerifyMatchMetadata(currentMatchData->MatchDetails, matchDataToVerify);
    }

    void VerifyPreviousMatchMetadata(Tournaments::PreviousMatchMetadata^ previousMatchData, web::json::value matchDataToVerify)
    {
        if (previousMatchData == nullptr) return;

        VERIFY_ARE_EQUAL_STR_IGNORE_CASE(previousMatchData->Result->State.ToString()->Data(), matchDataToVerify[L"result"][L"outcome"].as_string().c_str());
        if (previousMatchData->Result->State == TournamentGameResultState::Rank)
        {
            VERIFY_ARE_EQUAL_INT(previousMatchData->Result->Ranking, matchDataToVerify[L"result"][L"ranking"].as_integer());
        }

        VERIFY_ARE_EQUAL(DateTimeToString(
            previousMatchData->EndTime).substr(0, DATETIME_STRING_LENGTH_TO_SECOND).c_str(),
            matchDataToVerify[L"endTime"].as_string().substr(0, DATETIME_STRING_LENGTH_TO_SECOND));
        VerifyMatchMetadata(previousMatchData->MatchDetails, matchDataToVerify);
    }

    void VerifyTeam(Tournaments::TeamInfo^ teamInfo, web::json::value teamToVerify)
    {
        VERIFY_ARE_EQUAL(teamInfo->Id->Data(), teamToVerify[L"id"].as_string());
        VERIFY_ARE_EQUAL(teamInfo->Name->Data(), teamToVerify[L"name"].as_string());
        VERIFY_ARE_EQUAL(teamInfo->Standing->Data(), teamToVerify[L"standing"].as_string());
        VERIFY_ARE_EQUAL_INT(teamInfo->Ranking, teamToVerify[L"ranking"].as_integer());
        //VERIFY_ARE_EQUAL(teamInfo->ContinuationUri->Data(), teamToVerify[L"name"].as_string());
        VERIFY_ARE_EQUAL_STR_IGNORE_CASE(teamInfo->TeamState.ToString()->Data(), teamToVerify[L"state"].as_string().c_str());
        VERIFY_ARE_EQUAL_STR_IGNORE_CASE(teamInfo->CompletedReason.ToString()->Data(), teamToVerify[L"completedReason"].as_string().c_str());

        VERIFY_ARE_EQUAL(DateTimeToString(
            teamInfo->RegistrationDate).substr(0, DATETIME_STRING_LENGTH_TO_SECOND).c_str(),
            teamToVerify[L"registrationDate"].as_string().substr(0, DATETIME_STRING_LENGTH_TO_SECOND));

        VerifyCurrentMatchMetadata(teamInfo->CurrentMatchMetadata, teamToVerify[L"currentMatch"]);
        VerifyPreviousMatchMetadata(teamInfo->PreviousMatchMetadata, teamToVerify[L"previousMatch"]);
    }

    void VerifyTeamSummary(Tournaments::TeamSummary^ teamSummary, web::json::value teamSummaryToVerify)
    {
        if (teamSummary == nullptr) return;

        VERIFY_ARE_EQUAL(teamSummary->Id->Data(), teamSummaryToVerify[L"id"].as_string());
        VERIFY_ARE_EQUAL_INT(teamSummary->Ranking, teamSummaryToVerify[L"ranking"].as_integer());
        VERIFY_ARE_EQUAL_STR_IGNORE_CASE(teamSummary->TeamState.ToString()->Data(), teamSummaryToVerify[L"state"].as_string().c_str());
    }

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
        VerifyTeamSummary(tournament->TeamSummary ,teamJson);
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
        TournamentRequestResult^ result = create_task(xboxLiveContext->TournamentService->GetTournamentsAsync(request)).get();
        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://tournamentshub.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/tournaments?titleId=1234&teamForMember=TestXboxUserId&memberId=TestXboxUserId", httpCall->PathQueryFragment.to_string());
        VERIFY_ARE_EQUAL_INT(result->Tournaments->Size, responseJson.as_object()[L"value"].as_array().size());
        for (uint32_t i = 0; i < result->Tournaments->Size; i++)
        {
            VerifyTournament(result->Tournaments->GetAt(i), responseJson.as_object()[L"value"].as_array()[i]);
        }

        VERIFY_IS_TRUE(result->HasNext);
        result = create_task(result->GetNextAsync()).get();
        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://tournamentshub.xboxlive.com/tournaments?organizer=xbox-live&continuationToken=10", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/", httpCall->PathQueryFragment.to_string());    // Creates an empty uri
        VERIFY_ARE_EQUAL_INT(result->Tournaments->Size, responseJson.as_object()[L"value"].as_array().size());
        for (uint32_t i = 0; i < result->Tournaments->Size; i++)
        {
            VerifyTournament(result->Tournaments->GetAt(i), responseJson.as_object()[L"value"].as_array()[i]);
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

        TournamentRequestResult^ result = create_task(xboxLiveContext->TournamentService->GetTournamentsAsync(request)).get();
        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://tournamentshub.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/tournaments?titleId=1234&teamForMember=TestXboxUserId&state=Active,Completed", httpCall->PathQueryFragment.to_string().c_str());
        VERIFY_ARE_EQUAL_INT(result->Tournaments->Size, responseJson.as_object()[L"value"].as_array().size());
        for (uint32_t i = 0; i < result->Tournaments->Size; i++)
        {
            VerifyTournament(result->Tournaments->GetAt(i), responseJson.as_object()[L"value"].as_array()[i]);
        }
    }

    DEFINE_TEST_CASE(TestGetTeamsAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetTeamsAsync);

        const string_t defaultGetTournamentsResponse = testResponseJsonFromFile[L"defaultGetTeamsResponse"].serialize();
        auto responseJson = web::json::value::parse(defaultGetTournamentsResponse);
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        TeamRequest^ request = ref new TeamRequest(
            ref new Platform::String(L"xbox-live"),
            ref new Platform::String(L"MyTestTournamentId"),
            false
            );

        Platform::Collections::Vector<TeamState>^ states = ref new Platform::Collections::Vector<TeamState>();
        states->Append(TeamState::Registered);
        states->Append(TeamState::CheckedIn);
        request->StateFilter = states->GetView();

        TeamRequestResult^ result = create_task(xboxLiveContext->TournamentService->GetTeamsAsync(request)).get();
        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://tournamentshub.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/tournaments/xbox-live/MyTestTournamentId/teams?state=Registered,CheckedIn", httpCall->PathQueryFragment.to_string());
        VERIFY_ARE_EQUAL_INT(result->Teams->Size, responseJson.as_object()[L"value"].as_array().size());
        for (uint32_t i = 0; i < result->Teams->Size; i++)
        {
            VerifyTeam(result->Teams->GetAt(i), responseJson.as_object()[L"value"].as_array()[i]);
        }

        VERIFY_IS_TRUE(result->HasNext);
        result = create_task(result->GetNextAsync()).get();
        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://tournamentshub.xboxlive.com/tournaments/xbox-live/MyTestTournamentId/teams?memberId=TestXboxUserId&continuationToken=10", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/", httpCall->PathQueryFragment.to_string());    // Creates an empty uri
        VERIFY_ARE_EQUAL_INT(result->Teams->Size, responseJson.as_object()[L"value"].as_array().size());
        for (uint32_t i = 0; i < result->Teams->Size; i++)
        {
            VerifyTeam(result->Teams->GetAt(i), responseJson.as_object()[L"value"].as_array()[i]);
        }
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END