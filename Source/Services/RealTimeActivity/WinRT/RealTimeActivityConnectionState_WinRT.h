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