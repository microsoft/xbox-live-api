// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "ClubRoster_WinRT.h"
#include "xsapi/clubs.h"
#include "Utils_WinRT.h"
#include "ClubRoleRecord_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_BEGIN

using namespace Microsoft::Xbox::Services::System;
using namespace xbox::services::clubs;

ClubRoster::ClubRoster(_In_ xbox::services::clubs::club_roster cppObj)
    : m_cppObj(std::move(cppObj))
{
    m_moderators = UtilsWinRT::CreatePlatformVectorFromStdVectorObj<ClubRoleRecord, club_role_record>(m_cppObj.moderators());
    m_reqestedToJoin = UtilsWinRT::CreatePlatformVectorFromStdVectorObj<ClubRoleRecord, club_role_record>(m_cppObj.requested_to_join());
    m_recommended = UtilsWinRT::CreatePlatformVectorFromStdVectorObj<ClubRoleRecord, club_role_record>(m_cppObj.recommended());
    m_banned = UtilsWinRT::CreatePlatformVectorFromStdVectorObj<ClubRoleRecord, club_role_record>(m_cppObj.banned());
}

Windows::Foundation::Collections::IVectorView<ClubRoleRecord^>^ ClubRoster::Moderators::get()
{
    return m_moderators->GetView();
}

Windows::Foundation::Collections::IVectorView<ClubRoleRecord^>^ ClubRoster::RequestedToJoin::get()
{
    return m_reqestedToJoin->GetView();
}

Windows::Foundation::Collections::IVectorView<ClubRoleRecord^>^ ClubRoster::Recommended::get()
{
    return m_recommended->GetView();
}

Windows::Foundation::Collections::IVectorView<ClubRoleRecord^>^ ClubRoster::Banned::get()
{
    return m_banned->GetView();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_END