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
#include "DefineTestMacros.h"
#include "token_result.h"
#include "StockMocks.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

#define TEST_CLASS_OWNER L"jameslao"

class TokenResultTests : public UnitTestBase
{
public:
    TEST_CLASS(TokenResultTests);
    DEFINE_TEST_CLASS_SETUP();

    DEFINE_TEST_CASE_WITH_DESC(UserTokenResponseDeserialization, L"Deserialize a valid XASU response")
    DEFINE_TEST_CASE_WITH_DESC(XTokenResponseDeserialization, L"Deserialize a valid XSTS response")
};

void TokenResultTests::UserTokenResponseDeserialization()
{
    auto tokenResult = 
        token_result::deserialize(StockMocks::GetStockJsonMockUserTokenResult());

    //auto tokenResult = token.payload();
    //VERIFY_IS_TRUE(!token.err());
    VERIFY_ARE_EQUAL(L"TestToken", tokenResult.token());
    VERIFY_ARE_EQUAL(130385528743709609, tokenResult.expiration());

    VERIFY_ARE_EQUAL(L"TestXboxUserHash", tokenResult.user_hash());
    VERIFY_ARE_EQUAL(L"", tokenResult.user_gamertag());
    VERIFY_ARE_EQUAL(L"", tokenResult.user_xuid());
}

void TokenResultTests::XTokenResponseDeserialization()
{
    auto tokenResult =
        token_result::deserialize(StockMocks::GetStockJsonMockXTokenResult());
/*
    auto tokenResult = token.payload();
    VERIFY_IS_TRUE(!token.err());*/
    VERIFY_ARE_EQUAL(L"TestToken", tokenResult.token());
    VERIFY_ARE_EQUAL(130385528743709609, tokenResult.expiration());

    VERIFY_ARE_EQUAL(L"TestXboxUserHash", tokenResult.user_hash());
    VERIFY_ARE_EQUAL(L"TestGamerTag", tokenResult.user_gamertag());
    VERIFY_ARE_EQUAL(L"TestXboxUserId", tokenResult.user_xuid());
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
