// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/game_server_platform.h"
#include "xbox_system_factory.h"
#include "utils.h"
#include "user_context.h"

using namespace pplx;

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_CPP_BEGIN

game_server_platform_service::game_server_platform_service(
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
    ) :
    m_userContext(std::move(userContext)),
    m_xboxLiveContextSettings(std::move(xboxLiveContextSettings)),
    m_appConfig(std::move(appConfig))
{
}

pplx::task<xbox::services::xbox_live_result<cluster_result>>
game_server_platform_service::allocate_cluster(
    _In_ uint32_t gameServerTitleId,
    _In_ const string_t& serviceConfigurationId,
    _In_ const string_t& sessionTemplateName,
    _In_ const string_t& sessionName,
    _In_ bool abortIfQueued
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(serviceConfigurationId, cluster_result, "serviceConfigurationId is required");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(sessionTemplateName, cluster_result, "sessionTemplateName is required");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(sessionName, cluster_result, "sessionName is required");

    auto pathAndQuery = pathandquery_game_server_create_cluster_subpath(
        gameServerTitleId,
        false
        );
    
    std::shared_ptr<http_call> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("POST"),
        utils::create_xboxlive_endpoint(_T("gameserverms"), m_appConfig),
        pathAndQuery,
        xbox_live_api::allocate_cluster
        );

    httpCall->set_retry_allowed(false);

    // Set request body to something like:
    // {
    //        "sessionId" : "/serviceconfigs/{scid}/sessionTemplates/{sessionTemplateName}/sessions/{sessionName}"
    //        "abortIfQueued" : false
    // }
    web::json::value request;
    request[_T("sessionId")] = web::json::value::string(game_server_session_id_path(serviceConfigurationId, sessionTemplateName, sessionName));
    if (abortIfQueued)
    {
        request[_T("abortIfQueued")] = web::json::value::boolean(abortIfQueued);
    }
    httpCall->set_request_body(request.serialize());

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([](std::shared_ptr<http_call_response> response)
    {
        return utils::generate_xbox_live_result<cluster_result>(
            cluster_result::_Deserialize(response->response_body_json()),
            response
            );
    });

    return utils::create_exception_free_task<cluster_result>(
        task
        );
}

pplx::task<xbox::services::xbox_live_result<cluster_result>>
game_server_platform_service::allocate_cluster_inline(
    _In_ uint32_t gameServerTitleId,
    _In_ const string_t& serviceConfigurationId,
    _In_ const string_t& sandboxId,
    _In_ const string_t& ticketId,
    _In_ const string_t& gsiSetId,
    _In_ const string_t& gameVariantId,
    _In_ uint64_t maxAllowedPlayers,
    _In_ const string_t& location,
    _In_ bool abortIfQueued
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(serviceConfigurationId, cluster_result, "serviceConfigurationId is required");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(sandboxId, cluster_result, "sandboxId is required");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(ticketId, cluster_result, "ticketId is required");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(gsiSetId, cluster_result, "gsiSetId is required");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(gameVariantId, cluster_result, "gameVariantId is required");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(location, cluster_result, "location is required");

    string_t pathAndQuery = pathandquery_game_server_create_cluster_subpath(
        gameServerTitleId,
        true
        );
    
    std::shared_ptr<http_call> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("POST"),
        utils::create_xboxlive_endpoint(_T("gameserverms"), m_appConfig),
        pathAndQuery,
        xbox_live_api::allocate_cluster_inline
        );
    httpCall->set_retry_allowed(false);
    httpCall->set_xbox_contract_version_header_value(_T("2"));

    // Set request body to something like:
    //  {
    //    "serviceConfigId": "88222fc5-b30c-43f2-ab0f-834e9b2266c4",
    //    "sandboxId": "MSCP.0",
    //    "ticketId": "6057ff49-28ff-4156-95dc-a733e9bfa8ef",
    //    "gsiSetId": "a0aaa8e0-0866-45ca-908b-0274b889b099",
    //    "gameVariantId": "905b665a-fe2f-4170-acd5-fd44118c15a0",
    //    "maxAllowedPlayers": 8,
    //    "location": "west us",
    //    "abortIfQueued": true
    //  }

    web::json::value request;
    request[_T("serviceConfigId")] = web::json::value::string(serviceConfigurationId);
    request[_T("sandboxId")] = web::json::value::string(sandboxId);
    request[_T("ticketId")] = web::json::value::string(ticketId);
    request[_T("gsiSetId")] = web::json::value::string(gsiSetId);
    request[_T("gameVariantId")] = web::json::value::string(gameVariantId);
    request[_T("maxAllowedPlayers")] = web::json::value::number(static_cast<double>(maxAllowedPlayers));
    request[_T("location")] = web::json::value::string(location);
    if (abortIfQueued)
    {
        request[_T("abortIfQueued")] = web::json::value::boolean(abortIfQueued);
    }
    httpCall->set_request_body(request.serialize());

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([](std::shared_ptr<http_call_response> response)
    {
        return utils::generate_xbox_live_result<cluster_result>(
            cluster_result::_Deserialize(response->response_body_json()),
            response
            );
    });

    return utils::create_exception_free_task<cluster_result>(
        task
        );
}
pplx::task<xbox::services::xbox_live_result<game_server_ticket_status>>
game_server_platform_service::get_ticket_status(
    _In_ uint32_t gameServerTitleId,
    _In_ const string_t& ticketId
    )
{
    if (ticketId.empty()) { return pplx::task_from_result(xbox_live_result<game_server_ticket_status>(xbox_live_error_code::invalid_argument, "ticketId is required")); }

    string_t pathAndQuery = pathandquery_game_server_ticket_status_subpath(
        gameServerTitleId,
        ticketId
        );
    
    std::shared_ptr<http_call> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("GET"),
        utils::create_xboxlive_endpoint(_T("gameserverms"), m_appConfig),
        pathAndQuery,
        xbox_live_api::get_ticket_status
        );
    httpCall->set_xbox_contract_version_header_value(_T("6"));

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([](std::shared_ptr<http_call_response> response)
    {
        return utils::generate_xbox_live_result<game_server_ticket_status>(
            game_server_ticket_status::_Deserialize(response->response_body_json()),
            response
            );
    });

    return utils::create_exception_free_task<game_server_ticket_status>(
        task
        );
}

