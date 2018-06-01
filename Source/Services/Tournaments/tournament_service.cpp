// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/tournaments.h"
#include "xbox_system_factory.h"
#include "utils.h"
#include "user_context.h"
#include "tournament_service_internal.h"

using namespace pplx;
using namespace xbox::services::system;
using namespace xbox::services::tournaments;

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_CPP_BEGIN

const string_t c_tournamentsHubServiceContractHeaderValue = _T("3");

tournament_service::tournament_service()
{
}

tournament_service::tournament_service(
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig,
    _In_ std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service> rtaService
    ) :
    m_userContext(std::move(userContext)),
    m_xboxLiveContextSettings(std::move(xboxLiveContextSettings)),
    m_appConfig(std::move(appConfig)),
    m_realTimeActivityService(rtaService),
    m_tournamentServiceImpl(std::make_shared<tournament_service_impl>(rtaService))
{
}

pplx::task<xbox::services::xbox_live_result<tournament_request_result>>
tournament_service::get_tournaments(
    _In_ tournament_request request
    )
{
    auto subPath = tournament_sub_path_url(request);
    return get_tournaments_internal(
        utils::create_xboxlive_endpoint(_T("tournamentshub"), m_appConfig),
        subPath
        );
}

pplx::task<xbox::services::xbox_live_result<tournament_request_result>>
tournament_service::_Get_tournaments(
    _In_ const string_t& nextLinkUrl
    )
{
    return get_tournaments_internal(nextLinkUrl, _T(""));
}

pplx::task<xbox::services::xbox_live_result<tournament_request_result>>
tournament_service::get_tournaments_internal(
    _In_ const string_t& serverName,
    _In_ const string_t& pathQueryFragment
    )
{
    std::shared_ptr<http_call> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("GET"),
        serverName,
        pathQueryFragment,
        xbox_live_api::get_tournaments
        );
    httpCall->set_xbox_contract_version_header_value(c_tournamentsHubServiceContractHeaderValue);

    auto userContext = m_userContext;
    auto xboxLiveContextSettings = m_xboxLiveContextSettings;
    auto appConfig = m_appConfig;
    auto rtaService = m_realTimeActivityService;

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([userContext, xboxLiveContextSettings, appConfig, rtaService](std::shared_ptr<http_call_response> response)
    {
        if (response->response_body_json().size() > 0)
        {
            auto jsonResult = xbox::services::tournaments::tournament_request_result::_Deserialize(response->response_body_json());
            auto result = utils::generate_xbox_live_result<tournament_request_result>(
                jsonResult,
                response
                );

            auto& tournamentResult = result.payload();
            tournamentResult._Init_next_page_info(
                userContext,
                xboxLiveContextSettings,
                appConfig,
                rtaService
                );

            return result;
        }
        else
        {
            return xbox_live_result<tournament_request_result>(response->err_code(), response->err_message());
        }
    });

    return utils::create_exception_free_task<tournament_request_result>(task);
}

pplx::task<xbox::services::xbox_live_result<tournament>>
tournament_service::get_tournament_details(
    _In_ const string_t& organizerId,
    _In_ const string_t& tournamentId
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(organizerId.empty(), tournament, "organizer id is empty");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(tournamentId.empty(), tournament, "tournament id is empty");

    stringstream_t subPath;
    subPath << _T("/tournaments/") << organizerId << _T("/") << tournamentId;
    
    std::shared_ptr<http_call> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("GET"),
        utils::create_xboxlive_endpoint(_T("tournamentshub"), m_appConfig),
        subPath.str(),
        xbox_live_api::get_tournament_details
        );
    httpCall->set_xbox_contract_version_header_value(c_tournamentsHubServiceContractHeaderValue);

    auto userContext = m_userContext;
    auto xboxLiveContextSettings = m_xboxLiveContextSettings;
    auto appConfig = m_appConfig;

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([userContext, xboxLiveContextSettings, appConfig](std::shared_ptr<http_call_response> response)
    {
        if (response->response_body_json().size() > 0)
        {
            auto jsonResult = tournament::_Deserialize(response->response_body_json(), web::json::value());
           return utils::generate_xbox_live_result<tournament>(
                jsonResult,
                response
                );
        }
        else
        {
            return xbox_live_result<tournament>(response->err_code(), response->err_message());
        }
    });

    return utils::create_exception_free_task<tournament>(task);
}

