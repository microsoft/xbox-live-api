// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/social_manager.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_BEGIN

public enum class SocialUserGroupType
{
    /// <summary>Social user group based off of filters</summary>
    FilterType,

    /// <summary>Social user group based off of list of users</summary>
    UserListType
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_END