// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi-c/social_manager_c.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_BEGIN

class SocialManager : public std::enable_shared_from_this<SocialManager>
{
public:
    SocialManager() noexcept;

    HRESULT AddLocalUser(
        User&& user,
        XblSocialManagerExtraDetailLevel detailLevel,
        TaskQueue&& queue
    ) noexcept;

    HRESULT RemoveLocalUser(
        const User& user
    ) noexcept;

    Result<std::shared_ptr<XblSocialManagerUserGroup>> CreateUserGroup(
        const User& user,
        XblPresenceFilter presenceFilter,
        XblRelationshipFilter relationshipFilter
    ) noexcept;

    Result<std::shared_ptr<XblSocialManagerUserGroup>> CreateUserGroup(
        const User& user,
        Vector<uint64_t>&& trackedUsers
    ) noexcept;

    // Because group lifetime is managed by SocialManager (handles are not refCounted), this API
    // is used to get a group if it is still valid.
    std::shared_ptr<XblSocialManagerUserGroup> GetUserGroup(
        XblSocialManagerUserGroupHandle groupHandle
    ) const;

    HRESULT DestroyUserGroup(
        XblSocialManagerUserGroupHandle groupHandle
    ) noexcept;

    const Vector<XblSocialManagerEvent>& DoWork() noexcept;

    HRESULT SetRichPresencePolling(
        const User& user,
        bool enabled
    ) noexcept;

    size_t LocalUserCount() const noexcept;

    HRESULT GetLocalUsers(
        _In_ size_t usersCount,
        _Out_writes_(usersCount) XblUserHandle* users
    ) const noexcept;

private:
    // Controls access to m_events, which is only be written during DoWork
    mutable std::mutex m_eventsMutex;
    // Controls access to all other state, which may be updated from non-UI threads and is read by UI-thread (DoWork)
    mutable std::mutex m_mutex;

    Vector<XblSocialManagerEvent> m_events;
    // Maintain lifetime for local users and XblSocialManagerUsers referenced in m_events
    Vector<std::shared_ptr<XblSocialManagerUser>> m_affectedUsersLifetime;
    List<std::shared_ptr<User>> m_removedUsersLifetime;

    UnorderedMap<uint64_t, std::shared_ptr<class SocialGraph>> m_graphs;

    // Lifetime of groups is managed entirely by SocialManager (i.e. they are not refCounted). To maintain legacy
    // behavior, when a local user is removed, clean up all their groups. If an API call is made on an invalid group,
    // this map can be used to detect that.
    UnorderedMap<XblSocialManagerUserGroupHandle, std::shared_ptr<XblSocialManagerUserGroup>> m_groups;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_END
