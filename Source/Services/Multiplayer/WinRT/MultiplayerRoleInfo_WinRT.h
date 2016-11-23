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
#include "xsapi/multiplayer.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

/// <summary>
/// Represents role info for a multiplayer role.
/// </summary>
public ref class MultiplayerRoleInfo sealed
{
public:
    /// <summary>
    /// Creates a MultiplayerRoleInfo object.
    /// </summary>
    MultiplayerRoleInfo();

    /// <summary>
    /// Member XboxUserIds currently assigned for this role.
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<Platform::String^>^ MemberXboxUserIds
    {
        Windows::Foundation::Collections::IVectorView<Platform::String^>^ get();
    }

    /// <summary>
    /// Number of slots occupied for this role.
    /// </summary>
    DEFINE_PROP_GET_OBJ(MembersCount, members_count, uint32);

    /// <summary>
    /// Number of target slots assigned for this role.
    /// Note: Only the session owner can modify role settings and only those that are multiplayer_role_type::mutable_role_settings()
    /// In your session template, you also need to set 'hasOwners' capability and 'ownerManaged' to true for the specific role type 
    /// that you want to modify the mutable_role_setting off.
    /// </summary>
    DEFINE_PROP_GETSET_OBJ(TargetCount, target_count, uint32);

    /// <summary>
    /// Maximum number of slots available for this role.
    /// Note: Only the session owner can modify role settings and only those that are multiplayer_role_type::mutable_role_settings()
    /// In your session template, you also need to set 'hasOwners' capability and 'ownerManaged' to true for the specific role type 
    /// that you want to modify the mutable_role_setting off.
    /// </summary>
    DEFINE_PROP_GETSET_OBJ(MaxMembersCount, max_members_count, uint32);

internal:
    MultiplayerRoleInfo(
        _In_ xbox::services::multiplayer::multiplayer_role_info cppObj
        );

    xbox::services::multiplayer::multiplayer_role_info GetCppObj() const;

private:
    xbox::services::multiplayer::multiplayer_role_info m_cppObj;
    Windows::Foundation::Collections::IVector<Platform::String^>^ m_memberXboxUserIds;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END