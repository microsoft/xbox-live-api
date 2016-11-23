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
#define TEST_CLASS_OWNER L"jasonsa2"
#define TEST_CLASS_AREA L"Achievements"
#include "UnitTestIncludes.h"
#include <xsapi/xbox_live_context.h>

using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::Achievements;


NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

const std::wstring defaultAchievementResponse =
LR"(
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

DEFINE_TEST_CLASS(AchievementsTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(AchievementsTests)

    void VerifyDefaultAchievement(Achievement^ result, web::json::value achievementToVerify)
    {
        // Do simple type verify 
        VERIFY_ARE_EQUAL(result->Id->Data(), achievementToVerify[L"id"].as_string());
        VERIFY_ARE_EQUAL(result->ServiceConfigurationId->Data(), achievementToVerify[L"serviceConfigId"].as_string());
        VERIFY_ARE_EQUAL(result->Name->Data(), achievementToVerify[L"name"].as_string());
        VERIFY_ARE_EQUAL(result->ProgressState.ToString()->Data(), achievementToVerify[L"progressState"].as_string());
        VERIFY_ARE_EQUAL(result->IsSecret, achievementToVerify[L"isSecret"].as_bool());
        VERIFY_ARE_EQUAL(result->UnlockedDescription->Data(), achievementToVerify[L"description"].as_string());
        VERIFY_ARE_EQUAL(result->LockedDescription->Data(), achievementToVerify[L"lockedDescription"].as_string());
        VERIFY_ARE_EQUAL(result->ProductId->Data(), achievementToVerify[L"productId"].as_string());
        VERIFY_ARE_EQUAL(result->AchievementType.ToString()->Data(), achievementToVerify[L"achievementType"].as_string());
        VERIFY_ARE_EQUAL(result->ParticipationType.ToString()->Data(), achievementToVerify[L"participationType"].as_string());
        VERIFY_ARE_EQUAL(result->DeepLink->Data(), achievementToVerify[L"deeplink"].as_string());
        VERIFY_ARE_EQUAL(result->IsRevoked, achievementToVerify[L"isRevoked"].as_bool());
        VERIFY_ARE_EQUAL(TimeSpanToString(result->EstimatedUnlockTime).c_str(), achievementToVerify[L"estimatedTime"].as_string());

        // platforms available on
        VERIFY_ARE_EQUAL_INT(result->PlatformsAvailableOn->Size, achievementToVerify[L"platforms"].as_array().size());
        VERIFY_ARE_EQUAL(result->PlatformsAvailableOn->GetAt(0)->Data(), std::wstring(L"Durango"));
        VERIFY_ARE_EQUAL(result->PlatformsAvailableOn->GetAt(1)->Data(), std::wstring(L"Xbox360"));

        //title association
        VERIFY_ARE_EQUAL_INT(result->TitleAssociations->Size, achievementToVerify[L"titleAssociations"].as_array().size());
        auto titleAssociation = result->TitleAssociations->GetAt(0);
        auto titleAssociationJson = achievementToVerify[L"titleAssociations"].as_array()[0];
        VERIFY_ARE_EQUAL(titleAssociation->Name->Data(), titleAssociationJson[L"name"].as_string());
        VERIFY_ARE_EQUAL_INT(titleAssociation->TitleId, (unsigned)titleAssociationJson[L"id"].as_integer());

        //progression
        // We lost precision after seconds in parsing, thus only compare first characters before that.
        auto progressionJson = achievementToVerify[L"progression"];
        VERIFY_ARE_EQUAL(DateTimeToString(result->Progression->TimeUnlocked).substr(0, DATETIME_STRING_LENGTH_TO_SECOND).c_str(),
            progressionJson[L"timeUnlocked"].as_string().substr(0, DATETIME_STRING_LENGTH_TO_SECOND));

        // progression/requirements
        VERIFY_ARE_EQUAL_INT(result->Progression->Requirements->Size, progressionJson[L"requirements"].as_array().size());
        // progression/requirements/0
        auto requirements0 = result->Progression->Requirements->GetAt(0);
        auto requirementsJson0 = progressionJson[L"requirements"].as_array()[0];
        VERIFY_ARE_EQUAL(requirements0->Id->Data(), requirementsJson0[L"id"].as_string());
        VERIFY_IS_TRUE(requirements0->CurrentProgressValue->IsEmpty());
        VERIFY_ARE_EQUAL(requirements0->TargetProgressValue->Data(), requirementsJson0[L"target"].as_string());

        // progression/requirements/1
        auto requirements1 = result->Progression->Requirements->GetAt(1);
        auto requirementsJson1 = progressionJson[L"requirements"].as_array()[1];
        VERIFY_ARE_EQUAL(requirements1->Id->Data(), requirementsJson1[L"id"].as_string());
        VERIFY_IS_TRUE(requirements1->CurrentProgressValue->IsEmpty());
        VERIFY_ARE_EQUAL(requirements1->TargetProgressValue->Data(), requirementsJson0[L"target"].as_string());

        // mediaAssets
        auto mediaAssets = result->MediaAssets;
        auto mediaAssetsJson = achievementToVerify[L"mediaAssets"];
        VERIFY_ARE_EQUAL_INT(mediaAssets->Size, mediaAssetsJson.as_array().size());
        auto mediaAsset0 = mediaAssets->GetAt(0);
        auto mediaAsset0Json = mediaAssetsJson[0];
        VERIFY_ARE_EQUAL(mediaAsset0->Name->Data(), mediaAsset0Json[L"name"].as_string());
        VERIFY_ARE_EQUAL(mediaAsset0->MediaAssetType.ToString()->Data(), mediaAsset0Json[L"type"].as_string());
        VERIFY_ARE_EQUAL(mediaAsset0->Url->Data(), mediaAsset0Json[L"url"].as_string());

        //timeWindow
        auto timeWindow = result->Available;
        auto timeWindowJson = achievementToVerify[L"timeWindow"];
        TEST_LOG(DateTimeToString(timeWindow->EndDate).substr(0, DATETIME_STRING_LENGTH_TO_SECOND).c_str());
        TEST_LOG(timeWindowJson[L"endDate"].as_string().substr(0, DATETIME_STRING_LENGTH_TO_SECOND).c_str());
        VERIFY_ARE_EQUAL(DateTimeToString(timeWindow->StartDate).substr(0, DATETIME_STRING_LENGTH_TO_SECOND).c_str(),
            timeWindowJson[L"startDate"].as_string().substr(0, DATETIME_STRING_LENGTH_TO_SECOND));
        VERIFY_ARE_EQUAL(DateTimeToString(timeWindow->EndDate).substr(0, DATETIME_STRING_LENGTH_TO_SECOND).c_str(),
            timeWindowJson[L"endDate"].as_string().substr(0, DATETIME_STRING_LENGTH_TO_SECOND));

        //rewards
        auto rewards = result->Rewards;
        auto rewardsJson = achievementToVerify[L"rewards"];
        VERIFY_ARE_EQUAL_INT(rewards->Size, rewardsJson.as_array().size());
        //rewards/0
        auto rewards0 = rewards->GetAt(0);
        auto rewards0Json = rewardsJson[0];
        VERIFY_IS_TRUE(rewards0->Name->IsEmpty());
        VERIFY_IS_TRUE(rewards0->Description->IsEmpty());
        VERIFY_ARE_EQUAL(rewards0->Data->Data(), rewards0Json[L"value"].as_string());
        VERIFY_ARE_EQUAL(rewards0->RewardType.ToString()->Data(), rewards0Json[L"type"].as_string());
        VERIFY_ARE_EQUAL_INT(rewards0->ValuePropertyType, Windows::Foundation::PropertyType::Int32);
    }

    DEFINE_TEST_CASE(TestGetAchievement)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetAchievement);
        auto responseJson = web::json::value::parse(defaultAchievementResponse);
        auto achievementToVerify = responseJson.as_object()[L"achievements"].as_array()[0];

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto result = pplx::create_task(xboxLiveContext->AchievementService->GetAchievementAsync(
            "xboxUserId",
            "serviceConfigurationId",
            "achievementId"
            )).get();
        VERIFY_IS_NOT_NULL(result);
        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://achievements.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/users/xuid(xboxUserId)/achievements/serviceConfigurationId/achievementId", httpCall->PathQueryFragment.to_string());

        VerifyDefaultAchievement(result, achievementToVerify);
    }

    DEFINE_TEST_CASE(TestGetAchievements)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetAchievements);
        auto responseJson = web::json::value::parse(defaultAchievementResponse);

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto result = create_task(xboxLiveContext->AchievementService->GetAchievementsForTitleIdAsync(
            "xboxUserId",
            0,
            AchievementType::All,
            false,
            AchievementOrderBy::Default,
            0,
            20
            )).get();

        VERIFY_IS_NOT_NULL(result);
        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://achievements.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/users/xuid(xboxUserId)/achievements?titleId=0&maxItems=20", httpCall->PathQueryFragment.to_string());

        auto achievementsJson = responseJson.as_object()[L"achievements"];
        VERIFY_ARE_EQUAL_INT(result->Items->Size, responseJson.as_object()[L"achievements"].as_array().size());
        VerifyDefaultAchievement(result->Items->GetAt(0), achievementsJson.as_array()[0]);

        VERIFY_IS_FALSE(result->HasNext);
        VERIFY_THROWS_CX(create_task(result->GetNextAsync(20)).get(), Platform::OutOfBoundsException);

        // more verify on sending url
        create_task(xboxLiveContext->AchievementService->GetAchievementsForTitleIdAsync(
            "xboxUserId1234",
            777,
            AchievementType::Challenge,
            true,
            AchievementOrderBy::UnlockTime,
            10,
            20
            )).get();
        VERIFY_ARE_EQUAL_STR(L"/users/xuid(xboxUserId1234)/achievements?titleId=777&types=challenge&unlockedOnly=true&orderBy=unlocktime&maxItems=20&skipItems=10", httpCall->PathQueryFragment.to_string());
    }

    DEFINE_TEST_CASE(TestGetAchievementsEmptyResult)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetAchievementsEmptyResult);
        auto responseJson = web::json::value::parse(LR"({"achievements":[]})");

        m_mockXboxSystemFactory->GetMockHttpCall()->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto task = create_task(xboxLiveContext->AchievementService->GetAchievementsForTitleIdAsync(
            "xboxUserId",
            0,
            AchievementType::All,
            false,
            AchievementOrderBy::Default,
            0,
            20
            ));
        auto result = task.get();

        VERIFY_IS_NOT_NULL(result);

        VERIFY_ARE_EQUAL_INT(result->Items->Size, 0);
    }

    DEFINE_TEST_CASE(TestGetAchievementsInvalidArgs)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetAchievementsInvalidArgs);
        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();

        auto responseJson = web::json::value::parse(LR"({"achievements":[]})");
        m_mockXboxSystemFactory->GetMockHttpCall()->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        TEST_LOG(L"TestGetAchievementsForTitleIdAsyncInvalidArgs: Null xboxUserId param.");
