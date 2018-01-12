// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "user_impl_c.h"
#include "social_manager_helper.h"
#include "social_manager_state.h"

using namespace xbox::services;
using namespace xbox::services::system;
using namespace xbox::services::social::manager;

std::shared_ptr<xsapi_singleton> get_singleton_for_social()
{
    auto singleton = get_xsapi_singleton();

    if (singleton->m_socialVars == nullptr)
    {
        singleton->m_socialVars = std::make_shared<XSAPI_SOCIAL_MANAGER_VARS>();
    }
    return singleton;
}

XBL_API bool XBL_CALLING_CONV
SocialManagerPresenceRecordIsUserPlayingTitle(
    _In_ XSAPI_SOCIAL_MANAGER_PRESENCE_RECORD* presenceRecord,
    _In_ uint32_t titleId
    ) XBL_NOEXCEPT
try
{
    verify_global_init();

    return presenceRecord->pImpl->cppSocialManagerPresenceRecord().is_user_playing_title(titleId);
}
CATCH_RETURN_WITH(false);

XBL_API XSAPI_XBOX_SOCIAL_USER** XBL_CALLING_CONV
XboxSocialUserGroupGetUsersFromXboxUserIds(
    _In_ XSAPI_XBOX_SOCIAL_USER_GROUP* group,
    _In_ PCSTR* xboxUserIds,
    _In_ uint32_t xboxUserIdsCount,
    _Out_ uint32_t* xboxSocialUsersCount
    )
try
{
    verify_global_init();

    std::vector<xbox_user_id_container> cXboxUserIdContainers = std::vector<xbox_user_id_container>(xboxUserIdsCount);
    std::vector<std::wstring> cXboxUserIds = std::vector<std::wstring>(xboxUserIdsCount);

    for (size_t i = 0; i < xboxUserIdsCount; i++)
    {
        cXboxUserIds[i] = utils::utf16_from_utf8(xboxUserIds[i]);
        cXboxUserIdContainers[i] = xbox_user_id_container(cXboxUserIds[i].c_str());
    }
       
    return group->pImpl->getUsersFromXboxUserIds(cXboxUserIdContainers, xboxSocialUsersCount);
}
CATCH_RETURN_WITH(nullptr)

XBL_API XBL_RESULT XBL_CALLING_CONV
SocialManagerAddLocalUser(
    _In_ XSAPI_XBOX_LIVE_USER *user,
    _In_ XSAPI_SOCIAL_MANAGER_EXTRA_DETAIL_LEVEL extraLevelDetail,
    _Out_ PCSTR* errMessage
    ) XBL_NOEXCEPT
try
{
    verify_global_init();
    auto singleton = get_singleton_for_social();

    social_manager_extra_detail_level cExtraLevelDetail = static_cast<social_manager_extra_detail_level>(extraLevelDetail);
    singleton->m_socialVars->cppVoidResult = social_manager::get_singleton_instance()->add_local_user(user->pImpl->cppUser(), cExtraLevelDetail);

    *errMessage = singleton->m_socialVars->cppVoidResult.err_message().c_str();
    auto cResult = utils_c::xsapi_result_from_xbox_live_result_err(singleton->m_socialVars->cppVoidResult.err());

    if (cResult == XBL_RESULT::XBL_RESULT_OK)
    {
        singleton->m_socialVars->cUsersMapping[user->pImpl->cppUser()] = user;
    }

    return cResult;
}
CATCH_RETURN()

XBL_API XBL_RESULT XBL_CALLING_CONV
SocialManagerRemoveLocalUser(
    _In_ XSAPI_XBOX_LIVE_USER *user,
    _Out_ PCSTR* errMessage
    ) XBL_NOEXCEPT
