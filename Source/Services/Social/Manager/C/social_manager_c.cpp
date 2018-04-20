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
    XblXboxSocialUserGroup* create_social_user_group(
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

        auto buffer = xbox::services::system::xsapi_memory::mem_alloc(sizeof(XblXboxSocialUserGroup));
        XblXboxSocialUserGroup *socialUserGroup = new (buffer) XblXboxSocialUserGroup
        {
            uint32_t(internalGroup->users().size()),
            static_cast<XblSocialUserGroupType>(internalGroup->social_user_group_type()),
            uint32_t(internalGroup->users_tracked_by_social_user_group().size()),
            userPtr,
            static_cast<XblPresenceFilter>(internalGroup->presence_filter_of_group()),
            static_cast<XblRelationshipFilter>(internalGroup->relationship_filter_of_group())
        };
        socialUserGroupsMap.insert(socialUserGroup, internalGroup);

        return socialUserGroup;
    }

#if !XDK_API
    xsapi_internal_unordered_map<xbox_live_user_t, xbl_user_handle> localUsersMap;
    xsapi_internal_vector<xbl_user_handle> localUsersVector;
#endif
    bimap<XblXboxSocialUserGroup*, std::shared_ptr<xbox_social_user_group_internal>> socialUserGroupsMap;
    xsapi_internal_vector<XblSocialEvent> socialEvents;
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

