// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "user_impl_c.h"
#include "singleton_c.h"
#include "xsapi/social_manager_c.h"
#include<time.h>

using namespace xbox::services;
using namespace xbox::services::system;
using namespace xbox::services::social::manager;

struct XSAPI_SOCIAL_MANAGER_PRESENCE_TITLE_RECORD_IMPL
{
public:
    XSAPI_SOCIAL_MANAGER_PRESENCE_TITLE_RECORD_IMPL(
        _In_ social_manager_presence_title_record cppPresenceTitleRecord,
        _In_ XSAPI_SOCIAL_MANAGER_PRESENCE_TITLE_RECORD* cPresenceTitleRecord
    );

    social_manager_presence_title_record cppPresenceTitleRecord() const;

private:
    std::string m_presenceText;

    social_manager_presence_title_record m_cppPresenceTitleRecord;
    XSAPI_SOCIAL_MANAGER_PRESENCE_TITLE_RECORD* m_cPresenceTitleRecord;
};

struct XSAPI_SOCIAL_MANAGER_PRESENCE_RECORD_IMPL
{
public:
    XSAPI_SOCIAL_MANAGER_PRESENCE_RECORD_IMPL(
        _In_ const social_manager_presence_record cppPresenceRecord,
        _In_ XSAPI_SOCIAL_MANAGER_PRESENCE_RECORD *cPresenceRecord
    );

    xbox::services::social::manager::social_manager_presence_record cppSocialManagerPresenceRecord() const;

private:
    void Refresh();

    XSAPI_USER_PRESENCE_STATE m_presenceState;
    std::vector<XSAPI_SOCIAL_MANAGER_PRESENCE_TITLE_RECORD *> m_titleRecords;

    const xbox::services::social::manager::social_manager_presence_record m_cppSocialManagerPresenceRecord;
    XSAPI_SOCIAL_MANAGER_PRESENCE_RECORD* m_cSocialManagerPresenceRecord;
};

struct XSAPI_PREFERRED_COLOR_IMPL
{
public:
    XSAPI_PREFERRED_COLOR_IMPL(
        _In_ preferred_color cppColor,
        _In_ XSAPI_PREFERRED_COLOR* cColor
    );

    preferred_color cppPreferredColor() const;
    
private:
    std::string m_primaryColor;
    std::string m_secondaryColor;
    std::string m_tertiaryColor;

    preferred_color m_cppColor;
    XSAPI_PREFERRED_COLOR* m_cColor;
};

struct XSAPI_XBOX_SOCIAL_USER_IMPL
{
public:
    XSAPI_XBOX_SOCIAL_USER_IMPL(
        _In_ xbox_social_user* cppXboxSocialUser,
        _In_ XSAPI_XBOX_SOCIAL_USER* cXboxSocialUser
    );

    xbox_social_user* cppXboxSocialUser() const;

private:
    std::string m_xboxUserId;
    std::string m_displayName;
    std::string m_realName;
    std::string m_displayPicUrlRaw;
    std::string m_gamerscore;
    std::string m_gamertag;
    XSAPI_SOCIAL_MANAGER_PRESENCE_RECORD* m_presenceRecord;
    XSAPI_TITLE_HISTORY* m_titleHistory;
    XSAPI_PREFERRED_COLOR* m_preferredColor;

    xbox_social_user* m_cppXboxSocialUser;
    XSAPI_XBOX_SOCIAL_USER* m_cXboxSocialUser;
};

struct XSAPI_XBOX_USER_ID_CONTAINER_IMPL
{
public:
    XSAPI_XBOX_USER_ID_CONTAINER_IMPL(
        _In_ xbox_user_id_container cppContainer,
        _In_ XSAPI_XBOX_USER_ID_CONTAINER* cContainer
    );

    xbox_user_id_container cppXboxUserIdContainer() const;

private:
    std::string m_xboxUserId;

    xbox_user_id_container m_cppContainer;
    XSAPI_XBOX_USER_ID_CONTAINER* m_cContainer;
};

struct XSAPI_XBOX_SOCIAL_USER_GROUP_IMPL
{
public:
    XSAPI_XBOX_SOCIAL_USER_GROUP_IMPL(
        _In_ std::shared_ptr<xbox::services::social::manager::xbox_social_user_group> cppSocialUserGroup,
        _In_ XSAPI_XBOX_SOCIAL_USER_GROUP *cSocialUserGroup
    );
    
    void Refresh();

    std::shared_ptr<xbox::services::social::manager::xbox_social_user_group> cppSocialUserGroup() const;

    XSAPI_XBOX_SOCIAL_USER** getUsersFromXboxUserIds(_In_ std::vector<xbox_user_id_container> xuids, _Out_ uint32_t* xboxSocialUsersCount);

private:
    void Init();

    std::vector<XSAPI_XBOX_SOCIAL_USER *> m_users;
    std::vector<XSAPI_XBOX_USER_ID_CONTAINER *> m_usersTrackedBySocialUserGroup;

    std::vector<XSAPI_XBOX_SOCIAL_USER*> m_getUsersFromXboxUserIdsList;

    XSAPI_XBOX_SOCIAL_USER_GROUP* m_cSocialUserGroup;
    std::shared_ptr<xbox::services::social::manager::xbox_social_user_group> m_cppSocialUserGroup;
};

struct XSAPI_SOCIAL_EVENT_IMPL {
public:
    XSAPI_SOCIAL_EVENT_IMPL(
        _In_ social_event cppEvent,
        _In_ XSAPI_SOCIAL_EVENT* cEvent,
        _In_ std::vector<XSAPI_XBOX_SOCIAL_USER_GROUP*> groups
    );

private:
    std::vector<XSAPI_XBOX_USER_ID_CONTAINER *> m_usersAffectedList;
    std::string m_errMessage;

    social_event m_cppEvent;
    XSAPI_SOCIAL_EVENT* m_cEvent;
};

XSAPI_SOCIAL_MANAGER_PRESENCE_TITLE_RECORD* CreateSocialManagerPresenceTitleRecordFromCpp(
    _In_ social_manager_presence_title_record cppPresenceTitleRecord
);

XSAPI_SOCIAL_MANAGER_PRESENCE_RECORD* CreateSocialManagerPresenceRecordFromCpp(
    _In_ const social_manager_presence_record cppPresenceRecord
);

XSAPI_PREFERRED_COLOR* CreatePreferredColorFromCpp(
    _In_ const preferred_color cppColor
);

XSAPI_XBOX_SOCIAL_USER* CreateXboxSocialUserFromCpp(
    _In_ xbox::services::social::manager::xbox_social_user* cppXboxSocialUser
);

XSAPI_SOCIAL_USER_GROUP_LOADED_EVENT_ARGS* CreateSocialUserGroupLoadedEventArgs(
    _In_ std::shared_ptr<xbox::services::social::manager::social_user_group_loaded_event_args> cppSocialUserGroupLoadedEventArgs,
    _In_ std::vector<XSAPI_XBOX_SOCIAL_USER_GROUP*> groups
);

XSAPI_SOCIAL_EVENT* CreateSocialEventFromCpp(
    _In_ xbox::services::social::manager::social_event cppSocialEvent,
    _In_ std::vector<XSAPI_XBOX_SOCIAL_USER_GROUP*> groups
);