try
{
    verify_global_init();
    auto singleton = get_singleton_for_social();

    singleton->m_socialVars->cppVoidResult = social_manager::get_singleton_instance()->remove_local_user(user->pImpl->cppUser());

    *errMessage = singleton->m_socialVars->cppVoidResult.err_message().c_str();
    auto cResult = utils_c::xsapi_result_from_xbox_live_result_err(singleton->m_socialVars->cppVoidResult.err());

    if (cResult == XBL_RESULT::XBL_RESULT_OK)
    {
        singleton->m_socialVars->cUsersMapping.erase(user->pImpl->cppUser());
    }

    return cResult;
}
CATCH_RETURN()


XBL_API XSAPI_SOCIAL_EVENT** XBL_CALLING_CONV
SocialManagerDoWork(
    _Out_ uint32_t* socialEventsCount
    )
try
{
    verify_global_init();
    auto singleton = get_singleton_for_social();

    std::vector<social_event> cppSocialEvents = social_manager::get_singleton_instance()->do_work();
   
    singleton->m_socialVars->cEvents.clear();

    if (cppSocialEvents.size() > 0) 
    {
        for (auto cEvent : cppSocialEvents) 
        {
            singleton->m_socialVars->cEvents.push_back(CreateSocialEventFromCpp(cEvent, singleton->m_socialVars->cGroups));
        }

        for (auto socialUserGroup : singleton->m_socialVars->cGroups)
        {
            if (socialUserGroup != nullptr) 
            {
                socialUserGroup->pImpl->Refresh();
            }
        }
    }

    *socialEventsCount = (uint32_t)singleton->m_socialVars->cEvents.size();
    return singleton->m_socialVars->cEvents.data();
}
CATCH_RETURN_WITH(nullptr)

XBL_API XBL_RESULT XBL_CALLING_CONV
SocialManagerCreateSocialUserGroupFromFilters(
    _In_ XSAPI_XBOX_LIVE_USER *user,
    _In_ XSAPI_PRESENCE_FILTER presenceDetailLevel,
    _In_ XSAPI_RELATIONSHIP_FILTER filter,
    _Out_ XSAPI_XBOX_SOCIAL_USER_GROUP** group,
    _Out_ PCSTR* errMessage
    ) XBL_NOEXCEPT
try
{
    verify_global_init();
    auto singleton = get_singleton_for_social();

    presence_filter cPresenceDetailLevel = static_cast<presence_filter>(presenceDetailLevel);
    relationship_filter cFilter = static_cast<relationship_filter>(filter);
    singleton->m_socialVars->cppGroupResult = social_manager::get_singleton_instance()->create_social_user_group_from_filters(user->pImpl->cppUser(), cPresenceDetailLevel, cFilter);

    auto socialUserGroup = new XSAPI_XBOX_SOCIAL_USER_GROUP();
    socialUserGroup->pImpl = new XSAPI_XBOX_SOCIAL_USER_GROUP_IMPL(singleton->m_socialVars->cppGroupResult.payload(), socialUserGroup);
    singleton->m_socialVars->cGroups.push_back(socialUserGroup);
    *group = socialUserGroup;
    

    *errMessage = singleton->m_socialVars->cppGroupResult.err_message().c_str();
    return utils_c::xsapi_result_from_xbox_live_result_err(singleton->m_socialVars->cppGroupResult.err());
}
CATCH_RETURN()

XBL_API XBL_RESULT XBL_CALLING_CONV
SocialManagerCreateSocialUserGroupFromList(
    _In_ XSAPI_XBOX_LIVE_USER *user,
    _In_ PCSTR* xboxUserIdList,
    _In_ uint32_t xboxUserIdListCount,
    _Out_ XSAPI_XBOX_SOCIAL_USER_GROUP** group,
    _Out_ PCSTR* errMessage
    ) XBL_NOEXCEPT
