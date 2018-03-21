// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/stats_manager.h"
#include "stats_manager_internal.h"
#include "http_call_impl.h"
#include "user_context.h"
#include "xbox_system_factory.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_STAT_MANAGER_CPP_BEGIN

simplified_stats_service::simplified_stats_service()
{
}

simplified_stats_service::simplified_stats_service(
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
    ) : 
    m_userContext(std::move(userContext)),
    m_xboxLiveContextSettings(std::move(xboxLiveContextSettings)),
    m_appConfig(std::move(appConfig))
{
}

pplx::task<xbox_live_result<void>>
simplified_stats_service::update_stats_value_document(
    _In_ stats_value_document& statsDocToPost
    )
{
    statsDocToPost.set_revision_from_clock();

    string_t pathAndQuery = pathandquery_simplified_stats_subpath(
        utils::string_t_from_internal_string(m_userContext->xbox_user_id()),
        m_appConfig->scid(),
        false
        );

    std::shared_ptr<http_call> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("POST"),
        utils::create_xboxlive_endpoint(_T("statswrite"), m_appConfig),
        pathAndQuery,
        xbox_live_api::update_stats_value_document
        );

    httpCall->set_request_body(statsDocToPost.serialize());

    auto task = httpCall->get_response_with_auth(m_userContext, http_call_response_body_type::json_body)
    .then([](std::shared_ptr<http_call_response> response)
    {
        return xbox_live_result<void>(response->err_code(), response->err_message());
    });

    return utils::create_exception_free_task<void>(
        task
        );
}

pplx::task<xbox_live_result<stats_value_document>>
simplified_stats_service::get_stats_value_document()
{
    string_t pathAndQuery = pathandquery_simplified_stats_subpath(
        utils::string_t_from_internal_string(m_userContext->xbox_user_id()),
        m_appConfig->scid(),
        false
        );

    std::shared_ptr<http_call> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("GET"),
        utils::create_xboxlive_endpoint(_T("statsread"), m_appConfig),
        pathAndQuery,
        xbox_live_api::get_stats_value_document
        );

    auto task = httpCall->get_response_with_auth(m_userContext, http_call_response_body_type::json_body)
    .then([](std::shared_ptr<http_call_response> response)
    {
        auto result = stats_value_document::_Deserialize(response->response_body_json());
        return utils::generate_xbox_live_result<stats_value_document>(
            result,
            response
            );
    });

    return utils::create_exception_free_task<stats_value_document>(
        task
        );
}

string_t
simplified_stats_service::pathandquery_simplified_stats_subpath(
    const string_t& xboxUserId,
    const string_t& serviceConfigurationId,
    bool useXuidTag
    ) const
{
    stringstream_t source;
    source << _T("/stats/users/");
    if (useXuidTag)
    {
        source << _T("xuid(");
    }
    source << xboxUserId;
    if (useXuidTag)
    {
        source << _T(")");
    }
    source << _T("/scids/");
    source << serviceConfigurationId;

    return source.str();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_STAT_MANAGER_CPP_END