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
#include "xsapi/achievements.h"
#include "xbox_system_factory.h"
#include "utils.h"
#include "user_context.h"

using namespace pplx;

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_BEGIN

achievements_result::achievements_result()
{
}

void achievements_result::_Init_next_page_info(
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig,
    _In_ std::weak_ptr<xbox_live_context_impl> xboxLiveContextImpl,
    _In_ string_t xboxUserId,
    _In_ std::vector<uint32_t> titleIds,
    _In_ achievement_type type,
    _In_ bool unlockedOnly,
    _In_ achievement_order_by orderBy
    )
{
    m_userContext = std::move(userContext);
    m_xboxLiveContextSettings = std::move(xboxLiveContextSettings);
    m_appConfig = std::move(appConfig);
    m_xboxLiveContextImpl = std::move(xboxLiveContextImpl);
    m_xboxUserId = std::move(xboxUserId);
    m_titleIds = std::move(titleIds);
    m_achievementType = type;
    m_unlockedOnly = unlockedOnly;
    m_orderBy = orderBy;
}

const std::vector<achievement>&
achievements_result::items() const
{
    return m_items;
}

bool
achievements_result::has_next()
{
    return !m_continuationToken.empty();
}

pplx::task<xbox::services::xbox_live_result<achievements_result>>
achievements_result::get_next(
    _In_ uint32_t maxItems
    )
{
    if (m_continuationToken.empty())
    {
        xbox_live_result<achievements_result> results(xbox_live_error_code::out_of_range, "achievements_result doesn't have next page");
        return pplx::task_from_result<xbox::services::xbox_live_result<achievements_result>>(results);
    }

    return achievement_service(
            m_userContext, 
            m_xboxLiveContextSettings, 
            m_appConfig, 
            m_xboxLiveContextImpl).get_achievements(
        m_xboxUserId,
        m_titleIds,
        m_achievementType,
        m_unlockedOnly,
        m_orderBy,
        0, // use continuationToken, ignore skipItems.
        maxItems,
        m_continuationToken
        );
}

xbox_live_result<achievements_result>
achievements_result::_Deserialize(
    _In_ const web::json::value& json
    )
{
    if (json.is_null()) return xbox_live_result<achievements_result>();

    achievements_result result;

    std::error_code errCode = xbox_live_error_code::no_error;

    result.m_items = utils::extract_json_vector<achievement>(achievement::_Deserialize, json, _T("achievements"), errCode, true);
    auto pageInfoJson = utils::extract_json_field(json, _T("pagingInfo"), errCode, false);
    if (!pageInfoJson.is_null())
    {
        result.m_continuationToken = utils::extract_json_string(pageInfoJson, _T("continuationToken"), errCode, true);
    }

    return xbox_live_result<achievements_result>(result, errCode);
}



NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_END