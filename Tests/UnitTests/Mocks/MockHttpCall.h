// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "pch.h"
#include "xsapi/system.h"
#include "http_call_impl.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

class MockHttpCall : public http_call_internal
{
public:
    virtual pplx::task<std::shared_ptr<http_call_response>> get_response(
        _In_ http_call_response_body_type httpCallResponseBodyType
        ) override;

#if XSAPI_XDK_AUTH
    virtual pplx::task<std::shared_ptr<http_call_response>> get_response_with_auth(
        _In_ Windows::Xbox::System::User^ user,
        _In_ http_call_response_body_type httpCallResponseBodyType = http_call_response_body_type::json_body,
        _In_ bool allUsersAuthRequired = false
        ) override;
#endif 

#if XSAPI_NONXDK_CPP_AUTH
    virtual pplx::task<std::shared_ptr<http_call_response>> get_response_with_auth(
        _In_ std::shared_ptr<xbox::services::system::xbox_live_user> user,
        _In_ http_call_response_body_type httpCallResponseBodyType = http_call_response_body_type::json_body,
        _In_ bool allUsersAuthRequired = false
    ) override;
#endif

#if XSAPI_NONXDK_WINRT_AUTH
    virtual pplx::task<std::shared_ptr<http_call_response>> get_response_with_auth(
        _In_ Microsoft::Xbox::Services::System::XboxLiveUser^ user,
        _In_ http_call_response_body_type httpCallResponseBodyType = http_call_response_body_type::json_body,
        _In_ bool allUsersAuthRequired = false
        ) override;
#endif 

    virtual pplx::task< std::shared_ptr<http_call_response> > get_response(
        _In_ http_call_response_body_type httpCallResponseBodyType,
        _In_ const web::http::http_request& httpRequest
        ) override;

    virtual xbox_live_result<void> get_response(
        _In_ http_call_response_body_type httpCallResponseBodyType,
        _In_ async_queue_handle_t queue,
        _In_ http_call_callback callback
        ) override;

    virtual pplx::task<std::shared_ptr<http_call_response>> get_response_with_auth(
        _In_ http_call_response_body_type httpCallResponseBodyType = http_call_response_body_type::json_body
        ) override;

    virtual pplx::task<std::shared_ptr<http_call_response>> get_response_with_auth(
        _In_ const std::shared_ptr< xbox::services::user_context >& userContext,
        _In_ http_call_response_body_type httpCallResponseBodyType,
        _In_ bool allUsersAuthRequired = false
        ) override;

    virtual xbox_live_result<void> get_response_with_auth(
        _In_ const std::shared_ptr<xbox::services::user_context>& userContext,
        _In_ http_call_response_body_type httpCallResponseBodyType,
        _In_ bool allUsersAuthRequired,
        _In_ async_queue_handle_t queue,
        _In_ xbox_live_callback<std::shared_ptr<http_call_response_internal>> callback
        ) override;

    virtual pplx::task<std::shared_ptr<http_call_response>> _Internal_get_response_with_auth(
        _In_ const std::shared_ptr<xbox::services::user_context>& userContext,
        _In_ http_call_response_body_type httpCallResponseBodyType = http_call_response_body_type::json_body,
        _In_ bool allUsersAuthRequired = false
        );

    virtual std::wstring server_name() const override;
    virtual const web::uri& path_query_fragment() const override;
    virtual std::wstring http_method() const override;

    virtual void set_long_http_call(_In_ bool value) override;
    virtual bool long_http_call() const override;

    virtual void set_retry_allowed(_In_ bool value) override;
    virtual bool retry_allowed() const override;

    virtual void set_request_body(_In_ const string_t& value) override;
    virtual void set_request_body(_In_ const web::json::value& value) override;
    virtual void set_request_body(_In_ const std::vector<BYTE>& value) override;
    virtual void set_request_body(_In_ const xsapi_internal_string& value) override;
    virtual const http_call_request_message_internal& request_body() const override;

    virtual void set_content_type_header_value(_In_ const std::wstring& value) override;
    virtual std::wstring content_type_header_value() const override;

    virtual void set_xbox_contract_version_header_value(_In_ const std::wstring& value) override;
    virtual std::wstring xbox_contract_version_header_value() const override;

    virtual void set_add_default_headers(bool value) override;
    virtual bool add_default_headers() const override;

    virtual void set_custom_header(
        _In_ const std::wstring& headerName,
        _In_ const std::wstring& headerValue
        ) override;

    virtual void set_custom_header(
        _In_ const xsapi_internal_string& headerName,
        _In_ const xsapi_internal_string& headerValue,
        _In_ bool allowTracing
        ) override;

    void remove_custom_header(
        _In_ const xsapi_internal_string& headerName
        );

    void reinit();

    MockHttpCall();

    HRESULT ResultHR;
    std::shared_ptr<http_call_response> ResultValue; // TODO remove
    std::shared_ptr<http_call_response_internal> ResultValueInternal;

    std::wstring HttpMethod;
    std::wstring ServerName;
    web::uri PathQueryFragment;
    xbox_live_api XboxLiveApi;

    int CallCounter;

    std::function<void(std::shared_ptr<http_call_response>&, const string_t& requestPost)> fRequestPostFunc; // TODO remove eventually
    xbox_live_callback<std::shared_ptr<http_call_response_internal>, const xsapi_internal_string&> fRequestPostFuncInternal; 

private:
    http_call_request_message_internal m_requestBody;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

