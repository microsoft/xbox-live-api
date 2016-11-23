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
#include "xsapi/contextual_search_service.h"
#include "contextual_config_result.h"
#include "xbox_system_factory.h"

using namespace pplx;
using namespace xbox::services;
using namespace xbox::services::system;

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_CPP_BEGIN

contextual_search_service::contextual_search_service(
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> settings,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
    ) : 
    m_userContext(std::move(userContext)),
    m_settings(std::move(settings)),
    m_appConfig(std::move(appConfig))
{
}

pplx::task<xbox_live_result<std::vector<contextual_search_configured_stat>>>
contextual_search_service::get_configuration(
    _In_ uint32_t titleId
    )
{
    stringstream_t pathAndQuery;
    pathAndQuery << _T("/titles/");
    pathAndQuery << titleId;
    pathAndQuery << _T("/configuration");

    std::shared_ptr<http_call> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_settings,
        _T("GET"),
        utils::create_xboxlive_endpoint(_T("contextualsearch"), m_appConfig),
        pathAndQuery.str(),
        xbox_live_api::get_configuration
        );

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([](std::shared_ptr<http_call_response> response)
    {
        return utils::generate_xbox_live_result<std::vector<contextual_search_configured_stat>>(
            contextual_search_service::deserialize_configuration_result(response->response_body_json()),
            response
            );
    });

    return utils::create_exception_free_task<std::vector<contextual_search_configured_stat>>(
        task
        );
}

xbox_live_result<std::vector<contextual_search_configured_stat>>
contextual_search_service::deserialize_configuration_result(_In_ const web::json::value& inputJson)
{
    std::error_code errCode = xbox_live_error_code::no_error;
    auto internalSet = utils::extract_json_vector<contextual_config_result>(
        contextual_config_result::_Deserialize,
        inputJson, 
        _T("serviceconfigs"),
        errCode,
        true);
    if (internalSet.size() == 0)
    {
        return std::vector<contextual_search_configured_stat>();
    }

    auto& firstItem = internalSet[0]; // Only grabbing first item in array since there should only be one when calling this endpoint
    return firstItem.get_contextual_search_configured_stats();
}

pplx::task<xbox_live_result<std::vector<contextual_search_broadcast>>> 
contextual_search_service::get_broadcasts(
    _In_ uint32_t titleId
    )
{
    return get_broadcasts(
        titleId,
        0,
        0,
        string_t(),
        false,
        string_t());
}

pplx::task<xbox_live_result<std::vector<contextual_search_broadcast>>>
contextual_search_service::get_broadcasts(
    _In_ uint32_t titleId,
    _In_ uint32_t skipItems,
    _In_ uint32_t maxItems,
    _In_ const string_t& orderByStatName,
    _In_ bool orderAscending,
    _In_ const string_t& searchQuery
    )
{
    auto pathAndQuery = create_broadcasts_url(titleId, skipItems, maxItems, orderByStatName, orderAscending, searchQuery);
    RETURN_TASK_CPP_IF_ERR(pathAndQuery, std::vector<contextual_search_broadcast>);

    std::shared_ptr<http_call> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_settings,
        _T("GET"),
        utils::create_xboxlive_endpoint(_T("contextualsearch"), m_appConfig),
        pathAndQuery.payload(),
        xbox_live_api::get_broadcasts
        );

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([](std::shared_ptr<http_call_response> response)
    {
        return utils::generate_xbox_live_result<std::vector<contextual_search_broadcast>>(
            contextual_search_service::deserialize_broadcasts_result(response->response_body_json()),
            response
            );
    });

    return utils::create_exception_free_task<std::vector<contextual_search_broadcast>>(
        task
        );
}

string_t contextual_search_service::convert_operator_to_string(
    _In_ contextual_search_filter_operator filterOperator
    )
{
    switch (filterOperator)
    {
        case contextual_search_filter_operator::greater_than: return _T("gt");
        case contextual_search_filter_operator::greater_than_or_equal: return _T("ge");
        case contextual_search_filter_operator::less_than: return _T("lt");
        case contextual_search_filter_operator::less_than_or_equal: return _T("le");
        case contextual_search_filter_operator::not_equal: return _T("ne");
        case contextual_search_filter_operator::equal: 
        default: return _T("eq");
    }
}

pplx::task<xbox_live_result<std::vector<contextual_search_broadcast>>> 
contextual_search_service::get_broadcasts(
    _In_ uint32_t titleId,
    _In_ uint32_t skipItems,
    _In_ uint32_t maxItems,
    _In_ const string_t& orderByStatName,
    _In_ bool orderAscending,
    _In_ const string_t& filterStatName,
    _In_ contextual_search_filter_operator filterOperator,
    _In_ const string_t& filterStatValue
    )
{
    stringstream_t filter;
    if( !filterStatName.empty() && !filterStatValue.empty() )
    {
        filter << _T("stats:");
        filter << filterStatName;
        filter << _T(" ");
        filter << convert_operator_to_string(filterOperator);
        filter << _T(" '");
        filter << utils::replace_sub_string(filterStatValue, _T("'"), _T("''"));
        filter << _T("'");
    }

    return get_broadcasts(titleId, skipItems, maxItems, orderByStatName, orderAscending, filter.str());
}

