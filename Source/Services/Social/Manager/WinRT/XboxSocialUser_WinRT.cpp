///*********************************************************
///
/// Copyright (c) Microsoft. All rights reserved.
/// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
/// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
/// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
/// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
///
///*********************************************************
#include "pch.h"
#include "XboxSocialUser_WinRT.h"
#include "TitleHistory_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_BEGIN

XboxSocialUser::XboxSocialUser(
    _In_ xbox::services::social::manager::xbox_social_user* cppObj
    )
{
    if (cppObj != nullptr)
    {
        m_cppObj = *cppObj;
        m_presenceRecord = ref new SocialManagerPresenceRecord(m_cppObj.presence_record());
        m_titleHistory = ref new Microsoft::Xbox::Services::Social::Manager::TitleHistory(m_cppObj.title_history());
        m_preferredColor = ref new Microsoft::Xbox::Services::Social::Manager::PreferredColor(m_cppObj.preferred_color());
    }
}

Microsoft::Xbox::Services::Social::Manager::SocialManagerPresenceRecord^
XboxSocialUser::PresenceRecord::get()
{
    return m_presenceRecord;
}

Microsoft::Xbox::Services::Social::Manager::TitleHistory^
XboxSocialUser::TitleHistory::get()
{
    return m_titleHistory;
}

Microsoft::Xbox::Services::Social::Manager::PreferredColor^
XboxSocialUser::PreferredColor::get()
{
    return m_preferredColor;
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_END