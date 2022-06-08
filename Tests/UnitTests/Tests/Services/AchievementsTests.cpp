// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "UnitTestIncludes.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

const char defaultAchievementResponse[] =
R"(
{
    "achievements":
    [{
        "id":"3",
        "serviceConfigId" : "b5dd9daf-0000-0000-0000-000000000000",
        "name" : "Default NameString for Microsoft Achievements Sample Achievement 3",
        "titleAssociations" :
        [{
            "name":"Microsoft Achievements Sample",
            "id" : 3051199919,
            "version" : "abc"
        }],
        "progressState":"Achieved",
        "progression" :
        {
            "requirements":
            [{
                "id":"12345678-1234-1234-1234-123456789111",
                "current" : null,
                "target" : "100",
                "operationType" : "sum",
                "ruleParticipationType" : "Individual"
            },
            {
                "id":"12345678-1234-1234-1234-123456789222",
                "current" : null,
                "target" : "100",
                "operationType" : "sum",
                "ruleParticipationType" : "Group"
            }],
            "timeUnlocked":"2013-01-17T03:19:00.3087016Z"
        },
        "mediaAssets":
        [{
            "name":"Icon Name",
            "type" : "Icon",
            "url" : "http://www.xbox.com/"
        }],
        "platforms" : ["Durango", "Xbox360"],
        "isSecret" : true,
        "description" : "Default DescriptionString for Microsoft Achievements Sample Achievement 3",
        "lockedDescription" : "Default UnachievedString for Microsoft Achievements Sample Achievement 3",
        "productId" : "12345678-1234-1234-1234-123456789012",
        "achievementType" : "Challenge",
        "participationType" : "Individual",
        "timeWindow" :
        {
            "startDate":"2013-02-01T00:00:00Z",
            "endDate" : "2100-07-01T00:00:00Z"
        },
        "rewards" :
        [{
            "name":null,
            "description" : null,
            "value" : "10",
            "type" : "Gamerscore",
            "valueType" : "Int",
            "mediaAsset" : null
        },
        {
            "name":"Default Name for InAppReward for Microsoft Achievements Sample Achievement 3",
            "description" : "Default Description for InAppReward for Microsoft Achievements Sample Achievement 3",
            "value" : "1",
            "type" : "InApp",
            "valueType" : "String",
            "mediaAsset" : {"name":"Icon Name", "type" : "Icon", "url" : "http://www.xbox.com"}
        }],
        "estimatedTime":"06:12:14",
        "deeplink" : "aWFtYWRlZXBsaW5r",
        "isRevoked" : false
    }],
    "pagingInfo":
    {
        "continuationToken":null,
        "totalRecords" : 1
    }
}
)";

const char rtaAchievementProgressChangedPayload[] =
R"(
{
    "progression": [
        {
            "id" : "1",
            "progressState" : "InProgress",
            "timeUnlocked" : "2013-01-17T03:19:00.3087016Z",
            "requirements" : [
                {
                    "id":"12345678-1234-1234-1234-123456789012", 
                    "current":"1", 
                    "target":"100",
                    "operationType" : "sum",
                    "valueType" : "Integer",
                    "ruleParticipationType" : "Individual"
                }
            ]
        }
    ],
    "serviceConfigId" : "87654321-4321-4321-4321-210987654321"
}
)";

