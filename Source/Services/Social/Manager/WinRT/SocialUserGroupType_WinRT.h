//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
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