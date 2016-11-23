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
#include "request_signer.h"
#include "signature_policy.h"

#include <string>
#include <sstream>

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

//using namespace Windows::Security::Cryptography;
//using namespace Windows::Storage::Streams;

#define TEST_CLASS_OWNER L"jameslao"

class RequestSignerTests
{
public:
    TEST_CLASS(RequestSignerTests);

    DEFINE_TEST_CASE_WITH_DESC(ContentLengthGreaterThanMaxBodyBytes, L"The content body is greater than the max body bytes of the policy")
    DEFINE_TEST_CASE_WITH_DESC(ContentLengthLessThanMaxBodyBytes, L"The content body is less than the max body bytes of the policy")
    DEFINE_TEST_CASE_WITH_DESC(ContentLengthEqualMaxBodyBytes, L"The content body is equal to the max body bytes of the policy")
    DEFINE_TEST_CASE_WITH_DESC(IgnoresHeadersNotInPolicy, L"Request headers not in the policy are not part of the hash")
    DEFINE_TEST_CASE_WITH_DESC(HeadersAreCaseInsensitive, L"Request headers are treated as case insensitive")
    DEFINE_TEST_CASE_WITH_DESC(AuthorizationHeaderIsHashed, L"Authorization header is included in the hash")
};

void TestWithPrecannedRequest(const signature_policy& policy, const std::wstring& expectedHash, const std::wstring& authHeader = L"")
{
    web::http::http_headers headers;
    headers.add(L"header1", L"value1");
    headers.add(L"header2", L"value2");
    headers.add(L"header3", L"value3");
    headers.add(L"header4", L"value4");

    if (!authHeader.empty())
    {
        headers.add(L"Authorization", authHeader);
    }

    std::vector<unsigned char> body({ 0, 1, 2, 3, 4, 5 });

    std::vector<unsigned char> hash(request_signer::hash_request(
        policy,
        130408746879313831,
        L"POST",
        L"/path/path?query=value",
        headers,
        body));

    std::wstring actual(utility::conversions::to_base64(hash));
    std::wstringstream ss;
    ss << L"actual hash = " << actual;
    Log::Comment(ss.str().c_str());

    ss.str(L"");
    ss << L"expected hash = " << expectedHash;
    Log::Comment(ss.str().c_str());

    VERIFY_ARE_EQUAL(expectedHash, actual);
}

void RequestSignerTests::ContentLengthGreaterThanMaxBodyBytes()
{
    TestWithPrecannedRequest(
        signature_policy(1, 3, std::vector<std::wstring>()),
        L"2pGxzP7yskl2AC9MQT9Y7H/xKkTlTMrpGR4IUfuxyxk=");
}

void RequestSignerTests::ContentLengthLessThanMaxBodyBytes()
{
    TestWithPrecannedRequest(
        signature_policy(1, 128, std::vector<std::wstring>()),
        L"zDQfr13QCXJXm4subu2XKeM29HSQ/fWin1NQ4rfE3Xw=");
}

void RequestSignerTests::ContentLengthEqualMaxBodyBytes()
{
    TestWithPrecannedRequest(
        signature_policy(1, 6, std::vector<std::wstring>()),
        L"zDQfr13QCXJXm4subu2XKeM29HSQ/fWin1NQ4rfE3Xw=");
}

void RequestSignerTests::IgnoresHeadersNotInPolicy()
{
    TestWithPrecannedRequest(
        signature_policy(1, 0, std::vector<std::wstring>{ L"header1", L"header3" }),
        L"iXMPW//hkb2xn4YMf+dQqMBd5c1mbAy4UX0tOIVGnjU=");
}

void RequestSignerTests::HeadersAreCaseInsensitive()
{
    TestWithPrecannedRequest(
        signature_policy(1, 0, std::vector<std::wstring>{ L"HEADER1", L"HeAdEr3" }),
        L"iXMPW//hkb2xn4YMf+dQqMBd5c1mbAy4UX0tOIVGnjU=");
}

void RequestSignerTests::AuthorizationHeaderIsHashed()
{
    TestWithPrecannedRequest(
        signature_policy(1, 0, std::vector<std::wstring>()),
        L"KX19PX3i0AGoK/4vNAnN/VbAbbp3Te7vIbrhLDf8/Yo=",
        L"XBL3.0 x=2934345;token");
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

