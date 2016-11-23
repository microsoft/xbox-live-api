//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#include "SocialUserGroupLoadedEventArgs_WinRT.h"

using namespace xbox::services::social::manager;
using namespace xbox::services;
using namespace Windows::Foundation::Collections;
NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_BEGIN

SocialUserGroupLoadedEventArgs::SocialUserGroupLoadedEventArgs(
    _In_ std::shared_ptr<xbox::services::social::manager::social_user_group_loaded_event_args> cppObj
    ) :
    m_cppObj(std::move(cppObj)),
    m_xboxSocialUserGroup(
        ref new XboxSocialUserGroup(m_cppObj->social_user_group())
        )
{
}

XboxSocialUserGroup^ SocialUserGroupLoadedEventArgs::SocialUserGroup::get()
{
    return m_xboxSocialUserGroup;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_END