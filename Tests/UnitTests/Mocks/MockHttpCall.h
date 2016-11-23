//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
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

    virtual pplx::task< std::shared_ptr<http_call_response> > get_response(
        _In_ http_call_response_body_type httpCallResponseBodyType,
        _In_ const web::http::http_request& httpRequest
        ) override;

    virtual pplx::task<std::shared_ptr<http_call_response>> get_response_with_auth(
        _In_ http_call_response_body_type httpCallResponseBodyType = http_call_response_body_type::json_body
        ) override;

    virtual pplx::task<std::shared_ptr<http_call_response>> get_response_with_auth(
        _In_ const std::shared_ptr< xbox::services::user_context >& userContext,
        _In_ http_call_response_body_type httpCallResponseBodyType,
        _In_ bool allUsersAuthRequired = false
        ) override;

    virtual pplx::task<std::shared_ptr<http_call_response>> _Internal_get_response_with_auth(
        _In_ const std::shared_ptr<xbox::services::user_context>& userContext,
        _In_ http_call_response_body_type httpCallResponseBodyType = http_call_response_body_type::json_body,
        _In_ bool allUsersAuthRequired = false
        ) override;

#if UNIT_TEST_SYSTEM
    virtual pplx::task<std::shared_ptr<http_call_response>> get_response(
        _In_ std::shared_ptr<ecdsa> proofKey,
        _In_ const signature_policy& signaturePolicy,
        _In_ http_call_response_body_type httpCallResponseBodyType
        ) override;
#endif

    virtual const std::wstring& server_name() const override;
    virtual const web::uri& path_query_fragment() const override;
    virtual const std::wstring& http_method() const override;

    virtual void set_long_http_call(_In_ bool value) override;
    virtual bool long_http_call() const override;

    virtual void set_retry_allowed(_In_ bool value) override;
    virtual bool retry_allowed() const override;

    virtual void set_request_body(_In_ const string_t& value) override;
    virtual void set_request_body(_In_ const web::json::value& value) override;
    virtual void set_request_body(_In_ const std::vector<BYTE>& value) override;
    virtual const http_call_request_message& request_body() const override;

    virtual void set_content_type_header_value(_In_ const std::wstring& value) override;
    virtual const std::wstring& content_type_header_value() const override;

    virtual void set_xbox_contract_version_header_value(_In_ const std::wstring& value) override;
    virtual const std::wstring& xbox_contract_version_header_value() const override;

    virtual void set_add_default_headers(bool value) override;
    virtual bool add_default_headers() const override;

    virtual void set_custom_header(
        _In_ const std::wstring& headerName,
        _In_ const std::wstring& headerValue
        ) override;

    void remove_custom_header(
        _In_ const std::wstring& headerName
        );

    virtual web::http::http_request get_default_request() override;

    void reinit();

    MockHttpCall();

    HRESULT ResultHR;
    std::shared_ptr<http_call_response> ResultValue;

    std::wstring HttpMethod;
    std::wstring ServerName;
    web::uri PathQueryFragment;
    xbox_live_api XboxLiveApi;

    int CallCounter;

    std::function<void(std::shared_ptr<http_call_response>&, const string_t& requestPost)> fRequestPostFunc;

private:
    http_call_request_message m_requestBody;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

