// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "UnitTestIncludes.h"

using namespace xbox::services::privacy;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

const char defaultCheckPermissionsResponse[] = R"(
{ 
    "isAllowed":false,
    "reasons" :
    [{
        "reason":"BlockListRestrictsTarget"
    },
    { 
        "reason":"MuteListRestrictsTarget" 
    }] 
}
)";

const char defaultCheckMultiplePermissionsResponse[] = R"(
{
    "responses":
    [{
        "user":
        {
            "xuid":"1"
        }, 
        "permissions" : 
        [{
            "isAllowed":true
         }, 
         {
             "isAllowed":true 
         }, 
         {
             "isAllowed":true 
         }]
    }, 
    { 
        "user":
        {
            "anonymousUser":"crossNetworkUser"
        },
        "permissions" : 
        [{
            "isAllowed":true
         },
         { 
             "isAllowed":false, 
             "reasons" : 
             [{
                 "reason":"BlockListRestrictsTarget"
             },
             { 
                 "reason":"MuteListRestrictsTarget" 
             }] 
         },
         { 
             "isAllowed":false, 
             "reasons" : 
             [{
                 "reason":"BlockListRestrictsTarget"
             }]
         }]
    }]
}
)";

DEFINE_TEST_CLASS(PrivacyTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(PrivacyTests);

    std::shared_ptr<HttpMock> CreatePrivacyListMock(
        const std::vector<uint64_t>& xuids
    )
    {
        auto mock = std::make_shared<HttpMock>("GET", "https://privacy.xboxlive.com/users");

        mock->SetMockMatchedCallback(
            [
                &xuids
            ]
        (HttpMock* mock, xsapi_internal_string requestUrl, xsapi_internal_string requestBody)
            {
                UNREFERENCED_PARAMETER(requestUrl);
                VERIFY_IS_TRUE(requestBody.empty());

                // Response sample:
                //{
                //    "users":
                //    [
                //        { "xuid":"12345" },
                //        { "xuid":"23456" }
                //    ]
                //}

                JsonDocument response(rapidjson::kObjectType);
                JsonValue usersJson(rapidjson::kArrayType);

                for (size_t i = 0; i < xuids.size(); ++i)
                {
                    JsonValue user(rapidjson::kObjectType);
                    user.AddMember("xuid", JsonValue(Utils::StringFromUint64(xuids[i]).c_str(), response.GetAllocator()).Move(), response.GetAllocator());
                    usersJson.PushBack(user, response.GetAllocator());
                }
                response.AddMember("users", usersJson, response.GetAllocator());

                mock->SetResponseBody(response);
            }
        );

        return mock;
    }

    void VerifyPermissionCheckResult(
        const XblPermissionCheckResult& result,
        uint64_t expectedXuid,
        XblAnonymousUserType expectedUserType,
        JsonValue& responseJson
    )
    {
        VERIFY_ARE_EQUAL(result.isAllowed, responseJson["isAllowed"].GetBool());
        VERIFY_ARE_EQUAL_INT(result.targetXuid, expectedXuid);
        VERIFY_IS_TRUE(result.targetUserType == expectedUserType);
        if (!result.isAllowed)
        {
            JsonValue& reasonsArray{ responseJson["reasons"]};
            VERIFY_ARE_EQUAL_INT(result.reasonsCount, reasonsArray.Size());
            for (uint64_t i = 0; i < result.reasonsCount; ++i)
            {
                auto expectedReason{ reasonsArray[i]["reason"].GetString() };
                VERIFY_ARE_EQUAL_STR(utils::internal_string_from_string_t(permission_deny_reason{ result.reasons[i] }.reason()), expectedReason);
            }
        }
    }

    void VerifyBatchPermissionsCheckResult(
        std::vector<XblPermissionCheckResult>&& batchResult,
        JsonValue& responseJson
    )
    {
        JsonValue& responsesArray = responseJson["responses"];
        uint64_t userCount = responsesArray.Size();
        uint64_t permissionCount = responsesArray[0]["permissions"].Size();
        VERIFY_ARE_EQUAL_INT(batchResult.size(), userCount * permissionCount);

        for (uint64_t userIndex = 0; userIndex < userCount; ++userIndex)
        {
            JsonValue& userJson = responsesArray[userIndex]["user"];

            uint64_t expectedXuid{ 0 };
            if (userJson.HasMember("xuid") && userJson["xuid"].IsString())
            {
                expectedXuid = utils::internal_string_to_uint64(userJson["xuid"].GetString());
            }
            XblAnonymousUserType expectedUserType{ XblAnonymousUserType::Unknown };
            if (userJson.HasMember("anonymousUser") && userJson["anonymousUser"].IsString())
            {
                expectedUserType = EnumValue<XblAnonymousUserType>(userJson["anonymousUser"].GetString());
            }
            JsonValue& permissionsArray = responsesArray[userIndex]["permissions"];

            for (uint64_t permissionIndex = 0; permissionIndex < permissionCount; ++permissionIndex)
            {
                VerifyPermissionCheckResult(batchResult[userIndex * permissionCount + permissionIndex], expectedXuid, expectedUserType, permissionsArray[permissionIndex]);
            }
        }
    }

    DEFINE_TEST_CASE(TestCheckAvoidList)
    {
        TEST_LOG(L"Test starting: TestCheckAvoidList");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        std::vector<uint64_t> avoidXuids{ 1, 2, 3, 4, 5 };
        auto mock = CreatePrivacyListMock(avoidXuids);

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblPrivacyGetAvoidListAsync(xboxLiveContext.get(), &async));

        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        
        size_t xuidCount{ 0 };
        VERIFY_SUCCEEDED(XblPrivacyGetAvoidListResultCount(&async, &xuidCount));
        VERIFY_IS_TRUE(avoidXuids.size() == xuidCount);

        std::vector<uint64_t> resultXuids(xuidCount);
        VERIFY_SUCCEEDED(XblPrivacyGetAvoidListResult(&async, xuidCount, &resultXuids[0]));

        for (size_t i = 0; i < avoidXuids.size(); ++i)
        {
            VERIFY_ARE_EQUAL_INT(avoidXuids[i], resultXuids[i]);
        }
    }

    DEFINE_TEST_CASE(TestCheckMuteList)
    {
        TEST_LOG(L"Test starting: TestCheckMuteList");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        std::vector<uint64_t> muteXuids{ };
        auto mock = CreatePrivacyListMock(muteXuids);

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblPrivacyGetMuteListAsync(xboxLiveContext.get(), &async));

        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));

        size_t xuidCount{ 0 };
        VERIFY_SUCCEEDED(XblPrivacyGetMuteListResultCount(&async, &xuidCount));
        VERIFY_IS_TRUE(muteXuids.size() == xuidCount);

        std::vector<uint64_t> resultXuids(xuidCount);
        VERIFY_SUCCEEDED(XblPrivacyGetMuteListResult(&async, xuidCount, resultXuids.data()));
    }

    DEFINE_TEST_CASE(TestCheckPermissionAsync)
    {
        TEST_LOG(L"Test starting: TestCheckPermissionAsync");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        JsonDocument defaultCheckPermissionsResponseJson;
        defaultCheckPermissionsResponseJson.Parse(defaultCheckPermissionsResponse);
        HttpMock mock{ "GET", "https://privacy.xboxlive.com", 200, defaultCheckPermissionsResponseJson };

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblPrivacyCheckPermissionAsync(xboxLiveContext.get(), XblPermission::CommunicateUsingVoice, 1, &async));

        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));

        size_t resultSize{ 0 };
        VERIFY_SUCCEEDED(XblPrivacyCheckPermissionResultSize(&async, &resultSize));
        
        std::vector<uint8_t> buffer(resultSize);
        XblPermissionCheckResult* result{ nullptr };
        VERIFY_SUCCEEDED(XblPrivacyCheckPermissionResult(&async, resultSize, buffer.data(), &result, nullptr));

        VerifyPermissionCheckResult(*result, 1, XblAnonymousUserType::Unknown, defaultCheckPermissionsResponseJson);
    }

    DEFINE_TEST_CASE(TestCheckPermissionWithLargeBufferAsync)
    {
        TEST_LOG(L"Test starting: TestCheckPermissionWithLargeBufferAsync");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        JsonDocument defaultCheckPermissionsResponseJson;
        defaultCheckPermissionsResponseJson.Parse(defaultCheckPermissionsResponse);
        HttpMock mock{ "GET", "https://privacy.xboxlive.com", 200, defaultCheckPermissionsResponseJson };

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblPrivacyCheckPermissionAsync(xboxLiveContext.get(), XblPermission::CommunicateUsingVoice, 1, &async));

        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));

        size_t resultSize{ 0 };
        VERIFY_SUCCEEDED(XblPrivacyCheckPermissionResultSize(&async, &resultSize));

        size_t bufferUsed{};
        std::vector<uint8_t> buffer(resultSize * 2);
        XblPermissionCheckResult* result{ nullptr };
        VERIFY_SUCCEEDED(XblPrivacyCheckPermissionResult(&async, resultSize * 2, buffer.data(), &result, &bufferUsed));
        VERIFY_ARE_EQUAL_UINT(resultSize, bufferUsed);

        VerifyPermissionCheckResult(*result, 1, XblAnonymousUserType::Unknown, defaultCheckPermissionsResponseJson);
    }

    DEFINE_TEST_CASE(TestCheckPermissionForAnonymousUserAsync)
    {
        TEST_LOG(L"Test starting: TestCheckPermissionForAnonymousUserAsync");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        JsonDocument defaultCheckPermissionsResponseJson;
        defaultCheckPermissionsResponseJson.Parse(defaultCheckPermissionsResponse);
        HttpMock mock{ "GET", "https://privacy.xboxlive.com", 200, defaultCheckPermissionsResponseJson };

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblPrivacyCheckPermissionForAnonymousUserAsync(
            xboxLiveContext.get(),
            XblPermission::PlayMultiplayer,
            XblAnonymousUserType::CrossNetworkUser,
            &async
        ));

        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));

        size_t resultSize{ 0 };
        VERIFY_SUCCEEDED(XblPrivacyCheckPermissionForAnonymousUserResultSize(&async, &resultSize));

        std::vector<uint8_t> buffer(resultSize);
        XblPermissionCheckResult* result{ nullptr };
        VERIFY_SUCCEEDED(XblPrivacyCheckPermissionForAnonymousUserResult(&async, resultSize, buffer.data(), &result, nullptr));

        VerifyPermissionCheckResult(*result, 0, XblAnonymousUserType::CrossNetworkUser, defaultCheckPermissionsResponseJson);
    }

    DEFINE_TEST_CASE(TestCheckPermissionForAnonymousUserWithLargeBufferAsync)
    {
        TEST_LOG(L"Test starting: TestCheckPermissionForAnonymousUserWithLargeBufferAsync");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        JsonDocument defaultCheckPermissionsResponseJson;
        defaultCheckPermissionsResponseJson.Parse(defaultCheckPermissionsResponse);
        HttpMock mock{ "GET", "https://privacy.xboxlive.com", 200, defaultCheckPermissionsResponseJson };

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblPrivacyCheckPermissionForAnonymousUserAsync(
            xboxLiveContext.get(),
            XblPermission::PlayMultiplayer,
            XblAnonymousUserType::CrossNetworkUser,
            &async
        ));

        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));

        size_t resultSize{ 0 };
        VERIFY_SUCCEEDED(XblPrivacyCheckPermissionForAnonymousUserResultSize(&async, &resultSize));

        size_t bufferUsed{};
        std::vector<uint8_t> buffer(resultSize * 2);
        XblPermissionCheckResult* result{ nullptr };
        VERIFY_SUCCEEDED(XblPrivacyCheckPermissionForAnonymousUserResult(&async, resultSize, buffer.data(), &result, &bufferUsed));
        VERIFY_ARE_EQUAL_UINT(resultSize, bufferUsed);

        VerifyPermissionCheckResult(*result, 0, XblAnonymousUserType::CrossNetworkUser, defaultCheckPermissionsResponseJson);
    }

    DEFINE_TEST_CASE(TestBatchCheckPermissionAsync)
    {
        TEST_LOG(L"Test starting: TestBatchCheckPermissionAsync");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        JsonDocument defaultCheckMultiplePermissionsResponseJson;
        defaultCheckMultiplePermissionsResponseJson.Parse(defaultCheckMultiplePermissionsResponse);
        HttpMock mock{ "POST", "https://privacy.xboxlive.com", 200, defaultCheckMultiplePermissionsResponseJson };

        std::vector<uint64_t> xuidsToCheck{ 1 };
        std::vector<XblAnonymousUserType> userTypesToCheck{ XblAnonymousUserType::CrossNetworkUser };
        std::vector<XblPermission> permissionsToCheck{ XblPermission::PlayMultiplayer, XblPermission::CommunicateUsingVoice, XblPermission::CommunicateUsingText };

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblPrivacyBatchCheckPermissionAsync(
            xboxLiveContext.get(),
            permissionsToCheck.data(),
            permissionsToCheck.size(),
            xuidsToCheck.data(),
            xuidsToCheck.size(),
            userTypesToCheck.data(),
            userTypesToCheck.size(),
            &async
        ));

        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));

        size_t bufferSize{ 0 };
        VERIFY_SUCCEEDED(XblPrivacyBatchCheckPermissionResultSize(&async, &bufferSize));

        std::vector<uint8_t> vectorBuffer(bufferSize);
        XblPermissionCheckResult* results;
        size_t resultsCount{ 0 };
        VERIFY_SUCCEEDED(XblPrivacyBatchCheckPermissionResult(&async, bufferSize, vectorBuffer.data(), &results, &resultsCount, nullptr));

        VerifyBatchPermissionsCheckResult({ results, results + resultsCount }, defaultCheckMultiplePermissionsResponseJson);
    }

    DEFINE_TEST_CASE(TestBatchCheckPermissionWithLargeBufferAsync)
    {
        TEST_LOG(L"Test starting: TestBatchCheckPermissionWithLargeBufferAsync");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        JsonDocument defaultCheckMultiplePermissionsResponseJson;
        defaultCheckMultiplePermissionsResponseJson.Parse(defaultCheckMultiplePermissionsResponse);
        HttpMock mock{ "POST", "https://privacy.xboxlive.com", 200, defaultCheckMultiplePermissionsResponseJson };

        std::vector<uint64_t> xuidsToCheck{ 1 };
        std::vector<XblAnonymousUserType> userTypesToCheck{ XblAnonymousUserType::CrossNetworkUser };
        std::vector<XblPermission> permissionsToCheck{ XblPermission::PlayMultiplayer, XblPermission::CommunicateUsingVoice, XblPermission::CommunicateUsingText };

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblPrivacyBatchCheckPermissionAsync(
            xboxLiveContext.get(),
            permissionsToCheck.data(),
            permissionsToCheck.size(),
            xuidsToCheck.data(),
            xuidsToCheck.size(),
            userTypesToCheck.data(),
            userTypesToCheck.size(),
            &async
        ));

        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));

        size_t bufferSize{ 0 };
        VERIFY_SUCCEEDED(XblPrivacyBatchCheckPermissionResultSize(&async, &bufferSize));

        size_t bufferUsed{};
        std::vector<uint8_t> vectorBuffer(bufferSize * 2);
        XblPermissionCheckResult* results;
        size_t resultsCount{ 0 };
        VERIFY_SUCCEEDED(XblPrivacyBatchCheckPermissionResult(&async, bufferSize * 2, vectorBuffer.data(), &results, &resultsCount, &bufferUsed));
        VERIFY_ARE_EQUAL_UINT(bufferSize, bufferUsed);

        VerifyBatchPermissionsCheckResult({ results, results + resultsCount }, defaultCheckMultiplePermissionsResponseJson);
    }

    DEFINE_TEST_CASE(TestInvalidArgs)
    {
        TEST_LOG(L"Test starting: TestInvalidArgs");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        std::vector<uint64_t> xuidsToCheck{ 1 };
        std::vector<XblAnonymousUserType> userTypesToCheck{ XblAnonymousUserType::CrossNetworkUser };
        std::vector<XblPermission> permissionsToCheck{ XblPermission::PlayMultiplayer };
        
        XAsyncBlock async{};
        VERIFY_FAILED(XblPrivacyBatchCheckPermissionAsync(
            xboxLiveContext.get(),
            nullptr, // invalid
            permissionsToCheck.size(),
            xuidsToCheck.data(),
            xuidsToCheck.size(),
            userTypesToCheck.data(),
            userTypesToCheck.size(),
            &async
        ));

        VERIFY_FAILED(XblPrivacyBatchCheckPermissionAsync(
            xboxLiveContext.get(),
            permissionsToCheck.data(),
            0, // invalid
            xuidsToCheck.data(),
            xuidsToCheck.size(),
            userTypesToCheck.data(),
            userTypesToCheck.size(),
            &async
        ));


        VERIFY_FAILED(XblPrivacyBatchCheckPermissionAsync(
            xboxLiveContext.get(),
            permissionsToCheck.data(),
            permissionsToCheck.size(),
            nullptr, // invalid
            xuidsToCheck.size(),
            userTypesToCheck.data(),
            userTypesToCheck.size(),
            &async
        ));

        VERIFY_FAILED(XblPrivacyBatchCheckPermissionAsync(
            xboxLiveContext.get(),
            permissionsToCheck.data(),
            permissionsToCheck.size(),
            xuidsToCheck.data(),
            xuidsToCheck.size(),
            nullptr, // invalid
            userTypesToCheck.size(),
            &async
        ));

        VERIFY_FAILED(XblPrivacyBatchCheckPermissionAsync(
            xboxLiveContext.get(),
            permissionsToCheck.data(),
            permissionsToCheck.size(),
            nullptr,
            0,
            nullptr,
            0,
            &async
        ));
    }

    DEFINE_TEST_CASE(CppTestCheckAvoidList)
    {
        TEST_LOG(L"Test starting: CppTestCheckAvoidList");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateLegacyMockXboxLiveContext();

        std::vector<uint64_t> avoidXuids{ 1, 2, 3, 4, 5 };
        auto mock = CreatePrivacyListMock(avoidXuids);

        auto task = xboxLiveContext->privacy_service().get_avoid_list();

        auto result{ task.get() };
        VERIFY_IS_TRUE(!result.err());

        auto& payload{ result.payload() };
        VERIFY_IS_TRUE(avoidXuids.size() == payload.size());

        for (size_t i = 0; i < avoidXuids.size(); ++i)
        {
            VERIFY_IS_TRUE(avoidXuids[i] == Utils::Uint64FromStringT(payload[i]));
        }
    }

    DEFINE_TEST_CASE(CppTestCheckMuteList)
    {
        TEST_LOG(L"Test starting: CppTestCheckMuteList");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateLegacyMockXboxLiveContext();

        std::vector<uint64_t> muteXuids{ };
        auto mock = CreatePrivacyListMock(muteXuids);

        auto task = xboxLiveContext->privacy_service().get_mute_list();

        auto result{ task.get() };
        VERIFY_IS_TRUE(!result.err());

        auto& payload{ result.payload() };
        VERIFY_IS_TRUE(muteXuids.size() == payload.size());
    }

    void VerifyPermissionCheckResultCpp(
        const permission_check_result& result,
        JsonValue& responseJson
    )
    {
        VERIFY_ARE_EQUAL(result.is_allowed(), responseJson["isAllowed"].GetBool());
        if (!result.is_allowed())
        {
            auto& reasonsArray{ responseJson["reasons"] };
            auto& actualReasons{ result.deny_reasons() };
            VERIFY_ARE_EQUAL_INT(actualReasons.size() , reasonsArray.Size());
            for (uint64_t i = 0; i < actualReasons.size(); ++i)
            {
                auto expectedReason{ reasonsArray[i]["reason"].GetString() };
                VERIFY_ARE_EQUAL_STR(utils::internal_string_from_string_t(actualReasons[i].reason()), expectedReason);
            }
        }
    }

    void VerifyBatchPermissionsCheckResultCpp(
        const std::vector<multiple_permissions_check_result>& result,
       JsonValue& responseJson
    )
    {
        JsonValue& responsesArray = responseJson["responses"];
        uint64_t userCount = responsesArray.Size();
        uint64_t permissionCount = responsesArray[0]["permissions"].Size();
        VERIFY_ARE_EQUAL_INT(result.size(), userCount);

        for (uint64_t userIndex = 0; userIndex < userCount; ++userIndex)
        {
            JsonValue& userJson = responsesArray[userIndex]["user"];

            xsapi_internal_string target;
            if (userJson.HasMember("xuid") && userJson["xuid"].IsString())
            {
                target = userJson["xuid"].GetString();
            }
            else if (userJson.HasMember("anonymousUser") && userJson["anonymousUser"].IsString())
            {
                target = userJson["anonymousUser"].GetString();
            }
            VERIFY_ARE_EQUAL_STR(utils::internal_string_from_string_t(result[userIndex].xbox_user_id()), target);
            JsonValue& permissionsArray = responsesArray[userIndex]["permissions"];

            for (uint64_t permissionIndex = 0; permissionIndex < permissionCount; ++permissionIndex)
            {
                VerifyPermissionCheckResultCpp(result[userIndex].items()[permissionIndex], permissionsArray[permissionIndex]);
            }
        }
    }

    DEFINE_TEST_CASE(CppTestCheckPermissionWithTargetUser)
    {
        TEST_LOG(L"Test starting: CppTestCheckPermissionWithTargetUser");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateLegacyMockXboxLiveContext();

        JsonDocument defaultCheckPermissionsResponseJson;
        defaultCheckPermissionsResponseJson.Parse(defaultCheckPermissionsResponse);
        HttpMock mock{ "GET", "https://privacy.xboxlive.com", 200, defaultCheckPermissionsResponseJson };

        auto task = xboxLiveContext->privacy_service().check_permission_with_target_user(permission_id_constants::communicate_using_voice(), _T("1"));

        auto result{ task.get() };
        VERIFY_IS_TRUE(!result.err());

        VerifyPermissionCheckResultCpp(result.payload(), defaultCheckPermissionsResponseJson);
    }

    DEFINE_TEST_CASE(CppTestCheckMultiplePermissionsWithMultipleTargetUsers)
    {
        TEST_LOG(L"Test starting: CppTestCheckMultiplePermissionsWithMultipleTargetUsers");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateLegacyMockXboxLiveContext();

        JsonDocument defaultCheckMultiplePermissionsResponseJson;
        defaultCheckMultiplePermissionsResponseJson.Parse(defaultCheckMultiplePermissionsResponse);
        HttpMock mock{ "POST", "https://privacy.xboxlive.com", 200, defaultCheckMultiplePermissionsResponseJson };

        std::vector<string_t> permissionsToCheck{
            permission_id_constants::play_multiplayer(),
            permission_id_constants::communicate_using_voice(),
            permission_id_constants::communicate_using_text()
        };

        std::vector<string_t> targets
        {
            _T("1"),
            anonymous_user_type_constants::cross_network_user()
        };

        auto task = xboxLiveContext->privacy_service().check_multiple_permissions_with_multiple_target_users(permissionsToCheck, targets);

        auto result{ task.get() };
        VERIFY_IS_TRUE(!result.err());

        VerifyBatchPermissionsCheckResultCpp(result.payload(), defaultCheckMultiplePermissionsResponseJson);
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