#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->AchievementService->GetAchievementsForTitleIdAsync(
            nullptr,// Invalid
            1,      //titleid
            AchievementType::All,
            false,
            AchievementOrderBy::Default,
            0,      // skipItems
            10      // maxItems
            )).get(),
            E_INVALIDARG);

        TEST_LOG(L"TestGetAchievementsForTitleIdAsyncInvalidArgs: Empty xboxUserId param.");
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->AchievementService->GetAchievementsForTitleIdAsync(
            L"", // Invalid
            1,
            AchievementType::All,
            false,
            AchievementOrderBy::Default,
            0, // skipItems
            10  // maxItems
            )).get(),
            E_INVALIDARG);

        TEST_LOG(L"TestGetAchievementsForTitleIdAsyncInvalidArgs: Empty Platform::String^ xboxUserId param.");
        Platform::String^ emptyXboxUserId;
#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->AchievementService->GetAchievementsForTitleIdAsync(
            emptyXboxUserId, // Invalid
            1,
            AchievementType::All,
            false,
            AchievementOrderBy::Default,
            0, // skipItems
            10  // maxItems
            )).get(),
            E_INVALIDARG);

        TEST_LOG(L"TestGetAchievementsForTitleIdAsyncInvalidArgs: AchievementType value < valid range.");
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->AchievementService->GetAchievementsForTitleIdAsync(
            "1",
            1,
            static_cast<AchievementType>((int)(AchievementType::Unknown) - 1), // invalid range
            false,
            AchievementOrderBy::Default,
            0, // skipItems
            10  // maxItems
            )).get(),
            E_INVALIDARG);

        TEST_LOG(L"TestGetAchievementsForTitleIdAsyncInvalidArgs: AchievementType value > valid range.");
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->AchievementService->GetAchievementsForTitleIdAsync(
            "1",
            1,
            static_cast<AchievementType>((int)(AchievementType::Challenge) + 1), // invalid range
            false,
            AchievementOrderBy::Default,
            0, // skipItems
            10  // maxItems
            )).get(),
            E_INVALIDARG);

        TEST_LOG(L"TestGetAchievementsForTitleIdAsyncInvalidArgs: AchievementOrderBy value < valid range.");
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->AchievementService->GetAchievementsForTitleIdAsync(
            "1",
            1,
            AchievementType::All,
            false,
            static_cast<AchievementOrderBy>((int)(AchievementOrderBy::Default) - 1), // invalid range
            0, // skipItems
            10  // maxItems
            )).get(),
            E_INVALIDARG);

        TEST_LOG(L"TestGetAchievementsForTitleIdAsyncInvalidArgs: AchievementType value > valid range.");
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->AchievementService->GetAchievementsForTitleIdAsync(
            "1",
            1,
            AchievementType::All,
            false,
            static_cast<AchievementOrderBy>((int)(AchievementOrderBy::UnlockTime) + 1), // invalid range
            0, // skipItems
            10  // maxItems
            )).get(),
            E_INVALIDARG);
    }

    DEFINE_TEST_CASE(TestGetAchievementInvalidArgs)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetAchievementInvalidArgs);
        Platform::String^ xuid = L"1234";
        Platform::String^ scidString = "1234";
        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();

        TEST_LOG(L"TestGetAchievementAsyncInvalidArgs: Null xboxUserId param.");
