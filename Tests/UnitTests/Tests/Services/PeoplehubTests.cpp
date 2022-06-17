// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "UnitTestIncludes.h"
#include "peoplehub_service.h"

using namespace xbox::services;
using namespace xbox::services::social::manager;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

const char peoplehubResponse[] =
R"(
{
    "people": [
        {
            "xuid": "2814664990767463",
            "isFavorite": true,
            "isFollowingCaller": true,
            "isFollowedByCaller": true,
            "isIdentityShared": false,
            "displayName": "2 Dev 123410299",
            "realName": "Dev Account",
            "displayPicRaw": "http://images-eds.xboxlive.com/image?url=z951ykn43p4FqWbbFvR2Ec.8vbDhj8G2Xe7JngaTToBrrCmIEEXHC9UNrdJ6P7KIwuPiuIs6TLDV4WsQAGzSwnTHQB9h_UfPa19pe4OAgFTWAPsXVneopydpL6qncU1N&format=png",
            "useAvatar": true,
            "gamertag": "2 Dev 123410299",
            "modernGamertag": "2 Dev 123410299",
            "modernGamertagSuffix": "",
            "uniqueModernGamertag": "2 Dev 123410299",
            "gamerScore": "0",
            "xboxOneRep": "GoodPlayer",
            "presenceState": "Offline",
            "presenceText": "Offline",
            "presenceDevices": null,
            "isBroadcasting": false,
            "suggestion": null,
            "titleHistory": null,
            "multiplayerSummary": {
                "InMultiplayerSession": 0,
                "InParty": 0
            },
            "recentPlayer": null,
            "follower": null,
            "preferredColor": {
                "primaryColor": "107c10",
                "secondaryColor": "05d21e",
                "tertiaryColor": "299c10"
            },
            "titlePresence": null,
            "titleSummaries": null,
            "presenceTitleIds": null,
            "detail": null,
            "presenceDetails": [{
                "IsBroadcasting": false,
                "Device": "PC",
                "PresenceText": "Home",
                "State": "Active",
                "TitleId": "1234"
            }]
        },
        {
            "xuid": "2814654081790887",
            "isFavorite": true,
            "isFollowingCaller": false,
            "isFollowedByCaller": true,
            "isIdentityShared": false,
            "displayName": "2 Dev 554865546",
            "realName": "",
            "displayPicRaw": "http://images-eds.xboxlive.com/image?url=z951ykn43p4FqWbbFvR2Ec.8vbDhj8G2Xe7JngaTToBrrCmIEEXHC9UNrdJ6P7KI6TRSO7kB1LAmJSmUVW5wJqn2n6xd9r5UHGMgD0o0KoOwHN61vlgC862huSRkTjhm&background=0xababab&mode=Padding&format=png",
            "useAvatar": true,
            "gamertag": "2 Dev 554865546",
            "modernGamertag": "2 Dev 554865546",
            "modernGamertagSuffix": "",
            "uniqueModernGamertag": "2 Dev 554865546",
            "gamerScore": "0",
            "xboxOneRep": "GoodPlayer",
            "presenceState": "Offline",
            "presenceText": "Offline",
            "presenceDevices": null,
            "isBroadcasting": false,
            "suggestion": null,
            "titleHistory": null,
            "multiplayerSummary": {
                "InMultiplayerSession": 0,
                "InParty": 0
            },
            "recentPlayer": null,
            "follower": null,
            "preferredColor": {
                "primaryColor": "107c10",
                "secondaryColor": "05d21e",
                "tertiaryColor": "299c10"
            },
            "titlePresence": null,
            "titleSummaries": null,
            "presenceTitleIds": null,
            "detail": null,
            "presenceDetails": [{
                "IsBroadcasting": false,
                "Device": "PC",
                "PresenceText": "Home",
                "State": "Active",
                "TitleId": "1234"
            }]
        },
        {
            "xuid": "2814613569642996",
            "isFavorite": false,
            "isFollowingCaller": false,
            "isFollowedByCaller": true,
            "isIdentityShared": false,
            "displayName": "2 Dev 417471033",
            "realName": "",
            "displayPicRaw": "http://images-eds.xboxlive.com/image?url=z951ykn43p4FqWbbFvR2Ec.8vbDhj8G2Xe7JngaTToBrrCmIEEXHC9UNrdJ6P7KI6TRSO7kB1LAmJSmUVW5wJqn2n6xd9r5UHGMgD0o0KoOwHN61vlgC862huSRkTjhm&background=0xababab&mode=Padding&format=png",
            "useAvatar": true,
            "gamertag": "2 Dev 417471033",
            "modernGamertag": "2 Dev 417471033",
            "modernGamertagSuffix": "",
            "uniqueModernGamertag": "2 Dev 417471033",
            "gamerScore": "0",
            "xboxOneRep": "InDanger",
            "presenceState": "Offline",
            "presenceText": "Offline",
            "presenceDevices": null,
            "isBroadcasting": false,
            "suggestion": null,
            "titleHistory": null,
            "multiplayerSummary": {
                "InMultiplayerSession": 0,
                "InParty": 0
            },
            "recentPlayer": null,
            "follower": null,
            "preferredColor": {
                "primaryColor": "107c10",
                "secondaryColor": "05d21e",
                "tertiaryColor": "299c10"
            },
            "titlePresence": null,
            "titleSummaries": null,
            "presenceTitleIds": null,
            "detail": null,
            "presenceDetails": [{
                "IsBroadcasting": false,
                "Device": "PC",
                "PresenceText": "Home",
                "State": "Active",
                "TitleId": "1234"
            }]
        },
        {
            "xuid": "2814671404555632",
            "isFavorite": false,
            "isFollowingCaller": true,
            "isFollowedByCaller": true,
            "isIdentityShared": false,
            "displayName": "2 Dev 766909125",
            "realName": "",
            "displayPicRaw": "http://images-eds.xboxlive.com/image?url=z951ykn43p4FqWbbFvR2Ec.8vbDhj8G2Xe7JngaTToBrrCmIEEXHC9UNrdJ6P7KI6TRSO7kB1LAmJSmUVW5wJqn2n6xd9r5UHGMgD0o0KoOwHN61vlgC862huSRkTjhm&background=0xababab&mode=Padding&format=png",
            "useAvatar": true,
            "gamertag": "2 Dev 766909125",
            "modernGamertag": "2 Dev 766909125",
            "modernGamertagSuffix": "",
            "uniqueModernGamertag": "2 Dev 766909125",
            "gamerScore": "0",
            "xboxOneRep": "AvoidMe",
            "presenceState": "Offline",
            "presenceText": "Offline",
            "presenceDevices": null,
            "isBroadcasting": false,
            "suggestion": null,
            "titleHistory": {
                "titleName": "Forza Horizon 2",
                "titleId": "446059611",
                "lastTimePlayed": "2015-01-26T22:54:54.6630000Z",
                "lastTimePlayedText": "8 months ago"
            },
            "multiplayerSummary": {
                "InMultiplayerSession": 0,
                "InParty": 0
            },
            "recentPlayer": null,
            "follower": null,
            "preferredColor": {
                "primaryColor": "107c10",
                "secondaryColor": "05d21e",
                "tertiaryColor": "299c10"
            },
            "titlePresence": null,
            "titleSummaries": null,
            "presenceTitleIds": null,
            "detail": null,
            "presenceDetails": [{
                "IsBroadcasting": false,
                "Device": "PC",
                "PresenceText": "Home",
                "State": "Active",
                "TitleId": "1234"
            },{
                "IsBroadcasting": false,
                "Device": "Xbox",
                "PresenceText": "Home",
                "State": "Active",
                "TitleId": "1234"
            }]
        }
    ]
})";

