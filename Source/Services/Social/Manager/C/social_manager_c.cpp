// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi-c/social_manager_c.h"
#if !XDK_API
#include "user_c.h"
#endif
#include "social_manager_internal.h"

using namespace xbox::services;
using namespace xbox::services::system;
using namespace xbox::services::social::manager;

#if XDK_API
#define GET_INTERNAL_USER(user) user
#else
#define GET_INTERNAL_USER(user) user->internalUser
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_BEGIN

class xbl_social_manager
{
public:
    XBL_XBOX_SOCIAL_USER_GROUP* create_social_user_group(
        _In_ std::shared_ptr<xbox_social_user_group_internal> internalGroup
    )
    {
        xbl_user_handle userPtr;
#if !XDK_API
        auto userIter = localUsersMap.find(internalGroup->local_user());
        if (userIter == localUsersMap.end())
        {
            throw new std::exception("User doesn't exist. Did you call AddLocalUser?");
        }
        userPtr = userIter->second;
#else
        userPtr = internalGroup->local_user();
#endif

        auto buffer = xbox::services::system::xsapi_memory::mem_alloc(sizeof(XBL_XBOX_SOCIAL_USER_GROUP));
        XBL_XBOX_SOCIAL_USER_GROUP *socialUserGroup = new (buffer) XBL_XBOX_SOCIAL_USER_GROUP
        {
            uint32_t(internalGroup->users().size()),
            static_cast<XBL_SOCIAL_USER_GROUP_TYPE>(internalGroup->social_user_group_type()),
            uint32_t(internalGroup->users_tracked_by_social_user_group().size()),
            userPtr,
            static_cast<XBL_PRESENCE_FILTER>(internalGroup->presence_filter_of_group()),
            static_cast<XBL_RELATIONSHIP_FILTER>(internalGroup->relationship_filter_of_group())
        };
        socialUserGroupsMap.insert(socialUserGroup, internalGroup);

        return socialUserGroup;
    }

#if !XDK_API
    xsapi_internal_unordered_map<xbox_live_user_t, xbl_user_handle> localUsersMap;
    xsapi_internal_vector<xbl_user_handle> localUsersVector;
#endif
    bimap<XBL_XBOX_SOCIAL_USER_GROUP*, std::shared_ptr<xbox_social_user_group_internal>> socialUserGroupsMap;
    xsapi_internal_vector<XBL_SOCIAL_EVENT> socialEvents;
    xsapi_internal_vector<std::shared_ptr<social_event_internal>> internalSocialEvents;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_END

std::shared_ptr<xbl_social_manager> get_xbl_social_manager()
{
    auto singleton = get_xsapi_singleton();
    if (singleton->m_xblSocialManagerState == nullptr)
    {
        singleton->m_xblSocialManagerState = xsapi_allocate_shared<xbl_social_manager>();
    }
    return singleton->m_xblSocialManagerState;
}

XBL_API bool XBL_CALLING_CONV
XblSocialManagerPresenceRecordIsUserPlayingTitle(
    _In_ XBL_SOCIAL_MANAGER_PRESENCE_RECORD* presenceRecord,
    _In_ uint32_t titleId
    ) XBL_NOEXCEPT
try
{
    verify_global_init();

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

XBL_RESULT populate_social_user_array(
    _In_ xsapi_internal_vector<xbox_social_user*> internalXboxSocialUsers,
    _Out_ XBL_XBOX_SOCIAL_USER* users
    )
{
    uint32_t i = 0;
    for (auto internalUser : internalXboxSocialUsers)
    {
        utils::utf8_from_char_t(internalUser->xbox_user_id(), users[i].xboxUserId, sizeof(users[i].xboxUserId));
        users[i].isFavorite = internalUser->is_favorite();
        users[i].isFollowingUser = internalUser->is_following_user();
        users[i].isFollowedByCaller = internalUser->is_followed_by_caller();
        utils::utf8_from_char_t(internalUser->display_name(), users[i].displayName, sizeof(users[i].displayName));
        utils::utf8_from_char_t(internalUser->real_name(), users[i].realName, sizeof(users[i].realName));
        utils::utf8_from_char_t(internalUser->display_pic_url_raw(), users[i].displayPicUrlRaw, sizeof(users[i].displayPicUrlRaw));
        users[i].useAvatar = internalUser->use_avatar();
        utils::utf8_from_char_t(internalUser->gamerscore(), users[i].gamerscore, sizeof(users[i].gamerscore));
        utils::utf8_from_char_t(internalUser->gamertag(), users[i].gamertag, sizeof(users[i].gamertag));

        // presence record
        users[i].presenceRecord.userState = static_cast<XBL_USER_PRESENCE_STATE>(internalUser->presence_record().user_state());
        
        uint8_t j = 0;
        for (auto& internalTitleRecord : internalUser->presence_record().presence_title_records())
        {
            auto& titleRecord = users[i].presenceRecord.presenceTitleRecords[j];

            titleRecord.titleId = internalTitleRecord.title_id();
            titleRecord.isTitleActive = internalTitleRecord.is_title_active();
            utils::utf8_from_char_t(internalTitleRecord.presence_text(), titleRecord.presenceText, sizeof(titleRecord.presenceText));
            titleRecord.isBroadcasting = internalTitleRecord.is_broadcasting();
            titleRecord.deviceType = static_cast<XBL_PRESENCE_DEVICE_TYPE>(internalTitleRecord.device_type());
            ++j;
        }
        users[i].presenceRecord.presenceTitleRecordCount = j;

        // title history
        users[i].titleHistory.lastTimeUserPlayed = utils::time_t_from_datetime(internalUser->title_history().last_time_user_played());
        users[i].titleHistory.hasUserPlayed = internalUser->title_history().has_user_played();

        // preferred color
        utils::utf8_from_char_t(internalUser->preferred_color().primary_color(), users[i].preferredColor.primaryColor, sizeof(users[i].preferredColor.primaryColor));
        utils::utf8_from_char_t(internalUser->preferred_color().secondary_color(), users[i].preferredColor.secondaryColor, sizeof(users[i].preferredColor.secondaryColor));
        utils::utf8_from_char_t(internalUser->preferred_color().tertiary_color(), users[i].preferredColor.tertiaryColor, sizeof(users[i].preferredColor.tertiaryColor));
    }

    return XBL_RESULT_OK;
}

XBL_API XBL_RESULT XBL_CALLING_CONV
XblXboxSocialUserGroupGetUsers(
    _In_ XBL_XBOX_SOCIAL_USER_GROUP* group,
    _Out_ XBL_XBOX_SOCIAL_USER* xboxSocialUsers
    ) XBL_NOEXCEPT
//try
{
    verify_global_init();
    auto state = get_xbl_social_manager();

    auto groupIter = state->socialUserGroupsMap.find(group);
    if (groupIter == state->socialUserGroupsMap.end())
    {
        return XBL_RESULT_INVALID_ARG;
    }

    return populate_social_user_array(
        groupIter->second->users(),
        xboxSocialUsers
        );
}
//CATCH_RETURN()

XBL_API XBL_RESULT XBL_CALLING_CONV
XblXboxSocialUserGroupGetUsersFromXboxUserIds(
    _In_ XBL_XBOX_SOCIAL_USER_GROUP* group,
    _In_ XBL_XBOX_USER_ID_CONTAINER* xboxUserIds,
    _In_ uint32_t xboxUserIdsCount,
    _Out_ XBL_XBOX_SOCIAL_USER* xboxSocialUsers,
    _Inout_ uint32_t* xboxSocialUsersCount
    ) XBL_NOEXCEPT
//try
{
    verify_global_init();
    auto state = get_xbl_social_manager();

    auto groupIter = state->socialUserGroupsMap.find(group);
    if (groupIter == state->socialUserGroupsMap.end())
    {
        return XBL_RESULT_INVALID_ARG;
    }

    xsapi_internal_vector<xbox_user_id_container> userIdsVector;
    for (uint32_t i = 0; i < xboxUserIdsCount; ++i)
    {
        userIdsVector.push_back(xbox_user_id_container(utils::string_t_from_utf8(xboxUserIds[i].xboxUserId).data()));
    }

    auto result = groupIter->second->get_users_from_xbox_user_ids(userIdsVector);

    auto inputArraySize = *xboxSocialUsersCount;
    *xboxSocialUsersCount = static_cast<uint32_t>(result.size());
    if (result.size() > inputArraySize)
    {
        return XBL_RESULT_INVALID_ARG;
    }
    return populate_social_user_array(result, xboxSocialUsers);
}
//CATCH_RETURN()

XBL_API XBL_RESULT XBL_CALLING_CONV
XblSocialUserGroupGetUsersTrackedByGroup(
    _In_ XBL_XBOX_SOCIAL_USER_GROUP* group,
    _Out_ XBL_XBOX_USER_ID_CONTAINER* trackedUsers
    ) XBL_NOEXCEPT
//try
{
    verify_global_init();
    auto state = get_xbl_social_manager();

    auto groupIter = state->socialUserGroupsMap.find(group);
    if (groupIter == state->socialUserGroupsMap.end())
    {
        return XBL_RESULT_INVALID_ARG;
    }

    uint32_t count = 0;
    for (auto& internalContainer : groupIter->second->users_tracked_by_social_user_group())
    {
        utils::utf8_from_char_t(internalContainer.xbox_user_id(), trackedUsers[count].xboxUserId, sizeof(trackedUsers[count].xboxUserId));
        ++count;
    }
    return XBL_RESULT_OK;
}
//CATCH_RETURN()

XBL_API XBL_RESULT XBL_CALLING_CONV
XblSocialManagerAddLocalUser(
    _In_ xbl_user_handle user,
    _In_ XBL_SOCIAL_MANAGER_EXTRA_DETAIL_LEVEL extraLevelDetail
    ) XBL_NOEXCEPT
//try
{
    verify_global_init();

    auto result = social_manager_internal::get_singleton_instance()->add_local_user(
        GET_INTERNAL_USER(user),
        static_cast<social_manager_extra_detail_level>(extraLevelDetail)
        );

#if !XDK_API
    if (!result.err())
    {
        auto state = get_xbl_social_manager();
        state->localUsersMap[user->internalUser] = user;
        state->localUsersVector.push_back(user);
    }
#endif
    return utils::create_xbl_result(result.err());
}
//CATCH_RETURN()

XBL_API XBL_RESULT XBL_CALLING_CONV
XblSocialManagerRemoveLocalUser(
    _In_ xbl_user_handle user
    ) XBL_NOEXCEPT
//try
{
    verify_global_init();

    auto result = social_manager_internal::get_singleton_instance()->remove_local_user(GET_INTERNAL_USER(user));
#if !XDK_API
    if (!result.err())
    {
        auto state = get_xbl_social_manager();
        state->localUsersMap.erase(user->internalUser);

        auto& usersVector = state->localUsersVector;
        for (auto iter = usersVector.begin(); iter != usersVector.end(); ++iter)
        {
            if (user == *iter)
            {
                usersVector.erase(iter);
                break;
            }
        }
    }
#endif
    return utils::create_xbl_result(result.err());
}
//CATCH_RETURN()


XBL_API XBL_SOCIAL_EVENT* XBL_CALLING_CONV
XblSocialManagerDoWork(
    _Out_ uint32_t* socialEventsCount
    ) XBL_NOEXCEPT
try
{
    verify_global_init();
    auto state = get_xbl_social_manager();

    state->socialEvents.clear();

    state->internalSocialEvents = social_manager_internal::get_singleton_instance()->do_work();
    if (state->internalSocialEvents.size() > 0)
    {
        for (auto internalEvent : state->internalSocialEvents)
        {
            xbl_user_handle userPtr;
#if !XDK_API
            auto userIter = state->localUsersMap.find(internalEvent->user());
            if (userIter == state->localUsersMap.end())
            {
                throw new std::exception("User doesn't exist. Did you call AddLocalUser?");
            }
            userPtr = userIter->second;
#else
            userPtr = internalEvent->user();
#endif

            state->socialEvents.push_back(XBL_SOCIAL_EVENT
            {
                userPtr,
                static_cast<XBL_SOCIAL_EVENT_TYPE>(internalEvent->event_type()),
                uint32_t(internalEvent->users_affected().size()),
                internalEvent->err().value(),
                static_cast<void*>(internalEvent.get())
            });
        }

        // TODO There might be a way to update only a subset of group, but just update all for now
        for (auto groupMapping : state->socialUserGroupsMap)
        {
            // TODO I think we only need to update the usersCount and trackedUsersCount properties
            groupMapping.first->trackedUsersCount = static_cast<uint32_t>(groupMapping.second->users_tracked_by_social_user_group().size());
            groupMapping.first->usersCount = static_cast<uint32_t>(groupMapping.second->users().size());
        }
    }

    *socialEventsCount = static_cast<uint32_t>(state->socialEvents.size());
    return state->socialEvents.data();
}
CATCH_RETURN_WITH(nullptr)

XBL_API XBL_RESULT XBL_CALLING_CONV
XblSocialEventGetUsersAffected(
    _In_ XBL_SOCIAL_EVENT* socialEvent,
    _Out_ XBL_XBOX_USER_ID_CONTAINER* usersAffected
    ) XBL_NOEXCEPT
//try
{
    if (socialEvent == nullptr || usersAffected == nullptr)
    {
        return XBL_RESULT_INVALID_ARG;
    }

    auto internalEvent = static_cast<social_event_internal*>(socialEvent->internalEvent);

    uint32_t count = 0;
    for (auto& internalContainer : internalEvent->users_affected())
    {
        utils::utf8_from_char_t(internalContainer.xbox_user_id(), usersAffected[count].xboxUserId, sizeof(usersAffected[count].xboxUserId));
        ++count;
    }
    return XBL_RESULT_OK;
}
//CATCH_RETURN()

XBL_API XBL_RESULT XBL_CALLING_CONV
XblSocialEventGetSocialUserGroup(
    _In_ XBL_SOCIAL_EVENT* socialEvent,
    _Out_ XBL_XBOX_SOCIAL_USER_GROUP** loadedGroup
    ) XBL_NOEXCEPT
//try
{
    if (socialEvent == nullptr || loadedGroup == nullptr || socialEvent->eventType != XBL_SOCIAL_EVENT_TYPE_SOCIAL_USER_GROUP_LOADED)
    {
        return XBL_RESULT_INVALID_ARG;
    }

    auto internalEvent = static_cast<social_event_internal*>(socialEvent->internalEvent);

    auto groupLoadedEventArgs = std::dynamic_pointer_cast<social_user_group_loaded_event_args_internal>(internalEvent->event_args());
    if (groupLoadedEventArgs == nullptr)
    {
        *loadedGroup = nullptr;
    }
    else
    {
        auto& groupMap = get_xbl_social_manager()->socialUserGroupsMap;

        auto iter = groupMap.reverse_find(groupLoadedEventArgs->social_user_group());
        if (iter == groupMap.reverse_end())
        {
            *loadedGroup = nullptr;
        }
        else
        {
            *loadedGroup = iter->second;
        }
    }
    return XBL_RESULT_OK;
}
//CATCH_RETURN()

XBL_API XBL_RESULT XBL_CALLING_CONV
XblSocialManagerCreateSocialUserGroupFromFilters(
    _In_ xbl_user_handle user,
    _In_ XBL_PRESENCE_FILTER presenceDetailLevel,
    _In_ XBL_RELATIONSHIP_FILTER filter,
    _Out_ XBL_XBOX_SOCIAL_USER_GROUP** group
    ) XBL_NOEXCEPT
//try
{
    verify_global_init();
    auto state = get_xbl_social_manager();

    auto result = social_manager_internal::get_singleton_instance()->create_social_user_group_from_filters(
        GET_INTERNAL_USER(user),
        static_cast<presence_filter>(presenceDetailLevel),
        static_cast<relationship_filter>(filter)
        );

    if (!result.err())
    {
        *group = state->create_social_user_group(result.payload());
    }
    return utils::create_xbl_result(result.err());
}
//CATCH_RETURN()

XBL_API XBL_RESULT XBL_CALLING_CONV
XblSocialManagerCreateSocialUserGroupFromList(
    _In_ xbl_user_handle user,
    _In_ PCSTR* xboxUserIdList,
    _In_ uint32_t xboxUserIdListCount,
    _Out_ XBL_XBOX_SOCIAL_USER_GROUP** group
    ) XBL_NOEXCEPT
//try
{
    verify_global_init();
    auto state = get_xbl_social_manager();

    auto xboxUserIdVector = xsapi_internal_vector<xsapi_internal_string>(xboxUserIdList, xboxUserIdList + xboxUserIdListCount);

    auto result = social_manager_internal::get_singleton_instance()->create_social_user_group_from_list(
        GET_INTERNAL_USER(user),
        xboxUserIdVector
        );

    if (!result.err())
    {
        *group = state->create_social_user_group(result.payload());
    }
    return utils::create_xbl_result(result.err());
}
//CATCH_RETURN()

XBL_API XBL_RESULT XBL_CALLING_CONV
XblSocialManagerDestroySocialUserGroup(
    _In_ XBL_XBOX_SOCIAL_USER_GROUP* group
    ) XBL_NOEXCEPT
//try
{
    verify_global_init();
    auto state = get_xbl_social_manager();

    auto groupIter = state->socialUserGroupsMap.find(group);
    if (groupIter == state->socialUserGroupsMap.end())
    {
        LOG_ERROR("User group not found");
        return XBL_RESULT_INVALID_ARG;
    }

    auto result = social_manager_internal::get_singleton_instance()->destroy_social_user_group(groupIter->second);
    if (!result.err())
    {
        state->socialUserGroupsMap.erase(groupIter);
    }
    return utils::create_xbl_result(result.err());
}
//CATCH_RETURN()

XBL_API xbl_user_handle const* XBL_CALLING_CONV
XblSocialManagerGetLocalUsers(
    _Out_ uint32_t* userCount
    ) XBL_NOEXCEPT
{
#if XDK_API
    auto& usersVector = social_manager_internal::get_singleton_instance()->local_users();
#else
    auto& usersVector = get_xbl_social_manager()->localUsersVector;
#endif
    *userCount = static_cast<uint32_t>(usersVector.size());
    return usersVector.data();
}

XBL_API XBL_RESULT XBL_CALLING_CONV
XblSocialManagerUpdateSocialUserGroup(
    _In_ XBL_XBOX_SOCIAL_USER_GROUP *group,
    _In_ PCSTR* users,
    _In_ uint32_t usersCount
    ) XBL_NOEXCEPT
//try
{
    verify_global_init();

    auto state = get_xbl_social_manager();

    auto groupIter = state->socialUserGroupsMap.find(group);
    if (groupIter == state->socialUserGroupsMap.end())
    {
        LOG_ERROR("User group not found");
        return XBL_RESULT_INVALID_ARG;
    }

    auto usersVector = xsapi_internal_vector<xsapi_internal_string>(users, users + usersCount);

    auto result = social_manager_internal::get_singleton_instance()->update_social_user_group(
        groupIter->second,
        usersVector
        );

    return utils::create_xbl_result(result.err());
}
//CATCH_RETURN()

XBL_API XBL_RESULT XBL_CALLING_CONV
XblSocialManagerSetRichPresencePollingStatus(
    _In_ xbl_user_handle user,
    _In_ bool shouldEnablePolling
    ) XBL_NOEXCEPT
//try
{
    verify_global_init();

    auto result = social_manager_internal::get_singleton_instance()->set_rich_presence_polling_status(
        GET_INTERNAL_USER(user),
        shouldEnablePolling
        );
   
    return utils::create_xbl_result(result.err());
}
//CATCH_RETURN()


XBL_API void XBL_CALLING_CONV
XblSocialManagerSetBackgroundWorkAsyncQueue(
    _In_ XBL_ASYNC_QUEUE queue
    ) XBL_NOEXCEPT
try
{
    verify_global_init();
    //social_manager_internal::get_singleton_instance()->set_social_graph_background_async_queue(queue->taskGroupId);
    // TODO
}
CATCH_RETURN_WITH(;)