// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "shared_macros.h"
#include "xbox_system_factory.h"
#include "xsapi/clubs.h"
#include "clubs_serializers.h"
#include "clubs_service_impl.h"
#include "utils.h"
#include "user_context.h"

using namespace xbox::services;
using namespace web;

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_CPP_BEGIN

clubs_service_impl::clubs_service_impl(
    _In_ std::shared_ptr<user_context> userContext,
    _In_ std::shared_ptr<xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox_live_app_config> appConfig
    ) :
    m_userContext(std::move(userContext)),
    m_xboxLiveContextSettings(std::move(xboxLiveContextSettings)),
    m_appConfig(std::move(appConfig))
{
}

pplx::task<xbox_live_result<club>> clubs_service_impl::get_club(
    _In_ const string_t& clubId
    )
{
    std::vector<string_t> decorations;
    decorations.push_back(_T("settings"));

    return get_club_internal(clubId, decorations);
}

pplx::task<xbox_live_result<std::vector<club>>> clubs_service_impl::get_clubs(
    _In_ const std::vector<string_t>& clubIds
    )
{
    json::value requestBody;
    std::vector<json::value> jsonClubIds;
    for (auto const& clubId : clubIds)
    {
        jsonClubIds.push_back(json::value::string(clubId));
    }
    requestBody[_T("Ids")] = json::value::array(jsonClubIds);

    auto sharedThis = shared_from_this();

    auto task = make_clubs_http_call(
        _T("POST"),
        _T("clubhub"),
        clubhub_subpath(_T("Batch"), true),
        xbox_live_api::get_club_batch,
        requestBody
        )
    .then([sharedThis](std::shared_ptr<http_call_response> response)
    {
        return clubs_serializers::generate_xbox_live_result<std::vector<club>>(
            clubs_serializers::deserialize_clubs_from_clubhub_reponse(response->response_body_json(), sharedThis),
            response
            );
    });
    return utils::create_exception_free_task(task);
}

pplx::task<xbox_live_result<clubs_owned_result>> clubs_service_impl::get_clubs_owned()
{
    auto task = make_clubs_http_call(
        _T("GET"),
        _T("clubaccounts"),
        clubaccounts_users_subpath(utils::string_t_from_internal_string(m_userContext->xbox_user_id())),
        xbox_live_api::get_clubs_owned
        )
    .then([](std::shared_ptr<http_call_response> response)
    {
        return clubs_serializers::generate_xbox_live_result<clubs_owned_result>(
            clubs_serializers::deserialize_get_clubs_owned_response(response->response_body_json()),
            response
            );
    });

    return utils::create_exception_free_task(task);
}

pplx::task<xbox_live_result<club>> clubs_service_impl::create_club(
    _In_ string_t name,
    _In_ club_type type,
    _In_ string_t titleFamilyId
    )
{
    json::value body;
    body[_T("name")] = json::value::string(name);
    body[_T("type")] = json::value::string(convert_club_type_to_string(type));
    body[_T("genre")] = json::value::string(_T("title"));
    body[_T("titleFamilyId")] = json::value::string(titleFamilyId);

    auto sharedThis = shared_from_this();

    auto task = make_clubs_http_call(
        _T("POST"),
        _T("clubaccounts"),
        _T("/clubs/create"),
        xbox_live_api::create_club,
        body
        )
    .then([sharedThis](std::shared_ptr<http_call_response> response)
    {
        auto result = clubs_serializers::generate_xbox_live_result<club>(
            xbox_live_result<club>(),
            response
            );

        if (result.err())
        {
            return pplx::task_from_result(result);
        }

        auto clubId = utils::extract_json_string(response->response_body_json(), _T("id"));
        
        return sharedThis->get_club_internal(clubId, std::vector<string_t>(1, _T("settings")));
    });

    return utils::create_exception_free_task(task);
}

pplx::task<xbox_live_result<void>> clubs_service_impl::delete_club(
    _In_ const string_t& clubId
    )
{
    auto task = make_clubs_http_call(
        _T("DELETE"),
        _T("clubaccounts"),
        clubaccounts_clubs_subpath(clubId),
        xbox_live_api::delete_club
        )
    .then([](std::shared_ptr<http_call_response> response)
    {
        return clubs_serializers::generate_xbox_live_result(response);
    });

    return utils::create_exception_free_task(task);
}

