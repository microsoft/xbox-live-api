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

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

/// <summary>
/// Enumeration for the types of statuses that may result from a WriteSessionAsync.
/// </summary>
public enum class MutableRoleSetting
{
    /// <summary>
    /// Allows you to set a max count for the multiplayer role
    /// </summary>
    Max = xbox::services::multiplayer::mutable_role_setting::max,

    /// <summary>
    /// Allows you to set a target count for the multiplayer role
    /// </summary>
    Target = xbox::services::multiplayer::mutable_role_setting::target
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END