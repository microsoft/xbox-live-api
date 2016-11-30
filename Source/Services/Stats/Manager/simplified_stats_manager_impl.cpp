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

xbox_live_result<void>
stats_manager_impl::add_local_user(
    _In_ const xbox_live_user_t& user
)
{
    std::lock_guard<std::mutex> guard(m_statsServiceMutex);
    string_t userStr(user->xbox_user_id().c_str());
    auto userIter = m_users.find(userStr);
    if (userIter != m_users.end())
    {
        return xbox_live_result<void>(xbox_live_error_code::invalid_argument, "User already in local map");
    }

    auto xboxLiveContextImpl = std::make_shared<xbox_live_context_impl>(user);
    xboxLiveContextImpl->user_context()->set_caller_context_type(caller_context_type::stats_manager);
    xboxLiveContextImpl->init();
    auto simplifiedStatsService = simplified_stats_service(
        xboxLiveContextImpl->user_context(),
        xboxLiveContextImpl->settings(),
        xboxLiveContextImpl->application_config()
        );

    std::weak_ptr<stats_manager_impl> thisWeak = shared_from_this();
    simplifiedStatsService.get_stats_value_document()
    .then([thisWeak, user, xboxLiveContextImpl, simplifiedStatsService](xbox_live_result<stats_value_document> statsValueDocResult)
    {
        std::shared_ptr<stats_manager_impl> pThis(thisWeak.lock());
        if (pThis == nullptr)
        {
            return;
        }

        std::lock_guard<std::mutex> guard(pThis->m_statsServiceMutex);
        if (!statsValueDocResult.err())
        {
            auto xuid = user->xbox_user_id().c_str();
            pThis->m_users[xuid] = stats_user_context(statsValueDocResult.payload(), xboxLiveContextImpl, simplifiedStatsService);
        }
        else
        {
            // log error, add local user failed event
        }

        pThis->m_statEventList.push_back(stat_event(stat_event_type::local_user_added, user, xbox_live_result<void>(statsValueDocResult.err(), statsValueDocResult.err_message())));
    });

    return xbox_live_result<void>();
}

xbox_live_result<void>
stats_manager_impl::remove_local_user(
    _In_ const xbox_live_user_t& user
)
{
    std::lock_guard<std::mutex> guard(m_statsServiceMutex);
    string_t userStr(user->xbox_user_id().c_str());
    auto userIter = m_users.find(userStr);
    if (userIter == m_users.end())
    {
        return xbox_live_result<void>(xbox_live_error_code::invalid_argument, "User not found in local map");
    }

    auto userSVD = userIter->second.statValueDocument;
    m_users.erase(userIter);
    if (userSVD.is_dirty())
    {
        std::weak_ptr<stats_manager_impl> thisWeak = shared_from_this();
        userIter->second.simplifiedStatsService.update_stats_value_document(userSVD)
        .then([thisWeak, userSVD](xbox_live_result<void> updateSVDResult)
        {
            std::shared_ptr<stats_manager_impl> pThis(thisWeak.lock());
            if (pThis == nullptr)
            {
                return;
            }

            if (updateSVDResult.err())
            {
                // write doc offline
            }
            else
            {
                // write remove local user event
            }
        });
    }
    else
    {
        m_statEventList.push_back(stat_event(stat_event_type::local_user_removed, user, xbox_live_result<void>()));
    }
    return xbox_live_result<void>();
}

xbox_live_result<void>
stats_manager_impl::request_flush_to_service(
    _In_ const xbox_live_user_t& user,
    _In_ bool isHighPriority
    )
{
    std::lock_guard<std::mutex> guard(m_statsServiceMutex);
    string_t userStr(user->xbox_user_id().c_str());
    auto userIter = m_users.find(userStr);
    if (userIter == m_users.end())
    {
        return xbox_live_result<void>(xbox_live_error_code::invalid_argument, "User not found in local map");
    }

    auto& userSVD = userIter->second.statValueDocument;
    if (userSVD.is_dirty())
    {
        std::weak_ptr<stats_manager_impl> thisWeak = shared_from_this();
        userIter->second.simplifiedStatsService.update_stats_value_document(userSVD)
        .then([thisWeak, userSVD](xbox_live_result<void> updateSVDResult)
        {
            std::shared_ptr<stats_manager_impl> pThis(thisWeak.lock());
            if (pThis == nullptr)
            {
                return;
            }

            if (updateSVDResult.err())
            {
                // TODO: write doc offline
            }
            else
            {
                // log
            }
        });
    }

    return xbox_live_result<void>();
}

