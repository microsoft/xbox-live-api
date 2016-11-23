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
#include "MultiplayerRoleInfo_WinRT.h"
#include "MutableRoleSetting_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

/// <summary>
/// Represents a collection of roles for this role type.
/// </summary>
public ref class MultiplayerRoleType sealed
{
public:
    /// <summary>
    /// Creates a MultiplayerRoleType object.
    /// </summary>
    MultiplayerRoleType();

    /// <summary>
    /// True if ownerManaged is set on the roleType.
    /// </summary>
    DEFINE_PROP_GET_OBJ(OwnerManaged, owner_managed, bool);

    /// <summary>
    /// Mutable role settings for this role.
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<MutableRoleSetting>^ MutableRoleSettings
    {
        Windows::Foundation::Collections::IVectorView<MutableRoleSetting>^ get();
    }

    /// <summary>
    /// A collection of roles for this role type.
    /// </summary>
    property Windows::Foundation::Collections::IMapView<Platform::String^, MultiplayerRoleInfo^>^ Roles
    {
        Windows::Foundation::Collections::IMapView<Platform::String^, MultiplayerRoleInfo^>^ get();
        void set(_In_ Windows::Foundation::Collections::IMapView<Platform::String^, MultiplayerRoleInfo^>^ roles);
    }

internal:
    MultiplayerRoleType(
        _In_ xbox::services::multiplayer::multiplayer_role_type cppObj
        );

    xbox::services::multiplayer::multiplayer_role_type GetCppObj() const;

private:
    xbox::services::multiplayer::multiplayer_role_type m_cppObj;
    Windows::Foundation::Collections::IVector<MutableRoleSetting>^ m_mutableRoleSettings;
    Windows::Foundation::Collections::IMapView<Platform::String^, MultiplayerRoleInfo^>^ m_roles;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END