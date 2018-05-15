// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "GameLogic\Game.h"
#include "Common\DirectXHelper.h"
#include "Utils\PerformanceCounters.h"
#include "httpClient\httpClient.h"
#include "xsapi\services.h"

using namespace Sample;
using namespace Windows::Foundation;
using namespace Windows::System::Threading;
using namespace Concurrency;

void Game::AddUserToSocialManager(xbl_user_handle user)
{
    stringstream_t source;
    source << _T("Adding user ");
    source << GetGamertag();
    source << _T(" to SocialManager");
    Log(source.str());

    XblSocialManagerAddLocalUser(user, XblSocialManagerExtraDetailLevel_All);
    xbox::services::system::xbox_live_services_settings::get_singleton_instance()->set_diagnostics_trace_level(xbox::services::xbox_services_diagnostics_trace_level::verbose);

    CreateSocialGroupsBasedOnUI(user);
}

void Game::RemoveUserFromSocialManager(xbl_user_handle user)
{
    stringstream_t source;
    source << _T("Removing user ") << GetGamertag() << _T(" from SocialManager");
    Log(source.str());

    auto it = m_socialGroups.begin();
    while (it != m_socialGroups.end())
    {
        uint64_t xuid;
        XblUserGetXboxUserId((*it)->localUser, &xuid);
        if (m_xuid == xuid)
        {
            it = m_socialGroups.erase(it);
        }
        else
        {
            ++it;
        }
    }
    XblSocialManagerRemoveLocalUser(user);
}

void Game::CreateOrUpdateSocialGroupFromList(
    xbl_user_handle user,
    std::vector<uint64_t> xuidList
    )
{
    for (auto group : m_socialGroups)
    {
        if (group->socialUserGroupType == XblSocialUserGroupType_UserListType && group->localUser == user)
        {
            XblSocialManagerUpdateSocialUserGroup(group, xuidList.data(), (uint32_t)xuidList.size());
            return;
        }
    }

    if (xuidList.size() > 0)
    {
        XblSocialManagerUserGroup* newGroup;
        auto hr = XblSocialManagerCreateSocialUserGroupFromList(user, xuidList.data(), (uint32_t)xuidList.size(), &newGroup);
        if (SUCCEEDED(hr))
        {
            m_socialGroups.push_back(newGroup);
        }
    }
}

void Game::DestroySocialGroup(xbl_user_handle user)
{
    auto it = m_socialGroups.begin();
    while (it != m_socialGroups.end())
    {
        if ((*it)->localUser == user && (*it)->socialUserGroupType == XblSocialUserGroupType_UserListType)
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

void Game::CreateSocialUserGroupFromFilters(
    xbl_user_handle user,
    XblPresenceFilter presenceFilter,
    XblRelationshipFilter relationshipFilter
    )
{
    XblSocialManagerUserGroup *newGroup;
    auto hr = XblSocialManagerCreateSocialUserGroupFromFilters(user, presenceFilter, relationshipFilter, &newGroup);

    if (SUCCEEDED(hr))
    {
        m_socialGroups.push_back(newGroup);
    }
}

void Game::DestroySocialGroup(
    xbl_user_handle user,
    XblPresenceFilter presenceFilter,
    XblRelationshipFilter relationshipFilter
    )
{
    auto it = m_socialGroups.begin();
    while (it != m_socialGroups.end())
    {
        auto group = *it;
        if (group->localUser == user &&
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

void Game::UpdateSocialManager()
{
#if PERF_COUNTERS
    auto perfInstance = performance_counters::get_singleton_instance();
    perfInstance->begin_capture(L"no_updates");
    perfInstance->begin_capture(L"updates");
#endif
    XblSocialManagerEvent* events;
    uint32_t eventCount;
    auto hr =  XblSocialManagerDoWork(&events, &eventCount);
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

std::vector<XblSocialManagerUserGroup*> Game::GetSocialGroups()
{
    return m_socialGroups;
}