#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->AchievementService->GetAchievementAsync(
            nullptr, // Invalid
            scidString,
            L"42"
            )).get(),
            E_INVALIDARG
            );

        TEST_LOG(L"TestGetAchievementAsyncInvalidArgs: Empty xboxUserId param.");
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->AchievementService->GetAchievementAsync(
            L"", // Invalid
            scidString,
            L"42"
            )).get(),
            E_INVALIDARG
            );

        TEST_LOG(L"TestGetAchievementAsyncInvalidArgs: Empty Platform::String^ xboxUserId param.");
        Platform::String^ emptyXboxUserId;
#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->AchievementService->GetAchievementAsync(
            emptyXboxUserId, // Invalid
            scidString,
            L"42"
            )).get(),
            E_INVALIDARG
            );

        TEST_LOG(L"TestGetAchievementAsyncInvalidArgs: Null achievementId param.");
#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->AchievementService->GetAchievementAsync(
            xuid,
            scidString,
            nullptr // Invalid
            )).get(),
            E_INVALIDARG
            );

        TEST_LOG(L"TestGetAchievementAsyncInvalidArgs: Empty achievementId param.");
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->AchievementService->GetAchievementAsync(
            xuid,
            scidString,
            L"" // Invalid
            )).get(),
            E_INVALIDARG
            );

        TEST_LOG(L"TestGetAchievementAsyncInvalidArgs: Empty Platform::String^ achievementId param.");
        Platform::String^ emptyAchievementId;
