// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "user_context.h"
#include "StatisticManager_WinRT.h"

using namespace xbox::services;
using namespace xbox::services::stats::manager;
using namespace Microsoft::Xbox::Services::System;

NAMESPACE_MICROSOFT_XBOX_SERVICES_STATISTIC_MANAGER_BEGIN

StatisticManager^
StatisticManager::SingletonInstance::get()
{
    auto xsapiSingleton = get_xsapi_singleton();
    if (xsapiSingleton->m_winrt_statisticManagerInstance == nullptr)
    {
        StatisticManager^ statisticManager = ref new StatisticManager();
        {
            std::lock_guard<std::mutex> lock(xsapiSingleton->m_singletonLock);
            if (xsapiSingleton->m_winrt_statisticManagerInstance == nullptr)
            {
                xsapiSingleton->m_winrt_statisticManagerInstance = statisticManager;
            }
        }
    }
    return xsapiSingleton->m_winrt_statisticManagerInstance;
}

StatisticManager::StatisticManager()
{
    m_cppObj = stats_manager::get_singleton_instance();
}

void StatisticManager::AddLocalUser(
    _In_ XboxLiveUser_t user
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(user);
    auto result = m_cppObj->add_local_user(
        user_context::user_convert(user)
        );

    THROW_IF_ERR(result);
}

void
StatisticManager::RemoveLocalUser(
    _In_ XboxLiveUser_t user
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(user);
    auto result = m_cppObj->remove_local_user(
        user_context::user_convert(user)
        );

    THROW_IF_ERR(result);
}

void
StatisticManager::RequestFlushToService(
    _In_ XboxLiveUser_t user
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(user);
    auto result = m_cppObj->request_flush_to_service(
        user_context::user_convert(user)
        );

    THROW_IF_ERR(result);
}

void
StatisticManager::RequestFlushToService(
    _In_ XboxLiveUser_t user,
    _In_ bool isHighPriority
)
{
    THROW_INVALIDARGUMENT_IF_NULL(user);
    auto result = m_cppObj->request_flush_to_service(
        user_context::user_convert(user),
        isHighPriority
        );

    THROW_IF_ERR(result);
}

Windows::Foundation::Collections::IVectorView<StatisticEvent^>^
StatisticManager::DoWork()
{
    auto eventList = m_cppObj->do_work();
    return UtilsWinRT::CreatePlatformVectorFromStdVectorObj<StatisticEvent>(eventList)->GetView();
}

void
StatisticManager::SetStatisticNumberData(
    _In_ XboxLiveUser_t user,
    _In_ Platform::String^ name,
    _In_ double value
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(user);
    THROW_INVALIDARGUMENT_IF_NULL(name);

    auto result = m_cppObj->set_stat_as_number(
        user_context::user_convert(user),
        STRING_T_FROM_PLATFORM_STRING(name),
        value
        );

    THROW_IF_ERR(result);
}

void
StatisticManager::SetStatisticIntegerData(
    _In_ XboxLiveUser_t user,
    _In_ Platform::String^ name,
    _In_ int64_t value
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(user);
    THROW_INVALIDARGUMENT_IF_NULL(name);

    auto result = m_cppObj->set_stat_as_integer(
        user_context::user_convert(user),
        STRING_T_FROM_PLATFORM_STRING(name),
        value
        );

    THROW_IF_ERR(result);
}

void
StatisticManager::SetStatisticStringData(
    _In_ XboxLiveUser_t user,
    _In_ Platform::String^ name,
    _In_ Platform::String^ value
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(user);
    THROW_CPP_INVALIDARGUMENT_IF_NULL(name);
    THROW_CPP_INVALIDARGUMENT_IF_NULL(value);

    auto valueStr = STRING_T_FROM_PLATFORM_STRING(value);
    auto result = m_cppObj->set_stat_as_string(
        user_context::user_convert(user),
        STRING_T_FROM_PLATFORM_STRING(name),
        valueStr.c_str()
        );

    THROW_IF_ERR(result);
}

Windows::Foundation::Collections::IVectorView<Platform::String^>^
StatisticManager::GetStatisticNames(
    _In_ XboxLiveUser_t user
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(user);
    std::vector<string_t> statNamesVec;
    auto result = m_cppObj->get_stat_names(
        user_context::user_convert(user),
        statNamesVec
        );

    THROW_IF_ERR(result);

    return UtilsWinRT::CreatePlatformVectorFromStdVectorString(statNamesVec)->GetView();
}

StatisticValue^
StatisticManager::GetStatistic(
    _In_ XboxLiveUser_t user,
    _In_ Platform::String^ name
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(user);
    THROW_INVALIDARGUMENT_IF_NULL(name);

    auto result = m_cppObj->get_stat(
        user_context::user_convert(user),
        STRING_T_FROM_PLATFORM_STRING(name)
        );

    THROW_IF_ERR(result);
    return ref new StatisticValue(result.payload());
}

void
StatisticManager::DeleteStatistic(
    _In_ XboxLiveUser_t user,
    _In_ Platform::String^ name
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(user);
    THROW_INVALIDARGUMENT_IF_NULL(name);

    auto result = m_cppObj->delete_stat(
        user_context::user_convert(user),
        STRING_T_FROM_PLATFORM_STRING(name)
        );

    THROW_IF_ERR(result);
}

void StatisticManager::GetLeaderboard(
    _In_ XboxLiveUser_t user,
    _In_ Platform::String^ statName,
    _In_ Leaderboard::LeaderboardQuery^ query
    )
{
    auto result = m_cppObj->get_leaderboard(
        user_context::user_convert(user),
        STRING_T_FROM_PLATFORM_STRING(statName),
        query->GetCppObj()
    );
    THROW_IF_ERR(result);
}

void StatisticManager::GetSocialLeaderboard(
    _In_ XboxLiveUser_t user,
    _In_ Platform::String^ statName,
    _In_ Platform::String^ socialGroup,
    _In_ Leaderboard::LeaderboardQuery^ query
    )
{
    auto result = m_cppObj->get_social_leaderboard(
        user_context::user_convert(user),
        STRING_T_FROM_PLATFORM_STRING(statName),
        STRING_T_FROM_PLATFORM_STRING(socialGroup),
        query->GetCppObj()
    );
    THROW_IF_ERR(result);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_STATISTIC_MANAGER_END