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
#include "UnitTestIncludes.h"
#define TEST_CLASS_OWNER L"jasonsa"
#include "DefineTestMacros.h"
#include "UserTokenResult.h"

NAMESPACE_MICROSOFT_XBOX_SYSTEM_CPP_BEGIN

class UserTokenResultTests : public UnitTestBase
{
public:
    TEST_CLASS(UserTokenResultTests);
    DEFINE_TEST_CLASS_SETUP();

    TEST_METHOD(TestUserToken)
    {
        DEFINE_TEST_CASE_PROPERTIES();

        std::shared_ptr<UserTokenResult> userTokenResult = Cpp::UserTokenResult::Deserialize(
            StockMocks::GetStockJsonMockUserTokenResult()
            );

        VERIFY_IS_NOT_NULL(userTokenResult);
        VERIFY_IS_EQUAL_STR(L"TestToken", userTokenResult->GetToken().c_str());
    }

    TEST_METHOD(TestUserToken_BadData)
    {
        DEFINE_TEST_CASE_PROPERTIES();

        web::json::value jsonRoot;

        jsonRoot = web::json::value::parse(L"{}");
        VERIFY_THROWS_HR(Cpp::UserTokenResult::Deserialize(jsonRoot), WEB_E_INVALID_JSON_STRING);

        // Null Token
        jsonRoot = web::json::value::parse(
            L"{\
                \"DisplayClaims\" :\
                {\
                    \"xui\" :\
                        [\
                            {\
                                \"agg\" : \"Adult\",\
                                \"gtg\" : \"TestGamerTag\",\
                                \"prv\" : \"191 192\",\
                                \"uhs\" : \"TestXboxUserHash\",\
                                \"xid\" : \"TestXboxUserId\"\
                            }\
                        ]\
                },\
                \"IssueInstant\" : \"2014-03-05T20:14:34.3709609Z\",\
                \"NotAfter\" : \"2014-03-06T04:14:34.3709609Z\",\
                \"Token\" : \"\"\
            }");
        VERIFY_THROWS_HR(Cpp::UserTokenResult::Deserialize(jsonRoot), WEB_E_INVALID_JSON_STRING);
    }
};

NAMESPACE_MICROSOFT_XBOX_SYSTEM_CPP_END

