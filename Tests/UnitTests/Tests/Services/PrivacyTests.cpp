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
#define TEST_CLASS_AREA L"Privacy"
#include "UnitTestIncludes.h"

using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::Privacy;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

const std::wstring defaultMuteListResponse =
LR"(
{
    "users":
    [{
        "xuid":"2814680291986301"
    },
    {
        "xuid":"2814614899724406"
    }]
}
)";

const std::wstring defaultAvoidListResponse =
LR"(
{
    "users":
    [{ 
        "xuid":"2814680291986301"
    }]
}
)";

const std::wstring defaultCheckPermissionsResponse =
LR"(
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

const std::wstring defaultCheckMultiplePermissionsResponse =
LR"(
{
    "responses":
    [{
        "user":
        {
            "xuid":"2814634309691161"
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
            "xuid":"2814680291986301"
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
    DEFINE_TEST_CLASS_PROPS(PrivacyTests)

    void VerifyPermissionCheckResult(
        Privacy::PermissionCheckResult^ result,
        web::json::value permissionCheckResultToVerify
        )
    {
        VERIFY_ARE_EQUAL(result->IsAllowed, permissionCheckResultToVerify[L"isAllowed"].as_bool());
        if (!result->IsAllowed)
        {
            VERIFY_ARE_EQUAL_INT(result->DenyReasons->Size, permissionCheckResultToVerify[L"reasons"].as_array().size());
            int index = 0;
            for (Privacy::PermissionDenyReason^ reason : result->DenyReasons)
            {
                auto reasonJson = permissionCheckResultToVerify[L"reasons"].as_array()[index];
                VERIFY_ARE_EQUAL(reason->Reason->Data(), reasonJson[L"reason"].as_string());
                index++;
            }
        }
    }

    void VerifyDefaultMultiplePermissionsCheckResult(
        Privacy::MultiplePermissionsCheckResult^ result,
        web::json::value multiplePermissionsCheckResultToVerify
        )
    {
        VERIFY_ARE_EQUAL_INT(result->Items->Size, multiplePermissionsCheckResultToVerify[L"permissions"].as_array().size());
        auto userJson = multiplePermissionsCheckResultToVerify[L"user"];
        VERIFY_ARE_EQUAL(result->XboxUserId->Data(), userJson[L"xuid"].as_string());

        int index = 0;
        for (Privacy::PermissionCheckResult^ checkResult : result->Items)
        {
            auto reasonJson = multiplePermissionsCheckResultToVerify[L"permissions"].as_array()[index];
            VerifyPermissionCheckResult(checkResult, reasonJson);
            index++;
        }
    }

    DEFINE_TEST_CASE(TestCheckAvoidList)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestCheckAvoidList);

        auto responseJson = web::json::value::parse(defaultAvoidListResponse);

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        IVectorView<Platform::String^>^ results =
            create_task(xboxLiveContext->PrivacyService->GetAvoidListAsync()).get();

        VERIFY_IS_NOT_NULL(results);
        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://privacy.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/users/xuid(TestXboxUserId)/people/avoid", httpCall->PathQueryFragment.to_string());
        VERIFY_ARE_EQUAL_INT(results->Size, responseJson.as_object()[L"users"].as_array().size());

        int index = 0;
        for (Platform::String^ avoidMember : results)
        {
            auto avoidMemberJson = responseJson.as_object()[L"users"].as_array()[index];
            VERIFY_ARE_EQUAL(avoidMember->Data(), avoidMemberJson[L"xuid"].as_string());
            index++;
        }
    }

    DEFINE_TEST_CASE(TestCheckMuteList)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestCheckMuteList);
        auto responseJson = web::json::value::parse(defaultMuteListResponse);

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        IVectorView<Platform::String^>^ results =
            create_task(xboxLiveContext->PrivacyService->GetMuteListAsync()).get();

        VERIFY_IS_NOT_NULL(results);
        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://privacy.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/users/xuid(TestXboxUserId)/people/mute", httpCall->PathQueryFragment.to_string());
        VERIFY_ARE_EQUAL_INT(results->Size, responseJson.as_object()[L"users"].as_array().size());

        int index = 0;
        for (Platform::String^ avoidMember : results)
        {
            auto avoidMemberJson = responseJson.as_object()[L"users"].as_array()[index];
            VERIFY_ARE_EQUAL(avoidMember->Data(), avoidMemberJson[L"xuid"].as_string());
            index++;
        }
    }

    DEFINE_TEST_CASE(TestCheckPermissionWithTargetUser)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestCheckPermissionWithTargetUser);
        auto responseJson = web::json::value::parse(defaultCheckPermissionsResponse);

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        Privacy::PermissionCheckResult^ result =
            create_task(xboxLiveContext->PrivacyService->CheckPermissionWithTargetUserAsync(
            L"CommunicateUsingVoice",
            L"2814680291986301"
            )).get();

        VERIFY_IS_NOT_NULL(result);
        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://privacy.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/users/xuid(TestXboxUserId)/permission/validate?setting=CommunicateUsingVoice&target=xuid(2814680291986301)", httpCall->PathQueryFragment.to_string());

        VerifyPermissionCheckResult(result, responseJson);
    }

    DEFINE_TEST_CASE(TestCheckMultiplePermissionsWithMultipleTargetUsers)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestCheckMultiplePermissionsWithMultipleTargetUsers);
        auto responseJson = web::json::value::parse(defaultCheckMultiplePermissionsResponse);

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        Vector<Platform::String^>^ permissionIds = ref new Vector<Platform::String^>();
        permissionIds->Append("PlayMultiplayer");
        permissionIds->Append("CommunicateUsingVoice");
        permissionIds->Append("CommunicateUsingText");

        Vector<Platform::String^>^ targetXboxUserIds = ref new Vector<Platform::String^>();
        targetXboxUserIds->Append("2814634309691161");
        targetXboxUserIds->Append("2814680291986301");

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        IVectorView<Privacy::MultiplePermissionsCheckResult^>^ results = create_task(
            xboxLiveContext->PrivacyService->CheckMultiplePermissionsWithMultipleTargetUsersAsync(
                permissionIds->GetView(),
                targetXboxUserIds->GetView()
                )
            ).get();

        VERIFY_IS_NOT_NULL(results);
        VERIFY_ARE_EQUAL_STR(L"POST", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://privacy.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/users/xuid(TestXboxUserId)/permission/validate", httpCall->PathQueryFragment.to_string());
        VERIFY_ARE_EQUAL_INT(results->Size, responseJson.as_object()[L"responses"].as_array().size());
        
        int index = 0;
        for (Privacy::MultiplePermissionsCheckResult^ checkResult: results)
        {
            auto checkPermissionsJson = responseJson.as_object()[L"responses"].as_array()[index];
            VerifyDefaultMultiplePermissionsCheckResult(checkResult, checkPermissionsJson);
            index++;
        }        
    }

    DEFINE_TEST_CASE(TestCheckPermissionWithTargetUserInvalidArgs)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestCheckPermissionWithTargetUserInvalidArgs);
        auto responseJson = web::json::value::parse(defaultCheckPermissionsResponse);

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);
        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();

        TEST_LOG(L"TestCheckPermissionWithTargetUserInvalidArgs: Empty permissionId param.");
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->PrivacyService->CheckPermissionWithTargetUserAsync(
            L"",    // Invalid
            L"2814680291986301"
            )).get(),
            E_INVALIDARG);

        TEST_LOG(L"TestCheckPermissionWithTargetUserInvalidArgs: Empty targetXboxUserId param.");
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->PrivacyService->CheckPermissionWithTargetUserAsync(
            L"CommunicateUsingVoice",
            L""     // Invalid
            )).get(),
            E_INVALIDARG);

        TEST_LOG(L"TestCheckPermissionWithTargetUserInvalidArgs: Null permissionId param.");
