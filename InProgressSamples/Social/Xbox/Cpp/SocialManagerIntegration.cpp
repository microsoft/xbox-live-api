// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include <time.h>
#include "Support\Game.h"
#include "Support\PerformanceCounters.h"

using namespace Concurrency;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;
using namespace xbox::services;
using namespace xbox::services::social::manager;

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
    // Is this needed
    //std::shared_ptr<xbox::services::xbox_live_context> xboxLiveContext = std::make_shared<xbox::services::xbox_live_context>(user);

    {
        std::lock_guard<std::mutex> guard(m_socialManagerLock);

        stringstream_t source;
        source << _T("Adding user ");
        source << user->DisplayInfo->Gamertag->Data();
        source << _T(" to SocialManager");
        Log(source.str());

        XblSocialManagerAddLocalUser(user, XBL_SOCIAL_MANAGER_EXTRA_DETAIL_LEVEL_ALL);
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

    auto it = m_socialGroups.begin();
    while (it != m_socialGroups.end()) 
    {
        if (wcscmp((*it)->localUser->XboxUserId->Data(), user->XboxUserId->Data()))
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

void 
Game::CreateSocialGroupFromList(
    _In_ Windows::Xbox::System::User^ user,
    _In_ std::vector<std::string> xuidList
    )
{
    PCSTR *xuidListArray;
    xuidListArray = new PCSTR[xuidList.size()];
    for (auto i = 0; i < xuidList.size(); ++i)
    {
        xuidListArray[i] = xuidList[i].data();
    }

    for (auto group : m_socialGroups)
    {
        if (group->socialUserGroupType == XBL_SOCIAL_USER_GROUP_TYPE_USER_LIST_TYPE && group->localUser == user)
        {
            XblSocialManagerUpdateSocialUserGroup(group, xuidListArray, (uint32_t)xuidList.size());
            delete[] xuidListArray;
            return;
        }
    }

    if( xuidList.size() > 0 )
    {
        XBL_XBOX_SOCIAL_USER_GROUP *newGroup;
        auto result = XblSocialManagerCreateSocialUserGroupFromList(user, xuidListArray, (uint32_t)xuidList.size(), &newGroup);
        if (result.errorCondition = XBL_ERROR_CONDITION_NO_ERROR)
        {
            m_socialGroups.push_back(newGroup);
        }
    }
    delete[] xuidListArray;
}

void
Game::CreateSocialGroupFromFilters(
    _In_ Windows::Xbox::System::User^ user,
    _In_ XBL_PRESENCE_FILTER presenceFilter,
    _In_ XBL_RELATIONSHIP_FILTER relationshipFilter
    )
{
    XBL_XBOX_SOCIAL_USER_GROUP *newGroup;
    auto result = XblSocialManagerCreateSocialUserGroupFromFilters(user, presenceFilter, relationshipFilter, &newGroup);

    if (!result.errorCondition)
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
        if ((*it)->localUser == user && (*it)->socialUserGroupType == XBL_SOCIAL_USER_GROUP_TYPE_USER_LIST_TYPE)
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
    _In_ XBL_PRESENCE_FILTER presenceFilter,
    _In_ XBL_RELATIONSHIP_FILTER relationshipFilter
    )
{
    std::lock_guard<std::mutex> guard(m_socialManagerLock);

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

void
Game::UpdateSocialManager()
{
    std::lock_guard<std::mutex> guard(m_socialManagerLock);

#if PERF_COUNTERS
    auto perfInstance = performance_counters::get_singleton_instance();
    perfInstance->begin_capture(L"no_updates");
    perfInstance->begin_capture(L"updates");
#endif
    uint32_t eventCount;
    auto socialEvents = XblSocialManagerDoWork(&eventCount);
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

    LogSocialEventList(socialEvents, eventCount);
}
