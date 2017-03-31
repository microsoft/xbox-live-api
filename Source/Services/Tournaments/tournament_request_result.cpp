// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/tournaments.h"
#include "xbox_system_factory.h"
#include "utils.h"
#include "user_context.h"

using namespace pplx;

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_CPP_BEGIN

tournament_request_result::tournament_request_result()
{
}

void tournament_request_result::_Init_next_page_info(
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig,
    _In_ std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service> rtaService
    )
{
    m_userContext = std::move(userContext);
    m_xboxLiveContextSettings = std::move(xboxLiveContextSettings);
    m_appConfig = std::move(appConfig);
    m_realTimeActivityService = std::move(rtaService);
}

const std::vector<tournament>&
tournament_request_result::tournaments() const
{
    return m_items;
}

bool
tournament_request_result::has_next() const
{
    return !m_nextLinkUrl.empty();
}

pplx::task<xbox::services::xbox_live_result<tournament_request_result>>
tournament_request_result::get_next()
{
    if (m_nextLinkUrl.empty())
    {
        xbox_live_result<tournament_request_result> results(xbox_live_error_code::out_of_range, "tournament_request_result doesn't have next page");
        return pplx::task_from_result<xbox::services::xbox_live_result<tournament_request_result>>(results);
    }

    return tournament_service(
        m_userContext,
        m_xboxLiveContextSettings,
        m_appConfig,
        m_realTimeActivityService
        )._Get_tournaments(m_nextLinkUrl);
}

xbox_live_result<tournament_request_result>
tournament_request_result::_Deserialize(
    _In_ const web::json::value& json
    )
{
    if (json.is_null()) return xbox_live_result<tournament_request_result>();

    tournament_request_result result;
    std::error_code errCode = xbox_live_error_code::no_error;

    web::json::value items = utils::extract_json_field(json, _T("value"), errCode, false);
    web::json::array itemsArray = items.as_array();
    for (const auto& item : itemsArray)
    {
        web::json::value tournamentlJson = utils::extract_json_field(item, _T("tournament"), false);
        web::json::value teamJson = utils::extract_json_field(item, _T("team"), false);

        auto tournamentResult = tournament::_Deserialize(tournamentlJson, teamJson);
        if (tournamentResult.err())
        {
            errCode = tournamentResult.err();
        }

        result.m_items.push_back(tournamentResult.payload());
    }

    result.m_nextLinkUrl = utils::extract_json_string(json, _T("@nextLink"), errCode, false);

    return xbox_live_result<tournament_request_result>(result, errCode);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_CPP_END