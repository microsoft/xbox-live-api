// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "social_manager_helper.h"
#include "social_manager_state.h"

XSAPI_SOCIAL_MANAGER_PRESENCE_TITLE_RECORD_IMPL::XSAPI_SOCIAL_MANAGER_PRESENCE_TITLE_RECORD_IMPL(
    _In_ social_manager_presence_title_record cppPresenceTitleRecord,
    _In_ XSAPI_SOCIAL_MANAGER_PRESENCE_TITLE_RECORD* cPresenceTitleRecord
) : m_cPresenceTitleRecord(cPresenceTitleRecord), m_cppPresenceTitleRecord(cppPresenceTitleRecord)
{
    m_cPresenceTitleRecord->isTitleActive = m_cppPresenceTitleRecord.is_title_active();

    m_cPresenceTitleRecord->deviceType = static_cast<XSAPI_PRESENCE_DEVICE_TYPE>(m_cppPresenceTitleRecord.device_type());

    m_cPresenceTitleRecord->isBroadcasting = m_cppPresenceTitleRecord.is_broadcasting();

    m_cPresenceTitleRecord->titleId = m_cppPresenceTitleRecord.title_id();

    m_presenceText = utils::utf8_from_utf16(m_cppPresenceTitleRecord.presence_text());
    m_cPresenceTitleRecord->presenceText = m_presenceText.c_str();
}

social_manager_presence_title_record XSAPI_SOCIAL_MANAGER_PRESENCE_TITLE_RECORD_IMPL::cppPresenceTitleRecord() const
{
    return m_cppPresenceTitleRecord;
}

XSAPI_SOCIAL_MANAGER_PRESENCE_RECORD_IMPL::XSAPI_SOCIAL_MANAGER_PRESENCE_RECORD_IMPL(
    _In_ const social_manager_presence_record cppPresenceRecord,
    _In_ XSAPI_SOCIAL_MANAGER_PRESENCE_RECORD *cPresenceRecord
) : m_cppSocialManagerPresenceRecord(cppPresenceRecord), m_cSocialManagerPresenceRecord(cPresenceRecord)
{
    Refresh();
}

xbox::services::social::manager::social_manager_presence_record XSAPI_SOCIAL_MANAGER_PRESENCE_RECORD_IMPL::cppSocialManagerPresenceRecord() const
{
    return m_cppSocialManagerPresenceRecord;
}

void XSAPI_SOCIAL_MANAGER_PRESENCE_RECORD_IMPL::Refresh()
{
    if (m_cSocialManagerPresenceRecord != nullptr)
    {
        m_presenceState = static_cast<XSAPI_USER_PRESENCE_STATE>(m_cppSocialManagerPresenceRecord.user_state());
        m_cSocialManagerPresenceRecord->userState = m_presenceState;

        m_titleRecords.clear();
        for (auto cppTitleRecord : m_cppSocialManagerPresenceRecord.presence_title_records())
        {
            m_titleRecords.push_back(CreateSocialManagerPresenceTitleRecordFromCpp(cppTitleRecord));
        }
        m_cSocialManagerPresenceRecord->presenceTitleRecords = m_titleRecords.data();
        m_cSocialManagerPresenceRecord->presenceTitleRecordsCount = (uint32_t)m_titleRecords.size();
    }
}

XSAPI_PREFERRED_COLOR_IMPL::XSAPI_PREFERRED_COLOR_IMPL(
    _In_ preferred_color cppColor,
    _In_ XSAPI_PREFERRED_COLOR* cColor
) : m_cColor(cColor), m_cppColor(cppColor)
{
    m_primaryColor = utils::utf8_from_utf16(m_cppColor.primary_color());
    m_cColor->primaryColor = m_primaryColor.c_str();

    m_secondaryColor = utils::utf8_from_utf16(m_cppColor.secondary_color());
    m_cColor->secondaryColor = m_secondaryColor.c_str();

    m_tertiaryColor = utils::utf8_from_utf16(m_cppColor.tertiary_color());
    m_cColor->tertiaryColor = m_tertiaryColor.c_str();
}

preferred_color XSAPI_PREFERRED_COLOR_IMPL::cppPreferredColor() const
{
    return m_cppColor;
}

