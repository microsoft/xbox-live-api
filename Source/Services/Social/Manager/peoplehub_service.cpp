// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "social_manager_internal.h"
#include "http_call_impl.h"
#include "xbox_system_factory.h"
#include "xbox_live_app_config_internal.h"

using namespace xbox::services;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_BEGIN

peoplehub_service::peoplehub_service(
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> httpCallSettings,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config_internal> appConfig
    ) :
    m_userContext(std::move(userContext)),
    m_httpCallSettings(std::move(httpCallSettings)),
    m_appConfig(std::move(appConfig))
{
}

void peoplehub_service::get_social_graph(
    _In_ const xsapi_internal_string& callerXboxUserId,
    _In_ social_manager_extra_detail_level decorations,
    _In_opt_ async_queue_handle_t queue,
    _In_ xbox_live_callback<xbox_live_result<xsapi_internal_vector<xbox_social_user>>> callback
    )
{
    return get_social_graph(
        callerXboxUserId,
        decorations,
        "social",
        xsapi_internal_vector<xsapi_internal_string>(),
        false,
        queue,
        callback
        );
}

void peoplehub_service::get_social_graph(
    _In_ const xsapi_internal_string& callerXboxUserId,
    _In_ social_manager_extra_detail_level decorations,
    _In_ const xsapi_internal_vector<xsapi_internal_string> xboxLiveUsers,
    _In_opt_ async_queue_handle_t queue,
    _In_ xbox_live_callback<xbox_live_result<xsapi_internal_vector<xbox_social_user>>> callback
    )
{
    return get_social_graph(
        callerXboxUserId,
        decorations,
        "",
        xboxLiveUsers,
        true,
        queue,
        callback
        );
}

void peoplehub_service::get_social_graph(
    _In_ const xsapi_internal_string& callerXboxUserId,
    _In_ social_manager_extra_detail_level decorations,
    _In_ const xsapi_internal_string& relationshipType,
    _In_ const xsapi_internal_vector<xsapi_internal_string> xboxLiveUsers,
    _In_ bool isBatch,
    _In_opt_ async_queue_handle_t queue,
    _In_ xbox_live_callback<xbox_live_result<xsapi_internal_vector<xbox_social_user>>> callback
    )
{
    xsapi_internal_string pathAndQuery = social_graph_subpath(
        callerXboxUserId,
        decorations,
        relationshipType,
        xboxLiveUsers,
        isBatch
        );

    std::shared_ptr<http_call_internal> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_httpCallSettings,
        isBatch ? "POST" : "GET",
        utils::create_xboxlive_endpoint("peoplehub", m_appConfig),
        utils::string_t_from_internal_string(pathAndQuery),
        xbox_live_api::get_social_graph
        );

    if (isBatch)
    {
        web::json::value postJSON;
        web::json::value xuidJSON = web::json::value::array();
        for (uint32_t i = 0; i < xboxLiveUsers.size(); ++i)
        {
            xuidJSON[i] = web::json::value::string(utils::string_t_from_internal_string(xboxLiveUsers[i]));
        }

        postJSON[_T("xuids")] = xuidJSON;
        httpCall->set_request_body(utils::internal_string_from_string_t(postJSON.serialize()));
    }

    auto task = httpCall->get_response_with_auth(m_userContext,
        http_call_response_body_type::json_body,
        false,
        queue,
        [callback](std::shared_ptr<http_call_response_internal> response)
    {
        std::error_code errc = xbox_live_error_code::no_error;
        web::json::value peopleArray = utils::extract_json_field(
            response->response_body_json(),
            "people",
            errc,
            false
            );

        if (errc)
        {
            callback(xbox_live_result<xsapi_internal_vector<xbox_social_user>>(
                response->err_code(),
                response->err_message().data()
                ));
            return;
        }

        xsapi_internal_vector<xbox_social_user> socialUserVec = utils::extract_json_vector_internal<xbox_social_user>(
            xbox_social_user::_Deserialize,
            peopleArray,
            errc,
            false
            );
        auto it = response->response_headers().find("x-xbl-servicedefault");
        if (it != response->response_headers().end())
        {
            xsapi_internal_stringstream stream;
            stream << "Peoplehub dependency failed to load: " << it->second;

            response->set_error_info(xbox_live_error_code::http_status_424_failed_dependency, stream.str());
        }

        auto result = utils::generate_xbox_live_result<xsapi_internal_vector<xbox_social_user>>(
            xbox_live_result<xsapi_internal_vector<xbox_social_user>>(socialUserVec, errc),
            response
            );

        callback(result);
    });
}

xsapi_internal_string peoplehub_service::social_graph_subpath(
    _In_ const xsapi_internal_string& xboxUserId,
    _In_ social_manager_extra_detail_level decorations,
    _In_ const xsapi_internal_string& relationshipType,
    _In_ const xsapi_internal_vector<xsapi_internal_string> xboxLiveUsers,
    _In_ bool isBatch
    ) const
{
    xsapi_internal_stringstream source;
    source << "/users/xuid(";
    source << xboxUserId;
    source << ")/people";
    if (!relationshipType.empty())
    {
        source << "/" << relationshipType;
    }

    if (isBatch)
    {
        source << "/batch";
    }

    if ((decorations | social_manager_extra_detail_level::no_extra_detail) != social_manager_extra_detail_level::no_extra_detail)
    {
        source << "/decoration/";
        xsapi_internal_vector<xsapi_internal_string> decorationList;
        if ((decorations & social_manager_extra_detail_level::title_history_level) == social_manager_extra_detail_level::title_history_level)
        {
            xsapi_internal_stringstream titleStream;
            titleStream << "titlehistory(";
            titleStream << m_appConfig->title_id();
            titleStream << ")";
            xsapi_internal_string decorationString = titleStream.str();
            decorationList.push_back(decorationString);
        }
        if ((decorations & social_manager_extra_detail_level::preferred_color_level) == social_manager_extra_detail_level::preferred_color_level)
        {
            decorationList.push_back("preferredcolor");
        }

        decorationList.push_back("presenceDetail");
        uint32_t i = 0;
        for (const auto& str : decorationList)
        {
            if (i != 0)
            {
                source << ",";
            }
            source << str;
            ++i;
        }
    }

    return source.str();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_END