const char peoplehubInvalid[] =
R"(
{
    "people": [
        {
        }
    ]
})";

DEFINE_TEST_CLASS(PeoplehubTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(PeoplehubTests);

    struct PeoplehubTestEnvironment : public TestEnvironment
    {
        PeoplehubTestEnvironment() noexcept
            : XboxLiveContext{ CreateMockXboxLiveContext() }
        {
            auto userResult = XboxLiveContext->User().Copy();
            PeoplehubService = std::make_shared<xbox::services::social::manager::PeoplehubService>(
                userResult.ExtractPayload(),
                std::make_shared<XboxLiveContextSettings>(),
                AppConfig::Instance()->TitleId()
            );
        }

        std::shared_ptr<XblContext> XboxLiveContext;
        std::shared_ptr<PeoplehubService> PeoplehubService;
    };

    static void VerifyPresenceRecord(const XblSocialManagerPresenceRecord & presenceRecord, JsonValue& response)
    {
        VERIFY_IS_TRUE(presenceRecord.presenceTitleRecordCount == response.Size());
        for (uint32_t i = 0; i < response.Size(); ++i)
        {
            auto& jsonEntry = response[i];
            auto& titleEntry = presenceRecord.presenceTitleRecords[i];

            VERIFY_ARE_EQUAL(jsonEntry["IsBroadcasting"].GetBool(), titleEntry.isBroadcasting);
            VERIFY_IS_TRUE(xbox::services::presence::DeviceRecord::DeviceTypeFromString(jsonEntry["Device"].GetString()) == static_cast<XblPresenceDeviceType>(titleEntry.deviceType));
            auto state = jsonEntry["State"].GetString();
            auto presenceState = (utils::str_icmp(state, "active") == 0);
            VERIFY_ARE_EQUAL(presenceState, titleEntry.isTitleActive);
            VERIFY_IS_TRUE(utils::internal_string_to_uint32(jsonEntry["TitleId"].GetString()) == titleEntry.titleId);
            VERIFY_ARE_EQUAL_STR(jsonEntry["PresenceText"].GetString(), titleEntry.presenceText);
        }
    }

    static void VerifyTitleHistory(const XblTitleHistory& titleHistory, JsonValue& response)
    {
        if (response.IsNull())
        {
            VERIFY_IS_FALSE(titleHistory.hasUserPlayed);
        }
        else
        {
            if (response.HasMember("lastTimePlayed") && response["lastTimePlayed"].IsString())
            {
                time_t jsonTime;
                JsonUtils::ExtractJsonTimeT(response, "lastTimePlayed", jsonTime);
                VERIFY_ARE_EQUAL_UINT(titleHistory.lastTimeUserPlayed, jsonTime);
            }

            if (response.HasMember("lastTimePlayedText") && response["lastTimePlayedText"].IsString())
            {
                char jsonTimeStr[XBL_LAST_TIME_PLAYED_CHAR_SIZE];
                JsonUtils::ExtractJsonStringToCharArray(response, "lastTimePlayedText", jsonTimeStr, XBL_LAST_TIME_PLAYED_CHAR_SIZE);
                VERIFY_ARE_EQUAL_STR(titleHistory.lastTimeUserPlayedText, jsonTimeStr);
            }
        }
    }

    static void VerifyPreferredColor(const XblPreferredColor & preferredColor, JsonValue& response)
    {
        if (response.IsNull())
        {
            VERIFY_IS_TRUE(strlen(preferredColor.primaryColor) == 0);
            VERIFY_IS_TRUE(strlen(preferredColor.secondaryColor) == 0);
            VERIFY_IS_TRUE(strlen(preferredColor.tertiaryColor) == 0);
        }
        else
        {
            VERIFY_ARE_EQUAL_STR(preferredColor.primaryColor, response["primaryColor"].GetString());
            VERIFY_ARE_EQUAL_STR(preferredColor.secondaryColor, response["secondaryColor"].GetString());
            VERIFY_ARE_EQUAL_STR(preferredColor.tertiaryColor, response["tertiaryColor"].GetString());
        }
    }

    static void VerifyXboxSocialUser(const XblSocialManagerUser & xboxSocialUser, JsonValue& jsonUser)
    {
        VERIFY_ARE_EQUAL(xboxSocialUser.isFavorite, jsonUser["isFavorite"].GetBool());
        VERIFY_ARE_EQUAL(xboxSocialUser.isFollowingUser, jsonUser["isFollowingCaller"].GetBool());
        VERIFY_ARE_EQUAL(xboxSocialUser.isFollowedByCaller, jsonUser["isFollowedByCaller"].GetBool());
        VERIFY_ARE_EQUAL_STR(xboxSocialUser.displayName, jsonUser["displayName"].GetString());
        std::cout << xboxSocialUser.realName << " : " << jsonUser["realName"].GetString() << std::endl;
        VERIFY_ARE_EQUAL_STR(xboxSocialUser.realName, jsonUser["realName"].GetString());
        VERIFY_ARE_EQUAL_STR(xboxSocialUser.displayPicUrlRaw, jsonUser["displayPicRaw"].GetString());
        VERIFY_ARE_EQUAL(xboxSocialUser.useAvatar, jsonUser["useAvatar"].GetBool());
        VERIFY_ARE_EQUAL_STR(xboxSocialUser.gamertag, jsonUser["gamertag"].GetString());
        VERIFY_ARE_EQUAL_STR(xboxSocialUser.modernGamertag, jsonUser["modernGamertag"].GetString());
        VERIFY_ARE_EQUAL_STR(xboxSocialUser.modernGamertagSuffix, jsonUser["modernGamertagSuffix"].GetString());
        VERIFY_ARE_EQUAL_STR(xboxSocialUser.uniqueModernGamertag, jsonUser["uniqueModernGamertag"].GetString());
        VERIFY_ARE_EQUAL_STR(xboxSocialUser.gamerscore, jsonUser["gamerScore"].GetString());
        VerifyTitleHistory(xboxSocialUser.titleHistory, jsonUser["titleHistory"]);
        VerifyPreferredColor(xboxSocialUser.preferredColor, jsonUser["preferredColor"]);
        VerifyPresenceRecord(xboxSocialUser.presenceRecord, jsonUser["presenceDetails"]);
    }

    DEFINE_TEST_CASE(TestGetSocialUsers)
    {
        TEST_LOG(L"Test starting: TestGetSocialUsers");

        PeoplehubTestEnvironment env{};

        JsonDocument jsonResponse;
        jsonResponse.Parse(peoplehubResponse);

        xsapi_internal_stringstream url;
        url << "https://peoplehub.xboxlive.com/users/xuid(" << env.XboxLiveContext->Xuid() << ")/people/batch/decoration/presenceDetail,preferredcolor";

        auto peoplehubMock = std::make_shared<HttpMock>(
            "POST",
            url.str(),
            200,
            jsonResponse
            );

        Event callComplete;
        Result<Vector<XblSocialManagerUser>> result;

        env.PeoplehubService->GetSocialUsers(env.XboxLiveContext->Xuid(), XblSocialManagerExtraDetailLevel::PreferredColorLevel, { 1 }, {
            [&] (Result<Vector<XblSocialManagerUser>> temp)
            {
                result = temp;
                callComplete.Set();
            }
            });

        callComplete.Wait();

        VERIFY_SUCCEEDED(result.Hresult());
        JsonValue& userGroupArr = jsonResponse["people"];
        VERIFY_ARE_EQUAL(result.Payload().size(), userGroupArr.Size());

        uint64_t counter{ 0 };
        for (auto& user : result.Payload())
        {
            VerifyXboxSocialUser(user, userGroupArr[counter++]);
        }
    }

    DEFINE_TEST_CASE(TestGetSocialGraph)
    {
        TEST_LOG(L"Test starting: TestGetSocialGraph");

        PeoplehubTestEnvironment env{};

        JsonDocument jsonResponse;
        jsonResponse.Parse(peoplehubResponse);

        xsapi_internal_stringstream url;
        url << "https://peoplehub.xboxlive.com/users/xuid(" << env.XboxLiveContext->Xuid() << ")/people/social/decoration/presenceDetail";

        auto peoplehubMock = std::make_shared<HttpMock>(
            "GET",
            url.str(),
            200,
            jsonResponse
            );

        Event callComplete;
        Result<Vector<XblSocialManagerUser>> result;

        env.PeoplehubService->GetSocialGraph(env.XboxLiveContext->Xuid(), XblSocialManagerExtraDetailLevel::NoExtraDetail, {
            [&] (Result<Vector<XblSocialManagerUser>> temp)
            {
                result = temp;
                callComplete.Set();
            }
            });

        callComplete.Wait();

        VERIFY_SUCCEEDED(result.Hresult());
        JsonValue& userGroupArr = jsonResponse["people"];
        VERIFY_ARE_EQUAL(result.Payload().size(), userGroupArr.Size());

        uint64_t counter{ 0 };
        for (auto& user : result.Payload())
        {
            VerifyXboxSocialUser(user, userGroupArr[counter++]);
        }
    }

    DEFINE_TEST_CASE(TestInvalidResponse)
    {
        TEST_LOG(L"Test starting: TestInvalidResponse");

        PeoplehubTestEnvironment env{};

        JsonDocument peoplehubInvalidJson;
        peoplehubInvalidJson.Parse(peoplehubInvalid);
        auto peoplehubMock = std::make_shared<HttpMock>(
            "",
            "https://peoplehub.xboxlive.com",
            200,
            peoplehubInvalidJson
            );

        Event callComplete;
        Result<Vector<XblSocialManagerUser>> result;

        env.PeoplehubService->GetSocialGraph(env.XboxLiveContext->Xuid(), XblSocialManagerExtraDetailLevel::PreferredColorLevel, {
            [&] (Result<Vector<XblSocialManagerUser>> temp)
            {
                result = temp;
                callComplete.Set();
            }
            });

        callComplete.Wait();
        VERIFY_FAILED(result.Hresult());
        VERIFY_IS_TRUE(result.Payload().empty());
    }

    DEFINE_TEST_CASE(TestPartialTitleHistory)
    {
        TEST_LOG(L"Test starting: TestPartialTitleHistory");

        // In some cases PeopleHub service returns a TitleHistory object that isn't fully populated. By design we should
        // deserialize the provided fields and ignore those that are "null"

        PeoplehubTestEnvironment env{};

        JsonDocument jsonResponse;
        jsonResponse.Parse(peoplehubResponse);

        JsonValue titleHistory{ rapidjson::kObjectType };
        titleHistory.AddMember("lastTimePlayed", JsonValue{ rapidjson::kNullType }, jsonResponse.GetAllocator());
        jsonResponse["people"][0]["titleHistory"] = titleHistory.Move();

        HttpMock peopleHubMock{ "GET", "https://peoplehub.xboxlive.com/", 200, jsonResponse };

        Event callComplete;
        Result<Vector<XblSocialManagerUser>> result;

        env.PeoplehubService->GetSocialGraph(env.XboxLiveContext->Xuid(), XblSocialManagerExtraDetailLevel::NoExtraDetail, {
            [&](Result<Vector<XblSocialManagerUser>> temp)
            {
                result = temp;
                callComplete.Set();
            }
            });

        callComplete.Wait();

        VERIFY_SUCCEEDED(result.Hresult());
        JsonValue& userGroupArr = jsonResponse["people"];
        VERIFY_ARE_EQUAL(result.Payload().size(), userGroupArr.Size());

        uint64_t counter{ 0 };
        for (auto& user : result.Payload())
        {
            VerifyXboxSocialUser(user, userGroupArr[counter++]);
        }

    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END