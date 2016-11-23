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
#include "xbox_system_factory.h"
#include "xsapi/system.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

string_service::string_service()
{
}

string_service::string_service(
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
    ) :
    m_userContext(std::move(userContext)),
    m_xboxLiveContextSettings(std::move(xboxLiveContextSettings)),
    m_appConfig(std::move(appConfig))
{
}

pplx::task<xbox_live_result<verify_string_result>>
string_service::verify_string(_In_ const string_t& stringToVerify)
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(stringToVerify, verify_string_result, "stringToVerify is empty");

    std::vector<string_t> stringsToVerify;
    stringsToVerify.push_back(stringToVerify);
    return verify_strings(std::vector<string_t>(stringsToVerify))
    .then([](xbox_live_result<std::vector<verify_string_result>> results)
    {
        auto& stringResult = results.payload();
        if (stringResult.empty() && results.err() == xbox_live_error_condition::generic_error)
        {
            return xbox_live_result<verify_string_result>(xbox_live_error_code::json_error, "string validation result not fond");
        }
        else if (stringResult.empty())
        {
            return xbox_live_result<verify_string_result>(results.err(), results.err_message());
        }

        return xbox_live_result<verify_string_result>(
            stringResult[0],
            results.err(),
            results.err_message()
            );
    });
}

pplx::task<xbox_live_result<std::vector<verify_string_result>>>
string_service::verify_strings(_In_ const std::vector<string_t>& stringsToVerify)
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(stringsToVerify, std::vector<verify_string_result>, "stringsToVerify is empty");

    std::shared_ptr<http_call> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("POST"),
        utils::create_xboxlive_endpoint(_T("client-strings"), m_appConfig),
        _T("/system/strings/validate"),
        xbox_live_api::verify_strings
        );

    auto request = web::json::value::object();
    auto& stringsJson = request[_T("stringstoVerify")];
    int i = 0;
    for (const auto& stringToVerify : stringsToVerify)
    {
        stringsJson[i++] = web::json::value::string(stringToVerify);
    }

    httpCall->set_request_body(request.serialize());
    httpCall->set_xbox_contract_version_header_value(_T("2"));

    return httpCall->get_response_with_auth(m_userContext)
    .then([](std::shared_ptr<http_call_response> response)
    {
        std::error_code errc = xbox_live_error_code::no_error;
        auto result = utils::extract_xbox_live_result_json_vector<verify_string_result>(
            verify_string_result::_Deserialize,
            response->response_body_json(),
            _T("verifyStringResult"),
            errc,
            true
            );

        return utils::generate_xbox_live_result<std::vector<verify_string_result>>(
            result,
            response
            );
    });

}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
