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
#include "Nsal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

#define TEST_CLASS_OWNER L"jameslao"

#define VERIFY_ENDPOINT(nsal, protocol, host, port, path, expectedInfo, actualInfo) \
    VERIFY_IS_TRUE(nsal.get_endpoint(protocol, host, port, path, actualInfo), L"Could not find endpoint"); \
    VERIFY_ARE_EQUAL(expectedInfo, actualInfo, L"Unexpected endpoint info")

#define VERIFY_NO_ENDPOINT(nsal, protocol, host, port, path, actualInfo) \
    VERIFY_IS_FALSE(nsal.get_endpoint(protocol, host, port, path, actualInfo), L"Found endpoint unexpectedly")

#define VERIFY_HOST_PORT_PATH(nsal, host, port, path, expectedInfo, actualInfo) \
    VERIFY_ENDPOINT(nsal, nsal_protocol::https, host, port, path, expectedInfo, actualInfo)

#define VERIFY_CONTOSO_PORT_PATH(nsal, port, path, expectedInfo, actualInfo) \
    VERIFY_HOST_PORT_PATH(nsal, L"contoso.com", port, path, expectedInfo, actualInfo)

#define VERIFY_CONTOSO_PATH(nsal, path, expectedInfo, actualInfo) VERIFY_CONTOSO_PORT_PATH(nsal, 443, path, expectedInfo, actualInfo)

#define VERIFY_NO_CONTOSO_PATH(nsal, path, actualInfo) \
    VERIFY_IS_FALSE(nsal.get_endpoint(nsal_protocol::https, L"contoso.com", 443, path, actualInfo), L"Unexpected endpoint match");

class NsalTest
{
public:
    TEST_CLASS(NsalTest);

    DEFINE_TEST_CASE_WITH_DESC(NsalFqdnEndpointTest, L"Simple mainline matching test")
    DEFINE_TEST_CASE_WITH_DESC(NsalWildcardEndpointTest, L"Simple mainline matching test")
    DEFINE_TEST_CASE_WITH_DESC(NsalIpEndpointTest, L"Add an endpoint where the host name is an IP address")
    DEFINE_TEST_CASE_WITH_DESC(NsalCidrEndpointTest, L"Add an endpoint where the host name is an CIDR IP range")
        
    DEFINE_TEST_CASE_WITH_DESC(NsalSimplePathRestriction, L"Path restriction test")
    DEFINE_TEST_CASE_WITH_DESC(NsalMultiplePathRestriction, L"Multiple paths for endpoint")
    DEFINE_TEST_CASE_WITH_DESC(NsalPortRestriction, L"Endpoint port restriction test")
    DEFINE_TEST_CASE_WITH_DESC(NsalFqdnMoreSpecificThanWildcard, L"FQDN endpoints take priority over wildcard endpoints")
    DEFINE_TEST_CASE_WITH_DESC(WildcardIsMatchTest, L"Wildcard matching test")
    DEFINE_TEST_CASE_WITH_DESC(InvalidWildcardEndpointTest, L"Invalid wildcard endpoint test")
};

void NsalTest::NsalFqdnEndpointTest()
{
    nsal nsal;

    signature_policy policy(1, 1024 * 8, std::vector<std::wstring>());
    nsal.add_signature_policy(policy);
    
    nsal.add_endpoint(
        nsal_protocol::https,
        L"Contoso.com",
        nsal_host_name_type::fqdn,
        443,
        std::wstring(),
        L"http://contoso.com",
        L"http://contoso.com",
        L"JWT",
        0);

    nsal_endpoint_info expectedInfo(L"http://contoso.com", L"http://contoso.com", L"JWT", 0);
    nsal_endpoint_info actualInfo;

    VERIFY_HOST_PORT_PATH(nsal, L"contoso.com", 443, L"", expectedInfo, actualInfo);
    VERIFY_HOST_PORT_PATH(nsal, L"cOnToSo.COM", 443, L"", expectedInfo, actualInfo);
    VERIFY_HOST_PORT_PATH(nsal, L"CONTOSO.com", 443, L"", expectedInfo, actualInfo);
}

void NsalTest::NsalWildcardEndpointTest()
{
    nsal nsal;

    signature_policy policy(1, 1024 * 8, std::vector<std::wstring>());
    nsal.add_signature_policy(policy);

    nsal.add_endpoint(
        nsal_protocol::https,
        L"*.CoNtoSo.com",
        nsal_host_name_type::wildcard,
        443,
        std::wstring(),
        L"http://contoso.com",
        L"http://contoso.com",
        L"JWT",
        0);

    nsal_endpoint_info expectedInfo(L"http://contoso.com", L"http://contoso.com", L"JWT", 0);
    nsal_endpoint_info actualInfo;

    VERIFY_HOST_PORT_PATH(nsal, L"sub.contoso.com", 443, L"", expectedInfo, actualInfo);
    VERIFY_HOST_PORT_PATH(nsal, L"SuB.cOnToSo.COM", 443, L"", expectedInfo, actualInfo);
    VERIFY_HOST_PORT_PATH(nsal, L"SUB.CONTOSO.com", 443, L"", expectedInfo, actualInfo);
}

