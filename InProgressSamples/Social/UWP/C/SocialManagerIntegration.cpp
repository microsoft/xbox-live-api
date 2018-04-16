// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "GameLogic\Game.h"
#include "Common\DirectXHelper.h"
#include "Utils\PerformanceCounters.h"
#include "httpClient\httpClient.h"

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

    XblSocialManagerAddLocalUser(user, XBL_SOCIAL_MANAGER_EXTRA_DETAIL_LEVEL_ALL);

    CreateSocialGroupsBasedOnUI(user);
}

void Game::RemoveUserFromSocialManager(xbl_user_handle user)
{
    stringstream_t source;
    source << _T("Removing user ");
    char gamertag[GamertagMaxBytes];
    XblUserGetGamertag(user, GamertagMaxBytes, gamertag, nullptr);
    source << utility::conversions::utf8_to_utf16(gamertag);
    source << _T(" from SocialManager");
    Log(source.str());

    // TODO update this after porting social manager to new async
    //auto it = m_socialGroups.begin();
    //while (it != m_socialGroups.end())
    //{
    //    if (strcmp(m_user->xboxUserId, (*it)->localUser->xboxUserId) == 0)
    //    {
    //        it = m_socialGroups.erase(it);
    //    }
    //    else
    //    {
    //        ++it;
    //    }
    //}
    XblSocialManagerRemoveLocalUser(user);
}

void Game::CreateOrUpdateSocialGroupFromList(
    xbl_user_handle user,
    std::vector<std::string> xuidList
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

    if (xuidList.size() > 0)
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

void Game::DestroySocialGroup(xbl_user_handle user)
{
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

void Game::CreateSocialUserGroupFromFilters(
    xbl_user_handle user,
    XBL_PRESENCE_FILTER presenceFilter,
    XBL_RELATIONSHIP_FILTER relationshipFilter
    )
{
    XBL_XBOX_SOCIAL_USER_GROUP *newGroup;
    auto result = XblSocialManagerCreateSocialUserGroupFromFilters(user, presenceFilter, relationshipFilter, &newGroup);

    if (!result.errorCondition)
    {
        m_socialGroups.push_back(newGroup);
    }
}

void Game::DestroySocialGroup(
    xbl_user_handle user,
    XBL_PRESENCE_FILTER presenceFilter,
    XBL_RELATIONSHIP_FILTER relationshipFilter
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