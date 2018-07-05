// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "shared_macros.h"
#include "xbox_system_factory.h"
#include "leaderboard_serializers.h"
#include "leaderboard_query.h"
#include "xsapi/leaderboard.h"
#include "leaderboard_service_impl.h"
#include "utils.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_BEGIN

#define c_leaderboard_social_stat_name_contract_version _T("1")
#define c_leaderboard_with_metadata_contract_version _T("3")
#define c_leaderboard_with_stats_2017_version _T("4")

// Forward
HRESULT create_leaderboard_url(
    _Out_ xsapi_internal_string* result,
    _In_ const xsapi_internal_string& scid,
    _In_ const xsapi_internal_string& name,
    _In_ uint32_t skipToRank,
    _In_ uint64_t skipToXuid,
    _In_ uint32_t maxItems,
    _In_ const xsapi_internal_string& continuationToken,
    _In_ bool metadata,
    _In_ uint64_t xuid,
    _In_ XblSocialGroupType socialGroup,
    _In_ leaderboard_version version
    );

HRESULT create_leaderboard_for_social_group_url(
    _In_ uint64_t xuid,
    _In_ const xsapi_internal_string& scid,
    _In_ const xsapi_internal_string& statName,
    _In_ XblSocialGroupType socialGroup,
    _In_ uint32_t skipToRank,
    _In_ uint64_t skipToXuid,
    _In_ sort_order sortOrder,
    _In_ uint32_t maxItems,
    _In_ const xsapi_internal_string& continuationToken,
    _Out_ xsapi_internal_string* result
    );

leaderboard_service_impl::leaderboard_service_impl(
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config_internal> appConfig,
    _In_ std::weak_ptr<xbox_live_context_impl> xboxLiveContextImpl
) :
    m_userContext(std::move(userContext)),
    m_xboxLiveContextSettings(std::move(xboxLiveContextSettings)),
    m_appConfig(std::move(appConfig)),
    m_xboxLiveContextImpl(std::move(xboxLiveContextImpl))
{
}


HRESULT leaderboard_service_impl::get_leaderboard_for_social_group_internal(
    _In_ uint64_t xuid,
    _In_ const xsapi_internal_string& scid,
    _In_ const xsapi_internal_string& statName,
    _In_ XblSocialGroupType socialGroup,
    _In_ uint32_t skipToRank,
    _In_ uint64_t skipToXuid,
    _In_ sort_order order,
    _In_ uint32_t maxItems,
    _In_ const xsapi_internal_string& continuationToken,
    _In_ leaderboard_version version,
    _In_opt_ async_queue_handle_t queue,
    _In_ xbox_live_callback<xbox_live_result<std::shared_ptr<leaderboard_result_internal>>> callback
    )
{
    // To align with People moniker support, we are mapping "People" to "all" until the
    // leaderboard service can align with naming conventions.
    xsapi_internal_string subPath;
    HRESULT hr = create_leaderboard_for_social_group_url(
        xuid,
        scid,
        statName,
        socialGroup,
        skipToRank,
        skipToXuid,
        order,
        maxItems,
        continuationToken,
        &subPath);
    if (FAILED(hr))
    {
        return hr;
    }

    auto query = xsapi_allocate_shared<leaderboard_social_query>();
    query->xuid = xuid;
    query->scid = scid;
    query->statName = statName;
    query->socialGroup = socialGroup;
    query->sortOrder = order;

    std::shared_ptr<http_call_internal> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call_internal(
        m_xboxLiveContextSettings,
        "GET",
        utils::create_xboxlive_endpoint("leaderboards", m_appConfig),
        subPath,
        xbox_live_api::get_leaderboard_for_social_group_internal
        );
    if (version == leaderboard_version_2017)
    {
        httpCall->set_xbox_contract_version_header_value(c_leaderboard_with_stats_2017_version);
        // TODO: jasonsa - query
        //lbQuery._Set_stat_name(utils::string_t_from_internal_string(statName));
        //lbQuery._Set_social_group(utils::string_t_from_internal_string(socialGroup));
    }
    else
    {
        httpCall->set_xbox_contract_version_header_value(c_leaderboard_social_stat_name_contract_version);
    }

    auto lbService = shared_from_this();

    httpCall->get_response_with_auth(
        m_userContext,
        http_call_response_body_type::json_body,
        false,
        queue,
    [callback, lbService, version, query](std::shared_ptr<http_call_response_internal> response)
    {
        auto xblLeaderboardResult = serializers::deserialize_result(
            response->response_body_json(),
            lbService,
            version);

        xblLeaderboardResult.payload()->_Set_next_query(query);
        auto xblResult = utils::generate_xbox_live_result<std::shared_ptr<leaderboard_result_internal>>(xblLeaderboardResult, response);
        callback(xblResult);
    });

    return S_OK;
}