pplx::task<xbox_live_result<std::vector<club>>> clubs_service_impl::get_club_associations()
{
    return get_club_associations(utils::string_t_from_internal_string(m_userContext->xbox_user_id()));
}

pplx::task<xbox_live_result<std::vector<club>>> clubs_service_impl::get_club_associations(
    _In_ const string_t& xuid
    )
{
    std::vector<string_t> decorations;
    decorations.push_back(_T("settings"));

    auto sharedThis = shared_from_this();

    auto task = make_clubs_http_call(
        _T("GET"),
        _T("clubhub"),
        clubhub_xuid_subpath(xuid),
        xbox_live_api::get_users_club_associations
        )
    .then([sharedThis, xuid](std::shared_ptr<http_call_response> response)
    {
        return clubs_serializers::generate_xbox_live_result(
            clubs_serializers::deserialize_clubs_from_clubhub_reponse(response->response_body_json(), sharedThis, xuid),
            response
            );
    });
    return utils::create_exception_free_task(task);
}

pplx::task<xbox_live_result<std::vector<club_recommendation_t>>> clubs_service_impl::get_club_recommendations()
{
    auto sharedThis = shared_from_this();

    auto task = make_clubs_http_call(
        _T("POST"),
        _T("clubhub"),
        _T("/clubs/recommendations"),
        xbox_live_api::recommend_clubs
        )
    .then([sharedThis](std::shared_ptr<http_call_response> response)
    {
        return clubs_serializers::generate_xbox_live_result<std::vector<club_recommendation_t>>(
            clubs_serializers::deserialize_recommendations_from_clubhub_response(response->response_body_json(), sharedThis),
            response
            );
    });

    return utils::create_exception_free_task(task);
}

pplx::task<xbox_live_result<clubs_search_result>> clubs_service_impl::search_clubs(
    _In_ const string_t& queryString,
    _In_ const std::vector<string_t>& titleIds,
    _In_ const std::vector<string_t>& tags    
)
{
    auto sharedThis = shared_from_this();

    auto task = make_clubs_http_call(
        _T("GET"), 
        _T("clubhub"), 
        clubhub_search_subpath(queryString, titleIds, tags),
        xbox_live_api::search_clubs
        )
    .then([sharedThis](std::shared_ptr<http_call_response> response)
    {
        return clubs_serializers::generate_xbox_live_result<clubs_search_result>(
            clubs_serializers::deserialize_clubs_search_result_from_clubhub_response(response->response_body_json(), sharedThis),
            response
            );
    });

    return utils::create_exception_free_task(task);
}

pplx::task<xbox_live_result<std::vector<club_search_auto_complete>>> clubs_service_impl::suggest_clubs(
    _In_ const string_t& query,
    _In_ const std::vector<string_t>& titleIds,
    _In_ const std::vector<string_t>& tags
    )
{
    auto task = make_clubs_http_call(
        _T("GET"),
        _T("clubsearch"),
        clubsearch_suggest_path(query, titleIds, tags),
        xbox_live_api::suggest_clubs
        )
    .then([](std::shared_ptr<http_call_response> response)
    {
        return clubs_serializers::generate_xbox_live_result<std::vector<club_search_auto_complete>>(
            clubs_serializers::deserialize_club_suggestions(response->response_body_json()),
            response
            );
    });
    return utils::create_exception_free_task(task);
}

