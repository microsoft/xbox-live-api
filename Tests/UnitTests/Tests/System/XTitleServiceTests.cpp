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
#include "xtitle_service.h"
#include "StockMocks.h"

#include "user_impl.h"
#include "xbox_system_factory.h"
#include "MockXboxSystemFactory.h"

using namespace xbox::services::system;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_BEGIN

#define TEST_CLASS_OWNER L"jameslao"

class XTitleServiceTests : public UnitTestBase
{
public:
    TEST_CLASS(XTitleServiceTests);
    DEFINE_TEST_CLASS_SETUP();

    TEST_METHOD_SETUP(SetupFactory)
    {
        m_mockXboxSystemFactory.reset(new MockXboxSystemFactory());
        xbox_system_factory_winstore_impl::set_factory( m_mockXboxSystemFactory );

        m_mockXboxSystemFactory->GetMockHttpCall()->ResultValue = StockMocks::CreateMockHttpCallResponse(
            StockMocks::GetStockNsal()
            );

        m_callSettings.reset(new xbox::services::xbox_live_context_settings());
        m_authConfig.reset(new xbox::services::system::auth_config(L".MockSandbox", L"MockPrefix-", L"MockEnv", false));

        return true;
    }

    DEFINE_TEST_CASE_WITH_DESC(DefaultNsalTest, L"Get the default NSAL")
    DEFINE_TEST_CASE_WITH_DESC(IgnoresEndpointsWithNoRelyingParty, L"If there is no relying party, there's no token so skip it")
    DEFINE_TEST_CASE_WITH_DESC(HostTypeParsing, L"Make sure all the host types are parsed correctly")
    DEFINE_TEST_CASE_WITH_DESC(SignaturePolicyVersionRequired, L"Version is a required field in signature policy")
    DEFINE_TEST_CASE_WITH_DESC(SignaturePolicyMaxBodyBytesRequired, L"MaxBodyBytes is a required field in signature policy")
    DEFINE_TEST_CASE_WITH_DESC(EndpointProtocolRequired, L"Protocol is a required field in endpoint")
    DEFINE_TEST_CASE_WITH_DESC(EndpointHostRequired, L"Host is a required field in endpoint")
    DEFINE_TEST_CASE_WITH_DESC(EndpointHostTypeRequired, L"HostType is a required field in endpoint")
    DEFINE_TEST_CASE_WITH_DESC(EndpointTokenTypeRequired, L"TokenType is a required field in endpoint")

private:
    xtitle_service_impl m_xtitle;
    std::shared_ptr<xbox::services::xbox_live_context_settings> m_callSettings;
    std::shared_ptr<xbox::services::system::auth_config> m_authConfig;
};

void XTitleServiceTests::DefaultNsalTest()
{
    nsal nsal(m_xtitle.get_default_nsal(m_callSettings, m_authConfig).get().payload());

    nsal_endpoint_info actual;

    nsal_endpoint_info expected(L"http://xboxlive.com", L"", L"JWT", 0);
    VERIFY_IS_TRUE(nsal.get_endpoint(nsal_protocol::https, L"profile.xboxlive.com", 443, L"/users/12345", actual));
    VERIFY_ARE_EQUAL(expected, actual);

    expected = nsal_endpoint_info(L"http://device.mgt.xboxlive.com", L"", L"JWT", 1);
    VERIFY_IS_TRUE(nsal.get_endpoint(nsal_protocol::https, L"device.mgt.xboxlive.com", 443, L"", actual));
    VERIFY_ARE_EQUAL(expected, actual);

    expected = nsal_endpoint_info(L"http://xlink.xboxlive.com", L"", L"JWT", 0);
    VERIFY_IS_TRUE(nsal.get_endpoint(nsal_protocol::https, L"hello.dfhosted.net", 443, L"", actual));
    VERIFY_ARE_EQUAL(expected, actual);

    expected = nsal_endpoint_info(L"http://music.xboxlive.com", L"", L"JWT", 0);
    VERIFY_IS_TRUE(nsal.get_endpoint(nsal_protocol::https, L"musicdelivery-ssl.xboxlive.com", 443, L"", actual));
    VERIFY_ARE_EQUAL(expected, actual);

    expected = nsal_endpoint_info(L"http://licensing.xboxlive.com", L"", L"JWT", 0);
    VERIFY_IS_TRUE(nsal.get_endpoint(nsal_protocol::https, L"inventory.xboxlive.com", 443, L"", actual));
    VERIFY_ARE_EQUAL(expected, actual);

    VERIFY_IS_TRUE(nsal.get_endpoint(nsal_protocol::https, L"licensing.xboxlive.com", 443, L"", actual));
    VERIFY_ARE_EQUAL(expected, actual);

    expected = nsal_endpoint_info(L"http://data-vef.xboxlive.com", L"", L"JWT", 1);
    VERIFY_IS_TRUE(nsal.get_endpoint(nsal_protocol::https, L"data-vef.xboxlive.com", 443, L"", actual));
    VERIFY_ARE_EQUAL(expected, actual);

    VERIFY_ARE_EQUAL(signature_policy(1, 8192, std::vector<std::wstring>()), nsal.get_signature_policy(0));
    VERIFY_ARE_EQUAL(signature_policy(1, 4294967295, std::vector<std::wstring>()), nsal.get_signature_policy(1));
}