STDAPI_(bool) XblSocialManagerPresenceRecordIsUserPlayingTitle(
    _In_ XblSocialManagerPresenceRecord* presenceRecord,
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

void populate_social_user_array(
    _In_ xsapi_internal_vector<xbox_social_user*> internalXboxSocialUsers,
    _Out_ XblXboxSocialUser* users
    )
{
    uint32_t i = 0;
    for (auto internalUser : internalXboxSocialUsers)
    {
        users[i].xboxUserId = utils::string_t_to_uint64(internalUser->xbox_user_id());
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
        users[i].presenceRecord.userState = static_cast<XblUserPresenceState>(internalUser->presence_record().user_state());
        
        uint8_t j = 0;
        for (auto& internalTitleRecord : internalUser->presence_record().presence_title_records())
        {
            auto& titleRecord = users[i].presenceRecord.presenceTitleRecords[j];

            titleRecord.titleId = internalTitleRecord.title_id();
            titleRecord.isTitleActive = internalTitleRecord.is_title_active();
            utils::utf8_from_char_t(internalTitleRecord.presence_text(), titleRecord.presenceText, sizeof(titleRecord.presenceText));
            titleRecord.isBroadcasting = internalTitleRecord.is_broadcasting();
            titleRecord.deviceType = static_cast<XblPresenceDeviceType>(internalTitleRecord.device_type());
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
}

STDAPI XblXboxSocialUserGroupGetUsers(
    _In_ XblXboxSocialUserGroup* group,
    _Out_writes_all_(group->usersCount) XblXboxSocialUser* xboxSocialUsers
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF(group == nullptr || xboxSocialUsers == nullptr)

    verify_global_init();
    auto state = get_xbl_social_manager();

    auto groupIter = state->socialUserGroupsMap.find(group);
    XSAPI_ASSERT(groupIter != state->socialUserGroupsMap.end());

    populate_social_user_array(groupIter->second->users(), xboxSocialUsers);
    return S_OK;
}
CATCH_RETURN()

STDAPI XblXboxSocialUserGroupGetUsersFromXboxUserIds(
    _In_ XblXboxSocialUserGroup* group,
    _In_ const uint64_t* xboxUserIds,
    _In_ uint32_t xboxUserIdsCount,
    _Out_writes_to_(xboxUserIdsCount, *xboxSocialUsersCount) XblXboxSocialUser* xboxSocialUsers,
    _Out_opt_ uint32_t* xboxSocialUsersCount
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF(group == nullptr || xboxUserIds == nullptr || xboxSocialUsers == nullptr);

    verify_global_init();
    auto state = get_xbl_social_manager();

    auto groupIter = state->socialUserGroupsMap.find(group);
    XSAPI_ASSERT(groupIter != state->socialUserGroupsMap.end());

    xsapi_internal_vector<xbox_user_id_container> userIdsVector;
    for (uint32_t i = 0; i < xboxUserIdsCount; ++i)
    {
        userIdsVector.push_back(xbox_user_id_container(utils::uint64_to_string_t(xboxUserIds[i]).data()));
    }

    auto result = groupIter->second->get_users_from_xbox_user_ids(userIdsVector);

    populate_social_user_array(result, xboxSocialUsers);
    if (xboxSocialUsersCount != nullptr)
    {
        *xboxSocialUsersCount = static_cast<uint32_t>(result.size());
    }
    return S_OK;
}
CATCH_RETURN()

STDAPI XblXboxSocialUserGroupGetUsersTrackedByGroup(
    _In_ XblXboxSocialUserGroup* group,
    _Out_writes_all_(group->trackedUsersCount) uint64_t* trackedUsers
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF(group == nullptr || trackedUsers == nullptr);

    verify_global_init();

    auto state = get_xbl_social_manager();
    auto groupIter = state->socialUserGroupsMap.find(group);
    XSAPI_ASSERT(groupIter != state->socialUserGroupsMap.end());

    uint32_t count = 0;
    for (auto& internalContainer : groupIter->second->users_tracked_by_social_user_group())
    {
        trackedUsers[count++] = utils::string_t_to_uint64(internalContainer.xbox_user_id());
    }
    return S_OK;
}
CATCH_RETURN()

STDAPI XblSocialManagerAddLocalUser(
    _In_ xbl_user_handle user,
    _In_ XblSocialManagerExtraDetailLevel extraLevelDetail
    ) XBL_NOEXCEPT
try
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
    return utils::hresult_from_error_code(result.err());
}
CATCH_RETURN()

STDAPI XblSocialManagerRemoveLocalUser(
    _In_ xbl_user_handle user
    ) XBL_NOEXCEPT
try
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
    return utils::hresult_from_error_code(result.err());
}
CATCH_RETURN()


STDAPI XblSocialManagerDoWork(
    _Outptr_ XblSocialEvent** socialEvents,
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
            xbl_user_handle userHandle;
#if !XDK_API
            auto userIter = state->localUsersMap.find(internalEvent->user());
            if (userIter == state->localUsersMap.end())
            {
                throw new std::exception("User doesn't exist. Did you call AddLocalUser?");
            }
            userHandle = userIter->second;
#else
            userHandle = internalEvent->user();
#endif
            XblXboxSocialUserGroup* loadedGroup = nullptr;
            if (internalEvent->event_type() == social_event_type::social_user_group_loaded)
            {
                auto groupLoadedEventArgs = std::dynamic_pointer_cast<social_user_group_loaded_event_args_internal>(internalEvent->event_args());
                if (groupLoadedEventArgs != nullptr)
                {
                    auto& groupMap = get_xbl_social_manager()->socialUserGroupsMap;

                    auto iter = groupMap.reverse_find(groupLoadedEventArgs->social_user_group());
                    if (iter != groupMap.reverse_end())
                    {
                        loadedGroup = iter->second;
                    }
                }
            }

            state->socialEvents.push_back(XblSocialEvent
            {
                userHandle,
                static_cast<XblSocialEventType>(internalEvent->event_type()),
                uint32_t(internalEvent->users_affected().size()),
                loadedGroup,
                internalEvent->err().value(),
                static_cast<void*>(internalEvent.get())
            });
        }

        for (auto groupMapping : state->socialUserGroupsMap)
        {
            groupMapping.first->trackedUsersCount = static_cast<uint32_t>(groupMapping.second->users_tracked_by_social_user_group().size());
            groupMapping.first->usersCount = static_cast<uint32_t>(groupMapping.second->users().size());
        }
    }

    *socialEvents = state->socialEvents.data();
    *socialEventsCount = static_cast<uint32_t>(state->socialEvents.size());
    return S_OK;
}
CATCH_RETURN()

STDAPI XblSocialEventGetUsersAffected(
    _In_ XblSocialEvent* socialEvent,
    _Out_writes_(socialEvent->affectedUsersCount) uint64_t* usersAffected
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF(socialEvent == nullptr || usersAffected == nullptr);

    auto internalPtr = static_cast<social_event_internal*>(socialEvent->internalPtr);

    uint32_t count = 0;
    for (auto& internalContainer : internalPtr->users_affected())
    {
        usersAffected[count++] = utils::string_t_to_uint64(internalContainer.xbox_user_id());
    }
    return S_OK;
}
CATCH_RETURN()

