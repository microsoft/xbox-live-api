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
#include "xsapi/game_server_platform.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_BEGIN

/// <summary>Defines values that indicate the the fulfillment state.</summary>
public enum class GameServerFulfillmentState
{
    /// <summary>The fulfillment state is unknown.</summary>
    Unknown = xbox::services::game_server_platform::game_server_fulfillment_state::unknown,

    /// <summary>Indicates that the allocate cluster request is fulfilled and the cluster is available.</summary>
    Fulfilled = xbox::services::game_server_platform::game_server_fulfillment_state::fulfilled,

    /// <summary>Indicates that the allocate cluster request is queued.</summary>
    Queued = xbox::services::game_server_platform::game_server_fulfillment_state::queued,

    /// <summary>Indicates that the cluster request was aborted. This will only be returned if "abortIfQueued" is set to true.</summary>
    Aborted = xbox::services::game_server_platform::game_server_fulfillment_state::aborted
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_END