pplx::task<xbox::services::xbox_live_result<game_server_metadata_result>>
game_server_platform_service::get_game_server_metadata(
    _In_ uint32_t titleId,
    _In_ uint32_t maxAllowedPlayers,
    _In_ bool publisherOnly,
    _In_ uint32_t maxVariants,
    _In_ const std::map<string_t, string_t>& filterTags
    )
{
    string_t pathAndQuery = pathandquery_game_server_gamevariant_list_subpath(
        titleId
        );
    
    std::shared_ptr<http_call> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("POST"),
        utils::create_xboxlive_endpoint(_T("gameserverds"), m_appConfig),
        pathAndQuery,
        xbox_live_api::get_game_server_metadata
        );

    // Set request body to something like:
    // {
    //        "maxAllowedPlayers": 12,
    //        "publisherOnly" : false,
    //        "maxVariants" : 100,
    //        "locale" : "en-us",
    //        "tags": [ { "name": "tagName1", "value": "tagValue1" }, { "name": "tagName2", "value": "tagValue2" } ]
    // }

    web::json::value tagsJson;
    int i = 0;
    for(const auto& tag : filterTags)
    {
        web::json::value tagJson;
        tagJson[_T("name")] = web::json::value::string(tag.first);
        tagJson[_T("value")] = web::json::value::string(tag.second);

        tagsJson[i++] = tagJson;
    }

    web::json::value request;
    request[_T("maxAllowedPlayers")] = web::json::value::number(static_cast<double>(maxAllowedPlayers));
    request[_T("publisherOnly")] = web::json::value::boolean(publisherOnly);
    request[_T("maxVariants")] = web::json::value::number(static_cast<double>(maxVariants));
    request[_T("locale")] = web::json::value::string(_T("en-us"));
    request[_T("tags")] = tagsJson;
    httpCall->set_request_body(request.serialize());

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([](std::shared_ptr<http_call_response> response)
    {
        return utils::generate_xbox_live_result<game_server_metadata_result>(
            game_server_metadata_result::_Deserialize(response->response_body_json()),
            response
            );
    });

    return utils::create_exception_free_task<game_server_metadata_result>(
        task
        );
}

pplx::task<xbox::services::xbox_live_result<std::vector<quality_of_service_server>>>
game_server_platform_service::get_quality_of_service_servers()
{
    const string_t path = m_xboxLiveContextSettings->use_crossplatform_qos_servers() ? _T("/xplatqosservers") : _T("/qosservers");

    std::shared_ptr<http_call> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("GET"),
        utils::create_xboxlive_endpoint(_T("gameserverds"), m_appConfig),
        path,
        xbox_live_api::get_quality_of_service_servers
        );

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([](std::shared_ptr<http_call_response> response)
    {
        std::error_code errc = xbox_live_error_code::no_error;

        auto qosServer = utils::extract_xbox_live_result_json_vector<quality_of_service_server>(
            quality_of_service_server::_Deserialize,
            response->response_body_json(),
            _T("qosServers"),
            errc,
            true
            );

        return utils::generate_xbox_live_result<std::vector<quality_of_service_server>>(
            qosServer,
            response
            );
    });

    return utils::create_exception_free_task<std::vector<quality_of_service_server>>(
        task
        );
}

