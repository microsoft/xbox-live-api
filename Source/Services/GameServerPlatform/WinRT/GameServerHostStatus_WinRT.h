// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once 
#include "xsapi/game_server_platform.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_BEGIN

/// <summary>Defines values that indicate the host state.</summary>
public enum class GameServerHostStatus
{
    /// <summary>The fulfillment state is unknown.</summary>
    Unknown = xbox::services::game_server_platform::game_server_host_status::unknown,

    /// <summary>Indicates that the cluster request is active.</summary>
    Active = xbox::services::game_server_platform::game_server_host_status::active,

    /// <summary>Indicates that the cluster request is queued.</summary>
    Queued = xbox::services::game_server_platform::game_server_host_status::queued,

    /// <summary>Indicates that the cluster request was aborted. This will only be returned if "abortIfQueued" is set to true.</summary>
    Aborted = xbox::services::game_server_platform::game_server_host_status::aborted,

    /// <summary>Indicates that the cluster request had an error.</summary>
    Error = xbox::services::game_server_platform::game_server_host_status::error
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_END