HRESULT leaderboard_service_impl::get_leaderboard_internal(
    _In_ const xsapi_internal_string& scid,
    _In_ const xsapi_internal_string& name,
    _In_ uint32_t skipToRank,
    _In_ uint64_t skipToXuid,
    _In_ uint64_t xuid,
    _In_ XblSocialGroupType socialGroup,
    _In_ uint32_t maxItems,
    _In_ const xsapi_internal_string& continuationToken,
    _In_ const xsapi_internal_vector<xsapi_internal_string>& additionalColumnNames,
    _In_ leaderboard_version version,
    _In_opt_ async_queue_handle_t queue,
    _In_ xbox_live_callback<xbox_live_result<std::shared_ptr<leaderboard_result_internal>>> callback
    )
{
    xsapi_internal_string subPath;
    HRESULT hr = create_leaderboard_url(
        &subPath,
        scid,
        name,
        skipToRank,
        skipToXuid,
        maxItems,
        continuationToken,
        additionalColumnNames.size() != 0,
        xuid,
        socialGroup,
        version
        );
    if (FAILED(hr)) return hr;
    
    auto query = xsapi_allocate_shared<leaderboard_global_query>();
    query->scid = scid;
    query->name = name;
    query->xuid = xuid;
    query->socialGroup = socialGroup;
    query->columns = additionalColumnNames;

    std::shared_ptr<http_call_internal> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call_internal(
        m_xboxLiveContextSettings,
        "GET",
        utils::create_xboxlive_endpoint("leaderboards", m_appConfig),
        subPath,
        xbox_live_api::get_leaderboard_internal);
    if (version == leaderboard_version_2017)
    {
        httpCall->set_xbox_contract_version_header_value(c_leaderboard_with_stats_2017_version);
        // TODO: jasonsa - query
        //lbQuery._Set_stat_name(utils::string_t_from_internal_string(name));
    }
    else
    {
        httpCall->set_xbox_contract_version_header_value(c_leaderboard_with_metadata_contract_version);
    }
    auto lbService = shared_from_this();

    httpCall->get_response_with_auth(
        m_userContext,
        http_call_response_body_type::json_body,
        false,
        queue,
        [callback, lbService, additionalColumnNames, version, query](std::shared_ptr<http_call_response_internal> response)
    {
        auto xblLeaderboardResult = serializers::deserialize_result(
            response->response_body_json(),
            lbService,
            version);

        // TODO: jasonsa - query
        //xblLeaderboardResult.payload()._Set_next_query(query);
        if (additionalColumnNames.size() > 0)
        {
            xblLeaderboardResult.payload()->_Parse_additional_columns(additionalColumnNames);
        }
        
        auto xblResult = utils::generate_xbox_live_result<std::shared_ptr<leaderboard_result_internal>>(xblLeaderboardResult, response);
        callback(xblResult);
    });

    return S_OK;
}

