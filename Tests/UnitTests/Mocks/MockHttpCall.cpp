c//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#include "MockHttpCall.h"
#include "StockMocks.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

MockHttpCall::MockHttpCall() :
    ResultHR(S_OK),
    CallCounter(0)
{
    reinit();
}

void MockHttpCall::reinit()
{
    m_requestBody = http_call_request_message();
    ResultValue = StockMocks::CreateMockHttpCallResponse(web::json::value::parse(L"{}"));
    ServerName = std::wstring();
    HttpMethod = std::wstring();
    PathQueryFragment = web::uri();
    ResultHR = S_OK;
    CallCounter = 0;
}

pplx::task<std::shared_ptr<http_call_response>> 
MockHttpCall::get_response(
    _In_ http_call_response_body_type httpCallResponseBodyType
    )
{
    if (FAILED(ResultHR))
    {
        throw ResultHR;
    }
    CallCounter++;
    return pplx::task_from_result(ResultValue);
}

pplx::task<std::shared_ptr<http_call_response>>
MockHttpCall::get_response(
    _In_ http_call_response_body_type httpCallResponseBodyType,
    _In_ const web::http::http_request& httpRequest
    )
{
    if (FAILED(ResultHR))
    {
        throw ResultHR;
    }
    CallCounter++;
    return pplx::task_from_result(ResultValue);
}

#if UNIT_TEST_SYSTEM
pplx::task<std::shared_ptr<http_call_response>>
MockHttpCall::get_response(
    _In_ std::shared_ptr<ecdsa> proofKey,
    _In_ const signature_policy& signaturePolicy,
    _In_ http_call_response_body_type httpCallResponseBodyType
    )
{
    if (FAILED(ResultHR))
    {
        throw ResultHR;
    }
    CallCounter++;
    return pplx::task_from_result(ResultValue);
}
#endif

pplx::task<std::shared_ptr<http_call_response>> MockHttpCall::get_response_with_auth(
    _In_ http_call_response_body_type httpCallResponseBodyType
)
{
    if (FAILED(ResultHR))
    {
        throw ResultHR;
    }
    CallCounter++;
    if (fRequestPostFunc != nullptr)
    {
        fRequestPostFunc(ResultValue, m_requestBody.request_message_string());
    }
    return pplx::task_from_result(ResultValue);
}

pplx::task<std::shared_ptr<http_call_response>>
MockHttpCall::get_response_with_auth(
    _In_ const std::shared_ptr<xbox::services::user_context>& userContext,
    _In_ http_call_response_body_type httpCallResponseBodyType,
    _In_ bool allUsersAuthRequired
    ) 
{
    if (FAILED(ResultHR))
    {
        throw ResultHR;
    }
    CallCounter++;
    if (fRequestPostFunc != nullptr)
    {
        fRequestPostFunc(ResultValue, m_requestBody.request_message_string());
    }
    ResultValue->_Set_full_url(ServerName);
    ResultValue->_Route_service_call();
    return pplx::task_from_result(ResultValue);
}

pplx::task<std::shared_ptr<http_call_response>>
MockHttpCall::_Internal_get_response_with_auth(
    _In_ const std::shared_ptr<xbox::services::user_context>& userContext,
    _In_ http_call_response_body_type httpCallResponseBodyType,
    _In_ bool allUsersAuthRequired
)
{
    if (FAILED(ResultHR))
    {
        throw ResultHR;
    }
    CallCounter++;
    return pplx::task_from_result(ResultValue);
}

const std::wstring& MockHttpCall::server_name() const
{
    return ServerName;
}

const web::uri& MockHttpCall::path_query_fragment() const
{
    return PathQueryFragment;
}

const std::wstring& MockHttpCall::http_method() const
{
    return HttpMethod;
}

void MockHttpCall::set_retry_allowed(
    _In_ bool value)
{
}

bool MockHttpCall::retry_allowed() const
{
    return true;
}

void MockHttpCall::set_long_http_call(
    _In_ bool value)
{
}

bool MockHttpCall::long_http_call() const
{
    return true;
}

void MockHttpCall::set_request_body(
    _In_ const std::wstring& value)
{
    m_requestBody = http_call_request_message(value);
}

void
MockHttpCall::set_request_body(
    _In_ const web::json::value& value
    )
{
    m_requestBody = http_call_request_message(value.serialize());
}

void
MockHttpCall::set_request_body(
    _In_ const std::vector<BYTE>& value
    )
{
    m_requestBody = http_call_request_message(value);
}

const http_call_request_message& MockHttpCall::request_body() const
{
    return m_requestBody;
}

void MockHttpCall::set_content_type_header_value(
    _In_ const std::wstring& value)
{
}

const std::wstring& MockHttpCall::content_type_header_value() const
{
    static std::wstring value = L"MockGetContentTypeHeaderValue";
    return value;
}

void MockHttpCall::set_xbox_contract_version_header_value(
    _In_ const std::wstring& value)
{
}

const std::wstring& MockHttpCall::xbox_contract_version_header_value() const
{
    static std::wstring value = L"MockGetXboxContractVersionHeaderValue";
    return value;
}

void MockHttpCall::set_add_default_headers(bool value)
{
}

bool MockHttpCall::add_default_headers() const
{
    return false;
}

void MockHttpCall::set_custom_header(
    _In_ const std::wstring& headerName,
    _In_ const std::wstring& headerValue
    )
{
#if UNIT_TEST_SERVICES
    ResultValue->_Add_response_header(headerName, headerValue);
#endif
}

void MockHttpCall::remove_custom_header(
    _In_ const std::wstring& headerName
    )
{
#if UNIT_TEST_SERVICES
    ResultValue->_Remove_response_header(headerName);
#endif
}

web::http::http_request MockHttpCall::get_default_request()
{
    web::http::http_request request(_T("GET"));
    return request;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
