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
#include "user_context.h"
#include "StatisticManager_WinRT.h"

using namespace xbox::services;
using namespace xbox::services::experimental::stats::manager;
using namespace Microsoft::Xbox::Services::System;

NAMESPACE_MICROSOFT_XBOX_SERVICES_STATISTIC_MANAGER_BEGIN

StatisticManager^
StatisticManager::SingletonInstance::get()
{
    static StatisticManager^ statisticManager = ref new StatisticManager();
    return statisticManager;
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
    auto result = m_cppObj.add_local_user(
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
    auto result = m_cppObj.remove_local_user(
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
    auto result = m_cppObj.request_flush_to_service(
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
    auto result = m_cppObj.request_flush_to_service(
        user_context::user_convert(user),
        isHighPriority
        );

    THROW_IF_ERR(result);
}

Windows::Foundation::Collections::IVectorView<StatisticEvent^>^
StatisticManager::DoWork()
{
    auto& eventList = m_cppObj.do_work();
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

    auto result = m_cppObj.set_stat_as_number(
        user_context::user_convert(user),
        STRING_T_FROM_PLATFORM_STRING(name),
        value
        );

    THROW_IF_ERR(result);
}

void
StatisticManager::SetStatisticNumberData(
    _In_ XboxLiveUser_t user,
    _In_ Platform::String^ name,
    _In_ double value,
    _In_ StatisticCompareType statisticReplaceCompareType
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(user);
    THROW_INVALIDARGUMENT_IF_NULL(name);

    auto result = m_cppObj.set_stat_as_number(
        user_context::user_convert(user),
        STRING_T_FROM_PLATFORM_STRING(name),
        value,
        static_cast<stat_compare_type>(statisticReplaceCompareType)
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

    auto result = m_cppObj.set_stat_as_integer(
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
    _In_ int64_t value,
    _In_ StatisticCompareType statisticReplaceCompareType
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(user);
    THROW_INVALIDARGUMENT_IF_NULL(name);

    auto result = m_cppObj.set_stat_as_integer(
        user_context::user_convert(user),
        STRING_T_FROM_PLATFORM_STRING(name),
        value,
        static_cast<stat_compare_type>(statisticReplaceCompareType)
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
    auto result = m_cppObj.set_stat_as_string(
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
    auto result = m_cppObj.get_stat_names(
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

    auto result = m_cppObj.get_stat(
        user_context::user_convert(user),
        STRING_T_FROM_PLATFORM_STRING(name)
        );

    THROW_IF_ERR(result);
    return ref new StatisticValue(result.payload());
}

Windows::Foundation::Collections::IVectorView<StatisticContext^>^
StatisticManager::GetStatisticContexts(
    _In_ XboxLiveUser_t user
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(user);

    std::vector<stat_context> statContextVec;
    auto result = m_cppObj.get_stat_contexts(
        user_context::user_convert(user),
        statContextVec
        );

    THROW_IF_ERR(result);
    return UtilsWinRT::CreatePlatformVectorFromStdVectorObj<StatisticContext>(statContextVec)->GetView();
}

void
StatisticManager::SetStatisticContexts(
    _In_ XboxLiveUser_t user,
    _In_ Windows::Foundation::Collections::IVectorView<StatisticContext^>^ statisticContextList
)
{
    THROW_INVALIDARGUMENT_IF_NULL(user);
    THROW_INVALIDARGUMENT_IF_NULL(statisticContextList);

    auto result = m_cppObj.set_stat_contexts(
        user_context::user_convert(user),
        UtilsWinRT::CreateStdVectorObjectFromPlatformVectorObj<stat_context>(statisticContextList)
        );

    THROW_IF_ERR(result);
}

void
StatisticManager::ClearStatisticContexts(
    _In_ XboxLiveUser_t user
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(user);
    auto result = m_cppObj.clear_stat_contexts(
        user_context::user_convert(user)
        );

    THROW_IF_ERR(result);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_STATISTIC_MANAGER_END