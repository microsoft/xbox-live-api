// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once 
#include "xsapi/multiplayer.h"
#include "MultiplayerRoleType_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

/// <summary>
/// Represents session role type values for a multiplayer session.
/// </summary>
public ref class MultiplayerSessionRoleTypes sealed
{
public:
    /// <summary>
    /// A collection of role types.
    /// </summary>
    property Windows::Foundation::Collections::IMapView<Platform::String^, MultiplayerRoleType^>^ RoleTypes
    {
        Windows::Foundation::Collections::IMapView<Platform::String^, MultiplayerRoleType^>^ get();
    }

internal:
    MultiplayerSessionRoleTypes(
        _In_ std::shared_ptr<xbox::services::multiplayer::multiplayer_session_role_types> cppObj
    );

private:
    std::shared_ptr<xbox::services::multiplayer::multiplayer_session_role_types> m_cppObj;
    Platform::Collections::Map<Platform::String^, MultiplayerRoleType^>^ m_roleTypes;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END