pplx::task<xbox::services::xbox_live_result<team_request_result>>
tournament_service::_Get_teams(
    _In_ const string_t& nextLinkUrl
    )
{
    return get_teams_internal(nextLinkUrl, _T(""));
}

pplx::task<xbox::services::xbox_live_result<team_request_result>>
tournament_service::get_teams(
    _In_ team_request request
    )
{
    auto subPath = team_sub_path_url(request);
    return get_teams_internal(
        utils::create_xboxlive_endpoint(_T("tournamentshub"), m_appConfig), 
        subPath
        );
}

pplx::task<xbox::services::xbox_live_result<team_request_result>> 
tournament_service::get_teams_internal(
    _In_ const string_t& serverName,
    _In_ const string_t& pathQueryFragment
    )
{
    std::shared_ptr<http_call> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("GET"),
        serverName,
        pathQueryFragment,
        xbox_live_api::get_teams
        );
    httpCall->set_xbox_contract_version_header_value(c_tournamentsHubServiceContractHeaderValue);

    auto userContext = m_userContext;
    auto xboxLiveContextSettings = m_xboxLiveContextSettings;
    auto appConfig = m_appConfig;
    auto rtaService = m_realTimeActivityService;

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([userContext, xboxLiveContextSettings, appConfig, rtaService](std::shared_ptr<http_call_response> response)
    {
        if (response->response_body_json().size() > 0)
        {
            auto jsonResult = team_request_result::_Deserialize(response->response_body_json());
            auto result = utils::generate_xbox_live_result<team_request_result>(
                jsonResult,
                response
                );

            auto& teamResult = result.payload();
            teamResult._Init_next_page_info(
                userContext,
                xboxLiveContextSettings,
                appConfig,
                rtaService
                );

            return result;
        }
        else
        {
            return xbox_live_result<team_request_result>(response->err_code(), response->err_message());
        }
    });

    return utils::create_exception_free_task<team_request_result>(task);
}

pplx::task<xbox::services::xbox_live_result<team_info>>
tournament_service::get_team_details(
    _In_ const string_t& organizerId,
    _In_ const string_t& tournamentId,
    _In_ const string_t& teamId
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(organizerId.empty(), team_info, "organizer id is empty");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(tournamentId.empty(), team_info, "tournament id is empty");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(teamId.empty(), team_info, "team id is empty");

    stringstream_t subPath;
    subPath << _T("/tournaments/") << organizerId << _T("/") << tournamentId << _T("/teams/") << teamId;

    std::shared_ptr<http_call> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("GET"),
        utils::create_xboxlive_endpoint(_T("tournamentshub"), m_appConfig),
        subPath.str(),
        xbox_live_api::get_tournament_details
    );
    httpCall->set_xbox_contract_version_header_value(c_tournamentsHubServiceContractHeaderValue);

    auto userContext = m_userContext;
    auto xboxLiveContextSettings = m_xboxLiveContextSettings;
    auto appConfig = m_appConfig;

    auto task = httpCall->get_response_with_auth(m_userContext)
        .then([userContext, xboxLiveContextSettings, appConfig](std::shared_ptr<http_call_response> response)
    {
        if (response->response_body_json().size() > 0)
        {
            auto jsonResult = team_info::_Deserialize(response->response_body_json());
            return utils::generate_xbox_live_result<team_info>(
                jsonResult,
                response
                );
        }
        else
        {
            return xbox_live_result<team_info>(response->err_code(), response->err_message());
        }
    });

    return utils::create_exception_free_task<team_info>(task);
}

