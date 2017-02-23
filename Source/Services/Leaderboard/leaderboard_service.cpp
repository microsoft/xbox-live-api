// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "shared_macros.h"
#include "xbox_system_factory.h"
#include "leaderboard_serializers.h"
#include "leaderboard_query.h"
#include "xsapi/leaderboard.h"
#include "utils.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_BEGIN

const string_t c_leaderboard_social_stat_name_contract_version = _T("1");
const string_t c_leaderboard_with_metadata_contract_version = _T("3");

leaderboard_service::leaderboard_service(
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
    ) :
    m_userContext(std::move(userContext)),
    m_xboxLiveContextSettings(std::move(xboxLiveContextSettings)),
    m_appConfig(std::move(appConfig))
{
}

pplx::task<xbox_live_result<leaderboard_result>> leaderboard_service::get_leaderboard(
    _In_ const string_t& scid,
    _In_ const string_t& name,
    _In_ const std::vector<string_t>& additionalColumnNames
    )
{
    return get_leaderboard_internal(
        scid,
        name,
        NO_SKIP_RANK,
        NO_SKIP_XUID,
        NO_XUID,
        NO_SOCIAL_GROUP,
        NO_MAX_ITEMS,
        NO_CONTINUATION,
        additionalColumnNames);
}

pplx::task<xbox_live_result<leaderboard_result>> leaderboard_service::get_leaderboard(
    _In_ const string_t& scid,
    _In_ const string_t& name,
    _In_ const string_t& xuid,
    _In_ const string_t& socialGroup,
    _In_ uint32_t maxItems,
    _In_ const std::vector<string_t>& additionalColumnNames
    )
{
    return get_leaderboard_internal(
        scid,
        name,
        NO_SKIP_RANK,
        NO_SKIP_XUID,
        xuid,
        socialGroup,
        maxItems,
        NO_CONTINUATION,
        additionalColumnNames);
}

pplx::task<xbox_live_result<leaderboard_result>> leaderboard_service::get_leaderboard(
    _In_ const string_t& scid,
    _In_ const string_t& name,
    _In_ uint32_t skipToRank,
    _In_ uint32_t maxItems,
    _In_ const std::vector<string_t>& additionalColumnNames
    )
{
    return get_leaderboard_internal(
        scid,
        name,
        skipToRank,
        NO_SKIP_XUID,
        NO_XUID,
        NO_SOCIAL_GROUP,
        maxItems,
        NO_CONTINUATION,
        additionalColumnNames);
}


pplx::task<xbox_live_result<leaderboard_result>> leaderboard_service::get_leaderboard(
    _In_ const string_t& scid,
    _In_ const string_t& name,
    _In_ uint32_t skipToRank,
    _In_ const string_t& xuid,
    _In_ const string_t& socialGroup,
    _In_ uint32_t maxItems,
    _In_ const std::vector<string_t>& additionalColumnNames
    )
{
    return get_leaderboard_internal(
        scid,
        name,
        skipToRank,
        NO_SKIP_XUID,
        xuid,
        socialGroup,
        maxItems,
        NO_CONTINUATION,
        additionalColumnNames);
}

pplx::task<xbox_live_result<leaderboard_result>> leaderboard_service::get_leaderboard_skip_to_xuid(
    _In_ const string_t& scid,
    _In_ const string_t& name,
    _In_ const string_t& skipToXuid,
    _In_ uint32_t maxItems,
    _In_ const std::vector<string_t>& additionalColumnNames
    )
{
    return get_leaderboard_internal(
        scid,
        name,
        NO_SKIP_RANK,
        skipToXuid,
        NO_XUID,
        NO_SOCIAL_GROUP,
        maxItems,
        NO_CONTINUATION,
        additionalColumnNames);
}

_XSAPIIMP pplx::task<xbox_live_result<leaderboard_result>> leaderboard_service::get_leaderboard_skip_to_xuid(
    _In_ const string_t& scid,
    _In_ const string_t& name,
    _In_ const string_t& xuid,
    _In_ const string_t& socialGroup,
    _In_ const string_t& skipToXuid,
    _In_ uint32_t maxItems,
    _In_ const std::vector<string_t>& additionalColumnNames
    )
{
    return get_leaderboard_internal(
        scid,
        name,
        NO_SKIP_RANK,
        skipToXuid,
        xuid,
        socialGroup,
        maxItems,
        NO_CONTINUATION,
        additionalColumnNames);
}

