// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

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
    m_requestBody = http_call_request_message_internal();
    ResultValue = StockMocks::CreateMockHttpCallResponse(web::json::value::parse(L"{}"));
    ResultValueInternal = StockMocks::CreateMockHttpCallResponseInternal(web::json::value::parse(L"{}"));
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
    UNREFERENCED_PARAMETER(httpCallResponseBodyType);
    if (FAILED(ResultHR))
    {
        throw ResultHR;
    }
    CallCounter++;
    return pplx::task_from_result(ResultValue);
}

#if XSAPI_XDK_AUTH
pplx::task<std::shared_ptr<http_call_response>>
MockHttpCall::get_response_with_auth(
    _In_ Windows::Xbox::System::User^ user,
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
#endif

#if XSAPI_NONXDK_CPP_AUTH
pplx::task<std::shared_ptr<http_call_response>>
MockHttpCall::get_response_with_auth(
    _In_ std::shared_ptr<xbox::services::system::xbox_live_user> user,
    _In_ http_call_response_body_type httpCallResponseBodyType,
    _In_ bool allUsersAuthRequired
)
{
    UNREFERENCED_PARAMETER(httpCallResponseBodyType);
    UNREFERENCED_PARAMETER(allUsersAuthRequired);
    if (FAILED(ResultHR))
    {
        throw ResultHR;
    }
    CallCounter++;
    return pplx::task_from_result(ResultValue);
}
#endif

#if XSAPI_NONXDK_WINRT_AUTH
pplx::task<std::shared_ptr<http_call_response>>
MockHttpCall::get_response_with_auth(
    _In_ Microsoft::Xbox::Services::System::XboxLiveUser^ user,
    _In_ http_call_response_body_type httpCallResponseBodyType,
    _In_ bool allUsersAuthRequired
    )
{
    UNREFERENCED_PARAMETER(httpCallResponseBodyType);
    UNREFERENCED_PARAMETER(allUsersAuthRequired);
    if (FAILED(ResultHR))
    {
        throw ResultHR;
    }
    CallCounter++;
    return pplx::task_from_result(ResultValue);
}
#endif

pplx::task<std::shared_ptr<http_call_response>>
MockHttpCall::get_response(
    _In_ http_call_response_body_type httpCallResponseBodyType,
    _In_ const web::http::http_request& httpRequest
    )
{
    UNREFERENCED_PARAMETER(httpCallResponseBodyType);
    UNREFERENCED_PARAMETER(httpRequest);
    if (FAILED(ResultHR))
    {
        throw ResultHR;
    }
    CallCounter++;
    return pplx::task_from_result(ResultValue);
}

xbox_live_result<void> 
MockHttpCall::get_response(
    _In_ http_call_response_body_type httpCallResponseBodyType,
    _In_ async_queue_handle_t queue,
    _In_ http_call_callback callback
    )
{
    UNREFERENCED_PARAMETER(queue);
    UNREFERENCED_PARAMETER(httpCallResponseBodyType);
    if (FAILED(ResultHR))
    {
        throw ResultHR;
    }
    CallCounter++;
    if (fRequestPostFuncInternal != nullptr)
    {
        fRequestPostFuncInternal(ResultValueInternal, m_requestBody.request_message_string());
    }
    ResultValueInternal->set_full_url(utils::internal_string_from_string_t(ServerName));
    ResultValueInternal->route_service_call();
    callback(ResultValueInternal);

    return xbox_live_result<void>();
}

pplx::task<std::shared_ptr<http_call_response>> MockHttpCall::get_response_with_auth(
    _In_ http_call_response_body_type httpCallResponseBodyType
)
{
    UNREFERENCED_PARAMETER(httpCallResponseBodyType);
    if (FAILED(ResultHR))
    {
        throw ResultHR;
    }
    CallCounter++;
    if (fRequestPostFunc != nullptr)
    {
        fRequestPostFunc(ResultValue, utils::string_t_from_internal_string(m_requestBody.request_message_string()));
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
    UNREFERENCED_PARAMETER(userContext);
    UNREFERENCED_PARAMETER(httpCallResponseBodyType);
    UNREFERENCED_PARAMETER(allUsersAuthRequired);
    if (FAILED(ResultHR))
    {
        throw ResultHR;
    }
    CallCounter++;
    if (fRequestPostFunc != nullptr)
    {
        fRequestPostFunc(ResultValue, utils::string_t_from_internal_string(m_requestBody.request_message_string()));
    }
    ResultValue->_Internal_response()->set_full_url(utils::internal_string_from_string_t(ServerName));
    ResultValue->_Internal_response()->route_service_call();
    return pplx::task_from_result(ResultValue);
}

xbox_live_result<void> MockHttpCall::get_response_with_auth(
    _In_ const std::shared_ptr<xbox::services::user_context>& userContext,
    _In_ http_call_response_body_type httpCallResponseBodyType,
    _In_ bool allUsersAuthRequired,
    _In_ async_queue_handle_t queue,
    _In_ xbox_live_callback<std::shared_ptr<http_call_response_internal>> callback
    )
{
    UNREFERENCED_PARAMETER(userContext);
    UNREFERENCED_PARAMETER(queue);
    UNREFERENCED_PARAMETER(httpCallResponseBodyType);
    UNREFERENCED_PARAMETER(allUsersAuthRequired);
    if (FAILED(ResultHR))
    {
        throw ResultHR;
    }
    CallCounter++;
    if (fRequestPostFuncInternal != nullptr)
    {
        fRequestPostFuncInternal(ResultValueInternal, m_requestBody.request_message_string());
    }
    ResultValueInternal->set_full_url(utils::internal_string_from_string_t(ServerName));
    ResultValueInternal->route_service_call();
    callback(ResultValueInternal);

    return xbox_live_result<void>();
}

pplx::task<std::shared_ptr<http_call_response>>
MockHttpCall::_Internal_get_response_with_auth(
    _In_ const std::shared_ptr<xbox::services::user_context>& userContext,
    _In_ http_call_response_body_type httpCallResponseBodyType,
    _In_ bool allUsersAuthRequired
)
{
    UNREFERENCED_PARAMETER(userContext);
    UNREFERENCED_PARAMETER(httpCallResponseBodyType);
    UNREFERENCED_PARAMETER(allUsersAuthRequired);
    if (FAILED(ResultHR))
    {
        throw ResultHR;
    }
    CallCounter++;
    return pplx::task_from_result(ResultValue);
}

std::wstring MockHttpCall::server_name() const
{
    return ServerName;
}

const web::uri& MockHttpCall::path_query_fragment() const
{
    return PathQueryFragment;
}

std::wstring MockHttpCall::http_method() const
{
    return HttpMethod;
}

void MockHttpCall::set_retry_allowed(
    _In_ bool value)
{
    UNREFERENCED_PARAMETER(value);
}

bool MockHttpCall::retry_allowed() const
{
    return true;
}

void MockHttpCall::set_long_http_call(
    _In_ bool value)
{
    UNREFERENCED_PARAMETER(value);
}

bool MockHttpCall::long_http_call() const
{
    return true;
}

void MockHttpCall::set_request_body(
    _In_ const string_t& value)
{
    m_requestBody = http_call_request_message_internal(utils::internal_string_from_string_t(value));
}

void
MockHttpCall::set_request_body(
    _In_ const web::json::value& value
    )
{
    m_requestBody = http_call_request_message_internal(utils::internal_string_from_string_t(value.serialize()));
}

void
MockHttpCall::set_request_body(
    _In_ const std::vector<BYTE>& value
    )
{
    m_requestBody = http_call_request_message_internal(xsapi_internal_vector<BYTE>(value.begin(), value.end()));
}

void
MockHttpCall::set_request_body(
    _In_ const xsapi_internal_string& value
)
{
    m_requestBody = http_call_request_message_internal(value);
}

const http_call_request_message_internal& MockHttpCall::request_body() const
{
    return m_requestBody;
}

void MockHttpCall::set_content_type_header_value(
    _In_ const std::wstring& value)
{
    UNREFERENCED_PARAMETER(value);
}

std::wstring MockHttpCall::content_type_header_value() const
{
    static std::wstring value = L"MockGetContentTypeHeaderValue";
    return value;
}

void MockHttpCall::set_xbox_contract_version_header_value(
    _In_ const std::wstring& value)
{
    UNREFERENCED_PARAMETER(value);
}

std::wstring MockHttpCall::xbox_contract_version_header_value() const
{
    static std::wstring value = L"MockGetXboxContractVersionHeaderValue";
    return value;
}

void MockHttpCall::set_add_default_headers(bool value)
{
    UNREFERENCED_PARAMETER(value);
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
    set_custom_header(utils::internal_string_from_string_t(headerName),
        utils::internal_string_from_string_t(headerValue),
        false
        );
}

void MockHttpCall::set_custom_header(
    _In_ const xsapi_internal_string& headerName,
    _In_ const xsapi_internal_string& headerValue,
    _In_ bool allowTracing
    )
{
    UNREFERENCED_PARAMETER(allowTracing);
#if UNIT_TEST_SERVICES
    ResultValueInternal->add_response_header(headerName, headerValue);
#endif
}

void MockHttpCall::remove_custom_header(
    _In_ const xsapi_internal_string& headerName
    )
{
#if UNIT_TEST_SERVICES
    ResultValueInternal->remove_response_header(headerName);
#endif
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
