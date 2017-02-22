// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once 

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

/// <summary>
/// Enumeration for the types of statuses that may result from a WriteSessionAsync.
/// </summary>
public enum class WriteSessionStatus
{
    /// <summary>
    /// Unknown status.
    /// </summary>
    Unknown = xbox::services::multiplayer::write_session_status::unknown,

    /// <summary>
    /// HTTP/403 status, access denied.
    /// </summary>
    AccessDenied = xbox::services::multiplayer::write_session_status::access_denied,

    /// <summary>
    /// HTTP/201 status, created.
    /// </summary>
    Created = xbox::services::multiplayer::write_session_status::created,

    /// <summary>
    /// HTTP/409 status, conflict.
    /// </summary>
    Conflict = xbox::services::multiplayer::write_session_status::conflict,

    /// <summary>
    /// HTTP/404 status, handle not found.
    /// </summary>
    HandleNotFound = xbox::services::multiplayer::write_session_status::handle_not_found,

    /// <summary>
    /// HTTP/412 status, out of sync.
    /// </summary>
    OutOfSync = xbox::services::multiplayer::write_session_status::out_of_sync,

    /// <summary>
    /// HTTP/204 status, session deleted.
    /// </summary>
    SessionDeleted = xbox::services::multiplayer::write_session_status::session_deleted,

    /// <summary>
    /// HTTP/200 status, updated.
    /// </summary>
    Updated = xbox::services::multiplayer::write_session_status::updated
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END

