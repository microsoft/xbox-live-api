// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "types_c.h"
#include "xsapi-c/errors_c.h"
#include "xsapi-c/system_c.h"
#include "xsapi-c/presence_c.h"

#if defined(__cplusplus)
extern "C" {
#endif

#if !XDK_API

struct XSAPI_PREFERRED_COLOR_IMPL;
struct XSAPI_SOCIAL_MANAGER_PRESENCE_TITLE_RECORD_IMPL;
struct XSAPI_XBOX_USER_ID_CONTAINER_IMPL;
struct XSAPI_SOCIAL_EVENT_IMPL;
struct XSAPI_XBOX_SOCIAL_USER_IMPL;
struct XSAPI_XBOX_SOCIAL_USER_GROUP_IMPL;
struct XSAPI_SOCIAL_MANAGER_PRESENCE_RECORD_IMPL;

typedef enum XSAPI_SOCIAL_MANAGER_EXTRA_DETAIL_LEVEL {
    XSAPI_SOCIAL_MANAGER_EXTRA_DETAIL_LEVEL_NO_EXTRA_DETAIL,
    XSAPI_SOCIAL_MANAGER_EXTRA_DETAIL_LEVEL_TITLE_HISTORY_LEVEL = 0x1,
    XSAPI_SOCIAL_MANAGER_EXTRA_DETAIL_LEVEL_PREFERRED_COLOR_LEVEL = 0x2,
} XSAPI_SOCIAL_MANAGER_EXTRA_DETAIL_LEVEL;

typedef enum XSAPI_PRESENCE_FILTER
{
    XSAPI_PRESENCE_FILTER_UNKNOWN,
    XSAPI_PRESENCE_FILTER_TITLE_ONLINE,
    XSAPI_PRESENCE_FILTER_TITLE_OFFLINE,
    XSAPI_PRESENCE_FILTER_ALL_ONLINE,
    XSAPI_PRESENCE_FILTER_ALL_OFFLINE,
    XSAPI_PRESENCE_FILTER_ALL_TITLE,
    XSAPI_PRESENCE_FILTER_ALL
} XSAPI_PRESENCE_FILTER;

typedef enum XSAPI_SOCIAL_EVENT_TYPE
{
    XSAPI_SOCIAL_EVENT_TYPE_USERS_ADDED_TO_SOCIAL_GRAPH,
    XSAPI_SOCIAL_EVENT_TYPE_USERS_REMOVED_FROM_SOCIAL_GRAPH,
    XSAPI_SOCIAL_EVENT_TYPE_PRESENCE_CHANGED,
    XSAPI_SOCIAL_EVENT_TYPE_PROFILES_CHANGED,
    XSAPI_SOCIAL_EVENT_TYPE_SOCIAL_RELATIONSHIPS,
    XSAPI_SOCIAL_EVENT_TYPE_LOCAL_USER_ADDED,
    XSAPI_SOCIAL_EVENT_TYPE_LOCAL_USER_REMOVED,
    XSAPI_SOCIAL_EVENT_TYPE_SOCIAL_USER_GROUP_LOADED,
    XSAPI_SOCIAL_EVENT_TYPE_SOCIAL_USER_GROUP_UPDATED,
    XSAPI_SOCIAL_EVENT_TYPE_UNKNOWN_EVENT
} XSAPI_SOCIAL_EVENT_TYPE;

typedef enum XSAPI_RELATIONSHIP_FILTER 
{
    XSAPI_RELATIONSHIP_FILTER_FRIENDS,
    XSAPI_RELATIONSHIP_FILTER_FAVORITE
} XSAPI_RELATIONSHIP_FILTER;

typedef enum XSAPI_SOCIAL_USER_GROUP_TYPE 
{
    XSAPI_SOCIAL_USER_GROUP_TYPE_FILTER_TYPE,
    XSAPI_SOCIAL_USER_GROUP_TYPE_USER_LIST_TYPE
} XSAPI_SOCIAL_USER_GROUP_TYPE;

typedef struct XSAPI_TITLE_HISTORY
{
    bool userHasPlayed;
    time_t lastTimeUserPlayed;
} XSAPI_TITLE_HISTORY;

typedef struct XSAPI_PREFERRED_COLOR
{
    PCSTR primaryColor;
    PCSTR secondaryColor;
    PCSTR tertiaryColor;

    XSAPI_PREFERRED_COLOR_IMPL* pImpl;
} XSAPI_PREFERRED_COLOR;

typedef struct XSAPI_SOCIAL_MANAGER_PRESENCE_TITLE_RECORD
{
    bool isTitleActive;
    bool isBroadcasting;
    XSAPI_PRESENCE_DEVICE_TYPE deviceType;
    uint32_t titleId;
    PCSTR presenceText;

    XSAPI_SOCIAL_MANAGER_PRESENCE_TITLE_RECORD_IMPL* pImpl;
} XSAPI_SOCIAL_MANAGER_PRESENCE_TITLE_RECORD;

typedef struct XSAPI_SOCIAL_MANAGER_PRESENCE_RECORD
{
    XSAPI_USER_PRESENCE_STATE userState;
    XSAPI_SOCIAL_MANAGER_PRESENCE_TITLE_RECORD** presenceTitleRecords;
    uint32_t presenceTitleRecordsCount;

    XSAPI_SOCIAL_MANAGER_PRESENCE_RECORD_IMPL* pImpl;
} XSAPI_SOCIAL_MANAGER_PRESENCE_RECORD;

XBL_API bool XBL_CALLING_CONV
SocialManagerPresenceRecordIsUserPlayingTitle(
    _In_ XSAPI_SOCIAL_MANAGER_PRESENCE_RECORD* presenceRecord,
    _In_ uint32_t titleId
    );

typedef struct XSAPI_XBOX_SOCIAL_USER
{
    PCSTR xboxUserId;
    bool isFavorite;
    bool isFollowingUser;
    bool isFollowedByCaller;
    PCSTR displayName;
    PCSTR realName;
    PCSTR displayPicUrlRaw;
    bool useAvatar;
    PCSTR gamerscore;
    PCSTR gamertag;
    XSAPI_SOCIAL_MANAGER_PRESENCE_RECORD* presenceRecord;
    XSAPI_TITLE_HISTORY* titleHistory;
    XSAPI_PREFERRED_COLOR* preferredColor;

    XSAPI_XBOX_SOCIAL_USER_IMPL* pImpl;
} XSAPI_XBOX_SOCIAL_USER;

typedef struct XSAPI_SOCIAL_EVENT_ARGS
{
} XSAPI_SOCIAL_EVENT_ARGS;

typedef struct XSAPI_XBOX_USER_ID_CONTAINER 
{
    PCSTR xboxUserId;

    XSAPI_XBOX_USER_ID_CONTAINER_IMPL* pImpl;
} XSAPI_XBOX_USER_ID_CONTAINER;

typedef struct XSAPI_SOCIAL_EVENT
{
    XBL_XBOX_LIVE_USER* user;
    XSAPI_SOCIAL_EVENT_TYPE eventType;
    XSAPI_XBOX_USER_ID_CONTAINER** usersAffected;
    uint32_t usersAffectedCount;
    XSAPI_SOCIAL_EVENT_ARGS* eventArgs;
    int32_t err;
    PCSTR errMessage;

    XSAPI_SOCIAL_EVENT_IMPL* pImpl;
} XSAPI_SOCIAL_EVENT;

typedef struct XSAPI_XBOX_SOCIAL_USER_GROUP
{
    XSAPI_XBOX_SOCIAL_USER** users;
    uint32_t usersCount;
    XSAPI_SOCIAL_USER_GROUP_TYPE socialUserGroupType;
    XSAPI_XBOX_USER_ID_CONTAINER** usersTrackedBySocialUserGroup;
    uint32_t usersTrackedBySocialUserGroupCount;
    XBL_XBOX_LIVE_USER* localUser;
    XSAPI_PRESENCE_FILTER presenceFilterOfGroup;
    XSAPI_RELATIONSHIP_FILTER relationshipFilterOfGroup;

    XSAPI_XBOX_SOCIAL_USER_GROUP_IMPL *pImpl;
} XSAPI_XBOX_SOCIAL_USER_GROUP;

XBL_API XSAPI_XBOX_SOCIAL_USER** XBL_CALLING_CONV
XboxSocialUserGroupGetUsersFromXboxUserIds(
    _In_ XSAPI_XBOX_SOCIAL_USER_GROUP* group,
    _In_ PCSTR* xboxUserIds,
    _In_ uint32_t xboxUserIdsCount,
    _Out_ uint32_t* xboxSocialUsersCount
);

typedef struct XSAPI_SOCIAL_USER_GROUP_LOADED_EVENT_ARGS : XSAPI_SOCIAL_EVENT_ARGS 
{
    XSAPI_XBOX_SOCIAL_USER_GROUP* socialUserGroup;
} XSAPI_SOCIAL_USER_GROUP_LOADED_EVENT_ARGS;

// todo set
typedef struct XSAPI_SOCIAL_MANAGER
{
    PCSTR localUsers;
} XSAPI_SOCIAL_MANAGER;

XBL_API XBL_RESULT XBL_CALLING_CONV
SocialManagerAddLocalUser(
    _In_ XBL_XBOX_LIVE_USER *user,
    _In_ XSAPI_SOCIAL_MANAGER_EXTRA_DETAIL_LEVEL extraLevelDetail,
    _Out_ PCSTR* errMessage
    );

XBL_API XBL_RESULT XBL_CALLING_CONV
SocialManagerRemoveLocalUser(
    _In_ XBL_XBOX_LIVE_USER *user,
    _Out_ PCSTR* errMessage
    );

XBL_API XSAPI_SOCIAL_EVENT** XBL_CALLING_CONV
SocialManagerDoWork(
    _Out_ uint32_t* socialEventsCount
    );

XBL_API XBL_RESULT XBL_CALLING_CONV
SocialManagerCreateSocialUserGroupFromFilters(
    _In_ XBL_XBOX_LIVE_USER *user,
    _In_ XSAPI_PRESENCE_FILTER presenceDetailLevel,
    _In_ XSAPI_RELATIONSHIP_FILTER filter,
    _Out_ XSAPI_XBOX_SOCIAL_USER_GROUP** group,
    _Out_ PCSTR* errMessage
    );

XBL_API XBL_RESULT XBL_CALLING_CONV
SocialManagerCreateSocialUserGroupFromList(
    _In_ XBL_XBOX_LIVE_USER *user,
    _In_ PCSTR* xboxUserIdList,
    _In_ uint32_t xboxUserIdListCount,
    _Out_ XSAPI_XBOX_SOCIAL_USER_GROUP** group,
    _Out_ PCSTR* errMessage
    );

XBL_API XBL_RESULT XBL_CALLING_CONV
SocialManagerDestroySocialUserGroup(
    _In_ XSAPI_XBOX_SOCIAL_USER_GROUP *group,
    _Out_ PCSTR* errMessage
    );

XBL_API XBL_RESULT XBL_CALLING_CONV
SocialManagerUpdateSocialUserGroup(
    _In_ XSAPI_XBOX_SOCIAL_USER_GROUP *group,
    _In_ PCSTR* users,
    _In_ uint32_t usersCount,
    _Out_ PCSTR* errMessage
    );

XBL_API XBL_RESULT XBL_CALLING_CONV
SocialManagerSetRichPresencePollingStatus(
    _In_ XBL_XBOX_LIVE_USER *user,
    _In_ bool shouldEnablePolling,
    _Out_ PCSTR* errMessage
    );
#endif //!XDK_API

#if defined(__cplusplus)
} // end extern "C"
#endif // defined(__cplusplus)