#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->AchievementService->GetAchievementAsync(
            xuid,
            scidString,
            emptyAchievementId // Invalid
            )).get(),
            E_INVALIDARG
            );

        TEST_LOG(L"TestGetAchievementAsyncInvalidArgs: Null serviceConfigurationId param.");
#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->AchievementService->GetAchievementAsync(
            xuid,
            nullptr, // Invalid,
            L"42"
            )).get(),
            E_INVALIDARG
            );

        TEST_LOG(L"TestGetAchievementAsyncInvalidArgs: Empty serviceConfigurationId param.");
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->AchievementService->GetAchievementAsync(
            xuid,
            L"", // Invalid
            L"42"
            )).get(),
            E_INVALIDARG
            );

        TEST_LOG(L"TestGetAchievementAsyncInvalidArgs: Empty Platform::String^ serviceConfigurationId param.");
        Platform::String^ emptyServiceConfigurationId;
#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->AchievementService->GetAchievementAsync(
            xuid,
            emptyServiceConfigurationId, // Invalid
            L"42"
            )).get(),
            E_INVALIDARG
            );
    }

    DEFINE_TEST_CASE(TestUpdateAchievement)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestUpdateAchievement);

        Platform::String^ xboxUserId = L"1";
        Platform::String^ achievementId = L"4";
        uint32 percentComplete = 5;

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        create_task(xboxLiveContext->AchievementService->UpdateAchievementAsync(
            xboxUserId,
            achievementId,
            percentComplete
        )).get();

        std::wstring expectedRequest =
            LR"({"achievements":[{"id":"4","percentComplete":5}],"action":"progressUpdate","serviceConfigId":"MockScid","titleId":1234,"userId":"1"})";

        VERIFY_ARE_EQUAL_STR(L"POST", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://achievements.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/users/xuid(1)/achievements/MockScid/update", httpCall->PathQueryFragment.to_string());
        VERIFY_ARE_EQUAL_STR(expectedRequest, httpCall->request_body().request_message_string());
    }

    DEFINE_TEST_CASE(TestUpdateAchievementInvalidArgs)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestUpdateAchievementInvalidArgs);

        Platform::String^ xboxUserId = L"1";
        uint32 titleId = 2;
        Platform::String^ serviceConfigurationId = L"3";
        Platform::String^ achievementId = L"4";
        uint32 percentComplete = 5;

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();

#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->AchievementService->UpdateAchievementAsync(
                nullptr, // Invalid
                achievementId,
                percentComplete
            )).get(),
            E_INVALIDARG
            );

#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->AchievementService->UpdateAchievementAsync(
                xboxUserId,
                nullptr,
                percentComplete
            )).get(),
            E_INVALIDARG
        );

#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->AchievementService->UpdateAchievementAsync(
                xboxUserId,
                achievementId,
                200
            )).get(),
            E_INVALIDARG
        );

    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