try
{
    verify_global_init();
    auto singleton = get_singleton_for_social();

    std::vector<string_t> xboxUserIdVector = std::vector<string_t>(xboxUserIdListCount);

    for (uint32_t i = 0; i < xboxUserIdListCount; i++)
    {
        xboxUserIdVector[i] = utils::utf16_from_utf8(xboxUserIdList[i]);
    }
    
    singleton->m_socialVars->cppGroupResult = social_manager::get_singleton_instance()->create_social_user_group_from_list(user->pImpl->cppUser(), xboxUserIdVector);

    auto socialUserGroup = new XSAPI_XBOX_SOCIAL_USER_GROUP();
    socialUserGroup->pImpl = new XSAPI_XBOX_SOCIAL_USER_GROUP_IMPL(singleton->m_socialVars->cppGroupResult.payload(), socialUserGroup);
    singleton->m_socialVars->cGroups.push_back(socialUserGroup);
    *group = socialUserGroup;

    *errMessage = singleton->m_socialVars->cppGroupResult.err_message().c_str();
    return utils_c::xsapi_result_from_xbox_live_result_err(singleton->m_socialVars->cppGroupResult.err());
}
CATCH_RETURN()

XBL_API XBL_RESULT XBL_CALLING_CONV
SocialManagerDestroySocialUserGroup(
    _In_ XSAPI_XBOX_SOCIAL_USER_GROUP *group,
    _Out_ PCSTR* errMessage
    ) XBL_NOEXCEPT
try
{
    verify_global_init();
    auto singleton = get_singleton_for_social();

    // Remove group from our local store of XboxSocialUserGroups
    auto newEnd = std::remove(singleton->m_socialVars->cGroups.begin(), singleton->m_socialVars->cGroups.end(), group);
    singleton->m_socialVars->cGroups.erase(newEnd, singleton->m_socialVars->cGroups.end());

    singleton->m_socialVars->cppVoidResult = social_manager::get_singleton_instance()->destroy_social_user_group(group->pImpl->cppSocialUserGroup());

    *errMessage = singleton->m_socialVars->cppVoidResult.err_message().c_str();
    return utils_c::xsapi_result_from_xbox_live_result_err(singleton->m_socialVars->cppVoidResult.err());
}
CATCH_RETURN()

XBL_API XBL_RESULT XBL_CALLING_CONV
SocialManagerUpdateSocialUserGroup(
    _In_ XSAPI_XBOX_SOCIAL_USER_GROUP *group,
    _In_ PCSTR* users,
    _In_ uint32_t usersCount,
    _Out_ PCSTR* errMessage
    ) XBL_NOEXCEPT
try
{
    verify_global_init();
    auto singleton = get_singleton_for_social();

    std::vector<string_t> usersVector = std::vector<string_t>();

    for (uint32_t i = 0; i < usersCount; i++)
    {
        usersVector.push_back(utils::utf16_from_utf8(users[i]));
    }

    singleton->m_socialVars->cppVoidResult = social_manager::get_singleton_instance()->update_social_user_group(group->pImpl->cppSocialUserGroup(), usersVector);
    group->pImpl->Refresh();

    *errMessage = singleton->m_socialVars->cppVoidResult.err_message().c_str();
    return utils_c::xsapi_result_from_xbox_live_result_err(singleton->m_socialVars->cppVoidResult.err());
}
CATCH_RETURN()

XBL_API XBL_RESULT XBL_CALLING_CONV
SocialManagerSetRichPresencePollingStatus(
    _In_ XSAPI_XBOX_LIVE_USER *user,
    _In_ bool shouldEnablePolling,
    _Out_ PCSTR* errMessage
    ) XBL_NOEXCEPT
try
{
    verify_global_init();
    auto singleton = get_singleton_for_social();
    
    singleton->m_socialVars->cppVoidResult = social_manager::get_singleton_instance()->set_rich_presence_polling_status(user->pImpl->cppUser(), shouldEnablePolling);
    user->pImpl->Refresh();

    *errMessage = singleton->m_socialVars->cppVoidResult.err_message().c_str();
    return utils_c::xsapi_result_from_xbox_live_result_err(singleton->m_socialVars->cppVoidResult.err());
}
CATCH_RETURN()