pplx::task<xbox::services::xbox_live_result<club>> clubs_service_impl::get_club_internal(
    _In_ const string_t& clubId,
    _In_ const std::vector<string_t>& decorations
    )
{
    auto sharedThis = shared_from_this();

    auto task = make_clubs_http_call(
        _T("GET"),
        _T("clubhub"),
        clubhub_club_id_subpath(clubId, decorations),
        xbox_live_api::get_club        
        )
    .then([sharedThis](std::shared_ptr<http_call_response> response)
    {
        auto deserializationResult = clubs_serializers::deserialize_clubs_from_clubhub_reponse(response->response_body_json(), sharedThis);

        auto finalResult = xbox_live_result<club>(deserializationResult.err(), deserializationResult.err_message());
        if (!finalResult.err() && deserializationResult.payload().size() >= 1)
        {
            finalResult.set_payload(deserializationResult.payload()[0]);
        }

        const std::error_code& httpErrorCode = response->err_code();
        if (httpErrorCode != xbox_live_error_code::no_error)
        {
            finalResult._Set_err(httpErrorCode);
            finalResult._Set_err_message(response->err_message());
        }

        return finalResult;
    });
    return utils::create_exception_free_task(task);
}

pplx::task<std::shared_ptr<http_call_response>> clubs_service_impl::make_clubs_http_call(
    _In_ const string_t& httpMethod,
    _In_ const string_t& clubsEndpointPrefix,
    _In_ const string_t& pathQueryFragment,
    _In_ xbox_live_api api,
    _In_ json::value requestBody
    )
{
    std::shared_ptr<http_call> httpCall = system::xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        httpMethod,
        utils::create_xboxlive_endpoint(clubsEndpointPrefix, m_appConfig),
        pathQueryFragment,
        api
        );

    switch (api)
    {
    case xbox_live_api::get_club:
    case xbox_live_api::get_club_batch:
    case xbox_live_api::get_users_club_associations:
    case xbox_live_api::search_clubs:
    case xbox_live_api::recommend_clubs:
        httpCall->set_xbox_contract_version_header_value(_T("4"));
        break;
    case xbox_live_api::get_clubs_owned:
        httpCall->set_xbox_contract_version_header_value(_T("2"));
        break;
    default:
        break;
    }

    if (!requestBody.is_null())
    {
        httpCall->set_request_body(requestBody.serialize());
    }

    return httpCall->get_response_with_auth(m_userContext);
}

string_t clubs_service_impl::convert_user_presence_to_string(club_user_presence presence)
{
    switch (presence)
    {
    case club_user_presence::chat:
        return _T("Chat");
    case club_user_presence::feed:
        return _T("Feed");
    case club_user_presence::in_club:
        return _T("InClub");
    case club_user_presence::not_in_club:
        return _T("NotInClub");
    case club_user_presence::play:
        return _T("Play");
    case club_user_presence::roster:
        return _T("Roster");
    default:
        return string_t();
    }
}

string_t clubs_service_impl::convert_club_type_to_string(club_type type)
{
    switch (type)
    {
    case club_type::private_club:
        return _T("closed");
    case club_type::public_club:
        return _T("open");
    case club_type::hidden_club:
        return _T("secret");
    default:
        return string_t();
    }
}

string_t clubs_service_impl::convert_club_role_to_string(club_role role)
{
    switch (role)
    {
    case club_role::banned:
        return _T("Banned");
    case club_role::follower:
        return _T("Follower");
    case club_role::invited:
        return _T("Invited");
    case club_role::member:
        return _T("Member");
    case club_role::moderator:
        return _T("Moderator");
    case club_role::owner:
        return _T("Owner");
    case club_role::recommended:
        return _T("Recommended");
    case club_role::requested_to_join:
        return _T("RequestedToJoin");
    default:
        return string_t();
    }
}

string_t clubs_service_impl::clubhub_club_id_subpath(
    _In_ const std::vector<string_t>& clubIds,
    _In_ const std::vector<string_t>& decorations
    )
{
    stringstream_t path;
    path << _T("/clubs/Ids(") << utils::vector_join(clubIds, L's') << _T(")");

    if (decorations.size() > 0)
    {
        path << _T("/decoration/");
        path << utils::vector_join(decorations, L',');
    }
    return path.str();
}

string_t clubs_service_impl::clubhub_club_id_subpath(
    _In_ const string_t clubId,
    _In_ const std::vector<string_t>& decorations
    )
{
    std::vector<string_t> ids(1, clubId);
    return clubhub_club_id_subpath(ids, decorations);
}

