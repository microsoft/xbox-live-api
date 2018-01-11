// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "UnitTestBase.h"
#define TEST_CLASS_OWNER L"jasonsa"
#include "DefineTestMacros.h"
#include "StockMocks.h"
#include "xsapi/system.h"
#include <thread>
#include <chrono>

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

class UserTests : public UnitTestBase
{
public:
    TEST_CLASS(UserTests);
    DEFINE_TEST_CLASS_SETUP();

    const std::wstring XboxRelyingParty = L"http://xboxlive.com";
    const std::wstring MusicRelyingParty = L"http://music.xboxlive.com";
    const std::wstring JwtTokenType = L"JWT";

    TEST_METHOD_SETUP(SetupFactory)
    {
        m_mockXboxSystemFactory = std::make_shared<MockXboxSystemFactory>();
        xbox_system_factory_winstore_impl::set_factory(m_mockXboxSystemFactory);
        return true;
    }
    
    TEST_METHOD(SignInAsyncGetsTokenAndSetsDisplayClaims)
    {
        DEFINE_TEST_CASE_PROPERTIES();

        auto curUser = SignInUserWithMocks();

        VERIFY_ARE_EQUAL(curUser.xbox_user_id(), L"TestXboxUserId"); // StockMock results
        VERIFY_ARE_EQUAL(curUser.gamertag(), L"TestGamerTag");
        VERIFY_IS_NOT_NULL(curUser.auth_config());
    }

    TEST_METHOD(TestGetTokenAndSignatureAsync3)
    {
        DEFINE_TEST_CASE_PROPERTIES();

        auto curUser = SignInUserWithMocks();

        pplx::task< xbox::services::xbox_live_result<token_and_signature_result> > asyncOp = curUser.get_token_and_signature(
            L"GET",
            L"https://xboxlive.com",
            L""
            );

        GetAndVerifyTokenResult( asyncOp );
    }

    TEST_METHOD(TestGetTokenAndSignatureAsync4)
    {
        DEFINE_TEST_CASE_PROPERTIES();
        auto curUser = SignInUserWithMocks();

        pplx::task< xbox::services::xbox_live_result<token_and_signature_result> > asyncOp = curUser.get_token_and_signature(
            L"GET",
            L"https://xboxlive.com",
            L"",
            L"MockRequestBody"
            );

        GetAndVerifyTokenResult(asyncOp);
    }

    TEST_METHOD(TestGetTokenAndSignatureArrayAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES();
        auto curUser = SignInUserWithMocks();

        std::vector<unsigned char> requestBodyArray { 1, 2 };
        pplx::task< xbox::services::xbox_live_result<token_and_signature_result> > asyncOp = curUser.get_token_and_signature_array(
            L"GET",
            L"https://xboxlive.com",
            L"",
            requestBodyArray
            );

        GetAndVerifyTokenResult(asyncOp);
    }

    TEST_METHOD(XTokensAreCached)
    {
        DEFINE_TEST_CASE_PROPERTIES();

        const std::wstring token(L"expectedtoken");
        const std::wstring userHash(L"userhash");
        const utility::datetime notAfter = utility::datetime::utc_now() + utility::datetime::from_hours(4);

        m_mockXboxSystemFactory->GetMockXstsTokenService()->ResultValue = 
            token_result(token, notAfter.to_interval(), userHash, L"gamertag", L"123456", L"123", L"", L"");

        auto curUser = SignInUserWithMocks();

        auto asyncOp = curUser.get_token_and_signature(L"GET", L"https://xboxlive.com/path", L"");
        GetAndVerifyTokenResult(asyncOp, userHash, token);
    }

    TEST_METHOD(UTokensAreCached)
    {
        DEFINE_TEST_CASE_PROPERTIES();

        std::wstring token(L"expectedtoken");
        std::wstring userHash(L"userhash");

        // U token is valid for a while
        m_mockXboxSystemFactory->GetMockUserTokenService()->ResultValue = token_result(
            L"utoken",
            (utility::datetime::utc_now() + utility::datetime::from_hours(1)).to_interval(),
            userHash,
            L"",
            L"",
            L"",
            L"",
            L"");

        // X token is already expired so it should refresh everytime
        m_mockXboxSystemFactory->GetMockXstsTokenService()->ResultValue = token_result(
            token,
            (utility::datetime::utc_now() - utility::datetime::from_hours(4)).to_interval(),
            userHash,
            L"gamertag",
            L"123456",
            L"123",
            L"123",
            L"123");

        auto curUser = SignInUserWithMocks();

        auto asyncOp = curUser.get_token_and_signature(L"GET", L"https://xboxlive.com", L"");
        GetAndVerifyTokenResult(asyncOp, userHash, token);
    }

