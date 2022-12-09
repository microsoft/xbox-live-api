// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "social_manager_user_group.h"
#include "perf_tester.h"

using namespace xbox::services;
using namespace xbox::services::social::manager;

XblSocialManagerUserGroup::XblSocialManagerUserGroup(
    std::shared_ptr<xbox::services::social::manager::SocialGraph> socialGraph,
    XblPresenceFilter _presenceFilter,
    XblRelationshipFilter _relationshipFilter
) noexcept :
    type{ XblSocialUserGroupType::FilterType },
    presenceFilter{ _presenceFilter },
    relationshipFilter{ _relationshipFilter },
    m_localUser{ socialGraph->LocalUser() },
    m_graph{ socialGraph }
{
}

XblSocialManagerUserGroup::XblSocialManagerUserGroup(
    std::shared_ptr<SocialGraph> socialGraph,
    Vector<uint64_t>&& trackedUsers
) noexcept :
    type{ XblSocialUserGroupType::UserListType },
    m_localUser{ socialGraph->LocalUser() },
    m_graph{ socialGraph },
    m_trackedUsersView{ std::move(trackedUsers) },
    m_trackedUsers{ m_trackedUsersView.begin(), m_trackedUsersView.end() }
{
    m_usersView.reserve(m_trackedUsersView.size());
    socialGraph->TrackUsers(m_trackedUsersView);
}

XblSocialManagerUserGroup::~XblSocialManagerUserGroup() noexcept
{
    auto graph{ m_graph.lock() };
    if (graph && type == XblSocialUserGroupType::UserListType )
    {
        graph->StopTrackingUsers(m_trackedUsersView);
    }
}

std::shared_ptr<XblSocialManagerUserGroup> XblSocialManagerUserGroup::Make(
    std::shared_ptr<xbox::services::social::manager::SocialGraph> socialGraph,
    XblPresenceFilter presenceFilter,
    XblRelationshipFilter relationshipFilter
) noexcept
{
    auto group = std::shared_ptr<XblSocialManagerUserGroup>(
        new (Alloc(sizeof(XblSocialManagerUserGroup))) XblSocialManagerUserGroup{ socialGraph, presenceFilter, relationshipFilter },
        Deleter<XblSocialManagerUserGroup>(),
        Allocator<XblSocialManagerUserGroup>()
        );
    socialGraph->RegisterGroup(group);
    return group;
}

std::shared_ptr<XblSocialManagerUserGroup> XblSocialManagerUserGroup::Make(
    std::shared_ptr<xbox::services::social::manager::SocialGraph> socialGraph,
    Vector<uint64_t>&& trackedUsers
) noexcept
{
    auto group = std::shared_ptr<XblSocialManagerUserGroup>(
        new (Alloc(sizeof(XblSocialManagerUserGroup))) XblSocialManagerUserGroup{ socialGraph, std::move(trackedUsers) },
        Deleter<XblSocialManagerUserGroup>(),
        Allocator<XblSocialManagerUserGroup>()
        );
    socialGraph->RegisterGroup(group);
    return group;
}

void XblSocialManagerUserGroup::Initialize(const UnorderedMap<uint64_t, std::shared_ptr<XblSocialManagerUser>>& graphSnapshot) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutex };
    switch (type)
    {
    case XblSocialUserGroupType::FilterType:
    {
        for (auto& pair : graphSnapshot)
        {
            if (IsMemberOfGroup(pair.second.get()))
            {
                m_users[pair.first] = pair.second.get();
            }
        }
        // Do allocations for view structures now, but don't actually populate them until they
        // are requested.
        m_usersView.reserve(m_users.size());
        m_trackedUsersView.reserve(m_users.size());
        break;
    }
    case XblSocialUserGroupType::UserListType:
    {
        for (auto& xuid : m_trackedUsers)
        {
            auto graphIter{ graphSnapshot.find(xuid) };
            if (graphIter != graphSnapshot.end())
            {
                m_users[xuid] = graphIter->second.get();
            }
        }
        break;
    }
    default:
    {
        assert(false);
    }
    }
}

void XblSocialManagerUserGroup::DoWork(
    _Inout_ Vector<XblSocialManagerEvent>& events
) noexcept
{
    PERF_START();
    std::lock_guard<std::mutex> lock{ m_mutex };

    // Update users based on graph events
    for (auto& event : events)
    {
        switch (event.eventType)
        {
        case XblSocialManagerEventType::ProfilesChanged:
        case XblSocialManagerEventType::SocialRelationshipsChanged:
        case XblSocialManagerEventType::PresenceChanged:
        case XblSocialManagerEventType::UsersAddedToSocialGraph:
        {
            for (uint8_t i = 0; i < std::extent<decltype(event.usersAffected)>::value && event.usersAffected[i]; ++i)
            {
                if (IsMemberOfGroup(event.usersAffected[i]))
                {
                    m_users[event.usersAffected[i]->xboxUserId] = event.usersAffected[i];
                }
                else
                {
                    m_users.erase(event.usersAffected[i]->xboxUserId);
                }
            }
            break;
        }
        case XblSocialManagerEventType::UsersRemovedFromSocialGraph:
        {
            for (uint8_t i = 0; i < std::extent<decltype(event.usersAffected)>::value && event.usersAffected[i]; ++i)
            {
                m_users.erase(event.usersAffected[i]->xboxUserId);
            }
            break;
        }
        default:
        {
            // We don't care about other types of events
            break;
        }
        }
    }

    // Generate loaded event if needed
    if (!m_loaded)
    {
        switch (type)
        {
        case XblSocialUserGroupType::FilterType:
        {
            // Because all followed users are automatically in the SocialGraph, Filter groups are loaded 
            // as soon as initialization has completed
            events.push_back(XblSocialManagerEvent{
                m_localUser->Handle(),
                XblSocialManagerEventType::SocialUserGroupLoaded,
                S_OK,
                this
            });

            m_loaded = true;
            break;
        }
        case XblSocialUserGroupType::UserListType:
        {
            // List groups are loaded when the users they track have been added to the SocialGraph
            if (m_users.size() == m_trackedUsers.size())
            {
                events.push_back(XblSocialManagerEvent{
                    m_localUser->Handle(),
                    m_updated ? XblSocialManagerEventType::SocialUserGroupUpdated : XblSocialManagerEventType::SocialUserGroupLoaded,
                    S_OK,
                    this
                });

                m_loaded = true;
            }
            break;
        }
        }
    }
    PERF_STOP();
}

