// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"

#include "xsapi/achievements.h"
#include "achievements_internal.h"

#include "shared_macros.h"
#include "Utils.h"

using namespace pplx;

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_BEGIN

achievement_service::achievement_service(
    std::shared_ptr<achievement_service_internal> internalObj
    ) :
    m_internalObj(std::move(internalObj))
{
}

pplx::task<xbox::services::xbox_live_result<void>> 
achievement_service::update_achievement(
    _In_ const string_t& xboxUserId,
    _In_ const string_t& achievementId,
    _In_ uint32_t percentComplete
    )
{
    task_completion_event<xbox_live_result<void>> tce;

    auto result = m_internalObj->update_achievement(
        utils::internal_string_from_string_t(xboxUserId),
        utils::internal_string_from_string_t(achievementId),
        percentComplete,
        get_xsapi_singleton(true)->m_asyncQueue,
        [tce](xbox_live_result<void> result) 
        {
            tce.set(result);
        });


    if (result.err())
    {
        return pplx::task_from_result(xbox_live_result<void>(result.err(), result.err_message()));
    }
    return pplx::task<xbox_live_result<void>>(tce);
}

pplx::task<xbox::services::xbox_live_result<void>> 
achievement_service::update_achievement(
    _In_ const string_t& xboxUserId,
    _In_ uint32_t titleId,
    _In_ const string_t& serviceConfigurationId,
    _In_ const string_t& achievementId,
    _In_ uint32_t percentComplete
    )
{
    task_completion_event<xbox_live_result<void>> tce;

    auto result = m_internalObj->update_achievement(
        utils::internal_string_from_string_t(xboxUserId),
        titleId,
        utils::internal_string_from_string_t(serviceConfigurationId),
        utils::internal_string_from_string_t(achievementId),
        percentComplete,
        nullptr,
        [tce](xbox_live_result<void> result)
        {
            tce.set(result);
        });


    if (result.err())
    {
        return pplx::task_from_result(xbox_live_result<void>(result.err(), result.err_message()));
    }        
    return pplx::task<xbox_live_result<void>>(tce);
}

pplx::task<xbox::services::xbox_live_result<achievements_result>>
achievement_service::get_achievements_for_title_id(
    _In_ const string_t& xboxUserId,
    _In_ uint32_t titleId,
    _In_ achievement_type type,
    _In_ bool unlockedOnly,
    _In_ achievement_order_by orderBy,
    _In_ uint32_t skipItems,
    _In_ uint32_t maxItems
    )
{
    task_completion_event<xbox_live_result<achievements_result>> tce;

    auto result = m_internalObj->get_achievements_for_title_id(
        utils::internal_string_from_string_t(xboxUserId),
        titleId,
        type,
        unlockedOnly,
        orderBy,
        skipItems,
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

pplx::task<xbox::services::xbox_live_result<achievement>>
achievement_service::get_achievement(
    _In_ const string_t& xboxUserId,
    _In_ const string_t& serviceConfigurationId,
    _In_ const string_t& achievementId
    )
{
    task_completion_event<xbox_live_result<achievement>> tce;

    auto result = m_internalObj->get_achievement(
        utils::internal_string_from_string_t(xboxUserId),
        utils::internal_string_from_string_t(serviceConfigurationId),
        utils::internal_string_from_string_t(achievementId),
        get_xsapi_singleton(true)->m_asyncQueue,
        [tce](xbox_live_result<std::shared_ptr<achievement_internal>> result) {
            tce.set(CREATE_EXTERNAL_XBOX_LIVE_RESULT(achievement, result));
        });


    if (result.err())
    {
        return pplx::task_from_result(xbox_live_result<achievement>(result.err(), result.err_message()));
    }
    return pplx::task<xbox_live_result<achievement>>(tce);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_END