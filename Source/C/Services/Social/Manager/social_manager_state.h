// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

struct XSAPI_SOCIAL_MANAGER_VARS
{
public:
    std::map<xbox_live_user_t, XBL_XBOX_LIVE_USER*> cUsersMapping;
    std::vector<XSAPI_XBOX_SOCIAL_USER*> cXboxSocialUserGroupUsers;
    std::vector<XSAPI_XBOX_SOCIAL_USER_GROUP*> cGroups;
    std::vector<XSAPI_SOCIAL_EVENT*> cEvents;
    xbox::services::xbox_live_result<void> cppVoidResult;
    xbox::services::xbox_live_result<std::shared_ptr<xbox_social_user_group>> cppGroupResult;
};