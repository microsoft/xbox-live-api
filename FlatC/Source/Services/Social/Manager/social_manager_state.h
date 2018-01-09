// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/social_manager_c.h"

using namespace xbox::services;
using namespace xbox::services::system;
using namespace xbox::services::social::manager;

struct XSAPI_SOCIAL_MANAGER_VARS
{
public:
    std::map<xbox_live_user_t, XSAPI_XBOX_LIVE_USER*> cUsersMapping;
    std::vector<XSAPI_XBOX_SOCIAL_USER*> cXboxSocialUserGroupUsers;
    std::vector<XSAPI_XBOX_SOCIAL_USER_GROUP*> cGroups;
    std::vector<XSAPI_SOCIAL_EVENT*> cEvents;
    xbox_live_result<void> cppVoidResult;
    xbox_live_result<std::shared_ptr<xbox_social_user_group>> cppGroupResult;
};