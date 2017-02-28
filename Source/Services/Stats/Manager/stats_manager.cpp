// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/stats_manager.h"
#include "stats_manager_internal.h"
#include "xsapi/services.h"
#include "xsapi/system.h"
#include "xbox_live_context_impl.h"

using namespace xbox::services;
using namespace xbox::services::system;

NAMESPACE_MICROSOFT_XBOX_SERVICES_STAT_MANAGER_CPP_BEGIN

std::shared_ptr<stats_manager>
stats_manager::get_singleton_instance()
{
    static std::mutex s_singletonLock;
    std::lock_guard<std::mutex> guard(s_singletonLock);
    static std::shared_ptr<stats_manager> instance = std::make_shared<stats_manager>();
    return instance;
}

stats_manager::stats_manager()
{
    m_statsManagerImpl = std::make_shared<stats_manager_impl>();
    m_statsManagerImpl->initialize();
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
    _In_ const string_t& statName,
    _In_ int64_t statValue
    )
{
    return m_statsManagerImpl->set_stat(
        user,
        statName,
        static_cast<double>(statValue)
        );
}

xbox_live_result<void>
stats_manager::set_stat_as_number(
    _In_ const xbox_live_user_t& user,
    _In_ const string_t& statName,
    _In_ double statValue
    )
{
    return m_statsManagerImpl->set_stat(
        user,
        statName,
        statValue
        );
}

xbox_live_result<void>
stats_manager::set_stat_as_string(
    _In_ const xbox_live_user_t& user,
    _In_ const string_t& statName,
    _In_ const string_t& statValue
    )
{
    return m_statsManagerImpl->set_stat(
        user,
        statName,
        statValue.c_str()
        );
}

xbox_live_result<stat_value>
stats_manager::get_stat(
    _In_ const xbox_live_user_t& user,
    _In_ const string_t& statName
    )
{
    return m_statsManagerImpl->get_stat(
        user,
        statName
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

xbox_live_result<void>
stats_manager::delete_stat(
    _In_ const xbox_live_user_t& user,
    _In_ const string_t& statName
    )
{
    return m_statsManagerImpl->delete_stat(
        user,
        statName
        );
}

xbox_live_result<void> stats_manager::get_leaderboard(
    const xbox_live_user_t& user, 
    const string_t& statName, 
    leaderboard::leaderboard_query query
    )
{
    return m_statsManagerImpl->get_leaderboard(
        user,
        statName,
        query
        );
}

xbox_live_result<void> stats_manager::get_social_leaderboard(
    const xbox_live_user_t& user, 
    const string_t& statName, 
    const string_t& socialGroup, 
    leaderboard::leaderboard_query query
    )
{
    return m_statsManagerImpl->get_social_leaderboard(
        user,
        statName,
        socialGroup,
        query
    );
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_STAT_MANAGER_CPP_END