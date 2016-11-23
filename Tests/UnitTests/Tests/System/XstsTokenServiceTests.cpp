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
#include "xsts_token_service.h"
#include "Ecdsa.h"
#include "token_error.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

class XstsTokenServiceTests : public UnitTestBase
{
public:
    TEST_CLASS(XstsTokenServiceTests);
    DEFINE_TEST_CLASS_SETUP();

    TEST_METHOD_SETUP(SetupFactory)
    {
        m_mockXboxSystemFactory = std::make_shared<MockXboxSystemFactory>();
        xbox_system_factory_winstore_impl::set_factory(m_mockXboxSystemFactory);

        m_mockXboxSystemFactory->GetMockHttpCall()->ResultValue = StockMocks::CreateMockHttpCallResponse(
            StockMocks::GetStockJsonMockXTokenResult()
            );
        return true;
    }

    TEST_METHOD(TestXstsTokenService)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestXstsTokenService);

        std::shared_ptr<xsts_token_service_impl> xstsToken = std::make_shared<xsts_token_service_impl>();
        std::shared_ptr<auth_config> authenticationConfiguration = std::make_shared<auth_config>(_T(".mockenv"), _T("MockPrefix-"), _T("MockEnv"), false);
        std::shared_ptr<xbox_live_context_settings> xboxLiveContextSettings = std::make_shared<xbox_live_context_settings>();
        std::shared_ptr<ecdsa> proofKey = std::make_shared<ecdsa>();

        token_result userTokenResult = token_result::deserialize(
            StockMocks::GetStockJsonMockUserTokenResult()
            );

        xbox::services::xbox_live_result<token_result> tokenResult = xstsToken->get_x_token_from_service(
            proofKey,
            L"",
            L"",
            userTokenResult.token(),
            L"",
            L"http://xboxlive.com",
            L"JWT",
            authenticationConfiguration,
            xboxLiveContextSettings,
            L"123"
            ).get();

        VERIFY_ARE_EQUAL(0, tokenResult.payload().xerr());
    }

    TEST_METHOD(GetTokenErrorAccountCreation)
    {
        DEFINE_TEST_CASE_PROPERTIES();

        std::shared_ptr<xsts_token_service_impl> xstsToken = std::make_shared<xsts_token_service_impl>();
        std::shared_ptr<auth_config> authenticationConfiguration = std::make_shared<auth_config>(_T(".mockenv"), _T("MockPrefix-"), _T("MockEnv"), false);
        std::shared_ptr<xbox_live_context_settings> xboxLiveContextSettings = std::make_shared<xbox_live_context_settings>();
        std::shared_ptr<ecdsa> proofKey = std::make_shared<ecdsa>();

        token_result userTokenResult = token_result::deserialize(
            StockMocks::GetStockJsonMockUserTokenResult()
            );

        auto resp = StockMocks::CreateMockHttpCallResponse(
            StockMocks::GetCreateAccountXErrorResponse(),
            web::http::status_codes::Unauthorized);

        m_mockXboxSystemFactory->GetMockHttpCall()->ResultValue = resp;

        auto tokenResult = xstsToken->get_x_token_from_service(
            proofKey,
            L"",
            L"",
            userTokenResult.token(),
            L"",
            L"http://xboxlive.com",
            L"JWT",
            authenticationConfiguration,
            xboxLiveContextSettings,
            L"123"
            ).get().payload();

        VERIFY_ARE_EQUAL(2148916233, tokenResult.xerr());
        VERIFY_ARE_EQUAL(L"https://start.ui.xboxlive.com/CreateAccount", tokenResult.error().redirect());
        VERIFY_ARE_EQUAL(L"0", tokenResult.error().identity());
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

