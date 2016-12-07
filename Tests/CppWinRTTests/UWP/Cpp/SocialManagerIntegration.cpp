// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
#include "pch.h"
#include "GameLogic\Game.h"
#include <time.h>
#include "Utils\Utils.h"
#include "Utils\PerformanceCounters.h"

using namespace Concurrency;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;
using namespace xbox::services;
using namespace xbox::services::social::manager;
using namespace Sample;

void
Game::InitializeSocialManager()
{
    m_socialManager = social_manager::get_singleton_instance();
}

void
Game::AddUserToSocialManager(
    _In_ std::shared_ptr<xbox::services::system::xbox_live_user> user
    )
{
    {
        std::lock_guard<std::mutex> guard(m_socialManagerLock);

        stringstream_t source;
        source << _T("Adding user ");
        source << user->gamertag();
        source << _T(" to SocialManager");
        Log(source.str());

        m_socialManager->add_local_user(user, social_manager_extra_detail_level::preferred_color_level | social_manager_extra_detail_level::title_history_level);
    }

    CreateSocialGroupsBasedOnUI(user);
}

void
Game::RemoveUserFromSocialManager(
    _In_ std::shared_ptr<xbox::services::system::xbox_live_user> user
    )
{
    std::lock_guard<std::mutex> guard(m_socialManagerLock);

    stringstream_t source;
    source << _T("Removing user ");
    source << user->gamertag();
    source << _T(" from SocialManager");
    Log(source.str());

    auto it = m_socialGroups.begin();
    while (it != m_socialGroups.end()) 
    {
        std::shared_ptr<xbox::services::social::manager::xbox_social_user_group> group = *it;
        if (wcscmp(group->local_user()->xbox_user_id().c_str(), user->xbox_user_id().c_str()))
        {
            it = m_socialGroups.erase(it);
        }
        else 
        {
            ++it;
        }
    }

    m_socialManager->remove_local_user(user);
}

void
Game::CreateOrUpdateSocialGroupFromList(
    _In_ std::shared_ptr<xbox::services::system::xbox_live_user> user,
    _In_ std::vector<string_t> xuidList
    )
{
    for (auto group : m_socialGroups)
    {
        if (group->social_user_group_type() == social_user_group_type::user_list_type && group->local_user()->xbox_user_id() == user->xbox_user_id())
        {
            m_socialManager->update_social_user_group(group, xuidList);
            return;
        }
    }

    if( xuidList.size() > 0 )
    {
        auto result = m_socialManager->create_social_user_group_from_list(user, xuidList);
        if (!result.err())
        {
            std::lock_guard<std::mutex> guard(m_socialManagerLock);
            m_socialGroups.push_back(result.payload());
        }
    }
}

void
Game::CreateSocialGroupFromFilters(
    _In_ std::shared_ptr<xbox::services::system::xbox_live_user> user,
    _In_ presence_filter presenceFilter,
    _In_ relationship_filter relationshipFilter
    )
{
    auto result = m_socialManager->create_social_user_group_from_filters(user, presenceFilter, relationshipFilter);
    if (!result.err())
    {
        std::lock_guard<std::mutex> guard(m_socialManagerLock);
        m_socialGroups.push_back(result.payload());
    }
}

void
Game::DestorySocialGroupFromList(
    _In_ std::shared_ptr<xbox::services::system::xbox_live_user> user
    )
{
    std::lock_guard<std::mutex> guard(m_socialManagerLock);

    auto it = m_socialGroups.begin();
    while (it != m_socialGroups.end())
    {
        std::shared_ptr<xbox::services::social::manager::xbox_social_user_group> group = *it;
        if (wcscmp(group->local_user()->xbox_user_id().c_str(), user->xbox_user_id().c_str()) == 0 &&
            group->social_user_group_type() == social_user_group_type::user_list_type)
        {
            m_socialManager->destroy_social_user_group(group);
            it = m_socialGroups.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void
Game::DestroySocialGroup(
    _In_ std::shared_ptr<xbox::services::system::xbox_live_user> user,
    _In_ presence_filter presenceFilter,
    _In_ relationship_filter relationshipFilter
    )
{
    std::lock_guard<std::mutex> guard(m_socialManagerLock);

    auto it = m_socialGroups.begin();
    while (it != m_socialGroups.end())
    {
        std::shared_ptr<xbox::services::social::manager::xbox_social_user_group> group = *it;
        if (wcscmp(group->local_user()->xbox_user_id().c_str(), user->xbox_user_id().c_str()) == 0 &&
            group->presence_filter_of_group() == presenceFilter && 
            group->relationship_filter_of_group() == relationshipFilter)
        {
            m_socialManager->destroy_social_user_group(group);
            it = m_socialGroups.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void
Game::UpdateSocialManager()
{
    std::lock_guard<std::mutex> guard(m_socialManagerLock);

#if PERF_COUNTERS
    auto perfInstance = performance_counters::get_singleton_instance();
    perfInstance->begin_capture(L"no_updates");
    perfInstance->begin_capture(L"updates");
#endif
    auto socialEvents = m_socialManager->do_work();
#if PERF_COUNTERS
    if (socialEvents.empty())
    {
        perfInstance->end_capture(L"no_updates");
    }
    else
    {
        perfInstance->end_capture(L"updates");
    }
#endif

    LogSocialEventList(socialEvents);
}

std::vector<std::shared_ptr<xbox::services::social::manager::xbox_social_user_group>> Game::GetSocialGroups()
{
    std::lock_guard<std::mutex> guard(m_socialManagerLock);
    return m_socialGroups;
}