HRESULT create_leaderboard_url(
    _Out_ xsapi_internal_string* result,
    _In_ const xsapi_internal_string& scid,
    _In_ const xsapi_internal_string& name,
    _In_ uint32_t skipToRank,
    _In_ uint64_t skipToXuid,
    _In_ uint32_t maxItems,
    _In_ const xsapi_internal_string& continuationToken,
    _In_ bool metadata,
    _In_ uint64_t xuid,
    _In_ XblSocialGroupType socialGroup,
    _In_ leaderboard_version version
    )
{
    if (scid.empty() || name.empty())
        return E_INVALIDARG;

    xsapi_uri_builder builder;

    xsapi_internal_stringstream path;
    path << "/scids/";
    path << utils::xsapi_encode_uri(scid);
    if (version == leaderboard_version_2017)
    {
        path << "/leaderboards/stat(";
        path << utils::xsapi_encode_uri(name);
        path << ")";
        builder.set_path(path.str());
    }
    else
    {
        path << "/leaderboards/";
        path << utils::xsapi_encode_uri(name);
        builder.set_path(path.str());
    }

    if (metadata)
    {
        builder.append_query("include", "valuemetadata");
    }

    if (xuid != 0)
    {
        builder.append_query("xuid", utils::uint64_to_internal_string(xuid));
    }

    if (maxItems > 0)
    {
        builder.append_query("maxItems", maxItems);
    }

    if (skipToXuid != 0)
    {
        if (skipToRank > 0)
        {
            // "Cannot skip to XUID and rank"
            return E_INVALIDARG;
        }

        builder.append_query("skipToUser", utils::uint64_to_internal_string(skipToXuid));
    }
    else
    {
        if (!continuationToken.empty())
        {
            builder.append_query("continuationToken", continuationToken);
        }
        else if (skipToRank > 0)
        {
            builder.append_query("skipToRank", skipToRank);
        }
    }

    if (socialGroup != XblSocialGroupType_None)
    {
        builder.append_query("view", "People");
        builder.append_query("viewTarget", socialGroup);
    }

    *result = builder.to_string();
    return S_OK;
}

HRESULT create_leaderboard_for_social_group_url(
    _In_ uint64_t xuid,
    _In_ const xsapi_internal_string& scid,
    _In_ const xsapi_internal_string& statName,
    _In_ XblSocialGroupType socialGroup,
    _In_ uint32_t skipToRank,
    _In_ uint64_t skipToXuid,
    _In_ sort_order sortOrder,
    _In_ uint32_t maxItems,
    _In_ const xsapi_internal_string& continuationToken,
    _Out_ xsapi_internal_string* result
    )
{
    if (xuid == 0 || scid.empty() || statName.empty() || socialGroup == XblSocialGroupType_None ) return E_INVALIDARG;

    // "/users/xuid({xuid})/scids/{scid}/stats/{statname}/people/{group}"

    xsapi_internal_string group = "";
    switch (socialGroup)
    {
        case XblSocialGroupType_People:
            group = "all";
            break;
        case XblSocialGroupType_Favorites:
            group = "Favorites";
            break;
    }

    xsapi_uri_builder builder;

    xsapi_internal_stringstream path;
    path << "/users/xuid(";
    path << utils::xsapi_encode_uri(utils::uint64_to_internal_string(xuid));
    path << ")/scids/";
    path << utils::xsapi_encode_uri(scid);
    path << "/stats/";
    path << utils::xsapi_encode_uri(statName);
    path << "/people/";
    path << utils::xsapi_encode_uri(group);
    builder.set_path(path.str());
    
    if (sortOrder == sort_order::descending)
    {
        builder.append_query("sort", "descending");
    }
    else
    {
        builder.append_query("sort", "ascending");
    }

    if (maxItems > 0)
    {
        builder.append_query("maxItems", maxItems);
    }

    if (skipToXuid != 0)
    {
        builder.append_query("skipToUser", utils::uint64_to_internal_string(skipToXuid));
    }
    else
    {
        if (!continuationToken.empty())
        {
            builder.append_query("continuationToken", continuationToken);
        }
        else if (skipToRank > 0)
        {
            builder.append_query("skipToRank", skipToRank);
        }
    }

    *result = builder.to_string();
    return S_OK;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_END