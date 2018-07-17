// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#define TEST_CLASS_OWNER L"johlafo"
#define TEST_CLASS_AREA L"Clubs"
#include "UnitTestIncludes.h"

#include "Utils_WinRT.h"
#include "xsapi/clubs.h"
#include "XboxLiveContext_WinRT.h"
#include "Clubs/clubs_serializers.h"

using namespace xbox::services;
using namespace xbox::services::clubs;

using namespace Platform;
using namespace Platform::Collections;
using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::System;
using namespace Microsoft::Xbox::Services::Clubs;
using namespace Windows::Foundation::Collections;
using namespace web;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

void VerifyString(Platform::String^ string, json::value& json)
{
    if (json.is_null())
    {
        VERIFY_IS_TRUE(string == nullptr || string->Length() == 0);
    }
    else
    {
        VERIFY_ARE_EQUAL_STR(string->Data(), json.as_string());
    }
}

void VerifyInt(int value, json::value& json)
{
    if (!json.is_null())
    {
        VERIFY_ARE_EQUAL_INT(value, json.as_integer());
    }
}

void VerifyDouble(double value, json::value& json)
{
    if (!json.is_null())
    {
        VERIFY_ARE_EQUAL(value, json.as_double());
    }
}

void VerifyBool(bool value, json::value& json)
{
    if (!json.is_null())
    {
        VERIFY_ARE_EQUAL(value, json.as_bool());
    }
}

void VerifyClubRole(ClubRole role, json::value& json)
{
    if (!json.is_null())
    {
        auto jsonRole = static_cast<ClubRole>(clubs_serializers::convert_string_to_club_role(json.as_string()).payload());
        VERIFY_ARE_EQUAL_INT(role, jsonRole);
    }
}

void VerifyDateTime(DateTime date, json::value& json)
{
    if (!json.is_null())
    {
        VERIFY_ARE_EQUAL(DateTimeToString(date).substr(0, DATETIME_STRING_LENGTH_TO_SECOND),
            json.as_string().substr(0, DATETIME_STRING_LENGTH_TO_SECOND));
    }
}

void VerifyClubRoleRecord(ClubRoleRecord^ record, json::value& json)
{
    VerifyString(record->ActorXuid, json[_T("actorXuid")]);
    VerifyDateTime(record->CreatedDate, json[_T("createdDate")]);
    VerifyClubRole(record->Role, json[_T("role")]);
    
    if (!json[_T("xuid")].is_null())
    {
        VERIFY_ARE_EQUAL_STR(record->Xuid->Data(), json[_T("xuid")].as_string());
    }
}

void VerifyClubPresenceRecord(ClubUserPresenceRecord^ record, json::value& json)
{
    VerifyString(record->Xuid, json[_T("xuid")]);
    VerifyDateTime(record->LastSeen, json[_T("lastSeenTimestamp")]);
    VERIFY_ARE_EQUAL_INT(record->LastSeenState, static_cast<ClubUserPresence>(clubs_serializers::convert_string_to_club_user_presence(json[_T("lastSeenState")].as_string()).payload()));
}

