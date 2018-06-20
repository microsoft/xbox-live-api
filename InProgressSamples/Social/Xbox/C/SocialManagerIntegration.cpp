// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include <time.h>
#include "Support\Game.h"
#include "Support\PerformanceCounters.h"
#include "AsyncIntegration.h"

using namespace Concurrency;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;
using namespace xbox::services;
using namespace xbox::services::social::manager;

IInspectable* AsInspectable(Platform::Object^ object)
{
    return reinterpret_cast<IInspectable*>(object);
}

void
Game::InitializeXboxLive()
{
    uint32_t sharedAsyncQueueId = 0;
    CreateSharedAsyncQueue(
        sharedAsyncQueueId,
        AsyncQueueDispatchMode::AsyncQueueDispatchMode_Manual,
        AsyncQueueDispatchMode::AsyncQueueDispatchMode_Manual,
        &m_queue);

    XblInitialize();
    InitializeAsync(m_queue, &m_asyncQueueCallbackToken);
}

void 
Game::CleanupXboxLive()
{
    CleanupAsync(m_queue, m_asyncQueueCallbackToken);

    XblCleanup();
}

void
Game::InitializeSocialManager(Windows::Foundation::Collections::IVectorView<Windows::Xbox::System::User^>^ userList)
{
    for (Windows::Xbox::System::User^ user : userList)
    {
        AddUserToSocialManager(user);
    }
}

void
Game::AddUserToSocialManager(
    _In_ Windows::Xbox::System::User^ user
    )
{
    {
        std::lock_guard<std::mutex> guard(m_socialManagerLock);

        stringstream_t source;
        source << _T("Adding user ");
        source << user->DisplayInfo->Gamertag->Data();
        source << _T(" to SocialManager");
        Log(source.str());

        XblSocialManagerAddLocalUser(AsInspectable(user), XblSocialManagerExtraDetailLevel_All);
        m_userAdded = true;
    }

    CreateSocialGroupsBasedOnUI(user);
}

void
Game::RemoveUserFromSocialManager(
    _In_ Windows::Xbox::System::User^ user
    )
{
    std::lock_guard<std::mutex> guard(m_socialManagerLock);

    stringstream_t source;
    source << _T("Removing user ");
    source << user->DisplayInfo->Gamertag->Data();
    source << _T(" from SocialManager");
    Log(source.str());

    auto inspectable = AsInspectable(user);

    auto it = m_socialGroups.begin();
    while (it != m_socialGroups.end()) 
    {
        if ((*it)->localUser == inspectable)
        {
            it = m_socialGroups.erase(it);
        }
        else 
        {
            ++it;
        }
    }
    XblSocialManagerRemoveLocalUser(inspectable);
}

void 
Game::CreateSocialGroupFromList(
    _In_ Windows::Xbox::System::User^ user,
    _In_ std::vector<uint64_t> xuidList
    )
{
    for (auto group : m_socialGroups)
    {
        if (group->socialUserGroupType == XblSocialUserGroupType_UserListType && group->localUser == AsInspectable(user))
        {
            XblSocialManagerUpdateSocialUserGroup(group, xuidList.data(), (uint32_t)xuidList.size());
            return;
        }
    }

    if( xuidList.size() > 0 )
    {
        XblSocialManagerUserGroup *newGroup;
        auto hr = XblSocialManagerCreateSocialUserGroupFromList(AsInspectable(user), xuidList.data(), (uint32_t)xuidList.size(), &newGroup);
        if (SUCCEEDED(hr))
        {
            m_socialGroups.push_back(newGroup);
        }
    }
}

void
Game::CreateSocialGroupFromFilters(
    _In_ Windows::Xbox::System::User^ user,
    _In_ XblPresenceFilter presenceFilter,
    _In_ XblRelationshipFilter relationshipFilter
    )
{
    XblSocialManagerUserGroup *newGroup;
    auto hr = XblSocialManagerCreateSocialUserGroupFromFilters(AsInspectable(user), presenceFilter, relationshipFilter, &newGroup);

    if (SUCCEEDED(hr))
    {
        m_socialGroups.push_back(newGroup);
    }
}

void
Game::DestorySocialGroup(
    _In_ Windows::Xbox::System::User^ user
    )
{
    std::lock_guard<std::mutex> guard(m_socialManagerLock);

    auto it = m_socialGroups.begin();
    while (it != m_socialGroups.end())
    {
        if ((*it)->localUser == AsInspectable(user) && (*it)->socialUserGroupType == XblSocialUserGroupType_UserListType)
        {
            XblSocialManagerDestroySocialUserGroup(*it);
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
    _In_ Windows::Xbox::System::User^ user,
    _In_ XblPresenceFilter presenceFilter,
    _In_ XblRelationshipFilter relationshipFilter
    )
{
    std::lock_guard<std::mutex> guard(m_socialManagerLock);

    auto it = m_socialGroups.begin();
    while (it != m_socialGroups.end())
    {
        auto group = *it;
        if (group->localUser == AsInspectable(user) &&
            group->presenceFilterOfGroup == presenceFilter &&
            group->relationshipFilterOfGroup == relationshipFilter)
        {
            XblSocialManagerDestroySocialUserGroup(*it);
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
    XblSocialManagerEvent* events;
    uint32_t eventCount;
    XblSocialManagerDoWork(&events, &eventCount);
#if PERF_COUNTERS
    if (eventCount == 0)
    {
        perfInstance->end_capture(L"no_updates");
    }
    else
    {
        perfInstance->end_capture(L"updates");
    }
#endif

    LogSocialEventList(events, eventCount);
}
