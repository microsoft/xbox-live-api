// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "shared_macros.h"
#include "xsapi/clubs.h"
#include "utils.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_CPP_BEGIN

using namespace xbox::services;

class clubs_service_impl : public std::enable_shared_from_this<clubs_service_impl>
{
public:
    clubs_service_impl(
        _In_ std::shared_ptr<user_context> userContext,
        _In_ std::shared_ptr<xbox_live_context_settings> xboxLiveContextSettings,
        _In_ std::shared_ptr<xbox_live_app_config> appConfig
        );

    pplx::task<xbox::services::xbox_live_result<club>> get_club(
        _In_ const string_t& clubId
        );

    pplx::task<xbox::services::xbox_live_result<std::vector<club>>> get_clubs(
        _In_ const std::vector<string_t>& clubIds
        );

    pplx::task<xbox::services::xbox_live_result<clubs_owned_result>> get_clubs_owned();

    pplx::task<xbox::services::xbox_live_result<club>> create_club(
        _In_ string_t name,
        _In_ club_type type,
        _In_ string_t titleFamilyId
        );

    pplx::task<xbox::services::xbox_live_result<void>> delete_club(
        _In_ const string_t& clubId
        );

    pplx::task<xbox::services::xbox_live_result<std::vector<club>>> get_club_associations();

    pplx::task<xbox::services::xbox_live_result<std::vector<club>>> get_club_associations(
        _In_ const string_t& xuid
        );

    pplx::task<xbox::services::xbox_live_result<std::vector<club_recommendation_t>>> get_club_recommendations();

    pplx::task<xbox::services::xbox_live_result<clubs_search_result>> search_clubs(
        _In_ const string_t& queryString,
        _In_ const std::vector<string_t>& titleIds,
        _In_ const std::vector<string_t>& tags
        );

    _XSAPIIMP pplx::task<xbox::services::xbox_live_result<std::vector<club_search_auto_complete>>> suggest_clubs(
        _In_ const string_t& query,
        _In_ const std::vector<string_t>& titleIds,
        _In_ const std::vector<string_t>& tags
        );

private:
    pplx::task<xbox::services::xbox_live_result<club>> get_club_internal(
        _In_ const string_t& clubId,
        _In_ const std::vector<string_t>& decorations
        );

    pplx::task<std::shared_ptr<http_call_response>> make_clubs_http_call(
        _In_ const string_t& httpMethod,
        _In_ const string_t& clubsEndpointPrefix,
        _In_ const string_t& pathQueryFragment,
        _In_ xbox_live_api api,
        _In_ web::json::value requestBody = web::json::value::null()
        );

    static string_t convert_user_presence_to_string(club_user_presence presence);
    static string_t convert_club_type_to_string(club_type type);
    static string_t convert_club_role_to_string(club_role role);

    // service subpaths
    static string_t clubhub_club_id_subpath(
        _In_ const std::vector<string_t>& clubIds,
        _In_ const std::vector<string_t>& decorations = std::vector<string_t>()
        );

    static string_t clubhub_club_id_subpath(
        _In_ const string_t clubId,
        _In_ const std::vector<string_t>& decorations = std::vector<string_t>()
        );  

    static string_t clubhub_xuid_subpath(
        _In_ const string_t xuid,
        _In_ const std::vector<string_t>& decorations = std::vector<string_t>()
        );

    static string_t clubhub_subpath(
        _In_ const string_t& moniker,
        _In_ bool includeDetailDecoration
        );

    static string_t clubhub_search_subpath(
        _In_ const string_t& query,
        _In_ const std::vector<string_t>& titleIds = std::vector<string_t>(),
        _In_ const std::vector<string_t>& tags = std::vector<string_t>(),
        _In_ uint32_t count = 200
        );

    static string_t clubhub_viewer_roles_subpath(
        _In_ const string_t& clubId
        );

    static string_t clubaccounts_clubs_subpath(
        _In_ const string_t& clubId,
        _In_ const string_t& actor = string_t()
        );
    
    static string_t clubaccounts_users_subpath(
        _In_ const string_t& xuid
        );
    
    static string_t clubpresence_subpath(
        _In_ const string_t& clubId,
        _In_ const string_t& xuid = string_t()
        );
    
    static string_t clubroster_subpath(
        _In_ const string_t& clubId,
        _In_ const string_t& xuid = string_t()
        );
    
    static string_t clubroster_roles_subpath(
        _In_ const string_t& clubId,
        _In_ const string_t& xuid
        );
    
    static string_t clubroster_roles_subpath(
        _In_ const string_t& clubId,
        _In_ const string_t& xuid,
        _In_ club_role role
        );

    static string_t clubsearch_suggest_path(
        _In_ const string_t& query,
        _In_ const std::vector<string_t>& titleIds = std::vector<string_t>(),
        _In_ const std::vector<string_t>& tags = std::vector<string_t>()
        );
   
    std::shared_ptr<xbox::services::user_context> m_userContext;
    std::shared_ptr<xbox::services::xbox_live_context_settings> m_xboxLiveContextSettings;
    std::shared_ptr<xbox::services::xbox_live_app_config> m_appConfig;

    friend club;
};


NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_CPP_END
