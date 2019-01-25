// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "shared_macros.h"
#include "xbox_system_factory.h"
#include "xsapi/multiplayer.h"
#include "xsapi/matchmaking.h"
#include "utils.h"

using namespace xbox::services::multiplayer;
using namespace xbox::services::system;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MATCHMAKING_CPP_BEGIN

matchmaking_service::matchmaking_service(
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
    ) :
    m_userContext(std::move(userContext)),
    m_xboxLiveContextSettings(std::move(xboxLiveContextSettings)),
    m_appConfig(std::move(appConfig))
{
}

pplx::task<xbox_live_result<create_match_ticket_response>>
matchmaking_service::create_match_ticket(
    _In_ const multiplayer_session_reference& ticketSessionReference,
    _In_ const string_t& matchmakingServiceConfigurationId,
    _In_ const string_t& hopperName,
    _In_ const std::chrono::seconds& ticketTimeout,
    _In_ preserve_session_mode preserveSession,
    _In_ const web::json::value& ticketAttributesJson,
    _In_ bool isSymmetric
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(ticketSessionReference.is_null(), create_match_ticket_response, "ticket session reference is null");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(matchmakingServiceConfigurationId, create_match_ticket_response, "hopper name is empty");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(hopperName, create_match_ticket_response, "hopper name is empty");

    string_t subPath = matchmaking_sub_path(
        matchmakingServiceConfigurationId,
        hopperName,
        string_t() // ticketId
        );

    web::json::value request;
    request[_T("giveUpDuration")] = web::json::value::number(static_cast<int32_t>(ticketTimeout.count()));
    request[_T("preserveSession")] = web::json::value::string(convert_preserve_session_mode_to_string(preserveSession));
    request[_T("isSymmetric")] = web::json::value::boolean(isSymmetric);
    request[_T("ticketSessionRef")] = ticketSessionReference._Serialize();
    if (!ticketAttributesJson.is_null())
    {
        request[_T("ticketAttributes")] = std::move(ticketAttributesJson);
    }

    std::shared_ptr<http_call> httpCall = xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("POST"),
        utils::create_xboxlive_endpoint(_T("smartmatch"), m_appConfig),
        subPath,
        xbox_live_api::create_match_ticket
        );

    httpCall->set_retry_allowed(false);
    httpCall->set_xbox_contract_version_header_value(_T("103"));
    httpCall->set_request_body(request.serialize());

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([](std::shared_ptr<http_call_response> response)
    {
        return utils::generate_xbox_live_result<create_match_ticket_response>(
            create_match_ticket_response::_Deserialize(response->response_body_json()),
            response
            );
    });

    return utils::create_exception_free_task<create_match_ticket_response>(
        task
        );
}

pplx::task<xbox_live_result<void>>
matchmaking_service::delete_match_ticket(
    _In_ const string_t& serviceConfigurationId,
    _In_ const string_t& hopperName,
    _In_ const string_t& ticketId
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(serviceConfigurationId, void, "service configuration id is empty");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(hopperName, void, "hopper name is empty");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(ticketId, void, "ticket id is empty");

    string_t subPath = matchmaking_sub_path(
        serviceConfigurationId,
        hopperName,
        ticketId
        );

    std::shared_ptr<http_call> httpCall = xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("DELETE"),
        utils::create_xboxlive_endpoint(_T("smartmatch"), m_appConfig),
        subPath,
        xbox_live_api::delete_match_ticket
        );
    httpCall->set_xbox_contract_version_header_value(_T("103"));

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([](std::shared_ptr<http_call_response> response)
    {
        return xbox_live_result<void>(response->err_code(), response->err_message());
    });

    return utils::create_exception_free_task<void>(
        task
        );
}

pplx::task<xbox_live_result<match_ticket_details_response>>
matchmaking_service::get_match_ticket_details(
    _In_ const string_t& serviceConfigurationId,
    _In_ const string_t& hopperName,
    _In_ const string_t& ticketId
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(serviceConfigurationId, match_ticket_details_response, "service configuration id is empty");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(hopperName, match_ticket_details_response, "hopper name is empty");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(ticketId, match_ticket_details_response, "ticket id is empty");

    string_t subPath = matchmaking_sub_path(
        serviceConfigurationId,
        hopperName,
        ticketId
        );

    std::shared_ptr<http_call> httpCall = xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("GET"),
        utils::create_xboxlive_endpoint(_T("smartmatch"), m_appConfig),
        subPath,
        xbox_live_api::get_match_ticket_details
        );
    httpCall->set_xbox_contract_version_header_value(_T("103"));

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([](std::shared_ptr<http_call_response> response)
    {
        return utils::generate_xbox_live_result<match_ticket_details_response>(
            match_ticket_details_response::_Deserialize(response->response_body_json()),
            response
            );
    });

    return utils::create_exception_free_task<match_ticket_details_response>(
        task
        );
}

pplx::task<xbox_live_result<hopper_statistics_response>>
matchmaking_service::get_hopper_statistics(
    _In_ const string_t& serviceConfigurationId,
    _In_ const string_t& hopperName
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(serviceConfigurationId, hopper_statistics_response, "service configuration id is empty");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(hopperName, hopper_statistics_response, "hopper name is empty");

    string_t subPath = hopper_sub_path(
        serviceConfigurationId,
        hopperName
        );

    std::shared_ptr<http_call> httpCall = xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("GET"),
        utils::create_xboxlive_endpoint(_T("smartmatch"), m_appConfig),
        subPath,
        xbox_live_api::get_hopper_statistics
        );
    httpCall->set_xbox_contract_version_header_value(_T("103"));

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([](std::shared_ptr<http_call_response> response)
    {
        return utils::generate_xbox_live_result<hopper_statistics_response>(
            hopper_statistics_response::_Deserialize(response->response_body_json()),
            response
            );
    });

    return utils::create_exception_free_task<hopper_statistics_response>(
        task
        );
}

string_t
matchmaking_service::matchmaking_sub_path(
    _In_ const string_t& serviceConfigId,
    _In_ const string_t& hopperName,
    _In_ const string_t& ticketId
    )
{
    stringstream_t ss;
    ss << _T("/serviceconfigs/");
    ss << serviceConfigId;
    ss << _T("/hoppers/");
    ss << hopperName;

    if (!ticketId.empty())
    {
        ss << _T("/tickets/");
        ss << ticketId;
    }

    return ss.str();
}

string_t matchmaking_service::hopper_sub_path(
    _In_ const string_t& serviceConfigId,
    _In_ const string_t& hopperName
    )
{
    stringstream_t ss;
    ss << _T("/serviceconfigs/");
    ss << serviceConfigId;
    ss << _T("/hoppers/");
    ss << hopperName;
    ss << _T("/stats");

    return ss.str();
}

string_t
matchmaking_service::convert_preserve_session_mode_to_string(
    _In_ preserve_session_mode preserve_session
    )
{
    string_t retval(_T("unknown"));
    if (preserve_session == preserve_session_mode::always)
    {
        retval = _T("always");
    }
    else if (preserve_session == preserve_session_mode::never)
    {
        retval = _T("never");
    }

    return retval;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_END