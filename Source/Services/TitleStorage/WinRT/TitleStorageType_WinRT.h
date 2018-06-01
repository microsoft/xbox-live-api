// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_BEGIN

/// <summary> An enumeration that defines values that indicate the title storage type.</summary>
public enum class TitleStorageType
{
    /// <summary>
    /// Per-user data storage such as game state or game settings for trusted platforms including Xbox One.
    /// The service configuration for the game specifies if the storage is public or restricted to the owner only.
    /// </summary>
    TrustedPlatformStorage = xbox::services::title_storage::title_storage_type::trusted_platform_storage,

    /// <summary>
    /// JSON storage has been deprecated.  Use universal instead.
    ///
    /// Per-user JSON data storage such as game state, game settings, and user generated content for any platform.
    /// The data type is restricted to TitleStorageBlobType::Json.
    /// The service configuration for the game specifies if the storage is public or restricted to the owner only.
    /// </summary>
    JsonStorage = xbox::services::title_storage::title_storage_type::json_storage,

    /// <summary>
    /// Global data storage.  This storage type is only writable via title configuration sites or Xbox Live developer tools.
    /// Any platform may read from this storage type. Data can be rosters, maps, challenges, art resources, etc.
    ///</summary>
    GlobalStorage = xbox::services::title_storage::title_storage_type::global_storage,

    /// <summary>
    /// DEPRECATED
    /// Title Storage for sessions has been deprecated. Use universal instead.
    ///
    /// Per-session data storage for multiplayer game sessions.  This storage type is only writable to users joined
    /// to the session.  Read access is configurable to public or session-joined only in the service configuration.
    ///</summary>
    SessionStorage = xbox::services::title_storage::title_storage_type::session_storage,
    
    /// <summary>
    /// Per-user data storage such as game state or game settings for untrusted platforms.
    /// User restrictions can be configured to public or owner only in the service configuration.
    /// </summary>
    UntrustedPlatformStorage = xbox::services::title_storage::title_storage_type::untrusted_platform_storage,

    /// <summary>
    /// Per-user data storage such as game state or game settings for universal platforms
    /// User restrictions can be configured to public or owner only in the service configuration.
    /// </summary>
    Universal = xbox::services::title_storage::title_storage_type::universal
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_END