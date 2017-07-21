// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "Club_WinRT.h"
#include "xsapi/clubs.h"
#include "Utils_WinRT.h"

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;
using namespace Microsoft::Xbox::Services::System;
using namespace xbox::services::clubs;
using namespace xbox::services;

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_BEGIN

#define PROJECT_CLUB_ROLES(task) task.then([](xbox_live_result<std::vector<club_role>> result)                      \
    {                                                                                                               \
        THROW_IF_ERR(result);                                                                                       \
        return UtilsWinRT::CreatePlatformVectorFromStdVectorEnum<ClubRole, club_role>(result.payload())->GetView(); \
    });

Club::Club(_In_ xbox::services::clubs::club cppObj)
    : m_cppObj(std::move(cppObj))
{
}

ClubProfile^ Club::Profile::get()
{
    if (m_profile == nullptr)
    {
        m_profile = ref new ClubProfile(m_cppObj.profile());
    }
    return m_profile;
}

ClubActionSettings^ Club::ActionSettings::get()
{
    if (m_actionSettings == nullptr)
    {
        m_actionSettings = ref new ClubActionSettings(m_cppObj.action_settings());
    }
    return m_actionSettings;
}

Windows::Foundation::Collections::IVectorView<ClubRole>^ Club::ViewerRoles::get()
{
    if (m_viewerRoles == nullptr)
    {
        m_viewerRoles = UtilsWinRT::CreatePlatformVectorFromStdVectorEnum<ClubRole, club_role>(m_cppObj.viewer_roles());
    }
    return m_viewerRoles->GetView();
}

IAsyncOperation<ClubRoster^>^ Club::GetRosterAsync()
{
    auto task = m_cppObj.get_roster()
    .then([](xbox_live_result<club_roster> result)
    {
        THROW_IF_ERR(result);
        return ref new ClubRoster(result.payload());
    });
    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<IVectorView<ClubUserPresenceRecord^>^>^ Club::GetClubUserPresenceRecordsAsync()
{
    auto task = m_cppObj.get_club_user_presence_records()
    .then([](xbox_live_result<std::vector<club_user_presence_record>> result)
    {
        THROW_IF_ERR(result);
        return UtilsWinRT::CreatePlatformVectorFromStdVectorObj<ClubUserPresenceRecord, club_user_presence_record>(result.payload())->GetView();
    });
    return ASYNC_FROM_TASK(task);
}

IAsyncAction^ Club::RenameClubAsync(
    _In_ String^ newName
)
{
    auto task = m_cppObj.rename_club(STRING_T_FROM_PLATFORM_STRING(newName))
        .then([](xbox_live_result<void> result)
    {
        THROW_IF_ERR(result);
    });
    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<IVectorView<ClubRole>^>^ Club::GetRolesAsync(
    _In_ String^ xuid
    )
{
    auto task = PROJECT_CLUB_ROLES(
        m_cppObj.get_roles(
            STRING_T_FROM_PLATFORM_STRING(xuid)
        ));

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<IVectorView<ClubRoleRecord^>^>^ Club::GetRoleRecordsAsync()
{
    auto task = m_cppObj.get_role_records()
    .then([](xbox_live_result<std::vector<club_role_record>> result)
    {
        THROW_IF_ERR(result);
        return UtilsWinRT::CreatePlatformVectorFromStdVectorObj<ClubRoleRecord, club_role_record>(result.payload())->GetView();
    });
    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<IVectorView<ClubRoleRecord^>^>^ Club::GetRoleRecordsAsync(
    _In_ String^ xuid
    )
{
    auto task = m_cppObj.get_role_records(STRING_T_FROM_PLATFORM_STRING(xuid))
    .then([](xbox_live_result<std::vector<club_role_record>> result)
    {
        THROW_IF_ERR(result);
        return UtilsWinRT::CreatePlatformVectorFromStdVectorObj<ClubRoleRecord, club_role_record>(result.payload())->GetView();
    });
    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<ClubPresenceCounts^>^ Club::GetPresenceCountsAsync()
{
    auto task = m_cppObj.get_presence_counts()
    .then([](xbox_live_result<club_presence_counts> result)
    {
        THROW_IF_ERR(result);
        return ref new ClubPresenceCounts(result.payload());
    });
    return ASYNC_FROM_TASK(task);
}

IAsyncAction^ Club::SetPresenceWithinClubAsync(
    _In_ ClubUserPresence presence
    )
{
    auto task = m_cppObj.set_presence_within_club(static_cast<club_user_presence>(presence))
    .then([](xbox_live_result<void> result)
    {
        THROW_IF_ERR(result);
    });
    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<IVectorView<ClubRole>^>^ Club::AddUserToClubAsync()
{
    auto task = PROJECT_CLUB_ROLES(m_cppObj.add_user_to_club());
    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<IVectorView<ClubRole>^>^ Club::RemoveUserFromClubAsync()
{
    auto task = PROJECT_CLUB_ROLES(m_cppObj.remove_user_from_club());
    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<IVectorView<ClubRole>^>^ Club::FollowClubAsync()
{
    auto task = PROJECT_CLUB_ROLES(m_cppObj.follow_club());
    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<IVectorView<ClubRole>^>^ Club::UnfollowClubAsync()
{
    auto task = PROJECT_CLUB_ROLES(m_cppObj.unfollow_club());
    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<IVectorView<ClubRole>^>^ Club::AddUserToClubAsync(
    _In_ String^ xuid
    )
{
    auto task = PROJECT_CLUB_ROLES(
        m_cppObj.add_user_to_club(
            STRING_T_FROM_PLATFORM_STRING(xuid)
            ));

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<IVectorView<ClubRole>^>^ Club::RemoveUserFromClubAsync(
    _In_ String^ xuid
    )
{
    auto task = PROJECT_CLUB_ROLES(
        m_cppObj.remove_user_from_club(
            STRING_T_FROM_PLATFORM_STRING(xuid)
            ));

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<IVectorView<ClubRole>^>^ Club::BanUserFromClubAsync(
    _In_ String^ xuid
    )
{
    auto task = PROJECT_CLUB_ROLES(
        m_cppObj.ban_user_from_club(
            STRING_T_FROM_PLATFORM_STRING(xuid)
            ));

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<IVectorView<ClubRole>^>^ Club::UnbanUserFromClubAsync(
    _In_ String^ xuid
    )
{
    auto task = PROJECT_CLUB_ROLES(
        m_cppObj.unban_user_from_club(
            STRING_T_FROM_PLATFORM_STRING(xuid)
            ));

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<IVectorView<ClubRole>^>^ Club::AddClubModeratorAsync(
    _In_ String^ xuid
    )
{
    auto task = PROJECT_CLUB_ROLES(
        m_cppObj.add_club_moderator(
            STRING_T_FROM_PLATFORM_STRING(xuid)
            ));

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<IVectorView<ClubRole>^>^ Club::RemoveClubModeratorAsync(
    _In_ String^ xuid
    )
{
    auto task = PROJECT_CLUB_ROLES(
        m_cppObj.remove_club_moderator(
            STRING_T_FROM_PLATFORM_STRING(xuid)
            ));

    return ASYNC_FROM_TASK(task);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_END