void XTitleServiceTests::IgnoresEndpointsWithNoRelyingParty()
{
    m_mockXboxSystemFactory->GetMockHttpCall()->ResultValue = StockMocks::CreateMockHttpCallResponse(
        web::json::value::parse(L"{\"EndPoints\":[{\"Protocol\":\"https\",\"Host\":\"contoso.com\",\"HostType\":\"fqdn\"}]}")
        );

    nsal nsal(m_xtitle.get_default_nsal(m_callSettings, m_authConfig).get().payload());

    nsal_endpoint_info actual;
    VERIFY_IS_FALSE(nsal.get_endpoint(nsal_protocol::https, L"contoso.com", 443, L"", actual));
}

void XTitleServiceTests::HostTypeParsing()
{
    m_mockXboxSystemFactory->GetMockHttpCall()->ResultValue = StockMocks::CreateMockHttpCallResponse(
        web::json::value::parse(L"{\"EndPoints\":[{\"Protocol\":\"https\",\"Host\":\"contoso.com\",\"HostType\":\"fqdn\",\"RelyingParty\":\"http://contoso1.com\",\"TokenType\":\"JWT\"},{\"Protocol\":\"https\",\"Host\":\"*.contoso.com\",\"HostType\":\"wildcard\",\"RelyingParty\":\"http://contoso2.com\",\"TokenType\":\"JWT\"},{\"Protocol\":\"https\",\"Host\":\"1.2.3.4\",\"HostType\":\"ip\",\"RelyingParty\":\"http://contoso3.com\",\"TokenType\":\"JWT\"},{\"Protocol\":\"https\",\"Host\":\"4.3.0.0/16\",\"HostType\":\"cidr\",\"RelyingParty\":\"http://contoso4.com\",\"TokenType\":\"JWT\"}]}")
        );

    nsal nsal(m_xtitle.get_default_nsal(m_callSettings, m_authConfig).get().payload());

    nsal_endpoint_info actual;

    nsal_endpoint_info expected(L"http://contoso1.com", L"", L"JWT", -1);
    VERIFY_IS_TRUE(nsal.get_endpoint(nsal_protocol::https, L"contoso.com", 443, L"", actual));
    VERIFY_ARE_EQUAL(expected, actual);

    expected = nsal_endpoint_info(L"http://contoso2.com", L"", L"JWT", -1);
    VERIFY_IS_TRUE(nsal.get_endpoint(nsal_protocol::https, L"sub.contoso.com", 443, L"", actual));
    VERIFY_ARE_EQUAL(expected, actual);

    expected = nsal_endpoint_info(L"http://contoso3.com", L"", L"JWT", -1);
    VERIFY_IS_TRUE(nsal.get_endpoint(nsal_protocol::https, L"1.2.3.4", 443, L"", actual));
    VERIFY_ARE_EQUAL(expected, actual);

    expected = nsal_endpoint_info(L"http://contoso4.com", L"", L"JWT", -1);
    VERIFY_IS_TRUE(nsal.get_endpoint(nsal_protocol::https, L"4.3.2.1", 443, L"", actual));
    VERIFY_ARE_EQUAL(expected, actual);
}

