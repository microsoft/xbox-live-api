// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/clubs.h"
#include "Macros_WinRT.h"
#include "ClubRole_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_BEGIN

public ref class ClubRoleRecord sealed
{
public:
    /// <summary>Xuid that belongs to the role.</summary>
    DEFINE_PROP_GET_STR_OBJ(Xuid, xuid);

    /// <summary>Role of the user.</summary>
    DEFINE_PROP_GET_ENUM_OBJ(Role, role, ClubRole);

    /// <summary>Actor Xuid that was responsible for user belonging to the role.</summary>
    DEFINE_PROP_GET_STR_OBJ(ActorXuid, actor_xuid);

    /// <summary>When the user was added to the role.</summary>
    DEFINE_PROP_GET_DATETIME_OBJ(CreatedDate, created_date);

internal:
    ClubRoleRecord(_In_ xbox::services::clubs::club_role_record cppObj);

private:
    xbox::services::clubs::club_role_record m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_END