XSAPI_XBOX_SOCIAL_USER_IMPL::XSAPI_XBOX_SOCIAL_USER_IMPL(
    _In_ xbox_social_user* cppXboxSocialUser,
    _In_ XSAPI_XBOX_SOCIAL_USER* cXboxSocialUser
) : m_cXboxSocialUser(cXboxSocialUser), m_cppXboxSocialUser(cppXboxSocialUser)
{
    m_xboxUserId = utils::utf8_from_utf16(std::wstring(m_cppXboxSocialUser->xbox_user_id()));
    m_cXboxSocialUser->xboxUserId = m_xboxUserId.c_str();

    m_cXboxSocialUser->isFavorite = m_cppXboxSocialUser->is_favorite();

    m_cXboxSocialUser->isFollowingUser = m_cppXboxSocialUser->is_following_user();

    m_cXboxSocialUser->isFollowedByCaller = m_cppXboxSocialUser->is_followed_by_caller();

    m_displayName = utils::utf8_from_utf16(std::wstring(m_cppXboxSocialUser->display_name()));
    m_cXboxSocialUser->displayName = m_displayName.c_str();

    m_realName = utils::utf8_from_utf16(std::wstring(m_cppXboxSocialUser->real_name()));
    m_cXboxSocialUser->realName = m_realName.c_str();

    m_displayPicUrlRaw = utils::utf8_from_utf16(std::wstring(m_cppXboxSocialUser->display_pic_url_raw()));
    m_cXboxSocialUser->displayPicUrlRaw = m_displayPicUrlRaw.c_str();

    m_cXboxSocialUser->useAvatar = m_cppXboxSocialUser->use_avatar();

    m_gamerscore = utils::utf8_from_utf16(std::wstring(m_cppXboxSocialUser->gamerscore()));
    m_cXboxSocialUser->gamerscore = m_gamerscore.c_str();

    m_gamertag = utils::utf8_from_utf16(std::wstring(m_cppXboxSocialUser->gamertag()));
    m_cXboxSocialUser->gamertag = m_gamertag.c_str();

    m_presenceRecord = CreateSocialManagerPresenceRecordFromCpp(m_cppXboxSocialUser->presence_record());
    m_cXboxSocialUser->presenceRecord = m_presenceRecord;

    auto cppTitleHistory = m_cppXboxSocialUser->title_history();
    m_titleHistory = new XSAPI_TITLE_HISTORY();
    m_titleHistory->userHasPlayed = cppTitleHistory.has_user_played();
    m_titleHistory->lastTimeUserPlayed = utils::time_t_from_datetime(cppTitleHistory.last_time_user_played());
    m_cXboxSocialUser->titleHistory = m_titleHistory;

    m_preferredColor = new XSAPI_PREFERRED_COLOR();
    m_preferredColor->pImpl = new XSAPI_PREFERRED_COLOR_IMPL(m_cppXboxSocialUser->preferred_color(), m_preferredColor);
    m_cXboxSocialUser->preferredColor = m_preferredColor;
}

xbox_social_user* XSAPI_XBOX_SOCIAL_USER_IMPL::cppXboxSocialUser() const
{
    return m_cppXboxSocialUser;
}

XSAPI_XBOX_USER_ID_CONTAINER_IMPL::XSAPI_XBOX_USER_ID_CONTAINER_IMPL(
    _In_ xbox_user_id_container cppContainer,
    _In_ XSAPI_XBOX_USER_ID_CONTAINER* cContainer
) : m_cContainer(cContainer), m_cppContainer(cppContainer)
{
    m_xboxUserId = utils::utf8_from_utf16(m_cppContainer.xbox_user_id());
    m_cContainer->xboxUserId = m_xboxUserId.c_str();
}

xbox_user_id_container XSAPI_XBOX_USER_ID_CONTAINER_IMPL::cppXboxUserIdContainer() const
{
    return m_cppContainer;
}

XSAPI_XBOX_SOCIAL_USER_GROUP_IMPL::XSAPI_XBOX_SOCIAL_USER_GROUP_IMPL(
    _In_ std::shared_ptr<xbox::services::social::manager::xbox_social_user_group> cppSocialUserGroup,
    _In_ XSAPI_XBOX_SOCIAL_USER_GROUP *cSocialUserGroup
) : m_cppSocialUserGroup(cppSocialUserGroup), m_cSocialUserGroup(cSocialUserGroup)
{
    Init();
}

