// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/clubs.h"
#include "Macros_WinRT.h"
#include "ClubRoleRecord_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_BEGIN

public ref class ClubRoster sealed
{
public:
    /// <summary>Club moderators</summary>
    property Windows::Foundation::Collections::IVectorView<ClubRoleRecord^>^ Moderators
    {
        Windows::Foundation::Collections::IVectorView<ClubRoleRecord^>^ get();
    }

    /// <summary>Users who've requested to join the club</summary>
    property Windows::Foundation::Collections::IVectorView<ClubRoleRecord^>^ RequestedToJoin
    {
        Windows::Foundation::Collections::IVectorView<ClubRoleRecord^>^ get();
    }

    /// <summary>Users who've been recommended for the club</summary>
    property Windows::Foundation::Collections::IVectorView<ClubRoleRecord^>^ Recommended
    {
        Windows::Foundation::Collections::IVectorView<ClubRoleRecord^>^ get();
    }

    /// <summary>Users who've been banned from the club</summary>
    property Windows::Foundation::Collections::IVectorView<ClubRoleRecord^>^ Banned
    {
        Windows::Foundation::Collections::IVectorView<ClubRoleRecord^>^ get();
    }

internal:
    ClubRoster(_In_ xbox::services::clubs::club_roster cppObj);

private:
    xbox::services::clubs::club_roster m_cppObj;
    Windows::Foundation::Collections::IVector<ClubRoleRecord^>^ m_moderators;
    Windows::Foundation::Collections::IVector<ClubRoleRecord^>^ m_reqestedToJoin;
    Windows::Foundation::Collections::IVector<ClubRoleRecord^>^ m_recommended;
    Windows::Foundation::Collections::IVector<ClubRoleRecord^>^ m_banned;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_END