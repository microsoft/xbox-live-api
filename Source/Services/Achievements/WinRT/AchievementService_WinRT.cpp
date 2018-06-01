// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "AchievementService_winrt.h"
#include "utils_winrt.h"

using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Platform;
using namespace concurrency;
using namespace xbox::services;
using namespace xbox::services::achievements;
using namespace Microsoft::Xbox::Services::System;

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_BEGIN

AchievementService::AchievementService(
    _In_ xbox::services::achievements::achievement_service cppObj
    ):
    m_cppObj(std::move(cppObj))
{
}

Windows::Foundation::IAsyncAction^
AchievementService::UpdateAchievementAsync(
    _In_ Platform::String^ xboxUserId,
    _In_ Platform::String^ achievementId,
    _In_ uint32 percentComplete
    )
{
    auto task = m_cppObj.update_achievement(
        STRING_T_FROM_PLATFORM_STRING(xboxUserId),
        STRING_T_FROM_PLATFORM_STRING(achievementId),
        percentComplete)
    .then([](xbox_live_result<void> cppResult)
    {
        THROW_IF_ERR(cppResult);
    });

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncOperation<int32>^
AchievementService::TryUpdateAchievementAsync(
    _In_ Platform::String^ xboxUserId,
    _In_ Platform::String^ achievementId,
    _In_ uint32 percentComplete
    )
{
    auto task = m_cppObj.update_achievement(
        STRING_T_FROM_PLATFORM_STRING(xboxUserId),
        STRING_T_FROM_PLATFORM_STRING(achievementId),
        percentComplete)
    .then([](xbox_live_result<void> cppResult)
    {
        return (int32)xbox::services::utils::convert_xbox_live_error_code_to_hresult(cppResult.err());
    });

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncAction^
AchievementService::UpdateAchievementAsync(
    _In_ Platform::String^ xboxUserId,
    _In_ uint32 titleId,
    _In_ Platform::String^ serviceConfigurationId,
    _In_ Platform::String^ achievementId,
    _In_ uint32 percentComplete
    )
{
    auto task = m_cppObj.update_achievement(
        STRING_T_FROM_PLATFORM_STRING(xboxUserId),
        titleId,
        STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
        STRING_T_FROM_PLATFORM_STRING(achievementId),
        percentComplete)
    .then([](xbox_live_result<void> cppResult)
    {
        THROW_IF_ERR(cppResult);
    });

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncOperation<int32>^
AchievementService::TryUpdateAchievementAsync(
    _In_ Platform::String^ xboxUserId,
    _In_ uint32 titleId,
    _In_ Platform::String^ serviceConfigurationId,
    _In_ Platform::String^ achievementId,
    _In_ uint32 percentComplete
    )
{
    auto task = m_cppObj.update_achievement(
        STRING_T_FROM_PLATFORM_STRING(xboxUserId),
        titleId,
        STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
        STRING_T_FROM_PLATFORM_STRING(achievementId),
        percentComplete)
    .then([](xbox_live_result<void> cppResult)
    {
        return (int32)xbox::services::utils::convert_xbox_live_error_code_to_hresult(cppResult.err());
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<AchievementsResult^>^
AchievementService::GetAchievementsForTitleIdAsync(
    _In_ Platform::String^ xboxUserId,
    _In_ uint32 titleId,
    _In_ AchievementType type,
    _In_ bool unlockedOnly,
    _In_ AchievementOrderBy orderBy,
    _In_ uint32 skipItems,
    _In_ uint32 maxItems
    )
{
    auto task = m_cppObj.get_achievements_for_title_id(
        STRING_T_FROM_PLATFORM_STRING(xboxUserId),
        titleId,
        static_cast<achievement_type>(type),
        unlockedOnly,
        static_cast<achievement_order_by>(orderBy),
        skipItems,
        maxItems)
    .then([](xbox_live_result<achievements_result> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new AchievementsResult(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<Achievement^>^
AchievementService::GetAchievementAsync(
    _In_ Platform::String^ xboxUserId,
    _In_ Platform::String^ serviceConfigurationId,
    _In_ Platform::String^ achievementId
    )
{
    auto task = m_cppObj.get_achievement(
        STRING_T_FROM_PLATFORM_STRING(xboxUserId),
        STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
        STRING_T_FROM_PLATFORM_STRING(achievementId))
    .then([](xbox_live_result<achievement> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new Achievement(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_END