DEFINE_TEST_CLASS(AchievementsTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(AchievementsTests);

    static void VerifyDefaultAchievement(XblAchievementsResultHandle achievementsHandle, JsonValue& achievementToVerify)
    {
        UNREFERENCED_PARAMETER(achievementToVerify);

        const XblAchievement* achievements{ nullptr };
        size_t achievementsCount{ 0 };
        VERIFY_SUCCEEDED(XblAchievementsResultGetAchievements(achievementsHandle, &achievements, &achievementsCount));

        VERIFY_ARE_EQUAL_UINT(1, achievementsCount);
        auto& achievement = achievements[0];

        // Do simple type verify 
        VERIFY_ARE_EQUAL_STR(achievement.id, achievementToVerify["id"].GetString());
        VERIFY_ARE_EQUAL_STR(achievement.serviceConfigurationId, achievementToVerify["serviceConfigId"].GetString());
        VERIFY_ARE_EQUAL_STR(achievement.name, achievementToVerify["name"].GetString());
        //VERIFY_ARE_EQUAL_STR(achievement.progressState, achievementToVerify[L"progressState"].as_string());
        VERIFY_ARE_EQUAL(achievement.isSecret, achievementToVerify["isSecret"].GetBool());
        VERIFY_ARE_EQUAL_STR(achievement.unlockedDescription, achievementToVerify["description"].GetString());
        VERIFY_ARE_EQUAL_STR(achievement.lockedDescription, achievementToVerify["lockedDescription"].GetString());
        VERIFY_ARE_EQUAL_STR(achievement.productId, achievementToVerify["productId"].GetString());
        //VERIFY_ARE_EQUAL_STR(achievement.type, achievementToVerify[L"achievementType"].as_string());
        //VERIFY_ARE_EQUAL_STR(achievement.participationType, achievementToVerify[L"participationType"].as_string());
        VERIFY_ARE_EQUAL_STR(achievement.deepLink, achievementToVerify["deeplink"].GetString());
        VERIFY_ARE_EQUAL(achievement.isRevoked, achievementToVerify["isRevoked"].GetBool());
        //VERIFY_ARE_EQUAL_STR(TimeSpanToString(achievement.estimatedUnlockTime), achievementToVerify[L"estimatedTime"].as_string());

        // platforms available on
        VERIFY_ARE_EQUAL_INT(achievement.platformsAvailableOnCount, achievementToVerify["platforms"].Size());
        VERIFY_ARE_EQUAL_STR(achievement.platformsAvailableOn[0], "Durango");
        VERIFY_ARE_EQUAL_STR(achievement.platformsAvailableOn[1], "Xbox360");

        // title association
        VERIFY_ARE_EQUAL_INT(achievement.titleAssociationsCount, achievementToVerify["titleAssociations"].Size());
        auto titleAssociation = achievement.titleAssociations[0];
        JsonValue& titleAssociationJson = achievementToVerify["titleAssociations"][0];
        VERIFY_ARE_EQUAL_STR(titleAssociation.name, titleAssociationJson["name"].GetString());
        VERIFY_ARE_EQUAL_INT(titleAssociation.titleId, titleAssociationJson["id"].GetUint());

        //progression
        // We lost precision after seconds in parsing, thus only compare first characters before that.
        JsonValue& progressionJson = achievementToVerify["progression"];
        //VERIFY_ARE_EQUAL(DateTimeToString(achievement.Progression->TimeUnlocked).substr(0, DATETIME_STRING_LENGTH_TO_SECOND),
        //progressionJson[L"timeUnlocked"].as_string().substr(0, DATETIME_STRING_LENGTH_TO_SECOND));

        // progression/requirements
        VERIFY_ARE_EQUAL_INT(achievement.progression.requirementsCount, progressionJson["requirements"].Size());
        // progression/requirements/0
        auto requirements0 = achievement.progression.requirements[0];
        JsonValue& requirementsJson0 = progressionJson["requirements"][0];
        VERIFY_ARE_EQUAL_STR(requirements0.id, requirementsJson0["id"].GetString());
        VERIFY_ARE_EQUAL_STR(requirements0.currentProgressValue, "");
        VERIFY_ARE_EQUAL_STR(requirements0.targetProgressValue, requirementsJson0["target"].GetString());

        // progression/requirements/1
        auto requirements1 = achievement.progression.requirements[1];
        JsonValue& requirementsJson1 = progressionJson["requirements"][1];
        VERIFY_ARE_EQUAL_STR(requirements1.id, requirementsJson1["id"].GetString());
        VERIFY_ARE_EQUAL_STR(requirements1.currentProgressValue, "");
        VERIFY_ARE_EQUAL_STR(requirements1.targetProgressValue, requirementsJson0["target"].GetString());

        // mediaAssets
        auto mediaAssets = achievement.mediaAssets;
        JsonValue& mediaAssetsJson = achievementToVerify["mediaAssets"];
        VERIFY_ARE_EQUAL_INT(achievement.mediaAssetsCount, mediaAssetsJson.Size());
        auto mediaAsset0 = mediaAssets[0];
        JsonValue& mediaAsset0Json = mediaAssetsJson[0];
        VERIFY_ARE_EQUAL_STR(mediaAsset0.name, mediaAsset0Json["name"].GetString());
        //VERIFY_ARE_EQUAL_STR(mediaAsset0.mediaAssetType, mediaAsset0Json[L"type"].as_string());
        VERIFY_ARE_EQUAL_STR(mediaAsset0.url, mediaAsset0Json["url"].GetString());

        // timeWindow
        auto timeWindow = achievement.available;
        JsonValue& timeWindowJson = achievementToVerify["timeWindow"];
        UNREFERENCED_PARAMETER(timeWindowJson);
        //TEST_LOG(DateTimeToString(timeWindow->EndDate).substr(0, DATETIME_STRING_LENGTH_TO_SECOND));
        //TEST_LOG(timeWindowJson[L"endDate"].as_string().substr(0, DATETIME_STRING_LENGTH_TO_SECOND));
        //VERIFY_ARE_EQUAL(DateTimeToString(timeWindow->StartDate).substr(0, DATETIME_STRING_LENGTH_TO_SECOND), timeWindowJson[L"startDate"].as_string().substr(0, DATETIME_STRING_LENGTH_TO_SECOND));
        //VERIFY_ARE_EQUAL(DateTimeToString(timeWindow->EndDate).substr(0, DATETIME_STRING_LENGTH_TO_SECOND), timeWindowJson[L"endDate"].as_string().substr(0, DATETIME_STRING_LENGTH_TO_SECOND));

        // rewards
        auto rewards = achievement.rewards;
        JsonValue& rewardsJson = achievementToVerify["rewards"];
        VERIFY_ARE_EQUAL_INT(achievement.rewardsCount, rewardsJson.Size());
        //rewards/0
        auto rewards0 = rewards[0];
        JsonValue& rewards0Json = rewardsJson[0];
        VERIFY_ARE_EQUAL_STR(rewards0.name, "");
        VERIFY_ARE_EQUAL_STR(rewards0.description, "");
        VERIFY_ARE_EQUAL_STR(rewards0.value, rewards0Json["value"].GetString());
        //VERIFY_ARE_EQUAL_STR(rewards0.rewardType, rewards0Json[L"type"].as_string());
        VERIFY_ARE_EQUAL_STR(rewards0.valueType, "Int");
    }

    DEFINE_TEST_CASE(TestXblAchievementsUpdateAchievementAsync)
    {
        TEST_LOG(L"Test starting: TestXblAchievementsUpdateAchievementAsync");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        xsapi_internal_stringstream url;
        url << "https://achievements.xboxlive.com/users/xuid(" << xboxLiveContext->Xuid() << ")/achievements/mockscid/update";
        auto mock = std::make_shared<HttpMock>("POST", url.str(), 304 );
        bool requestWellFormed{ true };
        mock->SetMockMatchedCallback([&requestWellFormed](HttpMock* mock, xsapi_internal_string requestUrl, xsapi_internal_string requestBody)
            {
                UNREFERENCED_PARAMETER(mock);
                UNREFERENCED_PARAMETER(requestUrl);
                auto expectedRequest = R"({"action":"progressUpdate","serviceConfigId":"mockscid","titleId":1234,"userId":"101010101010","achievements":[{"id":"1","percentComplete":100}]})";
                requestWellFormed &= (requestBody == expectedRequest);
            });

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblAchievementsUpdateAchievementAsync(
            xboxLiveContext.get(),
            xboxLiveContext->Xuid(),
            "1",
            100,
            &async
        ));

        auto hr = XAsyncGetStatus(&async, true);
        VERIFY_ARE_EQUAL_UINT(hr, HTTP_E_STATUS_NOT_MODIFIED);
        VERIFY_ARE_EQUAL_UINT(XblGetErrorCondition(hr), XblErrorCondition::Http304NotModified);
        VERIFY_IS_TRUE(requestWellFormed);
    }

    DEFINE_TEST_CASE(TestGetAchievement)
    {
        TEST_LOG(L"Test starting: TestGetAchievement");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        JsonDocument responseJson;
        responseJson.Parse(defaultAchievementResponse);

        xsapi_internal_stringstream url;
        url << "https://achievements.xboxlive.com/users/xuid(" << xboxLiveContext->Xuid() << ")/achievements/serviceconfigurationid/achievementId";
        HttpMock mock("POST", url.str(), 200, responseJson);

        JsonValue& achievementToVerify = responseJson["achievements"][0];

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblAchievementsGetAchievementAsync(xboxLiveContext.get(), xboxLiveContext->Xuid(), "serviceConfigurationId", "achievementId", &async));
        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));

        XblAchievementsResultHandle achievementsHandle{ nullptr };
        VERIFY_SUCCEEDED(XblAchievementsGetAchievementResult(&async, &achievementsHandle));

        VerifyDefaultAchievement(achievementsHandle, achievementToVerify);
    }

    DEFINE_TEST_CASE(TestGetAchievementBadData)
    {
        TEST_LOG(L"Test starting: TestGetAchievementBadData");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        JsonDocument responseJson;
        responseJson.Parse(defaultAchievementResponse);

        xsapi_internal_stringstream url;
        url << "https://achievements.xboxlive.com/users/xuid(" << xboxLiveContext->Xuid() << ")/achievements/serviceconfigurationid/achievementId";
        HttpMock mock("POST", url.str(), 404);

        JsonValue& achievementToVerify = responseJson["achievements"][0];
        UNREFERENCED_PARAMETER(achievementToVerify);

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblAchievementsGetAchievementAsync(xboxLiveContext.get(), xboxLiveContext->Xuid(), "serviceConfigurationId", "achievementId", &async));

        auto hr = XAsyncGetStatus(&async, true);
        VERIFY_ARE_EQUAL_UINT(HTTP_E_STATUS_NOT_FOUND, hr);
    }

    DEFINE_TEST_CASE(TestGetAchievements)
    {
        TEST_LOG(L"Test starting: TestGetAchievements");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        JsonDocument responseJson; 
        responseJson.Parse(defaultAchievementResponse);

        xsapi_internal_stringstream url;
        url << "https://achievements.xboxlive.com/users/xuid(" << xboxLiveContext->Xuid() << ")/achievements?titleId=1234&maxItems=100";
        auto mock = std::make_shared<HttpMock>("GET", url.str(), 200, responseJson);
        bool requestWellFormed{ true };
        mock->SetMockMatchedCallback([&requestWellFormed](HttpMock* mock, xsapi_internal_string requestUrl, xsapi_internal_string requestBody)
            {
                UNREFERENCED_PARAMETER(mock);
                UNREFERENCED_PARAMETER(requestUrl);
                requestWellFormed &= (requestBody == "");
            });

        JsonValue& achievementToVerify = responseJson["achievements"][0];

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblAchievementsGetAchievementsForTitleIdAsync(
            xboxLiveContext.get(), 
            xboxLiveContext->Xuid(), 
            1234,
            XblAchievementType::All,
            false,
            XblAchievementOrderBy::DefaultOrder,
            0,
            100,
            &async));

        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));

        XblAchievementsResultHandle achievements{ nullptr };
        VERIFY_SUCCEEDED(XblAchievementsGetAchievementResult(&async, &achievements));
        VERIFY_IS_TRUE(requestWellFormed);

        VerifyDefaultAchievement(achievements, achievementToVerify);
    }

    DEFINE_TEST_CASE(TestGetAchievementsEmptyResult)
    {
        TEST_LOG(L"Test starting: TestGetAchievementsEmptyResult");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        JsonDocument responseJson;
        responseJson.Parse(R"({"achievements":[]})");

        xsapi_internal_stringstream url;
        url << "https://achievements.xboxlive.com/users/xuid(" << xboxLiveContext->Xuid() << ")/achievements?titleId=1234&maxItems=100";
        auto mock = std::make_shared<HttpMock>("GET", url.str(), 200, responseJson);
        bool requestWellFormed{ true };
        mock->SetMockMatchedCallback([&requestWellFormed](HttpMock* mock, xsapi_internal_string requestUrl, xsapi_internal_string requestBody)
            {
                UNREFERENCED_PARAMETER(mock);
                UNREFERENCED_PARAMETER(requestUrl);
                requestWellFormed &= (requestBody == "");
            });

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblAchievementsGetAchievementsForTitleIdAsync(
            xboxLiveContext.get(),
            xboxLiveContext->Xuid(),
            1234,
            XblAchievementType::All,
            false,
            XblAchievementOrderBy::DefaultOrder,
            0,
            100,
            &async));

        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));

        XblAchievementsResultHandle achievementsHandle{ nullptr };
        VERIFY_SUCCEEDED(XblAchievementsGetAchievementResult(&async, &achievementsHandle));
        VERIFY_IS_TRUE(requestWellFormed);

        const XblAchievement* achievements{ nullptr };
        size_t achievementsCount{ 0 };
        VERIFY_SUCCEEDED(XblAchievementsResultGetAchievements(achievementsHandle, &achievements, &achievementsCount));
        VERIFY_ARE_EQUAL_INT(0, achievementsCount);
    }

    DEFINE_TEST_CASE(TestGetAchievementsInvalidArgs)
    {
        TEST_LOG(L"Test starting: TestGetAchievementsInvalidArgs");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        JsonDocument responseJson;
        responseJson.Parse(R"({"achievements":[]})");

        TEST_LOG(L"TestGetAchievementsForTitleIdAsyncInvalidArgs: Null xboxUserId param.");
        xsapi_internal_stringstream url;
        url << "https://achievements.xboxlive.com/users/xuid(" << xboxLiveContext->Xuid() << ")/achievements?titleId=1234&maxItems=100";
        HttpMock mock("GET", url.str(), 200, responseJson);

        XAsyncBlock async{};
        VERIFY_ARE_EQUAL_UINT(XblAchievementsGetAchievementsForTitleIdAsync(
            xboxLiveContext.get(),
            xboxLiveContext->Xuid(),
            1234,
            XblAchievementType::All,
            false,
            XblAchievementOrderBy::DefaultOrder,
            0,
            100,
            nullptr), E_INVALIDARG);

        VERIFY_ARE_EQUAL_UINT(XblAchievementsGetAchievementsForTitleIdAsync(
            nullptr,
            xboxLiveContext->Xuid(),
            1234,
            XblAchievementType::All,
            false,
            XblAchievementOrderBy::DefaultOrder,
            0,
            100,
            &async), E_INVALIDARG);
    }

    DEFINE_TEST_CASE(TestGetAchievementInvalidArgs)
    {
        TEST_LOG(L"Test starting: TestGetAchievementInvalidArgs");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        JsonDocument responseJson;
        responseJson.Parse(R"({"achievements":[]})");

        TEST_LOG(L"TestGetAchievementsForTitleIdAsyncInvalidArgs: Null xboxUserId param.");
        xsapi_internal_stringstream url;
        url << "https://achievements.xboxlive.com/users/xuid(" << xboxLiveContext->Xuid() << ")/achievements?titleId=1234&maxItems=100";
        HttpMock mock("GET", url.str(), 200, responseJson);

        XAsyncBlock async{};
        VERIFY_ARE_EQUAL_UINT(XblAchievementsGetAchievementAsync(
            nullptr,
            xboxLiveContext->Xuid(),
            "serviceConfigId",
            "1",
            &async), E_INVALIDARG);

        VERIFY_ARE_EQUAL_UINT(XblAchievementsGetAchievementAsync(
            xboxLiveContext.get(),
            xboxLiveContext->Xuid(),
            nullptr,
            "1",
            &async), E_INVALIDARG);

        VERIFY_ARE_EQUAL_UINT(XblAchievementsGetAchievementAsync(
            xboxLiveContext.get(),
            xboxLiveContext->Xuid(),
            "serviceConfigId",
            nullptr,
            &async), E_INVALIDARG);

        VERIFY_ARE_EQUAL_UINT(XblAchievementsGetAchievementAsync(
            xboxLiveContext.get(),
            xboxLiveContext->Xuid(),
            "serviceConfigId",
            "1",
            nullptr), E_INVALIDARG);
    }

    DEFINE_TEST_CASE(TestUpdateAchievementInvalidArgs)
    {
        TEST_LOG(L"Test starting: TestUpdateAchievementInvalidArgs");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        JsonDocument responseJson;
        responseJson.Parse(R"({"achievements":[]})");

        TEST_LOG(L"TestGetAchievementsForTitleIdAsyncInvalidArgs: Null xboxUserId param.");
        xsapi_internal_stringstream url;
        url << "https://achievements.xboxlive.com/users/xuid(" << xboxLiveContext->Xuid() << ")/achievements?titleId=1234&maxItems=100";
        HttpMock mock("GET", url.str(), 200, responseJson);

        XAsyncBlock async{};
        VERIFY_ARE_EQUAL_UINT(XblAchievementsUpdateAchievementAsync(
            nullptr,
            xboxLiveContext->Xuid(),
            "1",
            100,
            &async), 
            E_INVALIDARG);

        VERIFY_ARE_EQUAL_UINT(XblAchievementsUpdateAchievementAsync(
            xboxLiveContext.get(),
            xboxLiveContext->Xuid(),
            nullptr,
            100,
            &async),
            E_INVALIDARG);

        VERIFY_ARE_EQUAL_UINT(XblAchievementsUpdateAchievementAsync(
            xboxLiveContext.get(),
            xboxLiveContext->Xuid(),
            "1",
            100,
            nullptr),
            E_INVALIDARG);
    }

    DEFINE_TEST_CASE(TestRTAAchievementProgressChange)
    {
        TEST_LOG(L"Test starting: TestRTAAchievementProgressChange");
        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        auto& mockRtaService{ MockRealTimeActivityService::Instance() };

        mockRtaService.SetSubscribeHandler([&](uint32_t n, xsapi_internal_string uri)
        {
            xsapi_internal_stringstream expectedUri;
            expectedUri << "https://achievements.xboxlive.com/users/xuid(" << xboxLiveContext->Xuid() << ")/achievements/" << AppConfig::Instance()->Scid();
            VERIFY_ARE_EQUAL_STR_IGNORE_CASE(uri.data(), expectedUri.str().data());

            mockRtaService.CompleteSubscribeHandshake(n);

            // Immediately raise an event
            mockRtaService.RaiseEvent(uri, rtaAchievementProgressChangedPayload);
        });

        struct HandlerContext
        {
            Event notificationReceived;
            struct Entry
            {
                xbox::services::String achievementId;
                XblAchievementProgressState progressState{ XblAchievementProgressState::Unknown };
                xbox::services::Vector<XblAchievementRequirement> requirements;
            };
            xbox::services::Vector<Entry> entries;
            
            // To keep the strings alive outside the handler for this test
            xbox::services::String idContainer{};
            xbox::services::String currentProgressContainer{};
            xbox::services::String targetProgressContainer{};
        } context;

        auto handlerToken = XblAchievementsAddAchievementProgressChangeHandler(xboxLiveContext.get(),
            [](const XblAchievementProgressChangeEventArgs* args, void* context)
            {
                auto c{ static_cast<HandlerContext*>(context) };
                for (uint32_t entryIndex = 0; entryIndex < args->entryCount; ++entryIndex)
                {
                    HandlerContext::Entry entry;
                    const XblAchievementProgressChangeEntry& updateEntry = args->updatedAchievementEntries[entryIndex];
                    entry.achievementId = updateEntry.achievementId;
                    entry.progressState = updateEntry.progressState;
                    entry.requirements = xbox::services::Vector<XblAchievementRequirement>(updateEntry.progression.requirementsCount);

                    c->idContainer = updateEntry.progression.requirements[0].id;
                    c->currentProgressContainer = updateEntry.progression.requirements[0].currentProgressValue;
                    c->targetProgressContainer = updateEntry.progression.requirements[0].targetProgressValue;
                    entry.requirements[0] = { c->idContainer.c_str(), c->currentProgressContainer.c_str(), c->targetProgressContainer.c_str() };

                    c->entries.push_back(entry);
                }

                c->notificationReceived.Set();
            },
            &context
        );

        context.notificationReceived.Wait();

        VERIFY_ARE_EQUAL_INT(1, context.entries.size());
        VERIFY_ARE_EQUAL_STR("1", context.entries[0].achievementId.c_str());
        VERIFY_IS_TRUE(context.entries[0].progressState == XblAchievementProgressState::InProgress);
        VERIFY_ARE_EQUAL_INT(1, context.entries[0].requirements.size());
        VERIFY_ARE_EQUAL_STR("12345678-1234-1234-1234-123456789012", context.entries[0].requirements[0].id);
        VERIFY_ARE_EQUAL_STR("1", context.entries[0].requirements[0].currentProgressValue);
        VERIFY_ARE_EQUAL_STR("100", context.entries[0].requirements[0].targetProgressValue);

        VERIFY_SUCCEEDED(XblAchievementsRemoveAchievementProgressChangeHandler(xboxLiveContext.get(), handlerToken));
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

