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
#include "algorithm_provider.h"
#include "sha256.h"
#include "Ecdsa.h"
#include "DefineTestMacros.h"

#include <string>

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

#define TEST_CLASS_OWNER L"jameslao"

class EcdsaTests
{
public:
    TEST_CLASS(EcdsaTests);

    DEFINE_TEST_CASE_WITH_DESC(RoundtripSignTest, L"Sign and verify roundtrip test")
    DEFINE_TEST_CASE_WITH_DESC(InvalidSignatureTest, L"VerifyHash returns false when signature is invalid")
    DEFINE_TEST_CASE_WITH_DESC(ManagedInteropTest, L"Verify a signature computed using C# to test interop")
};

void EcdsaTests::RoundtripSignTest()
{
    sha256 sha256;
    sha256.add_bytes(std::vector<unsigned char> { 0, 1, 2, 3, 4 });
    std::vector<unsigned char> hash(sha256.get_hash());
    
    ecdsa ecdsaInstance;
    std::vector<unsigned char> signature(ecdsaInstance.sign_hash(hash));

    Log::Comment(utility::conversions::to_base64(signature).data());

    ecc_pub_key pubKey = ecdsaInstance.pub_key();
    ecdsa pubEcdsa(pubKey);
    VERIFY_IS_TRUE(pubEcdsa.verify_signature(hash, signature));
}

void EcdsaTests::InvalidSignatureTest()
{
    sha256 sha256;
    sha256.add_bytes(std::vector<unsigned char> { 0, 1, 2, 3, 4 });
    std::vector<unsigned char> hash(sha256.get_hash());

    ecdsa ecdsa;
    std::vector<unsigned char> signature(ecdsa.sign_hash(hash));
    signature[0]++;

    VERIFY_IS_FALSE(ecdsa.verify_signature(hash, signature));
}

void EcdsaTests::ManagedInteropTest()
{
    std::vector<unsigned char> hash(utility::conversions::from_base64(L"Dr2NPgii2AZL92s/VXq1MxgTSkCJoEhGvOIkt5Mj2/I="));
    std::vector<unsigned char> publicBlob(utility::conversions::from_base64(L"RUNTMSAAAAAtWa8kkNNwm7eda9Udqt6Sjqv5mnMM6JITdtVsbPFqtYls0htKP5r3DqMttlkc7m41VWz3OK7yoVgGFl4kKhuw"));
    std::vector<unsigned char> signature(utility::conversions::from_base64(L"dfVQu2nnKD65o2dfJp+pl5nLo17cjKvEVF3iPK+8I0YCrCHwIhTeImHCRsWH96ZbU5F83vlIr1mmlfk4dqjxhA=="));

    std::vector<unsigned char> x(publicBlob.begin() + 8, publicBlob.begin() + 8 + 32);
    std::vector<unsigned char> y(publicBlob.begin() + 8 + 32, publicBlob.end());

    ecdsa ecdsa(ecc_pub_key(x, y));
    VERIFY_IS_TRUE(ecdsa.verify_signature(hash, signature));
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