void XTitleServiceTests::SignaturePolicyVersionRequired()
{
    m_mockXboxSystemFactory->GetMockHttpCall()->ResultValue = StockMocks::CreateMockHttpCallResponse(
        web::json::value::parse(L"{\"EndPoints\":[{\"Protocol\":\"https\",\"Host\":\"contoso.com\",\"HostType\":\"fqdn\",\"RelyingParty\":\"http://contoso0.com\",\"TokenType\":\"JWT\",\"SignaturePolicyIndex\":0}],\"SignaturePolicies\":[{\"MaxBodyBytes\":1024}]}")
        );

    VERIFY_THROWS(m_xtitle.get_default_nsal(m_callSettings, m_authConfig).get(), web::json::json_exception);
}

void XTitleServiceTests::SignaturePolicyMaxBodyBytesRequired()
{
    m_mockXboxSystemFactory->GetMockHttpCall()->ResultValue = StockMocks::CreateMockHttpCallResponse(
        web::json::value::parse(L"{\"EndPoints\":[{\"Protocol\":\"https\",\"Host\":\"contoso.com\",\"HostType\":\"fqdn\",\"RelyingParty\":\"http://contoso0.com\",\"TokenType\":\"JWT\",\"SignaturePolicyIndex\":0}],\"SignaturePolicies\":[{\"Version\":1}]}")
        );

    VERIFY_THROWS(m_xtitle.get_default_nsal(m_callSettings, m_authConfig).get(), web::json::json_exception);
}

void XTitleServiceTests::EndpointProtocolRequired()
{
    m_mockXboxSystemFactory->GetMockHttpCall()->ResultValue = StockMocks::CreateMockHttpCallResponse(
        web::json::value::parse(L"{\"EndPoints\":[{\"Host\":\"contoso.com\",\"HostType\":\"fqdn\",\"RelyingParty\":\"http://contoso0.com\",\"TokenType\":\"JWT\"}]}")
        );

    VERIFY_THROWS(m_xtitle.get_default_nsal(m_callSettings, m_authConfig).get(), web::json::json_exception);
}

void XTitleServiceTests::EndpointHostRequired()
{
    m_mockXboxSystemFactory->GetMockHttpCall()->ResultValue = StockMocks::CreateMockHttpCallResponse(
        web::json::value::parse(L"{\"EndPoints\":[{\"Protocol\":\"https\",\"HostType\":\"fqdn\",\"RelyingParty\":\"http://contoso0.com\",\"TokenType\":\"JWT\"}]}")
        );

    VERIFY_THROWS(m_xtitle.get_default_nsal(m_callSettings, m_authConfig).get(), web::json::json_exception);
}

void XTitleServiceTests::EndpointHostTypeRequired()
{
    m_mockXboxSystemFactory->GetMockHttpCall()->ResultValue = StockMocks::CreateMockHttpCallResponse(
        web::json::value::parse(L"{\"EndPoints\":[{\"Protocol\":\"https\",\"Host\":\"contoso.com\",\"RelyingParty\":\"http://contoso0.com\",\"TokenType\":\"JWT\"}]}")
        );

    VERIFY_THROWS(m_xtitle.get_default_nsal(m_callSettings, m_authConfig).get(), web::json::json_exception);
}

void XTitleServiceTests::EndpointTokenTypeRequired()
{
    m_mockXboxSystemFactory->GetMockHttpCall()->ResultValue = StockMocks::CreateMockHttpCallResponse(
        web::json::value::parse(L"{\"EndPoints\":[{\"Protocol\":\"https\",\"Host\":\"contoso.com\",\"HostType\":\"fqdn\",\"RelyingParty\":\"http://contoso0.com\"}]}")
        );

    VERIFY_THROWS(m_xtitle.get_default_nsal(m_callSettings, m_authConfig).get(), web::json::json_exception);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_END