STDAPI XblSocialManagerCreateSocialUserGroupFromFilters(
    _In_ xbl_user_handle user,
    _In_ XblPresenceFilter presenceDetailLevel,
    _In_ XblRelationshipFilter filter,
    _Outptr_ XblXboxSocialUserGroup** group
    ) XBL_NOEXCEPT
try
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
    return utils::hresult_from_error_code(result.err());
}
CATCH_RETURN()

STDAPI XblSocialManagerCreateSocialUserGroupFromList(
    _In_ xbl_user_handle user,
    _In_ uint64_t* xboxUserIdList,
    _In_ uint32_t xboxUserIdListCount,
    _Outptr_ XblXboxSocialUserGroup** group
    ) XBL_NOEXCEPT
try
{
    verify_global_init();
    auto state = get_xbl_social_manager();

    auto xboxUserIdVector = utils::xuid_array_to_internal_string_vector(xboxUserIdList, xboxUserIdListCount);

    auto result = social_manager_internal::get_singleton_instance()->create_social_user_group_from_list(
        GET_INTERNAL_USER(user),
        xboxUserIdVector
        );

    if (!result.err())
    {
        *group = state->create_social_user_group(result.payload());
    }
    return utils::hresult_from_error_code(result.err());
}
CATCH_RETURN()

STDAPI XblSocialManagerDestroySocialUserGroup(
    _In_ XblXboxSocialUserGroup* group
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF_NULL(group);

    verify_global_init();

    auto state = get_xbl_social_manager();
    auto groupIter = state->socialUserGroupsMap.find(group);
    XSAPI_ASSERT(groupIter != state->socialUserGroupsMap.end());

    auto result = social_manager_internal::get_singleton_instance()->destroy_social_user_group(groupIter->second);
    if (!result.err())
    {
        xsapi_memory::mem_free(groupIter->first);
        state->socialUserGroupsMap.erase(groupIter);
    }
    return utils::hresult_from_error_code(result.err());
}
CATCH_RETURN()

STDAPI XblSocialManagerGetLocalUsers(
    _Outptr_ xbl_user_handle** users,
    _Out_ uint32_t* userCount
    ) XBL_NOEXCEPT
{
    RETURN_C_INVALIDARGUMENT_IF(users == nullptr || userCount == nullptr);
#if XDK_API
    auto& usersVector = social_manager_internal::get_singleton_instance()->local_users();
#else
    auto& usersVector = get_xbl_social_manager()->localUsersVector;
#endif
    *userCount = static_cast<uint32_t>(usersVector.size());
    *users = usersVector.data();

    return S_OK;
}

STDAPI XblSocialManagerUpdateSocialUserGroup(
    _In_ XblXboxSocialUserGroup* group,
    _In_ uint64_t* users,
    _In_ uint32_t usersCount
    ) XBL_NOEXCEPT
try
{
    verify_global_init();

    auto state = get_xbl_social_manager();
    auto groupIter = state->socialUserGroupsMap.find(group);
    XSAPI_ASSERT(groupIter != state->socialUserGroupsMap.end());

    auto usersVector = utils::xuid_array_to_internal_string_vector(users, usersCount);

    auto result = social_manager_internal::get_singleton_instance()->update_social_user_group(
        groupIter->second,
        usersVector
        );

    return utils::hresult_from_error_code(result.err());
}
CATCH_RETURN()

STDAPI XblSocialManagerSetRichPresencePollingStatus(
    _In_ xbl_user_handle user,
    _In_ bool shouldEnablePolling
    ) XBL_NOEXCEPT
try
{
    verify_global_init();

    auto result = social_manager_internal::get_singleton_instance()->set_rich_presence_polling_status(
        GET_INTERNAL_USER(user),
        shouldEnablePolling
        );
   
    return utils::hresult_from_error_code(result.err());
}
CATCH_RETURN()


STDAPI_(void) XblSocialManagerSetBackgroundWorkAsyncQueue(
    _In_ async_queue_handle_t queue
    ) XBL_NOEXCEPT
try
{
    verify_global_init();
    social_manager_internal::get_singleton_instance()->set_social_graph_background_async_queue(queue);
}
CATCH_RETURN_WITH(;)