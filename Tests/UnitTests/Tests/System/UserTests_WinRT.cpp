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
#include "StockMocks.h"
#include "user_impl.h"
#include "User_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SYSTEM_BEGIN

class UserTests_WinRT : public UnitTestBase
{
public:
    TEST_CLASS(UserTests_WinRT);
    DEFINE_TEST_CLASS_SETUP();

    TEST_METHOD_SETUP(SetupFactory)
    {
        m_mockXboxSystemFactory = std::make_shared<MockXboxSystemFactory>();
        xbox_system_factory_winstore_impl::set_factory(m_mockXboxSystemFactory);
        return true;
    }

    TEST_METHOD(TestSignInAsync_WinRT)
    {
        DEFINE_TEST_CASE_PROPERTIES();
        auto user = SignInUserWithMocks_WinRT();
        VERIFY_ARE_EQUAL_PLATFORM_STRING(L"TestXboxUserId", user->XboxUserId); // StockMock results
        VERIFY_ARE_EQUAL_PLATFORM_STRING(L"TestGamerTag", user->Gamertag);
        VERIFY_ARE_EQUAL_PLATFORM_STRING(L"Adult", user->AgeGroup);
        VERIFY_ARE_EQUAL_PLATFORM_STRING(L"191 192", user->Privileges);
        VERIFY_ARE_EQUAL(true, user->IsSignedIn);
    }

    TEST_METHOD(TestGetTokenAndSignatureAsync3)
    {
        DEFINE_TEST_CASE_PROPERTIES();
        auto user = SignInUserWithMocks_WinRT();

        IAsyncOperation< GetTokenAndSignatureResult^ >^ asyncOp = user->GetTokenAndSignatureAsync(
            L"GET",
            L"https://xboxlive.com",
            L""
            );

        GetAndVerifyTokenResult(asyncOp);
    }

    TEST_METHOD(TestGetTokenAndSignatureAsync4)
    {
        DEFINE_TEST_CASE_PROPERTIES();
        auto user = SignInUserWithMocks_WinRT();

        IAsyncOperation< GetTokenAndSignatureResult^ >^ asyncOp = user->GetTokenAndSignatureAsync(
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
        auto user = SignInUserWithMocks_WinRT();

        Platform::Array<byte>^ buffer = ref new Platform::Array<byte>(2);
        buffer->Data[0] = 1;
        buffer->Data[1] = 2;
        IAsyncOperation< GetTokenAndSignatureResult^ >^ asyncOp = user->GetTokenAndSignatureArrayAsync(
            L"GET",
            L"https://xboxlive.com",
            L"",
            buffer
            );

        GetAndVerifyTokenResult(asyncOp);
    }

    void GetAndVerifyTokenResult(
        IAsyncOperation< GetTokenAndSignatureResult^ >^ asyncOp
        )
    {
        GetTokenAndSignatureResult^ tokenResult;
        pplx::create_task(asyncOp)
        .then([&tokenResult](task< GetTokenAndSignatureResult^ > t)
        {
            VERIFY_NO_THROW(tokenResult = t.get());
        })
        .wait();

        VERIFY_IS_NOT_NULL(tokenResult);
        VERIFY_ARE_EQUAL_PLATFORM_STRING(L"XBL3.0 x=TestXboxUserHash;TestToken", tokenResult->Token);

        std::wstring sigHeader(tokenResult->Signature->Data());
        VERIFY_IS_FALSE(sigHeader.empty());
        std::vector<unsigned char> sigHeaderBytes = utility::conversions::from_base64(sigHeader);
        VERIFY_ARE_EQUAL(76, sigHeaderBytes.size());
    }
};

NAMESPACE_MICROSOFT_XBOX_SYSTEM_END