    TEST_METHOD(XTokensAreRefreshed)
    {
        DEFINE_TEST_CASE_PROPERTIES();

        std::wstring token(L"expectedtoken");
        std::wstring userHash(L"userhash");

        // X token is about to expire so it should refresh
        m_mockXboxSystemFactory->GetMockXstsTokenService()->ResultValue = token_result(
            token,
            (utility::datetime::utc_now() + utility::datetime::from_minutes(1)).to_interval(),
            userHash,
            L"gamertag",
            L"123456",
            L"123",
            L"123",
            L"123");

        auto curUser = SignInUserWithMocks();

        auto asyncOp = curUser.get_token_and_signature(L"GET", L"https://xboxlive.com", L"");

        // Sleep for a second to make sure the background refresh thread has time to finish.
        std::this_thread::sleep_for(std::chrono::seconds(1));

        GetAndVerifyTokenResult(asyncOp, userHash, token);
    }

    TEST_METHOD(CachesMultipleTokens)
    {
        DEFINE_TEST_CASE_PROPERTIES();

        std::wstring expectedToken(L"expectedtoken");
        std::wstring expectedUserHash(L"userhash");

        m_mockXboxSystemFactory->GetMockUserTokenService()->ResultValue = token_result(
            L"utoken",
            (utility::datetime::utc_now() + utility::datetime::from_hours(1)).to_interval(),
            expectedUserHash,
            L"",
            L"",
            L"123",
            L"",
            L"");

        m_mockXboxSystemFactory->GetMockXstsTokenService()->ResultValue = token_result(
            expectedToken,
            (utility::datetime::utc_now() + utility::datetime::from_hours(1)).to_interval(),
            expectedUserHash,
            L"gamertag",
            L"123456",
            L"123",
            L"",
            L"");

        auto curUser = SignInUserWithMocks();

        auto asyncOp = curUser.get_token_and_signature(L"GET", L"https://xboxlive.com/path", L"");
        GetAndVerifyTokenResult(asyncOp, expectedUserHash, expectedToken);

        m_mockXboxSystemFactory->GetMockXstsTokenService()->ResultValue = token_result(
            expectedToken,
            (utility::datetime::utc_now() + utility::datetime::from_hours(1)).to_interval(),
            expectedUserHash,
            L"gamertag",
            L"123456",
            L"123",
            L"",
            L"");

        // TODO 718329
        //m_mockXboxSystemFactory->ExpectGetXToken(true, MusicRelyingParty, JwtTokenType);

        asyncOp = curUser.get_token_and_signature(L"GET", L"https://music.xboxlive.com", L"");
        GetAndVerifyTokenResult(asyncOp, expectedUserHash, expectedToken);

        asyncOp = curUser.get_token_and_signature(L"GET", L"https://music.xboxlive.com", L"");
        GetAndVerifyTokenResult(asyncOp, expectedUserHash, expectedToken);
    }

    TEST_METHOD(TestUserCtors)
    {
        DEFINE_TEST_CASE_PROPERTIES();

        auto curUser = SignInUserWithMocks();
        auto curUser2 = curUser;

        VERIFY_ARE_EQUAL(curUser.xbox_user_id(), curUser2.xbox_user_id());
        VERIFY_ARE_EQUAL(curUser.gamertag(), curUser2.gamertag());

        auto curUser3(curUser);

        VERIFY_ARE_EQUAL(curUser.xbox_user_id(), curUser3.xbox_user_id());
        VERIFY_ARE_EQUAL(curUser.gamertag(), curUser3.gamertag());

        //user userMoved = std::move(SignInUserWithMocks());
        //VERIFY_ARE_EQUAL(curUser.xbox_user_id(), userMoved.xbox_user_id());
        //VERIFY_ARE_EQUAL(curUser.gamertag(), userMoved.gamertag());
    }

    void GetAndVerifyTokenResult(
        pplx::task< xbox::services::xbox_live_result<token_and_signature_result> > asyncOp,
        const std::wstring& expectedUserHash = L"TestXboxUserHash",
        const std::wstring& expectedToken = L"TestToken"
        )
    {
        token_and_signature_result tokenResult;
        VERIFY_NO_THROW(tokenResult = asyncOp.get().payload());

        VERIFY_ARE_EQUAL(tokenResult.token(), L"XBL3.0 x=" + expectedUserHash + L";" + expectedToken);
        VERIFY_IS_TRUE(!tokenResult.signature().empty());
    }

    static xbox_live_user SignInUserWithMocks()
    {
        xbox_live_user userTemp;
        VERIFY_NO_THROW(userTemp.signin_silently().get());
        return userTemp;
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

