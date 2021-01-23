// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "social_manager_internal.h"
#include "social_manager_user_group.h"
#include "social_graph.h"
#include "perf_tester.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_BEGIN

SocialManager::SocialManager() noexcept
{
    // MAX_GRAPH_UPDATES_PER_FRAME doesn't directly map to a number XblSocialManagerEvents, but it is correlated so
    // we can use it to preallocate some memory for events nonetheless
    m_events.reserve(MAX_GRAPH_UPDATES_PER_FRAME * 2);
    m_affectedUsersLifetime.reserve(m_events.capacity() * std::extent<decltype(XblSocialManagerEvent::usersAffected)>::value);
}

HRESULT SocialManager::AddLocalUser(
    User&& user,
    XblSocialManagerExtraDetailLevel detailLevel,
    TaskQueue&& queue
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutex };

    auto xuid{ user.Xuid() };
    if (m_graphs.find(xuid) != m_graphs.end())
    {
        LOGS_ERROR << "User " << xuid << " already added to SocialManager";
        return E_UNEXPECTED;
    }

    auto socialGraph = SocialGraph::Make(std::move(user), detailLevel, queue, GlobalState::Get()->RTAManager());
    RETURN_HR_IF_FAILED(socialGraph.Hresult());

    m_graphs[xuid] = socialGraph.ExtractPayload();
    return S_OK;
}

HRESULT SocialManager::RemoveLocalUser(
    const User& user
) noexcept
{
    // Destroy graph outside lock so DoWork thread isn't blocked
    std::shared_ptr<SocialGraph> graph{ nullptr };
    std::lock_guard<std::mutex> lock{ m_mutex };

    auto xuid = user.Xuid();
    auto graphIter{ m_graphs.find(xuid) };
    if (graphIter == m_graphs.end())
    {
        LOGS_ERROR << "User " << user.Xuid() << " was not added to SocialManager";
        return E_UNEXPECTED;
    }
    graph = std::move(graphIter->second);
    m_graphs.erase(graphIter);

    // Ensure lifetime of User object since it is referenced in returned events
    m_removedUsersLifetime.push_back(graph->LocalUser());

    // When a user is removed, also destroy any groups associated with the user
    for (auto iter = m_groups.begin(); iter != m_groups.end();)
    {
        if (iter->second->LocalUser()->Xuid() == user.Xuid())
        {
            iter = m_groups.erase(iter);
        }
        else
        {
            ++iter;
        }
    }

    return S_OK;
}

Result<std::shared_ptr<XblSocialManagerUserGroup>> SocialManager::CreateUserGroup(
    const User& user,
    XblPresenceFilter presenceFilter,
    XblRelationshipFilter relationshipFilter
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutex };

    auto graphIter = m_graphs.find(user.Xuid());
    if (graphIter == m_graphs.end())
    {
        LOGS_ERROR << "Add user " << user.Xuid() << " to SocialManager before creating a user group for them";
        return E_UNEXPECTED;
    }

    auto group = XblSocialManagerUserGroup::Make(graphIter->second, presenceFilter, relationshipFilter);
    m_groups[group.get()] = group;
    return group;
}

Result<std::shared_ptr<XblSocialManagerUserGroup>> SocialManager::CreateUserGroup(
    const xbox::services::User& user,
    Vector<uint64_t>&& trackedUsers
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutex };

    auto graphIter = m_graphs.find(user.Xuid());
    if (graphIter == m_graphs.end())
    {
        LOGS_ERROR << "Add user " << user.Xuid() << " to SocialManager before creating a user group for them";
        return E_UNEXPECTED;
    }

    auto group = XblSocialManagerUserGroup::Make(graphIter->second, std::move(trackedUsers));
    m_groups[group.get()] = group;
    return group;
}

std::shared_ptr<XblSocialManagerUserGroup> SocialManager::GetUserGroup(
    XblSocialManagerUserGroupHandle groupHandle
) const
{
    std::lock_guard<std::mutex> lock{ m_mutex };

    auto groupIter = m_groups.find(groupHandle);
    if (groupIter == m_groups.end())
    {
        LOGS_ERROR << "Group is not valid. It was destroyed with XblSocialManagerDestroyUserGroup or the associated local user was removed.";
        return nullptr;
    }

    return groupIter->second;
}

HRESULT SocialManager::DestroyUserGroup(
    XblSocialManagerUserGroupHandle groupHandle
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutex };

    auto groupIter = m_groups.find(groupHandle);
    if (groupIter == m_groups.end())
    {
        LOGS_ERROR << "Group handle provided is invalid.";
        return E_FAIL;
    }

    auto xuid{ groupIter->second->LocalUser()->Xuid() };
    auto graphIter = m_graphs.find(xuid);
    if (graphIter == m_graphs.end())
    {
        LOGS_ERROR << "Add user " << xuid << " to SocialManager before creating a user group for them";
        return E_UNEXPECTED;
    }

    graphIter->second->UnregisterGroup(groupIter->second);
    m_groups.erase(groupHandle);

    return S_OK;
}

const Vector<XblSocialManagerEvent>& SocialManager::DoWork() noexcept
{
    PERF_START();
    std::unique_lock<std::mutex> eventsLock{ m_eventsMutex };

    m_events.clear();
    m_affectedUsersLifetime.clear();
    m_removedUsersLifetime.clear();

    // For performance reasons, don't wait for the graph mutex if another thread holds it.
    std::unique_lock<std::mutex> graphLock{ m_mutex, std::defer_lock };
    if (graphLock.try_lock())
    {
        for (auto& pair : m_graphs)
        {
            pair.second->DoWork(m_events, m_affectedUsersLifetime);
        }
    }

    PERF_STOP();
    return m_events;
}

HRESULT SocialManager::SetRichPresencePolling(
    const User& user,
    bool enabled
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutex };

    auto graphIter = m_graphs.find(user.Xuid());
    if (graphIter == m_graphs.end())
    {
        LOGS_ERROR << "Add user " << user.Xuid() << " to SocialManager before creating a user group for them";
        return E_UNEXPECTED;
    }

    graphIter->second->SetRichPresencePolling(enabled);
    return S_OK;
}

size_t SocialManager::LocalUserCount() const noexcept
{
    return m_graphs.size();
}

HRESULT SocialManager::GetLocalUsers(
    _In_ size_t usersCount,
    _Out_writes_(usersCount) XblUserHandle* users
) const noexcept
{
    RETURN_HR_INVALIDARGUMENT_IF(usersCount < m_graphs.size());
    RETURN_HR_INVALIDARGUMENT_IF(users == nullptr && usersCount > 0);

    std::lock_guard<std::mutex> lock{ m_mutex };

    size_t index{ 0 };
    for (auto& pair : m_graphs)
    {
        users[index++] = pair.second->LocalUser()->Handle();
    }
    return S_OK;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_END