pplx::task<xbox_live_result<leaderboard_result>> leaderboard_service::get_leaderboard_for_social_group(
    _In_ const string_t& xuid,
    _In_ const string_t& scid,
    _In_ const string_t& statName,
    _In_ const string_t& socialGroup,
    _In_ uint32_t maxItems
    )
{
    return get_leaderboard_for_social_group_internal(
        xuid,
        scid,
        statName,
        socialGroup,
        NO_SKIP_RANK,
        NO_SKIP_XUID,
        NO_SORT_ORDER,
        maxItems,
        NO_CONTINUATION);
}

pplx::task<xbox_live_result<leaderboard_result>> leaderboard_service::get_leaderboard_for_social_group(
    _In_ const string_t& xuid,
    _In_ const string_t& scid,
    _In_ const string_t& statName,
    _In_ const string_t& socialGroup,
    _In_ const string_t& sortOrder,
    _In_ uint32_t maxItems
    )
{
    return get_leaderboard_for_social_group_internal(
        xuid,
        scid,
        statName,
        socialGroup,
        NO_SKIP_RANK,
        NO_SKIP_XUID,
        sortOrder,
        maxItems,
        NO_CONTINUATION);
}

pplx::task<xbox_live_result<leaderboard_result>> leaderboard_service::get_leaderboard_for_social_group_skip_to_rank(
    _In_ const string_t& xuid,
    _In_ const string_t& scid,
    _In_ const string_t& statName,
    _In_ const string_t& socialGroup,
    _In_ uint32_t skipToRank,
    _In_ const string_t& sortOrder,
    _In_ uint32_t maxItems
    )
{
    return get_leaderboard_for_social_group_internal(
        xuid,
        scid,
        statName,
        socialGroup,
        skipToRank,
        NO_SKIP_XUID,
        sortOrder,
        maxItems,
        NO_CONTINUATION);
}

pplx::task<xbox_live_result<leaderboard_result>> leaderboard_service::get_leaderboard_for_social_group_skip_to_xuid(
    _In_ const string_t& xuid,
    _In_ const string_t& scid,
    _In_ const string_t& statName,
    _In_ const string_t& socialGroup,
    _In_ const string_t& skipToXuid,
    _In_ const string_t& sortOrder,
    _In_ uint32_t maxItems
    )
{
    return get_leaderboard_for_social_group_internal(
        xuid,
        scid,
        statName,
        socialGroup,
        NO_SKIP_RANK,
        skipToXuid,
        sortOrder,
        maxItems,
        NO_CONTINUATION);
}

xbox_live_result<string_t> create_leaderboard_url(
    _In_ const string_t& scid,
    _In_ const string_t& name,
    _In_ uint32_t skipToRank,
    _In_ const string_t& skipToXuid,
    _In_ uint32_t maxItems,
    _In_ const string_t& continuationToken,
    _In_ bool metadata,
    _In_ const string_t& xuid = string_t(),
    _In_ const string_t& socialGroup = string_t()
    )
{
    if (scid.empty())
        return xbox_live_result<string_t>(xbox_live_error_code::invalid_argument, "scid is required for getting leaderboards");
    if (name.empty())
        return xbox_live_result<string_t>(xbox_live_error_code::invalid_argument, "name is required for getting leaderboards");

    web::uri_builder builder;

    stringstream_t path;
    path << _T("/scids/");
    path << web::uri::encode_uri(scid, web::uri::components::path);
    path << _T("/leaderboards/");
    path << web::uri::encode_uri(name, web::uri::components::path);
    builder.set_path(path.str());

    if (metadata)
    {
        builder.append_query(_T("include"), _T("valuemetadata"));
    }

    if (!xuid.empty())
    {
        builder.append_query(_T("xuid"), xuid);
    }

    if (maxItems > 0)
    {
        builder.append_query(_T("maxItems"), maxItems);
    }

    if (!skipToXuid.empty())
    {
        if (skipToRank > 0)
        {
            return xbox_live_result<string_t>(xbox_live_error_code::invalid_argument, "Cannot skip to XUID and rank");
        }

        builder.append_query(_T("skipToUser"), skipToXuid);
    }
    else
    {
        if (!continuationToken.empty())
        {
            builder.append_query(_T("continuationToken"), continuationToken);
        }
        else if (skipToRank > 0)
        {
            builder.append_query(_T("skipToRank"), skipToRank);
        }
    }

    if (!socialGroup.empty())
    {
        builder.append_query(_T("view"), _T("People"));
        builder.append_query(_T("viewTarget"), socialGroup);
    }

    return xbox_live_result<string_t>(builder.to_string());
}

