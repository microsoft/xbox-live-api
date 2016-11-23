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
/// Defines values used to indicate modes in MultiplayerService.WriteSessionAsync calls.
/// </summary>
public enum class MultiplayerSessionWriteMode
{
    /// <summary>
    /// Create a new multiplayer session.  Fails with HTTP_E_STATUS_PRECOND_FAILED if the session already exists.
    /// </summary>
    CreateNew = xbox::services::multiplayer::multiplayer_session_write_mode::create_new,

    /// <summary>
    /// Either update or create a new session. It doesn't matter whether the session exists or not.
    /// </summary>
    UpdateOrCreateNew = xbox::services::multiplayer::multiplayer_session_write_mode::update_or_create_new,

    /// <summary>
    /// Updates an existing multiplayer session.  Fails with HTTP_E_STATUS_PRECOND_FAILED if the session doesn't exist.
    /// </summary>
    UpdateExisting = xbox::services::multiplayer::multiplayer_session_write_mode::update_existing,

    /// <summary>
    /// Updates an existing multiplayer session to resolve any conflict between two devices trying to perform an operation
    /// at the same time. Fails with HTTP_E_STATUS_PRECOND_FAILED (HTTP status 412) if eTag on the local session doesn't match 
    /// the eTag on the server. Fails with the same error code if the session does not exist.
    /// </summary>
    SynchronizedUpdate = xbox::services::multiplayer::multiplayer_session_write_mode::synchronized_update,
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END