const Vector<const XblSocialManagerUser*>& XblSocialManagerUserGroup::Users() noexcept
{
    PERF_START();
    std::unique_lock<std::mutex> lock{ m_mutex };
    m_usersView.clear();
    if (m_loaded)
    {
        std::transform(m_users.begin(), m_users.end(), std::back_inserter(m_usersView), [](const auto& pair)
        {
            return pair.second;
        });
    }
    PERF_STOP();
    return m_usersView;
}

const Vector<uint64_t>& XblSocialManagerUserGroup::TrackedUsers() noexcept
{
    PERF_START();
    std::unique_lock<std::mutex> lock{ m_mutex };
    if (m_loaded)
    {
        switch (type)
        {
        case XblSocialUserGroupType::FilterType:
        {
            m_trackedUsersView.clear();
            std::transform(m_users.begin(), m_users.end(), std::back_inserter(m_trackedUsersView), [](const auto& pair)
                {
                    return pair.first;
                });
            break;
        }
        case XblSocialUserGroupType::UserListType:
        {
            // Tracked users view is static
            break;
        }
        }
    }
    else
    {
        m_trackedUsersView.clear();
    }
    PERF_STOP();
    return m_trackedUsersView;
}

std::shared_ptr<User> XblSocialManagerUserGroup::LocalUser() const noexcept
{
    return m_localUser;
}

HRESULT XblSocialManagerUserGroup::UpdateTrackedUsers(
    Vector<uint64_t>&& trackedUsers
) noexcept
{
    std::unique_lock<std::mutex> lock{ m_mutex };
    auto graph{ m_graph.lock() };
    assert(graph);

    switch (type)
    {
    case XblSocialUserGroupType::FilterType:
    {
        LOGS_ERROR << "Can't update tracked users for XblSocialUserGroupType::FilterType";
        return E_UNEXPECTED;
    }
    case XblSocialUserGroupType::UserListType:
    {
        // Track new users before untracking old users to avoid users being removed from and
        // then immediately readded to the graph.
        graph->TrackUsers(trackedUsers);
        graph->StopTrackingUsers(m_trackedUsersView);

        m_trackedUsersView = trackedUsers;
        m_trackedUsers.clear();
        m_trackedUsers.insert(m_trackedUsersView.begin(), m_trackedUsersView.end());
        m_users.clear();

        // Register again to retrigger initialization
        graph->RegisterGroup(shared_from_this());

        m_loaded = false;
        m_updated = true;
        return S_OK;
    }
    default:
    {
        assert(false);
        return S_OK;
    }
    }
}

bool XblSocialManagerUserGroup::IsMemberOfGroup(XblSocialManagerUser const* user) const noexcept
{
    switch (type)
    {
    case XblSocialUserGroupType::FilterType:
    {
        if ((relationshipFilter == XblRelationshipFilter::Friends && !user->isFollowedByCaller) ||
            (relationshipFilter == XblRelationshipFilter::Favorite && !user->isFavorite))
        {
            return false;
        }

        switch (presenceFilter)
        {
        case XblPresenceFilter::All:
        {
            return true;
        }
        case XblPresenceFilter::AllOffline:
        {
            return user->presenceRecord.userState == XblPresenceUserState::Offline;
        }
        case XblPresenceFilter::AllOnline:
        {
            return user->presenceRecord.userState == XblPresenceUserState::Online;
        }
        case XblPresenceFilter::AllTitle:
        {
            return user->titleHistory.hasUserPlayed;
        }
        case XblPresenceFilter::TitleOffline:
        {
            return user->titleHistory.hasUserPlayed && user->presenceRecord.userState == XblPresenceUserState::Offline;
        }
        case XblPresenceFilter::TitleOnline:
        {
            return XblSocialManagerPresenceRecordIsUserPlayingTitle(&user->presenceRecord, AppConfig::Instance()->TitleId());
        }
        case XblPresenceFilter::TitleOnlineOutsideTitle:
        {
            return user->titleHistory.hasUserPlayed &&
                user->presenceRecord.userState == XblPresenceUserState::Online &&
                !XblSocialManagerPresenceRecordIsUserPlayingTitle(&user->presenceRecord, AppConfig::Instance()->TitleId());
        }
        default:
        {
            return false;
        }
        }
        return true;
    }
    case XblSocialUserGroupType::UserListType:
    {
        return m_trackedUsers.find(user->xboxUserId) != m_trackedUsers.end();
    }
    default:
    {
        assert(false);
        return false;
    }
    }
}