string_t
tournament_service::tournament_sub_path_url(
    _In_ tournament_request request
    )
{
    web::uri_builder subPathBuilder;
    string_t xuid = utils::string_t_from_internal_string(m_userContext->xbox_user_id());

    stringstream_t path;
    path << _T("/tournaments");
    subPathBuilder.set_path(path.str());

    subPathBuilder.append_query(_T("titleId"), m_appConfig->title_id());
    subPathBuilder.append_query(_T("teamForMember"), xuid);
    if (request.filter_results_for_user())
    {
        subPathBuilder.append_query(_T("memberId"), xuid);
    }

    if (!request.organizer_id().empty())
    {
        subPathBuilder.append_query(_T("organizer"), request.organizer_id());
    }

    if (request.state_filter().size() > 0)
    {
        string_t statesArray;
        for (const auto& state : request.state_filter())
        {
            statesArray += convert_tournament_state_to_string(state);
            statesArray += _T(",");
        }
        statesArray.erase(statesArray.end() - 1, statesArray.end()); // remove the last ','
        subPathBuilder.append_query(_T("state"), statesArray);
    }

    if (request.sort_order() != tournament_sort_order::none)
    {
        subPathBuilder.append_query(_T("sortOrder"), convert_tournament_sort_order_to_string(request.sort_order()));
    }

    if (request.order_by() != tournament_order_by::none)
    {
        subPathBuilder.append_query(_T("orderBy"), convert_tournament_order_by_to_string(request.order_by()));
    }

    if (request.max_items() > 0)
    {
        subPathBuilder.append_query(_T("maxItems"), request.max_items());
    }

    return subPathBuilder.to_string();
}


string_t
tournament_service::team_sub_path_url(
    _In_ team_request request
    )
{
    web::uri_builder subPathBuilder;

    stringstream_t path;
    path << _T("/tournaments/") << request.organizer_id() << _T("/") << request.tournament_id() << _T("/teams");
    subPathBuilder.set_path(path.str());


    if (request.filter_results_for_user())
    {
        subPathBuilder.append_query(_T("memberId"), utils::string_t_from_internal_string(m_userContext->xbox_user_id()));
    }

    if (request.max_items() > 0)
    {
        subPathBuilder.append_query(_T("maxItems"), request.max_items());
    }

    if (request.state_filter().size() > 0)
    {
        string_t statesArray;
        for (const auto& state : request.state_filter())
        {
            statesArray += convert_team_state_to_string(state);
            statesArray += _T(",");
        }
        statesArray.erase(statesArray.end() - 1, statesArray.end()); // remove the last ','
        subPathBuilder.append_query(_T("state"), statesArray);
    }

    if (request.order_by() != team_order_by::none)
    {
        subPathBuilder.append_query(_T("orderBy"), convert_team_order_by_to_string(request.order_by()));
    }

    return subPathBuilder.to_string();
}

string_t
tournament_service::convert_tournament_state_to_string(
    _In_ tournament_state state
    )
{
    switch (state)
    {
        case tournament_state::active:
            return _T("Active");

        case tournament_state::canceled:
            return _T("Canceled");

        case tournament_state::completed:
            return _T("Completed");

        default:
            return _T("Unknown");
    }
}

string_t
tournament_service::convert_tournament_sort_order_to_string(
    _In_ tournament_sort_order order
    )
{
    switch (order)
    {
    case tournament_sort_order::ascending :
        return _T("asc");

    case tournament_sort_order::descending:
        return _T("desc");

    default:
        return _T("none");
    }
}

string_t
tournament_service::convert_tournament_order_by_to_string(
    _In_ tournament_order_by order
    )
{
    switch (order)
    {
    case tournament_order_by::start_time:
        return _T("playingStart");

    case tournament_order_by::end_time:
        return _T("endTime");

    default:
        return _T("none");
    }
}

