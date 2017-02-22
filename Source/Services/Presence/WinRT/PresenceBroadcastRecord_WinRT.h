// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/presence.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_BEGIN
/// <summary>
/// Represents a record of data about the broadcast. 
/// </summary>
/// <summary>
/// The record is only returned if the client specifies an Entertainment Discovery Services(EDS) service version(x - xbl - contract - version) of 3 or later.
/// </summary>

public ref class PresenceBroadcastRecord sealed
{
public:
    /// <summary>
    /// Id for this broadcast as defined by the broadcasting service.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(BroadcastId, broadcast_id);

    /// <summary>
    /// The GUID uniquely identifying the broadcasting session. 
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Session, session);

    /// <summary>
    /// Name of the streaming provider.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Provider, provider);

    /// <summary>
    /// Approximate number of current viewers. 
    /// </summary>
    DEFINE_PROP_GET_OBJ(ViewerCount, viewer_count, uint32);

    /// <summary>
    /// UTC timestamp when the broadcast was started.
    /// </summary>
    DEFINE_PROP_GET_DATETIME_OBJ(StartTime, start_time);

internal:
    PresenceBroadcastRecord(_In_ xbox::services::presence::presence_broadcast_record cppObj);

private:
    xbox::services::presence::presence_broadcast_record m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_END