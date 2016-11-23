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
#include "UnitTestBase.h"
#include "StockMocks.h"
#define TEST_CLASS_OWNER L"jasonsa"
#include "DefineTestMacros.h"
#include "user_token_service.h"
#include "user_impl.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

class UserTokenServiceTests : public UnitTestBase
{
public:
    TEST_CLASS(UserTokenServiceTests);
    DEFINE_TEST_CLASS_SETUP();

    TEST_METHOD_SETUP(SetupFactory)
    {
        m_mockXboxSystemFactory = std::make_shared<MockXboxSystemFactory>();
        xbox_system_factory_winstore_impl::set_factory(m_mockXboxSystemFactory);

        m_mockXboxSystemFactory->GetMockHttpCall()->ResultValue = StockMocks::CreateMockHttpCallResponse(
            StockMocks::GetStockJsonMockUserTokenResult()
            );
        return true;
    }

    TEST_METHOD(TestUserTokenService)
    {
        DEFINE_TEST_CASE_PROPERTIES();

        std::shared_ptr<user_token_service_impl> userToken = std::make_shared<user_token_service_impl>();
        std::shared_ptr<auth_config> authenticationConfiguration = std::make_shared<auth_config>(_T(".mockenv"), _T("MockPrefix-"), _T("MockEnv"), false);
        std::shared_ptr<xbox_live_context_settings> xboxLiveContextSettings = std::make_shared<xbox_live_context_settings>();

        xbox::services::xbox_live_result<token_result> tokenResult = userToken->get_u_token_from_service(
            L"TestRpsTicket",
            std::make_shared<ecdsa>(),
            authenticationConfiguration,
            xboxLiveContextSettings
            ).get();

        // TODO 718329: validate CreateHttpCall() input was proper in GetUserTokenAsync()
        // TODO 718329: validate SetRequestBody was proper in GetUserTokenAsync()

        VERIFY_IS_TRUE(!tokenResult.payload().token().empty());
    }

    TEST_METHOD(TestTokenAndSigResultCtors)
    {
        DEFINE_TEST_CASE_PROPERTIES();

        // verify ctor and move ctor and move assignment ctor
        token_and_signature_result tokenAndSig(token_and_signature_result(L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8", L"9"));
        VERIFY_ARE_EQUAL(L"1", tokenAndSig.token());
        VERIFY_ARE_EQUAL(L"2", tokenAndSig.signature());
        VERIFY_ARE_EQUAL(L"3", tokenAndSig.xbox_user_id());
        VERIFY_ARE_EQUAL(L"4", tokenAndSig.gamertag());
        VERIFY_ARE_EQUAL(L"5", tokenAndSig.xbox_user_hash());
        VERIFY_ARE_EQUAL(L"6", tokenAndSig.age_group());
        VERIFY_ARE_EQUAL(L"7", tokenAndSig.privileges());
        VERIFY_ARE_EQUAL(L"8", tokenAndSig.web_account_id());
        VERIFY_ARE_EQUAL(L"9", tokenAndSig.reserved());

        // verify copy assignment ctor
        token_and_signature_result tokenAndSig2 = tokenAndSig;
        VERIFY_ARE_EQUAL(L"1", tokenAndSig2.token());
        VERIFY_ARE_EQUAL(L"2", tokenAndSig2.signature());
        VERIFY_ARE_EQUAL(L"3", tokenAndSig2.xbox_user_id());
        VERIFY_ARE_EQUAL(L"4", tokenAndSig2.gamertag());
        VERIFY_ARE_EQUAL(L"5", tokenAndSig2.xbox_user_hash());
        VERIFY_ARE_EQUAL(L"6", tokenAndSig2.age_group());
        VERIFY_ARE_EQUAL(L"7", tokenAndSig2.privileges());
        VERIFY_ARE_EQUAL(L"8", tokenAndSig2.web_account_id());
        VERIFY_ARE_EQUAL(L"9", tokenAndSig2.reserved());

        // verify copy ctor
        token_and_signature_result tokenAndSig3( tokenAndSig );
        VERIFY_ARE_EQUAL(L"1", tokenAndSig3.token());
        VERIFY_ARE_EQUAL(L"2", tokenAndSig3.signature());
        VERIFY_ARE_EQUAL(L"3", tokenAndSig3.xbox_user_id());
        VERIFY_ARE_EQUAL(L"4", tokenAndSig3.gamertag());
        VERIFY_ARE_EQUAL(L"5", tokenAndSig3.xbox_user_hash());
        VERIFY_ARE_EQUAL(L"6", tokenAndSig3.age_group());
        VERIFY_ARE_EQUAL(L"7", tokenAndSig3.privileges());
        VERIFY_ARE_EQUAL(L"8", tokenAndSig3.web_account_id());
        VERIFY_ARE_EQUAL(L"9", tokenAndSig3.reserved());
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