void NsalTest::NsalIpEndpointTest()
{
    nsal nsal;

    nsal.add_endpoint(
        nsal_protocol::http,
        L"1.1.1.1",
        nsal_host_name_type::ip,
        54321,
        L"",
        L"http://contoso.com",
        L"http://contoso.com",
        L"JWT",
        -1);

    nsal_endpoint_info actualInfo;
    nsal_endpoint_info expectedInfo(L"http://contoso.com", L"http://contoso.com", L"JWT", -1);

    VERIFY_ENDPOINT(nsal, nsal_protocol::http, L"1.1.1.1", 54321, L"", expectedInfo, actualInfo);
}

void NsalTest::NsalCidrEndpointTest()
{
    nsal nsal;

    nsal.add_endpoint(
        nsal_protocol::https,
        L"192.168.0.0/16",
        nsal_host_name_type::cidr,
        54321,
        L"",
        L"http://contoso.com",
        L"http://contoso.com",
        L"JWT",
        -1);

    nsal_endpoint_info actualInfo;
    nsal_endpoint_info expectedInfo(L"http://contoso.com", L"http://contoso.com", L"JWT", -1);

    VERIFY_ENDPOINT(nsal, nsal_protocol::https, L"192.168.0.0", 54321, L"", expectedInfo, actualInfo);
    VERIFY_ENDPOINT(nsal, nsal_protocol::https, L"192.168.0.100", 54321, L"", expectedInfo, actualInfo);
    VERIFY_ENDPOINT(nsal, nsal_protocol::https, L"192.168.255.255", 54321, L"", expectedInfo, actualInfo);

    VERIFY_NO_ENDPOINT(nsal, nsal_protocol::https, L"192.169.0.0", 54321, L"", actualInfo);
}

void NsalTest::NsalSimplePathRestriction()
{
    nsal nsal;

    signature_policy policy(1, 1024 * 8, std::vector<std::wstring>());
    nsal.add_signature_policy(policy);

    nsal.add_endpoint(
        nsal_protocol::https,
        L"contoso.com",
        nsal_host_name_type::fqdn,
        443,
        L"/path1/path2",
        L"http://contoso.com",
        L"http://contoso.com",
        L"JWT",
        0);

    nsal_endpoint_info actualInfo;
    nsal_endpoint_info expectedInfo(L"http://contoso.com", L"http://contoso.com", L"JWT", 0);

    VERIFY_CONTOSO_PATH(nsal, L"/path1/path2", expectedInfo, actualInfo);
    VERIFY_CONTOSO_PATH(nsal, L"/path1/path2/path3", expectedInfo, actualInfo);

    VERIFY_NO_CONTOSO_PATH(nsal, L"", actualInfo);
    VERIFY_NO_CONTOSO_PATH(nsal, L"/", actualInfo);
    VERIFY_NO_CONTOSO_PATH(nsal, L"/path1", actualInfo);
}

void NsalTest::NsalMultiplePathRestriction()
{
    nsal nsal;

    signature_policy policy(1, 1024 * 8, std::vector<std::wstring>());
    nsal.add_signature_policy(policy);

    nsal.add_endpoint(
        nsal_protocol::https,
        L"contoso.com",
        nsal_host_name_type::fqdn,
        443,
        L"",
        L"http://contoso0.com",
        L"http://contoso00.com",
        L"JWT",
        0);

    nsal.add_endpoint(
        nsal_protocol::https,
        L"contoso.com",
        nsal_host_name_type::fqdn,
        443,
        L"/path1",
        L"http://contoso1.com",
        L"http://contoso11.com",
        L"JWT",
        0);

    nsal.add_endpoint(
        nsal_protocol::https,
        L"contoso.com",
        nsal_host_name_type::fqdn,
        443,
        L"/path1/path2/path3",
        L"http://contoso2.com",
        L"http://contoso22.com",
        L"JWT",
        0);

    nsal_endpoint_info actualInfo;
    nsal_endpoint_info expectedInfo(L"http://contoso0.com", L"http://contoso00.com", L"JWT", 0);

    VERIFY_CONTOSO_PATH(nsal, L"", expectedInfo, actualInfo);
    VERIFY_CONTOSO_PATH(nsal, L"/", expectedInfo, actualInfo);
    VERIFY_CONTOSO_PATH(nsal, L"/asdf", expectedInfo, actualInfo);

    expectedInfo = nsal_endpoint_info(L"http://contoso1.com", L"http://contoso11.com", L"JWT", 0);

    VERIFY_CONTOSO_PATH(nsal, L"/path1/", expectedInfo, actualInfo);
    VERIFY_CONTOSO_PATH(nsal, L"/path1/path2/", expectedInfo, actualInfo);
    VERIFY_CONTOSO_PATH(nsal, L"/path1/asdf/", expectedInfo, actualInfo);

    expectedInfo = nsal_endpoint_info(L"http://contoso2.com", L"http://contoso22.com", L"JWT", 0);

    VERIFY_CONTOSO_PATH(nsal, L"/path1/path2/path3", expectedInfo, actualInfo);
    VERIFY_CONTOSO_PATH(nsal, L"/path1/path2/path3/asdf", expectedInfo, actualInfo);
}

