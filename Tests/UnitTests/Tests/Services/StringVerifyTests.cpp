// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#define TEST_CLASS_OWNER L"jasonsa"
#define TEST_CLASS_AREA L"StringVerify"
#include "UnitTestIncludes.h"
#include "XboxLiveContext_WinRT.h"
#include "StringService_WinRT.h"

using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::System;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

const std::wstring defaultStringVerifyResult =
LR"(
    {
    "verifyStringResult":
    [
        {
            "resultCode": 0
        },
        {
            "resultCode": 1,
            "offendingString":"sdfasdf"
        },
        {
            "resultCode": 7,
            "offendingString":"whatasfkjasl"
        }
    ]}
    )";

DEFINE_TEST_CLASS(StringVerifyTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(StringVerifyTests);

    DEFINE_TEST_CASE(TestVerifyString)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestVerifyString);

        //UnitTestHelpers::SetupFactoryHelper(m_mockXboxSystemFactory);
        auto responseJson = web::json::value::parse(defaultStringVerifyResult);
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        auto requestString = ref new Platform::String(L"xboxUserId");
        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto result = create_task(xboxLiveContext->StringService->VerifyStringAsync(requestString)).get();
        VERIFY_IS_NOT_NULL(result);
        VERIFY_ARE_EQUAL_STR(L"POST", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://client-strings.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/system/strings/validate", httpCall->PathQueryFragment.to_string());

        //verify the request
        auto requestJson = web::json::value::parse(utils::string_t_from_internal_string(httpCall->request_body().request_message_string()));
        auto requestStringJson = requestJson[L"stringstoVerify"];
        VERIFY_ARE_EQUAL_INT(requestStringJson.size(), 1);
        VERIFY_ARE_EQUAL(requestStringJson[0].as_string(), requestString->Data());

        //verify the result
        VERIFY_ARE_EQUAL_INT(result->ResultCode, VerifyStringResultCode::Success);
        VERIFY_IS_TRUE(result->FirstOffendingSubstring->IsEmpty());
    }

    DEFINE_TEST_CASE(TestVerifyStrings)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestVerifyStrings);

        auto responseJson = web::json::value::parse(defaultStringVerifyResult);
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto requestStrings = ref new Vector<Platform::String^>();
        requestStrings->Append("asdf");
        requestStrings->Append("asdfasdf");
        auto result = create_task(xboxLiveContext->StringService->VerifyStringsAsync(requestStrings->GetView())).get();
        VERIFY_IS_NOT_NULL(result);

        //verify the request
        VERIFY_ARE_EQUAL_STR(L"POST", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://client-strings.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/system/strings/validate", httpCall->PathQueryFragment.to_string());
        auto requestJson = web::json::value::parse(utils::string_t_from_internal_string(httpCall->request_body().request_message_string()));
        auto requestStringJson = requestJson[L"stringstoVerify"];
        VERIFY_ARE_EQUAL_INT(requestStringJson.size(), 2);
        VERIFY_ARE_EQUAL(requestStringJson[0].as_string(), requestStrings->GetAt(0)->Data());
        VERIFY_ARE_EQUAL(requestStringJson[1].as_string(), requestStrings->GetAt(1)->Data());

        //verify the result
        VERIFY_ARE_EQUAL_INT(result->Size, 3);
        auto& resultArrryJson = responseJson[U("verifyStringResult")];
        auto result0 = result->GetAt(0);
        VERIFY_ARE_EQUAL_INT(result0->ResultCode, VerifyStringResultCode::Success);
        VERIFY_IS_TRUE(result0->FirstOffendingSubstring->IsEmpty());

        auto result1 = result->GetAt(1);
        VERIFY_ARE_EQUAL_INT(result1->ResultCode, VerifyStringResultCode::Offensive);
        VERIFY_ARE_EQUAL(result1->FirstOffendingSubstring->Data(), resultArrryJson[1][L"offendingString"].as_string());

        auto result2 = result->GetAt(2);
        VERIFY_ARE_EQUAL_INT(result2->ResultCode, resultArrryJson[2][L"resultCode"].as_integer());
        VERIFY_ARE_EQUAL(result2->FirstOffendingSubstring->Data(), resultArrryJson[2][L"offendingString"].as_string());
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

