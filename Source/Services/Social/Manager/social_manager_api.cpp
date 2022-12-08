// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "social_manager_internal.h"
#include "social_manager_user_group.h"

using namespace xbox::services;
using namespace xbox::services::social::manager;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_BEGIN

template<typename Ret, typename TWork>
Ret ApiImpl(Ret&& fallbackReturnValue, TWork&& work) noexcept
{
    auto state{ GlobalState::Get() };
    if (!state)
    {
        return fallbackReturnValue;
    }

    assert(state->SocialManager());
    return work(*state->SocialManager());
}

template<typename TWork>
HRESULT ApiImpl(TWork&& work) noexcept
{
    return ApiImpl<HRESULT, TWork>(E_XBL_NOT_INITIALIZED, std::move(work));
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_END

STDAPI_(bool) XblSocialManagerPresenceRecordIsUserPlayingTitle(
    _In_ const XblSocialManagerPresenceRecord* presenceRecord,
    _In_ uint32_t titleId
) XBL_NOEXCEPT
try
{
    if (presenceRecord == nullptr ||
        presenceRecord->userState == XblPresenceUserState::Offline ||
        presenceRecord->userState == XblPresenceUserState::Unknown)
    {
        return false;
    }

    for (uint32_t i = 0; i < presenceRecord->presenceTitleRecordCount; ++i)
    {
        if (presenceRecord->presenceTitleRecords[i].titleId == titleId)
        {
            return true;
        }
    }
    return false;
}
CATCH_RETURN_WITH(false);

STDAPI XblSocialManagerUserGroupGetType(
    _In_ XblSocialManagerUserGroupHandle groupHandle,
    _Out_ XblSocialUserGroupType* type
) XBL_NOEXCEPT
try
{
    return ApiImpl([&](SocialManager& socialManager)
    {
        RETURN_HR_INVALIDARGUMENT_IF(groupHandle == nullptr || type == nullptr);

        auto group{ socialManager.GetUserGroup(groupHandle) };
        if (!group)
        {
            return E_UNEXPECTED;
        }

        *type = group->type;
        return S_OK;
    });
}
CATCH_RETURN()

STDAPI XblSocialManagerUserGroupGetLocalUser(
    _In_ XblSocialManagerUserGroupHandle groupHandle,
    _Out_ XblUserHandle* localUser
) XBL_NOEXCEPT
try
{
    return ApiImpl([&](SocialManager& socialManager)
    {
        RETURN_HR_INVALIDARGUMENT_IF(groupHandle == nullptr || localUser == nullptr);

        auto group{ socialManager.GetUserGroup(groupHandle) };
        if (!group)
        {
            return E_UNEXPECTED;
        }

        *localUser = group->LocalUser()->Handle();
        return S_OK;
    });
}
CATCH_RETURN()

STDAPI XblSocialManagerUserGroupGetFilters(
    _In_ XblSocialManagerUserGroupHandle groupHandle,
    _Out_opt_ XblPresenceFilter* presenceFilter,
    _Out_opt_ XblRelationshipFilter* relationshipFilter
) XBL_NOEXCEPT
try
{
    return ApiImpl([&](SocialManager& socialManager)
    {
        RETURN_HR_INVALIDARGUMENT_IF(groupHandle == nullptr);

        auto group{ socialManager.GetUserGroup(groupHandle) };
        if (!group || group->type != XblSocialUserGroupType::FilterType)
        {
            return E_UNEXPECTED;
        }

        if (presenceFilter)
        {
            *presenceFilter = group->presenceFilter;
        }
        if (relationshipFilter)
        {
            *relationshipFilter = group->relationshipFilter;
        }
        return S_OK;
    });
}
CATCH_RETURN()

STDAPI XblSocialManagerUserGroupGetUsers(
    _In_ XblSocialManagerUserGroupHandle groupHandle,
    _Outptr_result_maybenull_ XblSocialManagerUserPtrArray* users,
    _Out_ size_t* usersCount
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(groupHandle == nullptr || users == nullptr || usersCount == nullptr);
    *users = nullptr;
    
    return ApiImpl([&](SocialManager& socialManager)
    {
        auto group{ socialManager.GetUserGroup(groupHandle) };
        if (!group)
        {
            *users = nullptr;
            *usersCount = 0;
            return E_UNEXPECTED;
        }

        auto& groupUsers = group->Users();
        *users = groupUsers.data();
        *usersCount = groupUsers.size();

        return S_OK;
    });
}
CATCH_RETURN()

STDAPI XblSocialManagerUserGroupGetUsersTrackedByGroup(
    _In_ XblSocialManagerUserGroupHandle groupHandle,
    _Outptr_result_maybenull_ const uint64_t** trackedUsers,
    _Out_ size_t* trackedUsersCount
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(groupHandle == nullptr || trackedUsers == nullptr || trackedUsersCount == nullptr);
    *trackedUsers = nullptr;
    
    return ApiImpl([&](SocialManager& socialManager)
    {
        auto group{ socialManager.GetUserGroup(groupHandle) };
        if (!group)
        {
            *trackedUsers = nullptr;
            *trackedUsersCount = 0;
            return E_UNEXPECTED;
        }

        const auto& groupTrackedUsers = group->TrackedUsers();
        *trackedUsers = groupTrackedUsers.data();
        *trackedUsersCount = groupTrackedUsers.size();

        return S_OK;
    });
}
CATCH_RETURN()

STDAPI XblSocialManagerAddLocalUser(
    _In_ XblUserHandle user,
    _In_ XblSocialManagerExtraDetailLevel extraLevelDetail,
    _In_opt_ XTaskQueueHandle queue
) XBL_NOEXCEPT
try
{
    return ApiImpl([&](SocialManager& socialManager)
        {
            RETURN_HR_INVALIDARGUMENT_IF(user == nullptr);
            auto wrapUserResult{ User::WrapHandle(user) };
            RETURN_HR_IF_FAILED(wrapUserResult.Hresult());

            return socialManager.AddLocalUser(wrapUserResult.ExtractPayload(), extraLevelDetail, TaskQueue::DeriveWorkerQueue(queue));
        });
}
CATCH_RETURN()

STDAPI XblSocialManagerRemoveLocalUser(
    _In_ XblUserHandle user
) XBL_NOEXCEPT
try
{
    return ApiImpl([&](SocialManager& socialManager)
        {
            RETURN_HR_INVALIDARGUMENT_IF_NULL(user);
            auto wrapUserResult{ User::WrapHandle(user) };
            RETURN_HR_IF_FAILED(wrapUserResult.Hresult());

            return socialManager.RemoveLocalUser(wrapUserResult.Payload());
        });
}
CATCH_RETURN()

STDAPI XblSocialManagerDoWork(
    _Outptr_result_maybenull_ const XblSocialManagerEvent** socialEvents,
    _Out_ size_t* socialEventsCount
) XBL_NOEXCEPT
try
{
    INIT_OUT_PTR_PARAM(socialEvents);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(socialEvents);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(socialEventsCount);

    return ApiImpl([&](SocialManager& socialManager)
        {
            auto& events = socialManager.DoWork();
            *socialEvents = events.empty() ? nullptr : events.data();
            *socialEventsCount = events.size();

            return S_OK;
        });
}
CATCH_RETURN()

STDAPI XblSocialManagerCreateSocialUserGroupFromFilters(
    _In_ XblUserHandle user,
    _In_ XblPresenceFilter presenceFilter,
    _In_ XblRelationshipFilter relationshipFilter,
    _Outptr_result_maybenull_ XblSocialManagerUserGroupHandle* group
) XBL_NOEXCEPT
try
{
    INIT_OUT_PTR_PARAM(group);

    return ApiImpl([&](SocialManager& socialManager)
        {
            RETURN_HR_INVALIDARGUMENT_IF(user == nullptr || group == nullptr);
            auto wrapUserResult{ User::WrapHandle(user) };
            RETURN_HR_IF_FAILED(wrapUserResult.Hresult());

            auto result = socialManager.CreateUserGroup(wrapUserResult.Payload(), presenceFilter, relationshipFilter);
            if (Succeeded(result))
            {
                *group = result.ExtractPayload().get();
            }
            return result.Hresult();
        });
}
CATCH_RETURN()

STDAPI XblSocialManagerCreateSocialUserGroupFromList(
    _In_ XblUserHandle user,
    _In_ uint64_t* xuids,
    _In_ size_t xuidsCount,
    _Outptr_result_maybenull_ XblSocialManagerUserGroup** group
) XBL_NOEXCEPT
try
{
    INIT_OUT_PTR_PARAM(group);

    return ApiImpl([&](SocialManager& socialManager)
        {
            RETURN_HR_INVALIDARGUMENT_IF(user == nullptr || xuids == nullptr || xuidsCount <= 0 || xuidsCount > XBL_SOCIAL_MANAGER_MAX_USERS_FROM_LIST || group == nullptr);
            auto wrapUserResult{ User::WrapHandle(user) };
            RETURN_HR_IF_FAILED(wrapUserResult.Hresult());
            
            auto result = socialManager.CreateUserGroup(wrapUserResult.Payload(), Vector<uint64_t>(xuids, xuids + xuidsCount));
            if (Succeeded(result))
            {
                *group = result.ExtractPayload().get();
            }
            return result.Hresult();
        });
}
CATCH_RETURN()

STDAPI XblSocialManagerDestroySocialUserGroup(
    _In_ XblSocialManagerUserGroupHandle groupHandle
) XBL_NOEXCEPT
try
{
    return ApiImpl([&](SocialManager& socialManager)
        {
            RETURN_HR_INVALIDARGUMENT_IF_NULL(groupHandle);
            return socialManager.DestroyUserGroup(groupHandle);
        });
}
CATCH_RETURN()

STDAPI_(size_t) XblSocialManagerGetLocalUserCount() XBL_NOEXCEPT
try
{
    return ApiImpl<size_t>(0, [](SocialManager& socialManager)
        {
            return socialManager.LocalUserCount();
        });
}
CATCH_RETURN_WITH(0)

STDAPI XblSocialManagerGetLocalUsers(
    _In_ size_t usersCount,
    _Out_writes_(usersCount) XblUserHandle* users
) XBL_NOEXCEPT
try
{
    return ApiImpl([&](SocialManager& socialManager)
        {
            return socialManager.GetLocalUsers(usersCount, users);
        });
}
CATCH_RETURN()

STDAPI XblSocialManagerUpdateSocialUserGroup(
    _In_ XblSocialManagerUserGroupHandle group,
    _In_ uint64_t* users,
    _In_ size_t usersCount
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(group == nullptr || users == nullptr || usersCount <= 0 || usersCount > XBL_SOCIAL_MANAGER_MAX_USERS_FROM_LIST);
    return group->UpdateTrackedUsers(Vector<uint64_t>(users, users + usersCount));
}
CATCH_RETURN()

STDAPI XblSocialManagerSetRichPresencePollingStatus(
    _In_ XblUserHandle user,
    _In_ bool shouldEnablePolling
) XBL_NOEXCEPT
try
{
    return ApiImpl([&](SocialManager& socialManager)
        {
            auto wrapUserResult{ User::WrapHandle(user) };
            RETURN_HR_IF_FAILED(wrapUserResult.Hresult());
            return socialManager.SetRichPresencePolling(wrapUserResult.Payload(), shouldEnablePolling);
        });
}
CATCH_RETURN()