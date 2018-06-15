// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "social_manager_internal.h"
#include "http_call_impl.h"
#include "xbox_system_factory.h"

using namespace xbox::services;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_BEGIN

peoplehub_service::peoplehub_service(
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> httpCallSettings,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
    ) :
    m_userContext(std::move(userContext)),
    m_httpCallSettings(std::move(httpCallSettings)),
    m_appConfig(std::move(appConfig))
{
}

pplx::task<xbox_live_result<std::vector<xbox_social_user>>>
peoplehub_service::get_social_graph(
    _In_ const string_t& callerXboxUserId,
    _In_ social_manager_extra_detail_level decorations
    )
{
    return get_social_graph(
        callerXboxUserId,
        decorations,
        _T("social"),
        std::vector<string_t>(),
        false
        );
}

pplx::task<xbox_live_result<std::vector<xbox_social_user>>>
peoplehub_service::get_social_graph(
    _In_ const string_t& callerXboxUserId,
    _In_ social_manager_extra_detail_level decorations,
    _In_ const std::vector<string_t> xboxLiveUsers
    )
{
    return get_social_graph(
        callerXboxUserId,
        decorations,
        _T(""),
        xboxLiveUsers,
        true
        );
}

pplx::task<xbox_live_result<std::vector<xbox_social_user>>>
peoplehub_service::get_social_graph(
    _In_ const string_t& callerXboxUserId,
    _In_ social_manager_extra_detail_level decorations,
    _In_ const string_t& relationshipType,
    _In_ const std::vector<string_t> xboxLiveUsers,
    _In_ bool isBatch
    )
{
    string_t pathAndQuery = social_graph_subpath(
        callerXboxUserId,
        decorations,
        relationshipType,
        xboxLiveUsers,
        isBatch
        );

    std::shared_ptr<http_call> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_httpCallSettings,
        isBatch ? _T("POST") : _T("GET"),
        utils::create_xboxlive_endpoint(_T("peoplehub"), m_appConfig),
        pathAndQuery,
        xbox_live_api::get_social_graph
        );

    if (isBatch)
    {
        web::json::value postJSON;
        web::json::value xuidJSON = web::json::value::array();
        for (uint32_t i = 0; i < xboxLiveUsers.size(); ++i)
        {
            xuidJSON[i] = web::json::value::string(xboxLiveUsers[i]);
        }

        postJSON[_T("xuids")] = xuidJSON;
        httpCall->set_request_body(postJSON);
    }

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([](std::shared_ptr<http_call_response> response)
    {
        std::error_code errc;
        web::json::value peopleArray = utils::extract_json_field(
            response->response_body_json(),
            _T("people"),
            errc,
            false
            );

        if (errc)
        {
            return xbox_live_result<std::vector<xbox_social_user>>(
                response->err_code(),
                response->err_message()
                );
        }

        std::vector<xbox_social_user> socialUserVec = utils::extract_json_vector<xbox_social_user>(
            xbox_social_user::_Deserialize,
            peopleArray,
            errc,
            false
            );

        std::vector<xbox_social_user> socialUser;
        socialUser.reserve(socialUserVec.size());
        for (auto& user : socialUserVec)
        {
            socialUser.push_back(user);
        }

        auto socialUserResult = xbox_live_result<std::vector<xbox_social_user>>(socialUser, errc);

        auto it = response->response_headers().find(L"x-xbl-servicedefault");
        if (it != response->response_headers().end())
        {
            stringstream_t stream;
            stream << L"Peoplehub dependency failed to load: " << it->second;

            response->_Set_error(xbox_live_error_code::http_status_424_failed_dependency, utils::convert_wide_string_to_standard_string(stream.str()));
        }

        return utils::generate_xbox_live_result<std::vector<xbox_social_user>>(
            socialUserResult,
            response
            );
    });

    return utils::create_exception_free_task<std::vector<xbox_social_user>>(
        task
        );
}

string_t peoplehub_service::social_graph_subpath(
    _In_ const string_t& xboxUserId,
    _In_ social_manager_extra_detail_level decorations,
    _In_ const string_t& relationshipType,
    _In_ const std::vector<string_t> xboxLiveUsers,
    _In_ bool isBatch
    ) const
{
    stringstream_t source;
    source << _T("/users/xuid(");
    source << xboxUserId;
    source << _T(")/people");
    if (!relationshipType.empty())
    {
        source << _T("/") << relationshipType;
    }

    if (isBatch)
    {
        source << _T("/batch");
    }

    if ((decorations | social_manager_extra_detail_level::no_extra_detail) != social_manager_extra_detail_level::no_extra_detail)
    {
        source << _T("/decoration/");
        std::vector<string_t> decorationList;
        if ((decorations & social_manager_extra_detail_level::title_history_level) == social_manager_extra_detail_level::title_history_level)
        {
            stringstream_t titleStream;
            titleStream << _T("titlehistory(");
            titleStream << m_appConfig->title_id();
            titleStream << _T(")");
            string_t decorationString = titleStream.str();
            decorationList.push_back(decorationString);
        }
        if ((decorations & social_manager_extra_detail_level::preferred_color_level) == social_manager_extra_detail_level::preferred_color_level)
        {
            decorationList.push_back(_T("preferredcolor"));
        }

        decorationList.push_back(_T("presenceDetail"));
        uint32_t i = 0;
        for (const auto& str : decorationList)
        {
            if (i != 0)
            {
                source << _T(",");
            }
            source << str;
            ++i;
        }
    }

    return source.str();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_END