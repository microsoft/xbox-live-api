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