pplx::task<xbox::services::xbox_live_result<allocation_result>>
game_server_platform_service::allocate_session_host(
    _In_ uint32_t gameServerTitleId,
    _In_ const std::vector<string_t>& locations,
    _In_ const string_t& sessionId,
    _In_ const string_t& cloudGameId,
    _In_opt_ const string_t& gameModeId,
    _In_opt_ const string_t& sessionCookie
    )
{        
    string_t pathAndQuery = pathandquery_game_server_allocate_session_host_subpath(
        gameServerTitleId
        );

    std::shared_ptr<http_call> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("POST"),
        utils::create_xboxlive_endpoint(_T("gameserverms"), m_appConfig),
        pathAndQuery,
        xbox_live_api::allocate_session_host
        );

    httpCall->set_retry_allowed(false);
    httpCall->set_xbox_contract_version_header_value(_T("8"));

    //// Set request body to something like:
    //{
    //    "sessionId": "3536d3e6-e85d-4f47-b898-9617d19dabcd",
    //    "cloudGameId" : "1b7f9925-369c-4301-b1f7-1125dce25776",
    //    "locations" : 
    //    [
    //        "West US",
    //        "East US",
    //        "West Europe"
    //    ],
    //    "sessionCookie" : "Caller provided opaque string",
    //    "gameModeId" : "2162d32c-7ac8-40e9-9b1f-56676b8b2513"
    //}

    web::json::value request;
    request[_T("sessionId")] = web::json::value::string(sessionId);
    request[_T("cloudGameId")] = web::json::value::string(cloudGameId);
    request[_T("locations")] = utils::serialize_vector<string_t>(utils::json_string_serializer, locations);
    if (!sessionCookie.empty())
    {
        request[_T("sessionCookie")] = web::json::value::string(sessionCookie);
    }
    if (!gameModeId.empty())
    {
        request[_T("gameModeId")] = web::json::value::string(gameModeId);
    }
    httpCall->set_request_body(request.serialize());

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([](std::shared_ptr<http_call_response> response)
    {
        return utils::generate_xbox_live_result<allocation_result>(
            allocation_result::_Deserialize(response->response_body_json()),
            response
            );
    });

    return utils::create_exception_free_task<allocation_result>(
        task
        );
}

pplx::task<xbox::services::xbox_live_result<allocation_result>> 
game_server_platform_service::get_session_host_allocation_status(
    _In_ uint32_t gameServerTitleId,
    _In_ const string_t& sessionId
    )
{
    string_t pathAndQuery = pathandquery_game_server_allocation_status_subpath(
        gameServerTitleId,
        sessionId
        );

    std::shared_ptr<http_call> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("POST"),
        utils::create_xboxlive_endpoint(_T("gameserverms"), m_appConfig),
        pathAndQuery,
        xbox_live_api::get_session_host_allocation_status
        );

    httpCall->set_xbox_contract_version_header_value(_T("8"));
    
    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([](std::shared_ptr<http_call_response> response)
    {
        return utils::generate_xbox_live_result<allocation_result>(
            allocation_result::_Deserialize(response->response_body_json()),
            response
            );
    });

    return utils::create_exception_free_task<allocation_result>(
        task
        );
}


string_t 
game_server_platform_service::pathandquery_game_server_create_cluster_subpath(
    _In_ uint32_t gameServerTitleId,
    _In_ bool inlineAlloc
    )
{
    stringstream_t source;
    source << _T("/titles/");
    source << gameServerTitleId;
    if (inlineAlloc)
    {
        source << _T("/inlineclusters");
    }
    else
    {
        source << _T("/clusters");
    }

    return source.str();
}

string_t 
game_server_platform_service::pathandquery_game_server_allocate_session_host_subpath(
    _In_ uint32_t titleId
    )
{
    stringstream_t source;
    source << _T("/titles/");
    source << titleId;
    source << _T("/sessionhosts");

    return source.str();
}

string_t 
game_server_platform_service::pathandquery_game_server_allocation_status_subpath(
    _In_ uint32_t titleId,
    _In_ const string_t& sessionId
    )
{
    stringstream_t source;
    source << _T("/titles/");
    source << titleId;
    source << _T("/sessions");
    source << sessionId;
    source << _T("/allocationStatus");

    return source.str();
}

string_t 
game_server_platform_service::pathandquery_game_server_gamevariant_list_subpath(
    _In_ uint32_t titleId
    )
{
    stringstream_t source;
    source << _T("/titles/");
    source << titleId;
    source << _T("/variants");

    return source.str();
}

string_t 
game_server_platform_service::game_server_session_id_path(
    _In_ const string_t& serviceConfigurationId,
    _In_ const string_t& sessionTemplateName,
    _In_ const string_t& sessionName
    )
{
    stringstream_t source;

    source << _T("/serviceconfigs/");
    source << serviceConfigurationId;
    source << _T("/sessionTemplates/");
    source << sessionTemplateName;
    source << _T("/sessions/");
    source << sessionName;

    return source.str();
}

string_t
game_server_platform_service::pathandquery_game_server_ticket_status_subpath(
    _In_ uint32_t titleId,
    _In_ const string_t& ticketId
    )
{
    stringstream_t source;

    source << _T("/titles/");
    source << titleId;
    source << _T("/tickets/");
    source << ticketId;
    source << _T("/status");

    return source.str();
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_CPP_END