pplx::task<xbox_live_result<leaderboard_result>> leaderboard_service::get_leaderboard_internal(
    _In_ const string_t& scid,
    _In_ const string_t& name,
    _In_ uint32_t skipToRank,
    _In_ const string_t& skipToXuid,
    _In_ const string_t& xuid,
    _In_ const string_t& socialGroup,
    _In_ uint32_t maxItems,
    _In_ const string_t& continuationToken,
    _In_ const std::vector<string_t>& additionalColumnNames
    )
{
    xbox_live_result<string_t> url = create_leaderboard_url(
        scid,
        name,
        skipToRank,
        skipToXuid,
        maxItems,
        continuationToken,
        additionalColumnNames.size() != 0,
        xuid,
        socialGroup
        );

    RETURN_TASK_CPP_IF_ERR(url, leaderboard_result);

    std::shared_ptr<leaderboard_global_query> query = std::make_shared<leaderboard_global_query>();
    query->scid = scid;
    query->name = name;
    query->xuid = xuid;
    query->socialGroup = socialGroup;
    query->columns = additionalColumnNames;

    std::shared_ptr<http_call> http_call = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("GET"),
        utils::create_xboxlive_endpoint(_T("leaderboards"), m_appConfig),
        url.payload(),
        xbox_live_api::get_leaderboard_internal
        );

    http_call->set_xbox_contract_version_header_value(c_leaderboard_with_metadata_contract_version);

    auto userContext = m_userContext;
    auto xboxLiveContextSettings = m_xboxLiveContextSettings;
    auto appConfig = m_appConfig;

    auto task = http_call->get_response_with_auth(m_userContext)
    .then([userContext, xboxLiveContextSettings, appConfig, additionalColumnNames](std::shared_ptr<http_call_response> response)
    {
        return utils::generate_xbox_live_result<leaderboard_result>(
            serializers::deserialize_result(
                response->response_body_json(),
                userContext,
                xboxLiveContextSettings,
                appConfig
                ),
            response
            );

    }).then([query, additionalColumnNames](xbox_live_result<leaderboard_result> lb)
    {
        leaderboard_result& lbRes = lb.payload();
        lbRes._Set_next_query(query);
        if (additionalColumnNames.size() > 0)
        {
            lbRes._Parse_additional_columns(additionalColumnNames);
        }

        return lb;
    });

    return utils::create_exception_free_task<leaderboard_result>(
        task
        );
}

