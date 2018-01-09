// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "user_impl_c.h"

XSAPI_XBOX_LIVE_USER_IMPL::XSAPI_XBOX_LIVE_USER_IMPL(
    _In_ Windows::System::User^ creationContext,
    _In_ XSAPI_XBOX_LIVE_USER* pUser
)
    : m_pUser(pUser)
{
    if (creationContext != nullptr)
    {
        m_cppUser = std::make_shared<xbox::services::system::xbox_live_user>(creationContext);
    }
    else
    {
        m_cppUser = std::make_shared<xbox::services::system::xbox_live_user>();
    }
}

XSAPI_XBOX_LIVE_USER_IMPL::XSAPI_XBOX_LIVE_USER_IMPL(
    _In_ std::shared_ptr<xbox::services::system::xbox_live_user> cppUser, 
    _In_ XSAPI_XBOX_LIVE_USER *cUser
)
    : m_pUser(cUser), m_cppUser(cppUser)
{
    Refresh();
}

void XSAPI_XBOX_LIVE_USER_IMPL::Refresh()
{
    if (m_pUser != nullptr)
    {
        m_xboxUserId = utils_c::to_utf8string(m_cppUser->xbox_user_id());
        m_pUser->xboxUserId = m_xboxUserId.data();

        m_gamertag = utils_c::to_utf8string(m_cppUser->gamertag());
        m_pUser->gamertag = m_gamertag.data();

        m_ageGroup = utils_c::to_utf8string(m_cppUser->age_group());
        m_pUser->ageGroup = m_ageGroup.data();

        m_privileges = utils_c::to_utf8string(m_cppUser->privileges());
        m_pUser->privileges = m_privileges.data();

        m_pUser->isSignedIn = m_cppUser->is_signed_in();

#if WINAPI_FAMILY && WINAPI_FAMILY==WINAPI_FAMILY_APP
        m_webAccountId = utils_c::to_utf8string(m_cppUser->web_account_id());
        m_pUser->webAccountId = m_webAccountId.data();
#endif
        m_pUser->windowsSystemUser = m_cppUser->windows_system_user();
    }
}

std::shared_ptr<xbox::services::system::xbox_live_user> XSAPI_XBOX_LIVE_USER_IMPL::cppUser() const
{
    return m_cppUser;
}