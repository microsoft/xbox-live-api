// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_BEGIN

/// <summary>
/// Enumeration for the possible connection states of the connection
/// to the real-time activity service.
/// </summary>
public enum class RealTimeActivityConnectionState
{
    /// <summary>
    /// Currently connected to the real-time activity service.
    /// </summary>
    Connected = xbox::services::real_time_activity::real_time_activity_connection_state::connected,

    /// <summary>
    /// Currently connecting to the real-time activity service.
    /// </summary>
    Connecting = xbox::services::real_time_activity::real_time_activity_connection_state::connecting,

    /// <summary>
    /// Currently disconnected from the real-time activity service .
    /// </summary>
    Disconnected = xbox::services::real_time_activity::real_time_activity_connection_state::disconnected
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_END