pplx::task<xbox_live_result<contextual_search_game_clips_result>>
contextual_search_service::get_game_clips_internal(
    _In_ uint32_t titleId,
    _In_ uint32_t skipItems,
    _In_ uint32_t maxItems,
    _In_ const string_t& orderByStatName,
    _In_ bool orderAscending,
    _In_ const string_t& searchQuery
    )
{
    auto pathAndQuery = create_game_clips_url(titleId, skipItems, maxItems, orderByStatName, orderAscending, searchQuery);
    RETURN_TASK_CPP_IF_ERR(pathAndQuery, contextual_search_game_clips_result);

    std::shared_ptr<http_call> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_settings,
        _T("GET"),
        utils::create_xboxlive_endpoint(_T("contextualsearch"), m_appConfig),
        pathAndQuery.payload(),
        xbox_live_api::get_game_clips
        );

    auto userContext = m_userContext;
    auto xboxLiveContextSettings = m_settings;
    auto appConfig = m_appConfig;

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([userContext, xboxLiveContextSettings, appConfig, skipItems, titleId](std::shared_ptr<http_call_response> response)
    {
        auto gameClipsResult = contextual_search_game_clips_result::_Deserialize(response->response_body_json());
        auto& result = gameClipsResult.payload();

        uint32_t itemSize = static_cast<uint32_t>(result.items().size());
        uint32_t continuationSkip = skipItems + itemSize;

        // Initialize the request params for get_next()
        result._Initialize(
            userContext,
            xboxLiveContextSettings,
            appConfig,
            titleId,
            continuationSkip
            );

        return utils::generate_xbox_live_result<contextual_search_game_clips_result>(
            result,
            response
            );
    });

    return utils::create_exception_free_task<contextual_search_game_clips_result>(
        task
        );
}

pplx::task<xbox_live_result<contextual_search_game_clips_result>> 
contextual_search_service::get_game_clips(
    _In_ uint32_t titleId,
    _In_ uint32_t skipItems,
    _In_ uint32_t maxItems,
    _In_ const string_t& orderByStatName,
    _In_ bool orderAscending,
    _In_ const string_t& searchQuery
    )
{
    return get_game_clips_internal(
        titleId,
        skipItems,
        maxItems,
        orderByStatName,
        orderAscending,
        searchQuery
        );
}


pplx::task<xbox_live_result<contextual_search_game_clips_result>> 
contextual_search_service::get_game_clips(
    _In_ uint32_t titleId,
    _In_ uint32_t skipItems,
    _In_ uint32_t maxItems,
    _In_ const string_t& orderByStatName,
    _In_ bool orderAscending,
    _In_ const string_t& filterStatName,
    _In_ contextual_search_filter_operator filterOperator,
    _In_ const string_t& filterStatValue
    )
{
    stringstream_t filter;
    if (!filterStatName.empty() && !filterStatValue.empty())
    {
        filter << _T("stats:");
        filter << filterStatName;
        filter << _T(" ");
        filter << convert_operator_to_string(filterOperator);
        filter << _T(" '");
        filter << utils::replace_sub_string(filterStatValue, _T("'"), _T("''"));
        filter << _T("'");
    }

    return get_game_clips_internal(
        titleId,
        skipItems,
        maxItems,
        orderByStatName,
        orderAscending,
        filter.str()
        );
}

xbox_live_result<std::vector<contextual_search_broadcast>>
contextual_search_service::deserialize_broadcasts_result(_In_ const web::json::value& inputJson)
{
    if (inputJson.is_null()) return xbox_live_result<std::vector<contextual_search_broadcast>>();

    std::error_code errc = xbox_live_error_code::no_error;
    auto result = utils::extract_json_vector<contextual_search_broadcast>(
        contextual_search_broadcast::_Deserialize,
        inputJson,
        _T("value"),
        errc,
        true);

    return xbox_live_result<std::vector<contextual_search_broadcast>>(result, errc);
}


xbox_live_result<string_t>
contextual_search_service::create_broadcasts_url(
    _In_ uint32_t titleId,
    _In_ uint32_t skipItems,
    _In_ uint32_t maxItems,
    _In_ const string_t& orderByStatName,
    _In_ bool orderAscending,
    _In_ const string_t& filter
    )
{
    web::uri_builder builder;

    stringstream_t path;
    path << _T("/titles/");
    path << titleId;
    path << _T("/broadcasts");
    builder.set_path(path.str());

    if (skipItems > 0)
    {
        builder.append_query(_T("$skip"), skipItems);
    }

    if (maxItems > 0)
    {
        builder.append_query(_T("$top"), maxItems);
    }

    if (!orderByStatName.empty())
    {
        stringstream_t queryValue;
        queryValue << _T("stats:");
        queryValue << orderByStatName;
        queryValue << _T(" ");
        queryValue << (orderAscending ? _T("asc") : _T("desc"));
        builder.append_query(_T("$orderby"), queryValue.str());
    }

    if (!filter.empty())
    {
        builder.append_query(_T("$filter"), filter);
    }

    return xbox_live_result<string_t>(builder.to_string());
}

xbox_live_result<string_t>
contextual_search_service::create_game_clips_url(
    _In_ uint32_t titleId,
    _In_ uint32_t skipItems,
    _In_ uint32_t maxItems,
    _In_ const string_t& orderByStatName,
    _In_ bool orderAscending,
    _In_ const string_t& searchQuery
    )
{
    web::uri_builder builder;

    stringstream_t path;
    path << _T("/titles/");
    path << titleId;
    path << _T("/gameclips");
    builder.set_path(path.str());

    if (maxItems > 0)
    {
        builder.append_query(_T("$top"), maxItems);
    }

    if (skipItems > 0)
    {
        builder.append_query(_T("$skip"), skipItems);
    }

    if (!orderByStatName.empty())
    {
        stringstream_t queryValue;
        queryValue << _T("stats:");
        queryValue << orderByStatName;
        queryValue << _T(" ");
        queryValue << (orderAscending ? _T("asc") : _T("desc"));
        builder.append_query(_T("$orderby"), queryValue.str());
    }

    if (!searchQuery.empty())
    {
        builder.append_query(_T("$filter"), searchQuery);
    }

    return xbox_live_result<string_t>(builder.to_string());
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_CPP_END