void XSAPI_XBOX_SOCIAL_USER_GROUP_IMPL::Refresh()
{
    if (m_cSocialUserGroup != nullptr)
    {
        const std::vector<xbox::services::social::manager::xbox_social_user *> cppUsers = m_cppSocialUserGroup->users();
        m_users.clear();
        for (auto cppUser : cppUsers)
        {
            m_users.push_back(CreateXboxSocialUserFromCpp(cppUser));
        }
        m_cSocialUserGroup->users = m_users.data();
        m_cSocialUserGroup->usersCount = (uint32_t)cppUsers.size();

        m_cSocialUserGroup->socialUserGroupType = static_cast<XSAPI_SOCIAL_USER_GROUP_TYPE>(m_cppSocialUserGroup->social_user_group_type());

        auto cppTrackedUsers = m_cppSocialUserGroup->users_tracked_by_social_user_group();
        m_usersTrackedBySocialUserGroup.clear();
        for (auto cppUserIdContainer : cppTrackedUsers)
        {
            auto cUserIdContainer = new XSAPI_XBOX_USER_ID_CONTAINER();
            cUserIdContainer->pImpl = new XSAPI_XBOX_USER_ID_CONTAINER_IMPL(cppUserIdContainer, cUserIdContainer);
            m_usersTrackedBySocialUserGroup.push_back(cUserIdContainer);
        }
        m_cSocialUserGroup->usersTrackedBySocialUserGroup = m_usersTrackedBySocialUserGroup.data();
        m_cSocialUserGroup->usersTrackedBySocialUserGroupCount = (uint32_t)m_usersTrackedBySocialUserGroup.size();

        m_cSocialUserGroup->localUser->pImpl->Refresh();

        m_cSocialUserGroup->presenceFilterOfGroup = static_cast<XSAPI_PRESENCE_FILTER>(m_cppSocialUserGroup->presence_filter_of_group());

        m_cSocialUserGroup->relationshipFilterOfGroup = static_cast<XSAPI_RELATIONSHIP_FILTER>(m_cppSocialUserGroup->relationship_filter_of_group());
    }
}

XSAPI_XBOX_SOCIAL_USER** XSAPI_XBOX_SOCIAL_USER_GROUP_IMPL::getUsersFromXboxUserIds(
    _In_ std::vector<xbox_user_id_container> xuids,
    _Out_ uint32_t* xboxSocialUsersCount
)
{
    auto cppXboxSocialUsers = m_cppSocialUserGroup->get_users_from_xbox_user_ids(xuids);
    *xboxSocialUsersCount = (uint32_t)cppXboxSocialUsers.size();

    m_getUsersFromXboxUserIdsList = std::vector<XSAPI_XBOX_SOCIAL_USER*>(*xboxSocialUsersCount);
    for (uint32_t i = 0; i < *xboxSocialUsersCount; i++)
    {
        m_getUsersFromXboxUserIdsList[i] = CreateXboxSocialUserFromCpp(cppXboxSocialUsers[i]);
    }
    return m_getUsersFromXboxUserIdsList.data();
}

std::shared_ptr<xbox::services::social::manager::xbox_social_user_group> XSAPI_XBOX_SOCIAL_USER_GROUP_IMPL::cppSocialUserGroup() const
{
    return m_cppSocialUserGroup;
}

void XSAPI_XBOX_SOCIAL_USER_GROUP_IMPL::Init()
{
    auto mapping = get_xsapi_singleton()->m_socialVars->cUsersMapping;

    auto user = new XBL_XBOX_LIVE_USER();
    if (mapping.find(m_cppSocialUserGroup->local_user()) != mapping.end())
    {
        user = mapping[m_cppSocialUserGroup->local_user()];
    }
    else
    {
        throw new std::exception("User doesn't exist. Did you call AddLocalUser?");
    }
    m_cSocialUserGroup->localUser = user;

    Refresh();
}