string_t
tournament_service::convert_team_state_to_string(
    _In_ team_state state
)
{
    switch (state)
    {
    case team_state::registered:
        return _T("Registered");

    case team_state::waitlisted:
        return _T("Waitlisted");

    case team_state::stand_by:
        return _T("Standby");

    case team_state::checked_in:
        return _T("CheckedIn");

    case team_state::playing:
        return _T("Playing");

    case team_state::completed:
        return _T("Completed");

    default:
        return _T("Unknown");
    }
}

string_t
tournament_service::convert_team_order_by_to_string(
    _In_ team_order_by order
)
{
    switch (order)
    {
    case team_order_by::name:
        return _T("name");

    case team_order_by::ranking:
        return _T("ranking");

    default:
        return _T("none");
    }
}

xbox::services::tournaments::team_state
tournament_service::_Convert_string_to_team_state(
    _In_ const string_t& value
    )
{
    if (utils::str_icmp(value, _T("registered")) == 0)
    {
        return xbox::services::tournaments::team_state::registered;
    }
    else if (utils::str_icmp(value, _T("waitlisted")) == 0)
    {
        return xbox::services::tournaments::team_state::waitlisted;
    }
    else if (utils::str_icmp(value, _T("standby")) == 0)
    {
        return xbox::services::tournaments::team_state::stand_by;
    }
    else if (utils::str_icmp(value, _T("checkedIn")) == 0)
    {
        return xbox::services::tournaments::team_state::checked_in;
    }
    else if (utils::str_icmp(value, _T("playing")) == 0)
    {
        return xbox::services::tournaments::team_state::playing;
    }
    else if (utils::str_icmp(value, _T("completed")) == 0)
    {
        return xbox::services::tournaments::team_state::completed;
    }

    return xbox::services::tournaments::team_state::unknown;
}

function_context
tournament_service::add_tournament_changed_handler(
    _In_ std::function<void(tournament_change_event_args)> handler
    )
{
    return m_tournamentServiceImpl->add_tournament_changed_handler(
        std::move(handler)
    );
}

void
tournament_service::remove_tournament_changed_handler(
    _In_ function_context context
    )
{
    return m_tournamentServiceImpl->remove_tournament_changed_handler(
        context
    );
}

xbox_live_result<std::shared_ptr<tournament_change_subscription>>
tournament_service::subscribe_to_tournament_change(
    _In_ const string_t& organizerId, 
    _In_ const string_t& tournamentId
    )
{
    return m_tournamentServiceImpl->subscribe_to_tournament_change(
        organizerId,
        tournamentId
        );
}

xbox_live_result<void>
tournament_service::unsubscribe_from_tournament_change(
    _In_ std::shared_ptr<tournament_change_subscription> subscription
    )
{
    return m_tournamentServiceImpl->unsubscribe_from_tournament_change(subscription);
}

function_context
tournament_service::add_team_changed_handler(
    _In_ std::function<void(team_change_event_args)> handler
    )
{
    return m_tournamentServiceImpl->add_team_changed_handler(
        std::move(handler)
    );
}

void
tournament_service::remove_team_changed_handler(
    _In_ function_context context
    )
{
    return m_tournamentServiceImpl->remove_team_changed_handler(
        context
    );
}

xbox_live_result<std::shared_ptr<team_change_subscription>>
tournament_service::subscribe_to_team_change(
    _In_ const string_t& organizerId,
    _In_ const string_t& tournamentId,
    _In_ const string_t& teamId
    )
{
    return m_tournamentServiceImpl->subscribe_to_team_change(
        organizerId,
        tournamentId,
        teamId
    );
}

xbox_live_result<void>
tournament_service::unsubscribe_from_team_change(
    _In_ std::shared_ptr<team_change_subscription> subscription
    )
{
    return m_tournamentServiceImpl->unsubscribe_from_team_change(subscription);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_CPP_END
