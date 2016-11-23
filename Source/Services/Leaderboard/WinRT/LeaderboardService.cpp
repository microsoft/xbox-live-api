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
#include "LeaderboardService.h"
#include "xsapi/leaderboard.h"
#include "Utils_WinRT.h"

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;
using namespace xbox::services::leaderboard;
using namespace Microsoft::Xbox::Services::System;
using namespace xbox::services;

#define PROJECT_LB(task) task.then([](xbox_live_result<leaderboard_result> lb)  \
    {                                                                           \
        THROW_IF_ERR(lb);                                                       \
        return ref new LeaderboardResult(lb.payload());                         \
    });

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_BEGIN

LeaderboardService::LeaderboardService(
    _In_ xbox::services::leaderboard::leaderboard_service cppObj) :
    m_cppObj(std::move(cppObj))
{
}

Windows::Foundation::IAsyncOperation<LeaderboardResult^>^
LeaderboardService::GetLeaderboardAsync(
    _In_ Platform::String^ serviceConfigurationId,
    _In_ Platform::String^ leaderboardName)
{
    auto task = PROJECT_LB(
        m_cppObj.get_leaderboard(
            STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
            STRING_T_FROM_PLATFORM_STRING(leaderboardName)
    ));

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncOperation<LeaderboardResult^>^
LeaderboardService::GetLeaderboardWithAdditionalColumnsAsync(
    _In_ Platform::String^ serviceConfigurationId,
    _In_ Platform::String^ leaderboardName,
    _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ additionalColumns)
{
    auto task = PROJECT_LB(
        m_cppObj.get_leaderboard(
            STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
            STRING_T_FROM_PLATFORM_STRING(leaderboardName),
            UtilsWinRT::CreateStdVectorStringFromPlatformVectorObj(additionalColumns)
            ));

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncOperation<LeaderboardResult^>^ 
LeaderboardService::GetLeaderboardAsync(
    _In_ Platform::String^ serviceConfigurationId,
    _In_ Platform::String^ leaderboardName,
    _In_ Platform::String^ xuid,
    _In_ Platform::String^ socialGroup,
    _In_ uint32 maxItems
    )
{
    auto task = PROJECT_LB(
        m_cppObj.get_leaderboard(
            STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
            STRING_T_FROM_PLATFORM_STRING(leaderboardName),
            STRING_T_FROM_PLATFORM_STRING(xuid),
            STRING_T_FROM_PLATFORM_STRING(socialGroup),
            maxItems,
            std::vector<string_t>()
            ));

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncOperation<LeaderboardResult^>^ 
LeaderboardService::GetLeaderboardWithAdditionalColumnsAsync(
    _In_ Platform::String^ serviceConfigurationId,
    _In_ Platform::String^ leaderboardName,
    _In_ Platform::String^ xuid,
    _In_ Platform::String^ socialGroup,
    _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ additionalColumns,
    _In_ uint32 maxItems
    )
{
    auto task = PROJECT_LB(
        m_cppObj.get_leaderboard(
            STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
            STRING_T_FROM_PLATFORM_STRING(leaderboardName),
            STRING_T_FROM_PLATFORM_STRING(xuid),
            STRING_T_FROM_PLATFORM_STRING(socialGroup),
            maxItems,
            UtilsWinRT::CreateStdVectorStringFromPlatformVectorObj(additionalColumns)
            ));

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncOperation<LeaderboardResult^>^ 
LeaderboardService::GetLeaderboardAsync(
    _In_ Platform::String^ serviceConfigurationId,
    _In_ Platform::String^ leaderboardName,
    _In_ uint32 skipToRank,
    _In_ uint32 maxItems)
{
    auto task = PROJECT_LB(
        m_cppObj.get_leaderboard(
            STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
            STRING_T_FROM_PLATFORM_STRING(leaderboardName),
            skipToRank,
            maxItems,
            std::vector<string_t>()));

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncOperation<LeaderboardResult^>^
LeaderboardService::GetLeaderboardWithAdditionalColumnsAsync(
    _In_ Platform::String^ serviceConfigurationId,
    _In_ Platform::String^ leaderboardName,
    _In_ uint32 skipToRank,
    _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ additionalColumns,
    _In_ uint32 maxItems)
{
    auto task = PROJECT_LB(
        m_cppObj.get_leaderboard(
            STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
            STRING_T_FROM_PLATFORM_STRING(leaderboardName),
            skipToRank,
            maxItems,
            UtilsWinRT::CreateStdVectorStringFromPlatformVectorObj(additionalColumns)));

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncOperation<LeaderboardResult^>^ 
LeaderboardService::GetLeaderboardAsync(
    _In_ Platform::String^ serviceConfigurationId,
    _In_ Platform::String^ leaderboardName,
    _In_ Platform::String^ xuid,
    _In_ Platform::String^ socialGroup,
    _In_ uint32 skipToRank,
    _In_ uint32 maxItems
    )
{
    auto task = PROJECT_LB(
        m_cppObj.get_leaderboard(
            STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
            STRING_T_FROM_PLATFORM_STRING(leaderboardName),
            skipToRank,
            STRING_T_FROM_PLATFORM_STRING(xuid),
            STRING_T_FROM_PLATFORM_STRING(socialGroup),
            maxItems,
            std::vector<string_t>()));

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncOperation<LeaderboardResult^>^ 
LeaderboardService::GetLeaderboardWithAdditionalColumnsAsync(
    _In_ Platform::String^ serviceConfigurationId,
    _In_ Platform::String^ leaderboardName,
    _In_ Platform::String^ xuid,
    _In_ Platform::String^ socialGroup,
    _In_ uint32 skipToRank,
    _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ additionalColumns,
    _In_ uint32 maxItems
    )
{
    auto task = PROJECT_LB(
        m_cppObj.get_leaderboard(
            STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
            STRING_T_FROM_PLATFORM_STRING(leaderboardName),
            skipToRank,
            STRING_T_FROM_PLATFORM_STRING(xuid),
            STRING_T_FROM_PLATFORM_STRING(socialGroup),
            maxItems,
            UtilsWinRT::CreateStdVectorStringFromPlatformVectorObj(additionalColumns)));

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncOperation<LeaderboardResult^>^
LeaderboardService::GetLeaderboardWithSkipToUserAsync(
    _In_ Platform::String^ serviceConfigurationId,
    _In_ Platform::String^ leaderboardName,
    _In_opt_ Platform::String^ skipToXboxUserId,
    _In_ uint32 maxItems)
{
    auto task = PROJECT_LB(
        m_cppObj.get_leaderboard_skip_to_xuid(
            STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
            STRING_T_FROM_PLATFORM_STRING(leaderboardName),
            STRING_T_FROM_PLATFORM_STRING(skipToXboxUserId),
            maxItems,
            std::vector<string_t>()));

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncOperation<LeaderboardResult^>^
LeaderboardService::GetLeaderboardWithSkipToUserWithAdditionalColumnsAsync(
    _In_ Platform::String^ serviceConfigurationId,
    _In_ Platform::String^ leaderboardName,
    _In_ Platform::String^ skipToXboxUserId,
    _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ additionalColumns,
    _In_ uint32 maxItems)
{
    auto task = PROJECT_LB(
        m_cppObj.get_leaderboard_skip_to_xuid(
            STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
            STRING_T_FROM_PLATFORM_STRING(leaderboardName),
            STRING_T_FROM_PLATFORM_STRING(skipToXboxUserId),
            maxItems,
            UtilsWinRT::CreateStdVectorStringFromPlatformVectorObj(additionalColumns)));

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncOperation<LeaderboardResult^>^ 
LeaderboardService::GetLeaderboardWithSkipToUserAsync(
    _In_ Platform::String^ serviceConfigurationId,
    _In_ Platform::String^ leaderboardName,
    _In_ Platform::String^ skipToXboxUserId,
    _In_ Platform::String^ xuid,
    _In_ Platform::String^ socialGroup,
    _In_ uint32 maxItems
    )
{
    auto task = PROJECT_LB(
        m_cppObj.get_leaderboard_skip_to_xuid(
            STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
            STRING_T_FROM_PLATFORM_STRING(leaderboardName),
            STRING_T_FROM_PLATFORM_STRING(skipToXboxUserId),
            STRING_T_FROM_PLATFORM_STRING(xuid),
            STRING_T_FROM_PLATFORM_STRING(socialGroup),
            maxItems,
            std::vector<string_t>()));

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncOperation<LeaderboardResult^>^ 
LeaderboardService::GetLeaderboardWithSkipToUserWithAdditionalColumnsAsync(
    _In_ Platform::String^ serviceConfigurationId,
    _In_ Platform::String^ leaderboardName,
    _In_ Platform::String^ skipToXboxUserId,
    _In_ Platform::String^ xuid,
    _In_ Platform::String^ socialGroup,
    _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ additionalColumns,
    _In_ uint32 maxItems
    )
{
    auto task = PROJECT_LB(
        m_cppObj.get_leaderboard_skip_to_xuid(
            STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
            STRING_T_FROM_PLATFORM_STRING(leaderboardName),
            STRING_T_FROM_PLATFORM_STRING(skipToXboxUserId),
            STRING_T_FROM_PLATFORM_STRING(xuid),
            STRING_T_FROM_PLATFORM_STRING(socialGroup),
            maxItems,
            UtilsWinRT::CreateStdVectorStringFromPlatformVectorObj(additionalColumns)));

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncOperation<LeaderboardResult^>^ 
LeaderboardService::GetLeaderboardForSocialGroupAsync(
    _In_ Platform::String^ xboxUserId,
    _In_ Platform::String^ serviceConfigurationId,
    _In_ Platform::String^ statisticName,
    _In_ Platform::String^ socialGroup,
    _In_ uint32 maxItems)
{
    auto task = PROJECT_LB(
        m_cppObj.get_leaderboard_for_social_group(
            STRING_T_FROM_PLATFORM_STRING(xboxUserId),
            STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
            STRING_T_FROM_PLATFORM_STRING(statisticName),
            STRING_T_FROM_PLATFORM_STRING(socialGroup),
            maxItems));
    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncOperation<LeaderboardResult^>^ 
LeaderboardService::GetLeaderboardForSocialGroupAsync(
    _In_ Platform::String^ xboxUserId,
    _In_ Platform::String^ serviceConfigurationId,
    _In_ Platform::String^ statisticName,
    _In_ Platform::String^ socialGroup,
    _In_ Platform::String^ sortOrder,
    _In_ uint32 maxItems)
{
    auto task = PROJECT_LB(
    m_cppObj.get_leaderboard_for_social_group(
        STRING_T_FROM_PLATFORM_STRING(xboxUserId),
        STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
        STRING_T_FROM_PLATFORM_STRING(statisticName),
        STRING_T_FROM_PLATFORM_STRING(socialGroup),
        STRING_T_FROM_PLATFORM_STRING(sortOrder),
        maxItems));

    return ASYNC_FROM_TASK(task);
}
Windows::Foundation::IAsyncOperation<LeaderboardResult^>^ 
LeaderboardService::GetLeaderboardForSocialGroupWithSkipToRankAsync(
    _In_ Platform::String^ xboxUserId,
    _In_ Platform::String^ serviceConfigurationId,
    _In_ Platform::String^ statisticName,
    _In_ Platform::String^ socialGroup,
    _In_ uint32 skipToRank,
    _In_ Platform::String^ sortOrder,
    _In_ uint32 maxItems)
{
    auto task = PROJECT_LB(
        m_cppObj.get_leaderboard_for_social_group_skip_to_rank(
            STRING_T_FROM_PLATFORM_STRING(xboxUserId),
            STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
            STRING_T_FROM_PLATFORM_STRING(statisticName),
            STRING_T_FROM_PLATFORM_STRING(socialGroup),
            skipToRank,
            STRING_T_FROM_PLATFORM_STRING(sortOrder),
            maxItems));

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncOperation<LeaderboardResult^>^
LeaderboardService::GetLeaderboardForSocialGroupWithSkipToUserAsync(
    _In_ Platform::String^ xboxUserId,
    _In_ Platform::String^ serviceConfigurationId,
    _In_ Platform::String^ statisticName,
    _In_ Platform::String^ socialGroup,
    _In_opt_ Platform::String^ skipToXboxUserId,
    _In_ Platform::String^ sortOrder,
    _In_ uint32 maxItems)
{
    auto task = PROJECT_LB(
        m_cppObj.get_leaderboard_for_social_group_skip_to_xuid(
            STRING_T_FROM_PLATFORM_STRING(xboxUserId),
            STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
            STRING_T_FROM_PLATFORM_STRING(statisticName),
            STRING_T_FROM_PLATFORM_STRING(socialGroup),
            STRING_T_FROM_PLATFORM_STRING(skipToXboxUserId),
            STRING_T_FROM_PLATFORM_STRING(sortOrder),
            maxItems));

    return ASYNC_FROM_TASK(task);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_END