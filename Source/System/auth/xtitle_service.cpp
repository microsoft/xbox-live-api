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
#include "shared_macros.h"
#include "xsapi/system.h"
#include "cpprest/http_msg.h"
#include "nsal.h"
#include "xbox_system_factory.h"
#include "utils.h"
#include "xtitle_service.h"

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_BEGIN
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN
#endif

pplx::task<XBOX_LIVE_NAMESPACE::xbox_live_result<nsal>>
xtitle_service_impl::get_default_nsal(
    _In_ std::shared_ptr<xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<auth_config> authConfig
    )
{
    std::shared_ptr<http_call> httpCall(xbox_system_factory::get_factory()->create_http_call(
        xboxLiveContextSettings,
        _T("GET"),
        authConfig->x_title_endpoint(),
        _T("/titles/default/endpoints?type=1"),
        xbox_live_api::unspecified
        ));

    return httpCall->get_response(http_call_response_body_type::json_body)
    .then([](std::shared_ptr<http_call_response> response)
    {
        return get_xbl_result_from_response<nsal>(response, nsal::deserialize);
    });
}

string_t create_title_nsal_path(_In_ const string_t& titleId)
{
    stringstream_t ss;
    ss << _T("/titles/");
    ss << titleId;
    ss << _T("/endpoints");
    return ss.str();
}

pplx::task<XBOX_LIVE_NAMESPACE::xbox_live_result<nsal>>
xtitle_service_impl::get_title_nsal(
    _In_ std::shared_ptr<auth_manager> authMan,
    _In_ const string_t& titleId,
    _In_ std::shared_ptr<XBOX_LIVE_NAMESPACE::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<auth_config> authConfig
    )
{
    std::shared_ptr<http_call_internal> httpCall(xbox_system_factory::get_factory()->create_http_call_internal(
        xboxLiveContextSettings,
        _T("GET"),
        authConfig->x_title_endpoint(),
        create_title_nsal_path(titleId))
        );

    // Can't use the typical httpCall->get_response_with_auth() for this as the user object isn't created yet
    web::http::http_request httpRequest = httpCall->get_default_request();
    string_t fullUrl = httpCall->server_name() + httpRequest.request_uri().to_string();
    std::wstring requestBody;

    return authMan->internal_get_token_and_signature(
        httpCall->http_method(),
        fullUrl,
        authConfig->xbox_live_endpoint(),
        utils::headers_to_string(httpRequest.headers()),
        std::vector<unsigned char>(),
        false,
        false
        )
    .then([httpCall, httpRequest](XBOX_LIVE_NAMESPACE::xbox_live_result<token_and_signature_result> xblResult) mutable -> pplx::task<std::shared_ptr<http_call_response>>
    {
        token_and_signature_result authResult = xblResult.payload(); // Auth failures will be handled later inside get_response()
        if (!authResult.token().empty())
        {
            httpRequest.headers().add(AUTH_HEADER, authResult.token());
        }

        if (!authResult.signature().empty())
        {
            httpRequest.headers().add(SIG_HEADER, authResult.signature());
        }

        return httpCall->get_response(http_call_response_body_type::json_body, httpRequest);
    })
    .then([](std::shared_ptr<http_call_response> response)
    {
        return get_xbl_result_from_response<nsal>(response, nsal::deserialize);
    });
}

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_END
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
#endif

