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
#include "xsts_token_service.h"
#include "xbox_system_factory.h"
#include "token_request.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN


pplx::task<XBOX_LIVE_NAMESPACE::xbox_live_result<token_result>>
xsts_token_service_impl::get_x_token_from_service(
    _In_ std::shared_ptr<ecdsa> proofKey,
    _In_ string_t deviceToken,
    _In_ string_t titleToken,
    _In_ string_t userToken,
    _In_ string_t serviceToken,
    _In_ string_t relyingParty,
    _In_ string_t tokenType,
    _In_ std::shared_ptr<auth_config> authConfig,
    _In_ std::shared_ptr<xbox_live_context_settings> xboxLiveContextSettings,
    _In_ string_t titleId
    )
{
    auto appConfig = xbox_live_app_config::get_app_config_singleton();
    auto sandbox = appConfig->sandbox();

    web::json::value request(token_request::create_xtoken_request(
        relyingParty,
        tokenType,
        deviceToken,
        titleToken,
        userToken,
        serviceToken,
        sandbox
        ));

    std::shared_ptr<http_call_internal> httpCall = xbox_system_factory::get_factory()->create_http_call_internal(
        xboxLiveContextSettings,
        _T("POST"),
        authConfig->x_token_endpoint(), 
        _T("/xsts/authorize")
        );
    httpCall->set_request_body(request.serialize());

    return httpCall->get_response(proofKey, token_request::auth_signature_policy, http_call_response_body_type::json_body)
    .then([titleId](std::shared_ptr<http_call_response> response)
    {
        auto status = response->http_status();
 
        if (status == web::http::status_codes::Unauthorized)
        {
            auto result = get_xbl_result_from_response<token_result>(response, token_error::deserialize);
            result.payload().set_http_status_code(response->http_status());
            return result;
        }
        stringstream_t str;
        str << "Http status: ";
        str << response->http_status();

        XBOX_LIVE_NAMESPACE::xbox_live_result<token_result> result = get_xbl_result_from_response<token_result>(response, token_result::deserialize);
        result.payload().set_title_id(titleId);
        result.payload().set_http_status_code(response->http_status());

        return result;
    });
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