void NsalTest::NsalPortRestriction()
{
    nsal nsal;

    signature_policy policy(1, 1024 * 8, std::vector<std::wstring>());
    nsal.add_signature_policy(policy);

    nsal.add_endpoint(
        nsal_protocol::https,
        L"contoso.com",
        nsal_host_name_type::fqdn,
        443,
        L"/",
        L"http://contoso0.com",
        L"http://contoso0.com",
        L"JWT",
        0);

    nsal.add_endpoint(
        nsal_protocol::https,
        L"contoso.com",
        nsal_host_name_type::fqdn,
        8443,
        L"/",
        L"http://contoso1.com",
        L"http://contoso1.com",
        L"JWT",
        0);

    nsal_endpoint_info actualInfo;
    nsal_endpoint_info expectedInfo(L"http://contoso0.com", L"http://contoso0.com", L"JWT", 0);

    VERIFY_CONTOSO_PATH(nsal, L"/", expectedInfo, actualInfo);

    expectedInfo = nsal_endpoint_info(L"http://contoso1.com", L"http://contoso1.com", L"JWT", 0);

    VERIFY_CONTOSO_PORT_PATH(nsal, 8443, L"/", expectedInfo, actualInfo);
}

void NsalTest::NsalFqdnMoreSpecificThanWildcard()
{
    nsal nsal;

    signature_policy policy(1, 1024 * 8, std::vector<std::wstring>());
    nsal.add_signature_policy(policy);

    nsal.add_endpoint(
        nsal_protocol::https,
        L"*.contoso.com",
        nsal_host_name_type::wildcard,
        443,
        L"/",
        L"http://contoso0.com",
        L"http://contoso0.com",
        L"JWT",
        0);

    nsal.add_endpoint(
        nsal_protocol::https,
        L"sub.contoso.com",
        nsal_host_name_type::fqdn,
        443,
        L"/",
        L"http://contoso1.com",
        L"http://contoso1.com",
        L"JWT",
        0);

    nsal_endpoint_info actualInfo;
    nsal_endpoint_info expectedInfo(L"http://contoso0.com", L"http://contoso0.com", L"JWT", 0);

    VERIFY_HOST_PORT_PATH(nsal, L"asdf.contoso.com", 443, L"/", expectedInfo, actualInfo);

    expectedInfo = nsal_endpoint_info(L"http://contoso1.com", L"http://contoso1.com", L"JWT", 0);

    VERIFY_HOST_PORT_PATH(nsal, L"sub.contoso.com", 443, L"/", expectedInfo, actualInfo);
}

void NsalTest::WildcardIsMatchTest()
{
    wildcard_nsal_endpoint endpoint(nsal_protocol::https, L"*.contoso.com", nsal_host_name_type::wildcard, 443);

    VERIFY_IS_TRUE(endpoint.is_match(nsal_protocol::https, L"a.contoso.com", 443), L"Did not match unexpectedly");
    VERIFY_IS_TRUE(endpoint.is_match(nsal_protocol::https, L"ab.contoso.com", 443), L"Did not match unexpectedly");

    VERIFY_IS_FALSE(endpoint.is_match(nsal_protocol::https, L"com", 443), L"No subdomain matched unexpectedly");
    VERIFY_IS_FALSE(endpoint.is_match(nsal_protocol::https, L"contoso.com", 443), L"No subdomain matched unexpectedly");
    VERIFY_IS_FALSE(endpoint.is_match(nsal_protocol::https, L"sub2.sub1.contoso.com", 443), L"Wildcard only matches a single subdomain level");
}

#define VERIFY_INVALID_WILDCARD(hostName) \
    VERIFY_THROWS(wildcard_nsal_endpoint(nsal_protocol::https, hostName, nsal_host_name_type::wildcard, 443), std::invalid_argument, L"Expected wildcard host name to be invalid")

void NsalTest::InvalidWildcardEndpointTest()
{
    VERIFY_INVALID_WILDCARD(L"*");
    VERIFY_INVALID_WILDCARD(L"contoso");
    VERIFY_INVALID_WILDCARD(L"contoso.com");
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