XSAPI_SOCIAL_EVENT_IMPL::XSAPI_SOCIAL_EVENT_IMPL(
    _In_ social_event cppEvent,
    _In_ XSAPI_SOCIAL_EVENT* cEvent,
    _In_ std::vector<XSAPI_XBOX_SOCIAL_USER_GROUP*> groups
) : m_cEvent(cEvent), m_cppEvent(cppEvent)
{
    auto mapping = get_xsapi_singleton()->m_socialVars->cUsersMapping;

    auto user = new XBL_XBOX_LIVE_USER();
    if (mapping.find(m_cppEvent.user()) != mapping.end())
    {
        user = mapping[m_cppEvent.user()];
    }
    else
    {
        throw new std::exception("User doesn't exist. Did you call AddLocalUser?");
    }
    m_cEvent->user = user;

    m_cEvent->eventType = static_cast<XSAPI_SOCIAL_EVENT_TYPE>(m_cppEvent.event_type());

    for (auto user : m_cppEvent.users_affected())
    {
        auto container = new XSAPI_XBOX_USER_ID_CONTAINER();
        container->pImpl = new XSAPI_XBOX_USER_ID_CONTAINER_IMPL(user, container);
        m_usersAffectedList.push_back(container);
    }

    m_cEvent->usersAffected = m_usersAffectedList.data();

    m_cEvent->usersAffectedCount = (uint32_t)m_cppEvent.users_affected().size();

    try
    {
        auto cSocialUserGroupLoadedEventArgs = std::dynamic_pointer_cast<xbox::services::social::manager::social_user_group_loaded_event_args>(m_cppEvent.event_args());
        if (cSocialUserGroupLoadedEventArgs != NULL)
        {
            m_cEvent->eventArgs = CreateSocialUserGroupLoadedEventArgs(cSocialUserGroupLoadedEventArgs, groups);
        }
    }
    catch (const std::exception&)
    {
        // not a social_user_group_loaded_event_args
    }

    m_cEvent->err = m_cppEvent.err().value();

    m_errMessage = m_cppEvent.err_message();
    m_cEvent->errMessage = m_errMessage.c_str();
}

XSAPI_SOCIAL_MANAGER_PRESENCE_TITLE_RECORD* CreateSocialManagerPresenceTitleRecordFromCpp(
    _In_ social_manager_presence_title_record cppPresenceTitleRecord
)
{
    auto cPresenceTitleRecord = new XSAPI_SOCIAL_MANAGER_PRESENCE_TITLE_RECORD();
    cPresenceTitleRecord->pImpl = new XSAPI_SOCIAL_MANAGER_PRESENCE_TITLE_RECORD_IMPL(cppPresenceTitleRecord, cPresenceTitleRecord);

    return cPresenceTitleRecord;
}

XSAPI_SOCIAL_MANAGER_PRESENCE_RECORD* CreateSocialManagerPresenceRecordFromCpp(
    _In_ const social_manager_presence_record cppPresenceRecord
)
{
    auto presenceRecord = new XSAPI_SOCIAL_MANAGER_PRESENCE_RECORD();
    presenceRecord->pImpl = new XSAPI_SOCIAL_MANAGER_PRESENCE_RECORD_IMPL(cppPresenceRecord, presenceRecord);
    return presenceRecord;
}

XSAPI_PREFERRED_COLOR* CreatePreferredColorFromCpp(
    _In_ const preferred_color cppColor
)
{
    auto preferredColor = new XSAPI_PREFERRED_COLOR();
    preferredColor->pImpl = new XSAPI_PREFERRED_COLOR_IMPL(cppColor, preferredColor);
    return preferredColor;
}

XSAPI_XBOX_SOCIAL_USER* CreateXboxSocialUserFromCpp(
    _In_ xbox::services::social::manager::xbox_social_user* cppXboxSocialUser
)
{
    auto cXboxSocialUser = new XSAPI_XBOX_SOCIAL_USER();
    cXboxSocialUser->pImpl = new XSAPI_XBOX_SOCIAL_USER_IMPL(cppXboxSocialUser, cXboxSocialUser);

    return cXboxSocialUser;
}

XSAPI_SOCIAL_USER_GROUP_LOADED_EVENT_ARGS* CreateSocialUserGroupLoadedEventArgs(
    _In_ std::shared_ptr<xbox::services::social::manager::social_user_group_loaded_event_args> cppSocialUserGroupLoadedEventArgs,
    _In_ std::vector<XSAPI_XBOX_SOCIAL_USER_GROUP*> groups
)
{
    XSAPI_XBOX_SOCIAL_USER_GROUP* groupAffected = new XSAPI_XBOX_SOCIAL_USER_GROUP();
    for (auto group : groups)
    {
        if (group->pImpl->cppSocialUserGroup() == cppSocialUserGroupLoadedEventArgs->social_user_group()) {
            groupAffected = group;
        }
    }

    auto args = new XSAPI_SOCIAL_USER_GROUP_LOADED_EVENT_ARGS();
    args->socialUserGroup = groupAffected;

    return args;
}

XSAPI_SOCIAL_EVENT* CreateSocialEventFromCpp(
    _In_ xbox::services::social::manager::social_event cppSocialEvent,
    _In_ std::vector<XSAPI_XBOX_SOCIAL_USER_GROUP*> groups
)
{
    auto cSocialEvent = new XSAPI_SOCIAL_EVENT();
    cSocialEvent->pImpl = new XSAPI_SOCIAL_EVENT_IMPL(cppSocialEvent, cSocialEvent, groups);

    return cSocialEvent;
}