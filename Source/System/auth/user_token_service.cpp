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
#include "token_request.h"
#include "xbox_system_factory.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN


pplx::task<XBOX_LIVE_NAMESPACE::xbox_live_result<token_result>>
user_token_service_impl::get_u_token_from_service(
    _In_ const string_t& rpsTicket,
    _In_ std::shared_ptr<ecdsa> proofKey,
    _In_ std::shared_ptr<auth_config> authenticationConfiguration,
    _In_ std::shared_ptr<xbox_live_context_settings> xboxLiveContextSettings
    )
{
    web::json::value request = token_request::create_user_token_request(
        authenticationConfiguration->xbox_live_relying_party(),
        _T("JWT"),
        _T("RPS"),
        authenticationConfiguration->user_token_site_name(),
        authenticationConfiguration->use_compact_ticket() ? rpsTicket : _T("d=") + rpsTicket, //compact ticket already have "t="
        proofKey);

    std::shared_ptr<http_call_internal> httpCall = xbox_system_factory::get_factory()->create_http_call_internal(
        xboxLiveContextSettings,
        _T("POST"),
        authenticationConfiguration->user_token_endpoint(),
        _T("/user/authenticate"));

    httpCall->set_request_body(request.serialize());

    return httpCall->get_response(proofKey, token_request::auth_signature_policy, http_call_response_body_type::json_body)
    .then([](std::shared_ptr<http_call_response> response) 
    {
        auto result = get_xbl_result_from_response<token_result>(response, token_result::deserialize);
        result.payload().set_http_status_code(response->http_status());
        return result;
    });
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
