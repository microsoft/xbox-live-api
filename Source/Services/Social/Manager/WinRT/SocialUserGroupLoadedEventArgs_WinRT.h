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
#include "XboxSocialUserGroup_WinRT.h"
#include "SocialEventArgs_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_BEGIN

public ref class SocialUserGroupLoadedEventArgs sealed : SocialEventArgs
{
public:
    /// <summary>
    /// The loaded social user group
    /// </summary>
    property XboxSocialUserGroup^ SocialUserGroup
    {
        XboxSocialUserGroup^ get();
    };

internal:
    SocialUserGroupLoadedEventArgs(
        _In_ std::shared_ptr<xbox::services::social::manager::social_user_group_loaded_event_args> cppObj
        );

private:
    std::shared_ptr<xbox::services::social::manager::social_user_group_loaded_event_args> m_cppObj;
    XboxSocialUserGroup^ m_xboxSocialUserGroup;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_END