xbox_live_result<string_t> create_leaderboard_for_social_group_url(
    _In_ const string_t& xuid,
    _In_ const string_t& scid,
    _In_ const string_t& statName,
    _In_ const string_t& socialGroup,
    _In_ uint32_t skipToRank,
    _In_ const string_t& skipToXuid,
    _In_ const string_t& sortOrder,
    _In_ uint32_t maxItems,
    _In_ const string_t& continuationToken
    )
{
    if (xuid.empty()) return xbox_live_result<string_t>(xbox_live_error_code::invalid_argument, "xuid is required for getting leaderboard for social group");
    if (scid.empty()) return xbox_live_result<string_t>(xbox_live_error_code::invalid_argument, "scid is required for getting leaderboard for social group");
    if (statName.empty()) return xbox_live_result<string_t>(xbox_live_error_code::invalid_argument, "statName is required for getting leaderboard for social group");
    if (socialGroup.empty()) return xbox_live_result<string_t>(xbox_live_error_code::invalid_argument, "socialGroup is required for getting leaderboard for social group");

    // "/users/xuid({xuid})/scids/{scid}/stats/{statname}/people/{group}"

    web::uri_builder builder;

    stringstream_t path;
    path << _T("/users/xuid(");
    path << web::uri::encode_uri(xuid, web::uri::components::path);
    path << _T(")/scids/");
    path << web::uri::encode_uri(scid, web::uri::components::path);
    path << _T("/stats/");
    path << web::uri::encode_uri(statName, web::uri::components::path);
    path << _T("/people/");
    path << web::uri::encode_uri(socialGroup, web::uri::components::path);
    builder.set_path(path.str());

    if (!sortOrder.empty())
    {
        builder.append_query(_T("sort"), sortOrder);
    }

    if (maxItems > 0)
    {
        builder.append_query(_T("maxItems"), maxItems);
    }

    if (!skipToXuid.empty())
    {
        builder.append_query(_T("skipToUser"), skipToXuid);
    }
    else
    {
        if (!continuationToken.empty())
        {
            builder.append_query(_T("continuationToken"), continuationToken);
        }
        else if (skipToRank > 0)
        {
            builder.append_query(_T("skipToRank"), skipToRank);
        }
    }

    return xbox_live_result<string_t>(builder.to_string());
}

pplx::task<xbox_live_result<leaderboard_result>> leaderboard_service::get_leaderboard_for_social_group_internal(
    _In_ const string_t& xuid,
    _In_ const string_t& scid,
    _In_ const string_t& statName,
    _In_ const string_t& socialGroup,
    _In_ uint32_t skipToRank,
    _In_ const string_t& skipToXuid,
    _In_ const string_t& sortOrder,
    _In_ uint32_t maxItems,
    _In_ const string_t& continuationToken
    )
{
    // To align with People moniker support, we are mapping "People" to "all" until the
    // leaderboard service can align with naming conventions.
    string_t group = socialGroup;
    if (utils::str_icmp(socialGroup, _T("People")) == 0)
    {
        group = _T("all");
    }

    xbox_live_result<string_t> url = create_leaderboard_for_social_group_url(
        xuid,
        scid,
        statName,
        group,
        skipToRank,
        skipToXuid,
        sortOrder,
        maxItems,
        continuationToken);

    if (url.err())
    {
        return pplx::task_from_result(xbox_live_result<leaderboard_result>(url.err(), url.err_message()));
    }
    auto query = std::make_shared<leaderboard_social_query>();
    query->xuid = xuid;
    query->scid = scid;
    query->statName = statName;
    query->socialGroup = group;
    query->sortOrder = sortOrder;

    std::shared_ptr<http_call> http_call = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("GET"),
        utils::create_xboxlive_endpoint(_T("leaderboards"), m_appConfig),
        url.payload(),
        xbox_live_api::get_leaderboard_for_social_group_internal
        );

    http_call->set_xbox_contract_version_header_value(c_leaderboard_social_stat_name_contract_version);

    auto userContext = m_userContext;
    auto xboxLiveContextSettings = m_xboxLiveContextSettings;
    auto appConfig = m_appConfig;

    auto task = http_call->get_response_with_auth(m_userContext)
    .then([userContext, xboxLiveContextSettings, appConfig](std::shared_ptr<http_call_response> response)
    {

        return utils::generate_xbox_live_result<leaderboard_result>( 
            serializers::deserialize_result(
                response->response_body_json(),
                userContext,
                xboxLiveContextSettings,
                appConfig
                ),
            response
            );

    }).then([query](xbox_live_result<leaderboard_result> lb)
    {
        auto leaderboardResult = lb.payload();
        leaderboardResult._Set_next_query(query);
        return xbox_live_result<leaderboard_result>(leaderboardResult, lb.err(), lb.err_message());
    });

    return utils::create_exception_free_task<leaderboard_result>(
        task
        );
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_END