#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->PrivacyService->CheckPermissionWithTargetUserAsync(
            nullptr,    // Invalid
            L"2814680291986301"
            )).get(),
            E_INVALIDARG);

        TEST_LOG(L"TestCheckPermissionWithTargetUserInvalidArgs: Null targetXboxUserId param.");
#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->PrivacyService->CheckPermissionWithTargetUserAsync(
            L"CommunicateUsingVoice",
            nullptr     // Invalid
            )).get(),
            E_INVALIDARG);
    }

    DEFINE_TEST_CASE(TestCheckMultiplePermissionsWithMultipleTargetUsersInvalidArgs)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestCheckMultiplePermissionsWithMultipleTargetUsersInvalidArgs);
        auto responseJson = web::json::value::parse(defaultCheckPermissionsResponse);

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);
        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();

        Vector<Platform::String^>^ permissionIds = ref new Vector<Platform::String^>();
        permissionIds->Append("PlayMultiplayer");
        permissionIds->Append("CommunicateUsingVoice");
        permissionIds->Append("CommunicateUsingText");

        Vector<Platform::String^>^ targetXboxUserIds = ref new Vector<Platform::String^>();
        targetXboxUserIds->Append("2814634309691161");
        targetXboxUserIds->Append("2814680291986301");

        TEST_LOG(L"TestCheckMultiplePermissionsWithMultipleTargetUsersInvalidArgs: Empty permissionIds param.");
#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->PrivacyService->CheckMultiplePermissionsWithMultipleTargetUsersAsync(
            {},    // empty
            targetXboxUserIds->GetView()
            )).get(),
            E_INVALIDARG);

        TEST_LOG(L"TestCheckMultiplePermissionsWithMultipleTargetUsersInvalidArgs: Empty targetXboxUserIds param.");
#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->PrivacyService->CheckMultiplePermissionsWithMultipleTargetUsersAsync(
            permissionIds->GetView(),
            {}     // empty
            )).get(),
            E_INVALIDARG);

        TEST_LOG(L"TestCheckMultiplePermissionsWithMultipleTargetUsersInvalidArgs: Null permissionIds param.");
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->PrivacyService->CheckMultiplePermissionsWithMultipleTargetUsersAsync(
            nullptr,    // Invalid
            targetXboxUserIds->GetView()
            )).get(),
            E_INVALIDARG);

        TEST_LOG(L"TestCheckMultiplePermissionsWithMultipleTargetUsersInvalidArgs: Null targetXboxUserIds param.");
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->PrivacyService->CheckMultiplePermissionsWithMultipleTargetUsersAsync(
            permissionIds->GetView(),
            nullptr     // Invalid
            )).get(),
            E_INVALIDARG);
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