std::vector<stat_event>
stats_manager_impl::do_work()
{
    std::lock_guard<std::mutex> guard(m_statsServiceMutex);
    auto copyList = m_statEventList;
    m_statEventList.clear();
    return copyList;
}

xbox_live_result<void>
stats_manager_impl::set_stat(
    _In_ const xbox_live_user_t& user,
    _In_ const string_t& name,
    _In_ double value,
    _In_ stat_compare_type statisticReplaceCompareType
)
{
    std::lock_guard<std::mutex> guard(m_statsServiceMutex);
    string_t userStr(user->xbox_user_id().c_str());
    auto userIter = m_users.find(userStr);
    if (userIter == m_users.end())
    {
        return xbox_live_result<void>(xbox_live_error_code::invalid_argument, "User not found in local map");
    }

    return userIter->second.statValueDocument.set_stat(name.c_str(), value);
}

xbox_live_result<void>
stats_manager_impl::set_stat(
    _In_ const xbox_live_user_t& user,
    _In_ const string_t& name,
    _In_ const char_t* value
)
{
    std::lock_guard<std::mutex> guard(m_statsServiceMutex);
    string_t userStr(user->xbox_user_id().c_str());
    auto userIter = m_users.find(userStr);
    if (userIter == m_users.end())
    {
        return xbox_live_result<void>(xbox_live_error_code::invalid_argument, "User not found in local map");
    }

    return userIter->second.statValueDocument.set_stat(name.c_str(), value);
}

xbox_live_result<std::shared_ptr<stat_value>>
stats_manager_impl::get_stat(
    _In_ const xbox_live_user_t& user,
    _In_ const string_t& name
    )
{
    std::lock_guard<std::mutex> guard(m_statsServiceMutex);
    string_t userStr(user->xbox_user_id().c_str());
    auto userIter = m_users.find(userStr);
    if (userIter == m_users.end())
    {
        return xbox_live_result<std::shared_ptr<stat_value>>(xbox_live_error_code::invalid_argument, "User not found in local map");
    }

    return userIter->second.statValueDocument.get_stat(name.c_str());
}

xbox_live_result<void>
stats_manager_impl::get_stat_contexts(
    _In_ const xbox_live_user_t& user,
    _Inout_ std::vector<stat_context>& statisticContextList
    )
{
    std::lock_guard<std::mutex> guard(m_statsServiceMutex);
    string_t userStr(user->xbox_user_id().c_str());
    auto userIter = m_users.find(userStr);
    if (userIter == m_users.end())
    {
        return xbox_live_result<void>(xbox_live_error_code::invalid_argument, "User not found in local map");
    }

    userIter->second.statValueDocument.get_stat_contexts(statisticContextList);

    return xbox_live_result<void>();
}

xbox_live_result<void>
stats_manager_impl::get_stat_names(
    _In_ const xbox_live_user_t& user,
    _Inout_ std::vector<string_t>& statNameList
    )
{
    std::lock_guard<std::mutex> guard(m_statsServiceMutex);
    string_t userStr(user->xbox_user_id().c_str());
    auto userIter = m_users.find(userStr);
    if (userIter == m_users.end())
    {
        return xbox_live_result<void>(xbox_live_error_code::invalid_argument, "User not found in local map");
    }

    userIter->second.statValueDocument.get_stat_names(statNameList);

    return xbox_live_result<void>();
}

xbox_live_result<void>
stats_manager_impl::set_stat_contexts(
    _In_ const xbox_live_user_t& user,
    _In_ const std::vector<stat_context>& statContextList
)
{
    std::lock_guard<std::mutex> guard(m_statsServiceMutex);
    string_t userStr(user->xbox_user_id().c_str());
    auto userIter = m_users.find(userStr);
    if (userIter == m_users.end())
    {
        return xbox_live_result<void>(xbox_live_error_code::invalid_argument, "User not found in local map");
    }

    userIter->second.statValueDocument.set_stat_contexts(
        statContextList
        );

    return xbox_live_result<void>();
}

xbox_live_result<void>
stats_manager_impl::clear_stat_contexts(
    _In_ const xbox_live_user_t& user
)
{
    std::lock_guard<std::mutex> guard(m_statsServiceMutex);
    string_t userStr(user->xbox_user_id().c_str());
    auto userIter = m_users.find(userStr);
    if (userIter == m_users.end())
    {
        return xbox_live_result<void>(xbox_live_error_code::invalid_argument, "User not found in local map");
    }

    userIter->second.statValueDocument.clear_stat_contexts();

    return xbox_live_result<void>();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_STAT_MANAGER_CPP_END