string_t clubs_service_impl::clubhub_xuid_subpath(
    _In_ const string_t xuid,
    _In_ const std::vector<string_t>& decorations
    )
{
    stringstream_t path;
    path << _T("/clubs/Xuid(") << xuid << _T(")");

    if (decorations.size() > 0)
    {
        path << _T("/decoration/");
        path << utils::vector_join(decorations, L',');
    }
    return path.str();
}

string_t clubs_service_impl::clubhub_subpath(
    _In_ const string_t& moniker,
    _In_ bool includeDetailDecoration
    )
{
    stringstream_t path;
    path << _T("/clubs/") << moniker;
    if (includeDetailDecoration)
    {
        path << _T("/decoration/detail");
    }
    return path.str();
}

string_t clubs_service_impl::clubhub_search_subpath(
    _In_ const string_t& query,
    _In_ const std::vector<string_t>& titleIds,
    _In_ const std::vector<string_t>& tags,
    _In_ uint32_t count
    )
{
    web::uri_builder builder;
    builder.set_path(_T("/clubs/search/decoration/settings"));
    builder.append_query(_T("q"), query);

    if (!titleIds.empty())
    {
        builder.append_query(_T("titles"), utils::vector_join(titleIds, L','));
    }
    if (!tags.empty())
    {
        builder.append_query(_T("tags"), utils::vector_join(tags, L','));
    }

    builder.append_query(_T("count"), count);

    return builder.to_string();
}

string_t clubs_service_impl::clubhub_viewer_roles_subpath(
    _In_ const string_t& clubId
    )
{
    stringstream_t path;
    path << _T("/clubs/clubid(") << clubId << _T(")/viewerRoles");
    return path.str();
}

string_t clubs_service_impl::clubaccounts_clubs_subpath(
    _In_ const string_t& clubId,
    _In_ const string_t& actor
    )
{
    stringstream_t path;
    path << _T("/clubs/clubid(") << clubId << _T(")");
    if (actor.length() > 0)
    {
        path << _T("/suspension/");
        path << actor;
    }
    return path.str();
}

string_t clubs_service_impl::clubaccounts_users_subpath(
    _In_ const string_t& xuid
    )
{
    stringstream_t path;
    path << _T("/users/xuid(") << xuid << _T(")/clubsowned");
    return path.str();
}

string_t clubs_service_impl::clubpresence_subpath(
    _In_ const string_t& clubId,
    _In_ const string_t& xuid
    )
{
    stringstream_t path;
    path << _T("/clubs/") << clubId << _T("/users/");
    if (xuid.length() > 0)
    {
        path << _T("xuid(") << xuid << _T(")");
    }
    else
    {
        path << _T("count");
    }
    return path.str();
}

string_t clubs_service_impl::clubroster_subpath(
    _In_ const string_t& clubId,
    _In_ const string_t& xuid
    )
{
    stringstream_t path;
    path << _T("/clubs/") << clubId << _T("/users");
    if (xuid.length() > 0)
    {
        path << _T("/xuid(") << xuid << _T(")");
    }
    return path.str();
}

string_t clubs_service_impl::clubroster_roles_subpath(
    _In_ const string_t& clubId,
    _In_ const string_t& xuid
    )
{
    stringstream_t path;
    path << clubroster_subpath(clubId, xuid);
    path << _T("/roles");
    return path.str();
}

string_t clubs_service_impl::clubroster_roles_subpath(
    _In_ const string_t& clubId,
    _In_ const string_t& xuid,
    _In_ club_role role
    )
{
    stringstream_t path;
    path << clubroster_roles_subpath(clubId, xuid);
    path << _T("/") << convert_club_role_to_string(role);
    return path.str();
}

string_t clubs_service_impl::clubsearch_suggest_path(
    _In_ const string_t& query,
    _In_ const std::vector<string_t>& titleIds,
    _In_ const std::vector<string_t>& tags
    )
{
    web::uri_builder builder;
    builder.set_path(_T("/suggest"));
    builder.append_query(_T("q"), query);

    if (!titleIds.empty())
    {
        builder.append_query(_T("titles"), utils::vector_join(titleIds, L','));
    }
    if (!tags.empty())
    {
        builder.append_query(_T("tags"), utils::vector_join(tags, L','));
    }
    return builder.to_string();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_CPP_END