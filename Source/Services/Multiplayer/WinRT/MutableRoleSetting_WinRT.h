// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

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