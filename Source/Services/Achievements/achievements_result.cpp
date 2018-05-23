// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "shared_macros.h"
#include "xsapi/achievements.h"
#include "achievements_internal.h"
#include "utils.h"

using namespace pplx;

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_BEGIN

achievements_result::achievements_result()
{
}

achievements_result::achievements_result(
    std::shared_ptr<achievements_result_internal> internalObj
    ) :
    m_internalObj(std::move(internalObj))
{
}

DEFINE_GET_VECTOR_INTERNAL_TYPE(achievements_result, achievement, items);
DEFINE_GET_BOOL(achievements_result, has_next);

achievements_result_internal::achievements_result_internal(
    xsapi_internal_vector<std::shared_ptr<achievement_internal>> items,
    xsapi_internal_string continuationToken
    ) :
    m_items(std::move(items)),
    m_continuationToken(std::move(continuationToken))
{
}

void achievements_result_internal::_Init_next_page_info(
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config_internal> appConfig,
    _In_ std::weak_ptr<xbox_live_context_impl> xboxLiveContextImpl,
    _In_ xsapi_internal_string xboxUserId,
    _In_ xsapi_internal_vector<uint32_t> titleIds,
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

const xsapi_internal_vector<std::shared_ptr<achievement_internal>>&
achievements_result_internal::items() const
{
    return m_items;
}

bool
achievements_result_internal::has_next() const
{
    return !m_continuationToken.empty();
}

pplx::task<xbox::services::xbox_live_result<achievements_result>>
achievements_result::get_next(
    _In_ uint32_t maxItems
    )
{
    task_completion_event<xbox_live_result<achievements_result>> tce;

    auto result = m_internalObj->get_next(
        maxItems,
        get_xsapi_singleton(true)->m_asyncQueue,
        [tce](xbox_live_result<std::shared_ptr<achievements_result_internal>> result)
        {
            tce.set(CREATE_EXTERNAL_XBOX_LIVE_RESULT(achievements_result, result));
        });

    if (result.err())
    {
        return pplx::task_from_result(xbox_live_result<achievements_result>(result.err(), result.err_message()));
    }
    return pplx::task<xbox_live_result<achievements_result>>(tce);
}

_XSAPIIMP xbox_live_result<void> achievements_result_internal::get_next(
    _In_ uint32_t maxItems,
    _In_opt_ async_queue_handle_t queue,
    _In_ xbox_live_callback<xbox_live_result<std::shared_ptr<achievements_result_internal>>> callback
    )
{
    if (m_continuationToken.empty())
    {
        return xbox_live_result<void>(xbox_live_error_code::out_of_range, "achievements_result doesn't have next page");
    }
    
    return xsapi_allocate_shared<achievement_service_internal>(
        m_userContext,
        m_xboxLiveContextSettings,
        m_appConfig,
        m_xboxLiveContextImpl)->get_achievements(
            m_xboxUserId,
            m_titleIds,
            m_achievementType,
            m_unlockedOnly,
            m_orderBy,
            0, // use continuationToken, ignore skipItems.
            maxItems,
            m_continuationToken,
            queue,
            [callback](xbox_live_result<std::shared_ptr<achievements_result_internal>> result) {
                callback(result);
            });
}


const xsapi_internal_string& achievements_result_internal::xbox_user_id() const
{
    return m_xboxUserId;
}

const xsapi_internal_vector<uint32_t>& achievements_result_internal::title_ids() const
{
    return m_titleIds;
}

const achievement_type& achievements_result_internal::type() const
{
    return m_achievementType;
}

const bool& achievements_result_internal::unlocked_only() const
{
    return m_unlockedOnly;
}

const achievement_order_by& achievements_result_internal::order_by() const
{
    return m_orderBy;
}

const xsapi_internal_string& achievements_result_internal::continuation_token() const
{
    return m_continuationToken;
}

xbox_live_result<std::shared_ptr<achievements_result_internal>>
achievements_result_internal::_Deserialize(
    _In_ const web::json::value& json
    )
{
    if (json.is_null()) return xbox_live_result<std::shared_ptr<achievements_result_internal>>();

    std::error_code errCode = xbox_live_error_code::no_error;

    auto achievements = utils::extract_json_vector<std::shared_ptr<achievement_internal>>(achievement_internal::_Deserialize, json, "achievements", errCode, true);

    xsapi_internal_string continuationToken;
    auto pageInfoJson = utils::extract_json_field(json, _T("pagingInfo"), errCode, false);
    if (!pageInfoJson.is_null())
    {
        continuationToken = utils::extract_json_string(pageInfoJson, "continuationToken", errCode, true);
    }
    auto result = xsapi_allocate_shared<achievements_result_internal>(
        achievements,
        continuationToken
        );

    return xbox_live_result<std::shared_ptr<achievements_result_internal>>(result, errCode);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_END