DEFINE_TEST_CLASS(ClubsTests)
{
private:
    const string_t filePath = _T("\\TestResponses\\Clubs.json");
    web::json::value testResponsesJsonFromFile = utils::read_test_response_file(filePath);

public:
    DEFINE_TEST_CLASS_PROPS(ClubsTests)

    template <typename T>
    void VerifyArray(void(*verifyElt)(T, json::value&), IVectorView<T>^ vector, json::value& json)
    {
        if (json.is_null())
        {
            VERIFY_IS_TRUE(vector == nullptr || vector->Size == 0);
        }
        else
        {
            VERIFY_IS_TRUE(json.is_array());
            auto jsonArray = json.as_array();
            VERIFY_ARE_EQUAL_INT(vector->Size, jsonArray.size());

            auto jsonIter = jsonArray.begin();
            for (const auto& elt : vector)
            {
                verifyElt(elt, *jsonIter);
                jsonIter++;
            }
        }
    }

    void VerifyClubAgainstAccountsResponse(Club^ club, json::value& json)
    {
        VerifyString(club->Profile->Name->Value, json[_T("name")]);
        VerifyString(club->Owner, json[_T("owner")]);
        VERIFY_ARE_EQUAL_INT(club->Type, static_cast<ClubType>(clubs_serializers::convert_string_to_club_type(json[_T("type")].as_string()).payload()));
        VerifyDateTime(club->Created, json[_T("created")]);
        VerifyString(club->TitleFamilyId, json[_T("titleFamilyId")]);
    }

    void VerifyClubProfileStringSetting(ClubStringSetting^ setting, json::value& json)
    {
        VerifyString(setting->Value, json[_T("value")]);
        VerifyArray(&VerifyString, setting->AllowedValues, json[_T("allowedValues")]);
        VerifyBool(setting->CanViewerChangeSetting, json[_T("canViewerChangeSetting")]);
    }

    void VerifyClubProfileStringMultiSetting(ClubStringMultiSetting^ setting, json::value& json)
    {
        VerifyArray(&VerifyString, setting->Values, json[_T("value")]);
        VerifyArray(&VerifyString, setting->AllowedValues, json[_T("allowedValues")]);
        VerifyBool(setting->CanViewerChangeSetting, json[_T("canViewerChangeSetting")]);
    }

    void VerifyClubProfileBooleanSetting(ClubBooleanSetting^ setting, json::value& json)
    {
        VerifyBool(setting->Value, json[_T("value")]);
        VerifyBool(setting->CanViewerChangeSetting, json[_T("canViewerChangeSetting")]);
    }

    void VerifyClubProfile(ClubProfile^ profile, json::value& json)
    {
        VerifyClubProfileStringSetting(profile->Name, json[_T("name")]);
        VerifyClubProfileStringSetting(profile->Description, json[_T("description")]);
        VerifyClubProfileBooleanSetting(profile->MatureContentEnabled, json[_T("matureContentEnabled")]);
        VerifyClubProfileBooleanSetting(profile->WatchClubTitlesOnly, json[_T("watchClubTitlesOnly")]);
        VerifyClubProfileBooleanSetting(profile->IsSearchable, json[_T("isSearchable")]);
        VerifyClubProfileBooleanSetting(profile->IsRecommendable, json[_T("isRecommendable")]);
        VerifyClubProfileBooleanSetting(profile->RequestToJoinEnabled, json[_T("requestToJoinEnabled")]);
        VerifyClubProfileBooleanSetting(profile->LeaveEnabled, json[_T("leaveEnabled")]);
        VerifyClubProfileBooleanSetting(profile->TransferOwnershipEnabled, json[_T("transferOwnershipEnabled")]);
        VerifyClubProfileStringSetting(profile->DisplayImageUrl, json[_T("displayImageUrl")]);
        VerifyClubProfileStringSetting(profile->BackgroundImageUrl, json[_T("backgroundImageUrl")]);
        VerifyClubProfileStringMultiSetting(profile->Tags, json[_T("tags")]);
        VerifyClubProfileStringSetting(profile->PreferredLocale, json[_T("preferredLocale")]);
        VerifyClubProfileStringMultiSetting(profile->AssociatedTitles, json[_T("associatedTitles")]);
        VerifyClubProfileStringSetting(profile->PrimaryColor, json[_T("primaryColor")]);
        VerifyClubProfileStringSetting(profile->SecondayColor, json[_T("secondaryColor")]);
        VerifyClubProfileStringSetting(profile->TertiaryColor, json[_T("tertiaryColor")]);
    }

    void VerfiyClubRoster(ClubRoster^ roster, json::value& json)
    {
        VerifyArray(&VerifyClubRoleRecord, roster->Moderators, json[_T("moderator")]);
        VerifyArray(&VerifyClubRoleRecord, roster->RequestedToJoin, json[_T("requestedToJoin")]);
        VerifyArray(&VerifyClubRoleRecord, roster->Recommended, json[_T("recommended")]);
        VerifyArray(&VerifyClubRoleRecord, roster->Banned, json[_T("banned")]);
    }

    void VerifyClubActionSetting(ClubActionSetting^ setting, json::value& json)
    {
        VerifyClubRole(setting->RequiredRole, json[_T("value")]);
        VerifyArray(&VerifyClubRole, setting->AllowedValues, json[_T("allowedValues")]);
        VerifyBool(setting->CanViewerAct, json[_T("canViewerAct")]);
        VerifyBool(setting->CanViewerChangeSetting, json[_T("canViewerChangeSetting")]);
    }

    void VerifyClubActionSettings(ClubActionSettings^ settings, json::value& json)
    {
        VerifyClubActionSetting(settings->PostToFeed, json[_T("feed")][_T("post")]);
        VerifyClubActionSetting(settings->ViewFeed, json[_T("feed")][_T("view")]);
        VerifyClubActionSetting(settings->WriteInChat, json[_T("chat")][_T("write")]);
        VerifyClubActionSetting(settings->ViewChat, json[_T("chat")][_T("view")]);
        VerifyClubActionSetting(settings->SetChatTopic, json[_T("chat")][_T("setChatTopic")]);
        VerifyClubActionSetting(settings->JoinLookingForGame, json[_T("lfg")][_T("join")]);
        VerifyClubActionSetting(settings->CreateLookingForGame, json[_T("lfg")][_T("create")]);
        VerifyClubActionSetting(settings->ViewLookingForGame, json[_T("lfg")][_T("view")]);
        VerifyClubActionSetting(settings->InviteOrAcceptJoinRequests, json[_T("roster")][_T("inviteOrAccept")]);
        VerifyClubActionSetting(settings->KickOrBan, json[_T("roster")][_T("kickOrBan")]);
        VerifyClubActionSetting(settings->ViewRoster, json[_T("roster")][_T("view")]);
        VerifyClubActionSetting(settings->UpdateProfile, json[_T("profile")][_T("update")]);
        VerifyClubActionSetting(settings->DeleteProfile, json[_T("profile")][_T("delete")]);
        VerifyClubActionSetting(settings->ViewProfile, json[_T("profile")][_T("view")]);
    }

    void VerifyClub(Club^ club, json::value& json)
    {
        VerifyString(club->Id, json[_T("id")]);
        
        auto typeJson = json[_T("clubType")];
        VERIFY_ARE_EQUAL_INT(club->Type, static_cast<ClubType>(clubs_serializers::convert_string_to_club_type(typeJson[_T("type")].as_string()).payload()));
        VerifyString(club->TitleFamilyId, typeJson[_T("titleFamilyId")]);
        VerifyString(club->TitleFamilyName, typeJson[_T("localizedTitleFamilyName")]);
        VerifyDateTime(club->Created, json[_T("creationDateUtc")]);
        VerifyString(club->Owner, json[_T("ownerXuid")]);        
        VerifyString(club->ShortName, json[_T("shortName")]);
        VerifyString(club->GlyphImageUrl, json[_T("glyphImageUrl")]);
        VerifyString(club->BannerImageUrl, json[_T("bannerImageUrl")]);
        VERIFY_ARE_EQUAL(club->IsClubSuspended, utils::str_icmp(json[_T("state")].as_string(), _T("suspended")) == 0);
        VerifyDateTime(club->SuspendedUntil, json[_T("suspendedUntilUtc")]);      
        VerifyString(club->FounderXuid, json[_T("founderXuid")]);
        VerifyInt(club->FollowersCount, json[_T("followersCount")]);
        VerifyInt(club->PresenceCount, json[_T("clubPresenceCount")]);
        VerifyInt(club->PresenceTodayCount, json[_T("clubPresenceTodayCount")]);
        VerifyInt(club->MembersCount, json[_T("membersCount")]);
        VerifyInt(club->ModeratorsCount, json[_T("moderatorsCount")]);
        VerifyInt(club->RecommendedCount, json[_T("recommendedCount")]);
        VerifyInt(club->RequestedToJoinCount, json[_T("requestedToJoinCount")]);
        VerifyInt(club->ReportCount, json[_T("reportCount")]);
        VerifyInt(club->ReporteItemsCount, json[_T("reportedItemsCount")]);
        VerifyClubActionSettings(club->ActionSettings, json[_T("settings")]);
        VerifyClubProfile(club->Profile, json[_T("profile")]);
        VerifyArray(&VerifyClubRole, club->ViewerRoles, json[_T("settings")][_T("viewerRoles")][_T("roles")]);
    }

    void VerifySearchFacetResults(SearchFacetResults_t facetResuls, json::value& json)
    {
        auto jsonAsObject = json.as_object();
        VERIFY_ARE_EQUAL_INT(jsonAsObject.size(), facetResuls->Size);

        auto sfrIt = jsonAsObject.begin();
        for (const auto& facet : facetResuls)
        {
            VERIFY_ARE_EQUAL_STR(facet->Key->Data(), sfrIt->first);

            auto facetValuesArray = sfrIt->second.as_array();
            VERIFY_ARE_EQUAL_INT(facetValuesArray.size(), facet->Value->Size);

            auto facetValuesIt = facetValuesArray.begin();
            for (const auto& facetValue : facet->Value)
            {
                VerifyInt(facetValue->Count, (*facetValuesIt)[_T("count")]);
                VerifyString(facetValue->Value, (*facetValuesIt)[_T("value")]);

                facetValuesIt++;
            }
            sfrIt++;
        }
    }

    void VerifyClubSearchAutoComplete(ClubSearchAutoComplete^ autoComplete, json::value& json)
    {
        VerifyString(autoComplete->SuggestedQueryText, json[_T("text")]);

        auto resultJson = json[_T("result")];

        VerifyString(autoComplete->Id, resultJson[_T("id")]);
        VerifyString(autoComplete->Name, resultJson[_T("name")]);
        VerifyString(autoComplete->Description, resultJson[_T("description")]);
        VerifyString(autoComplete->DisplayImageUrl, resultJson[_T("displayImageUrl")]);
        VerifyDouble(autoComplete->Score, resultJson[_T("score")]);
        VerifyArray(&VerifyString, autoComplete->Tags, resultJson[_T("tags")]);
        VerifyArray(&VerifyString, autoComplete->AssociatedTitles, resultJson[_T("titles")]);
    }

    Club^ GetClub()
    {
        auto responseJson = testResponsesJsonFromFile[_T("clubhubResponse")];
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();

        auto club = create_task(xboxLiveContext->ClubsService->GetClubAsync(_T("3379871642723170"))).get();

        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://clubhub.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/clubs/Ids(3379871642723170)/decoration/settings", httpCall->PathQueryFragment.to_string());

        auto clubsJsonArray = responseJson[_T("clubs")].as_array();
        VERIFY_ARE_EQUAL_INT(clubsJsonArray.size(), 1);

        VerifyClub(club, clubsJsonArray[0]);
        
        return club;
    }

    DEFINE_TEST_CASE(TestGetClubsOwned)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetClubsOwned);

        auto responseJson = testResponsesJsonFromFile[_T("getClubsOwnedResponse")];
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();

        auto result = create_task(xboxLiveContext->ClubsService->GetClubsOwnedAsync()).get();

        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://clubaccounts.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/users/xuid(TestXboxUserId)/clubsowned", httpCall->PathQueryFragment.to_string());
                
        auto clubsJsonArray = responseJson[_T("clubs")].as_array();
        VERIFY_ARE_EQUAL_INT(clubsJsonArray.size(), result->ClubIds->Size);
        
        auto jsonIter = clubsJsonArray.begin();   
        for (const auto& id : result->ClubIds)
        {
            VerifyString(id, (*jsonIter)[_T("id")]);
        }

        uint32 remainingClubs = utils::string_t_to_int32(responseJson[_T("remainingOpenAndClosedClubs")].as_string()) +
                                utils::string_t_to_int32(responseJson[_T("remainingSecretClubs")].as_string());

        VERIFY_ARE_EQUAL_INT(result->RemainingClubs, remainingClubs);
    }

    DEFINE_TEST_CASE(TestCreateClub)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestCreateClub);

        // Since create_club actually wraps two service calls, add the response json for both here
        auto responseJson = testResponsesJsonFromFile[_T("createClubResponse")];
        responseJson[_T("clubs")] = testResponsesJsonFromFile[_T("clubhubResponse")][_T("clubs")];

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();

        auto result = create_task(xboxLiveContext->ClubsService->CreateClubAsync(
            _T("The Silent Cartographers"),
            ClubType::Public,
            _T("11111111-2222-3333-4444-555555555555")
            )).get();
        
        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://clubhub.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/clubs/Ids(3379871642723170)/decoration/settings", httpCall->PathQueryFragment.to_string());

        VerifyClubAgainstAccountsResponse(result, responseJson);

        auto clubsJsonArray = responseJson[_T("clubs")].as_array();
        VERIFY_ARE_EQUAL_INT(clubsJsonArray.size(), 1);

        VerifyClub(result, clubsJsonArray[0]);
    }

    DEFINE_TEST_CASE(TestGetClub)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetClub);

        (void)GetClub();
    }

    DEFINE_TEST_CASE(TestGetClubRoster)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetClubRoster);

        Club^ club = GetClub();

        auto responseJson = testResponsesJsonFromFile[_T("clubhubResponse")];
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        auto roster = create_task(club->GetRosterAsync()).get();

        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://clubhub.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/clubs/Ids(3379871642723170)/decoration/roster", httpCall->PathQueryFragment.to_string());

        auto clubsJsonArray = responseJson[_T("clubs")].as_array();

        VerfiyClubRoster(roster, clubsJsonArray[0][_T("roster")]);
    }

    DEFINE_TEST_CASE(TestGetUserPresence)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetUserPresence);

        Club^ club = GetClub();

        auto responseJson = testResponsesJsonFromFile[_T("clubhubResponse")];
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        auto userPresence = create_task(club->GetClubUserPresenceRecordsAsync()).get();

        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://clubhub.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/clubs/Ids(3379871642723170)/decoration/clubPresence", httpCall->PathQueryFragment.to_string());

        auto clubsJsonArray = responseJson[_T("clubs")].as_array();
        VerifyArray(&VerifyClubPresenceRecord, userPresence, clubsJsonArray[0][_T("clubPresence")]);
    }

    DEFINE_TEST_CASE(TestGetClubPresenceCounts)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetClubPresenceCounts);

        Club^ club = GetClub();

        auto responseJson = testResponsesJsonFromFile[_T("clubPresenceCounts")];
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        auto result = create_task(club->GetPresenceCountsAsync()).get();

        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://clubpresence.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/clubs/3379871642723170/users/count", httpCall->PathQueryFragment.to_string());

        VerifyInt(result->TotalCount, responseJson[_T("totalCount")]);
        VerifyInt(result->HereNow, responseJson[_T("activeCount")]);
        VerifyInt(result->HereToday, responseJson[_T("hereTodayCount")]);
    }

    DEFINE_TEST_CASE(TestAddUserToClub)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestAddUserToClub);

        Club^ club = GetClub();

        auto responseJson = testResponsesJsonFromFile[_T("addUserToClub")];
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        auto result = create_task(club->AddUserToClubAsync(_T("2603643534573581"))).get();

        VERIFY_ARE_EQUAL_STR(L"PUT", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://clubroster.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/clubs/3379871642723170/users/xuid(2603643534573581)", httpCall->PathQueryFragment.to_string());

        VerifyArray(&VerifyClubRole, result, responseJson[_T("roles")]);
    }

    DEFINE_TEST_CASE(TestGetUsersClubAssociations)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetUsersClubAssociationsAndRoles);

        auto responseJson = testResponsesJsonFromFile[_T("clubhubResponse")];
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();

        auto clubsService = xboxLiveContext->ClubsService;

        auto result = create_task(clubsService->GetClubAssociationsAsync(_T("2535412074763788"))).get();

        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://clubhub.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/clubs/Xuid(2535412074763788)", httpCall->PathQueryFragment.to_string());

        auto clubsJsonArray = responseJson[_T("clubs")].as_array();
        VERIFY_ARE_EQUAL_INT(clubsJsonArray.size(), result->Size);

        auto jsonIter = clubsJsonArray.begin();
        for (auto club : result)
        {
            VerifyClub(club, *jsonIter);

            auto roleRecords = create_task(club->GetRoleRecordsAsync(_T("2535412074763788"))).get();
            
            VerifyArray(&VerifyClubRoleRecord, roleRecords, (*jsonIter)[_T("targetRoles")][_T("roles")]);
            jsonIter++;
        }
    }

    DEFINE_TEST_CASE(TestGetClubRecommendations)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetClubRecommendations);

        auto responseJson = testResponsesJsonFromFile[_T("clubhubResponse")];
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();

        auto result = create_task(xboxLiveContext->ClubsService->GetClubRecommendationsAsync()).get();

        VERIFY_ARE_EQUAL_STR(L"POST", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://clubhub.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/clubs/recommendations", httpCall->PathQueryFragment.to_string());

        auto clubsJsonArray = responseJson[_T("clubs")].as_array();
        VERIFY_ARE_EQUAL_INT(clubsJsonArray.size(), result->Size);

        auto clubsIt = clubsJsonArray.begin();
        for (const auto& recommendation : result)
        {
            VerifyClub(recommendation->RecommendedClub, *clubsIt);
            
            auto reasonsJsonArray = (((*clubsIt)[_T("recommendation")])[_T("reasons")]).as_array();
            VERIFY_ARE_EQUAL_INT(recommendation->Reasons->Size, reasonsJsonArray.size());

            auto reasonsIt = reasonsJsonArray.begin();
            for (const auto& reason : recommendation->Reasons)
            {
                VerifyString(reason, (*reasonsIt)[_T("localizedText")]);
                reasonsIt++;
            }
            clubsIt++;
        }       
    }

    DEFINE_TEST_CASE(TestSearchClubs)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestSearchClubs);

        auto responseJson = testResponsesJsonFromFile[_T("clubhubResponse")];
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();

        auto result = create_task(xboxLiveContext->ClubsService->SearchClubsAsync(
            _T("xbox"),
            nullptr,
            nullptr
            )).get();

        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://clubhub.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/clubs/search/decoration/settings?q=xbox&count=200", httpCall->PathQueryFragment.to_string());

        auto clubsJsonArray = responseJson[_T("clubs")].as_array();
        VERIFY_ARE_EQUAL_INT(clubsJsonArray.size(), result->Clubs->Size);

        auto clubsIt = clubsJsonArray.begin();
        for (const auto& club : result->Clubs)
        {
            VerifyClub(club, *clubsIt++);
        }

        VerifySearchFacetResults(result->SearchFacetResults, responseJson[_T("searchFacetResults")]);
    }

    DEFINE_TEST_CASE(TestSuggestClubs)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestSuggestClubs);

        auto responseJson = testResponsesJsonFromFile[_T("suggestResult")];
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();

        auto result = create_task(xboxLiveContext->ClubsService->SuggestClubsAsync(
            _T("eVALKYRIES"),
            nullptr,
            nullptr
        )).get();

        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://clubsearch.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/suggest?q=eVALKYRIES", httpCall->PathQueryFragment.to_string());

        auto resultsArray = responseJson[_T("results")].as_array();
        VERIFY_ARE_EQUAL_INT(resultsArray.size(), result->Size);

        auto resultsIt = resultsArray.begin();
        for (const auto& suggestion : result)
        {
            VerifyClubSearchAutoComplete(suggestion, *resultsIt++);
        }
    }

    DEFINE_TEST_CASE(TestAddClubModerator)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestSuggestClubs);

        Club^ club = GetClub();

        auto responseJson = testResponsesJsonFromFile[_T("addClubModeratorResponse")];
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto result = create_task(club->AddClubModeratorAsync(_T("2535458173220066"))).get();

        VERIFY_ARE_EQUAL_STR(L"POST", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://clubroster.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/clubs/3379871642723170/users/xuid(2535458173220066)/roles", httpCall->PathQueryFragment.to_string());

        VERIFY_ARE_EQUAL(result->Size, (unsigned int)3);
        bool modFound = false;
        for (const auto& role : result)
        {
            if (role == ClubRole::Moderator)
            {
                modFound = true;
                break;
            }
        }
        VERIFY_ARE_EQUAL(modFound, true);
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END