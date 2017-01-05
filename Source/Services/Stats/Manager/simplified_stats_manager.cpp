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
#include "xsapi/simple_stats.h"
#include "simplified_stats_internal.h"
#include "xsapi/services.h"
#include "xsapi/system.h"
#include "xbox_live_context_impl.h"

using namespace xbox::services;
using namespace xbox::services::system;

NAMESPACE_MICROSOFT_XBOX_SERVICES_STAT_MANAGER_CPP_BEGIN

stats_manager&
stats_manager::get_singleton_instance()
{
    static std::mutex s_singletonLock;
    std::lock_guard<std::mutex> guard(s_singletonLock);
    static stats_manager instance;
    return instance;
}

stats_manager::stats_manager()
{
    m_statsManagerImpl = std::make_shared<stats_manager_impl>();
}

xbox_live_result<void>
stats_manager::add_local_user(
    _In_ const xbox_live_user_t& user
)
{
    return m_statsManagerImpl->add_local_user(user);
}

xbox_live_result<void>
stats_manager::remove_local_user(
    _In_ const xbox_live_user_t& user
)
{
    return m_statsManagerImpl->remove_local_user(user);
}

xbox_live_result<void>
stats_manager::request_flush_to_service(
    _In_ const xbox_live_user_t& user,
    _In_ bool isHighPriority
    )
{
    return m_statsManagerImpl->request_flush_to_service(
        user,
        isHighPriority
        );
}

std::vector<stat_event>
stats_manager::do_work()
{
    return m_statsManagerImpl->do_work();
}

xbox_live_result<void>
stats_manager::set_stat_as_integer(
    _In_ const xbox_live_user_t& user,
    _In_ const string_t& name,
    _In_ int64_t value,
    _In_ stat_compare_type statisticReplaceCompareType
    )
{
    return m_statsManagerImpl->set_stat(
        user,
        name,
        static_cast<double>(value),
        statisticReplaceCompareType
        );
}

xbox_live_result<void>
stats_manager::set_stat_as_number(
    _In_ const xbox_live_user_t& user,
    _In_ const string_t& name,
    _In_ double value,
    _In_ stat_compare_type statisticReplaceCompareType
)
{
    return m_statsManagerImpl->set_stat(
        user,
        name,
        value,
        statisticReplaceCompareType
        );
}

xbox_live_result<void>
stats_manager::set_stat_as_string(
    _In_ const xbox_live_user_t& user,
    _In_ const string_t& name,
    _In_ const string_t& value
)
{
    return m_statsManagerImpl->set_stat(
        user,
        name,
        value.c_str()
        );
}

xbox_live_result<std::shared_ptr<stat_value>>
stats_manager::get_stat(
    _In_ const xbox_live_user_t& user,
    _In_ const string_t& name
    )
{
    return m_statsManagerImpl->get_stat(
        user,
        name
        );
}

xbox_live_result<void>
stats_manager::get_stat_names(
    _In_ const xbox_live_user_t& user,
    _Inout_ std::vector<string_t>& statNameList
    )
{
    return m_statsManagerImpl->get_stat_names(
        user,
        statNameList
        );
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_STAT_MANAGER_CPP_END