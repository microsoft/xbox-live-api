// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/title_storage.h"
#include "Macros_WinRT.h"
#include "TitleStorageType_WinRT.h"
#include "TitleStorageBlobType_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_BEGIN

/// <summary>
/// Metadata about a blob.
/// </summary>
public ref class TitleStorageBlobMetadata sealed
{
public:
    /// <summary>
    /// Initializes a new instance of the TitleStorageBlobMetadata class with the minimum arguments.
    /// </summary>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) of the title</param>
    /// <param name="storageType">The TitleStorageType to get blob metadata objects for.  Valid values are TrustedPlatormStorage, JsonStorage and GlobalStorage.</param>
    /// <param name="blobPath">The full path to to the blob.  examples: "gameconfig.json" and "user/settings/playerconfiguration.json".</param>
    /// <param name="blobType">The TitleStorageBlobType of this object.  Valid values are Binary, Json and Config.</param>
    /// <param name="xboxUserId">The Xbox User ID of the title storage to enumerate. Ignored when dealing with GlobalStorage, so passing nullptr is acceptable in that case. (Optional)</param>
    /// <remarks>
    /// All other properties of this class are optional.  To initialize optional properties, use the other constructors.
    /// ClientTimestamp.UniversalTime is initialized to 0. Length is initialized to 0.
    /// TitleStorageBlobMetadata objects retrieved using TitleStorageService::GetBlobMetadataAsync will have current Length and ETag values.
    ///</remarks>
    TitleStorageBlobMetadata(
        _In_ Platform::String^ serviceConfigurationId,
        _In_ TitleStorageType storageType,
        _In_ Platform::String^ blobPath,
        _In_ TitleStorageBlobType blobType,
        _In_opt_ Platform::String^ xboxUserId
        );

    /// <summary>
    /// Initializes a new instance of the TitleStorageBlobMetadata class including support for all optional properties except ClientTimestamp.
    /// </summary>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) of the title</param>
    /// <param name="storageType">The TitleStorageType to get blob metadata objects for.  Valid values are TrustedPlatormStorage, JsonStorage and GlobalStorage.</param>
    /// <param name="blobPath">The full path to to the blob.  examples: "gameconfig.json" and "user/settings/playerconfiguration.json".</param>
    /// <param name="blobType">The TitleStorageBlobType of this object.  Valid values are Binary, Json and Config.</param>
    /// <param name="xboxUserId">The Xbox User ID of the title storage to enumerate. Ignored when dealing with GlobalStorage, so passing nullptr is acceptable in that case. (Optional)</param>
    /// <param name="displayName">A display name suitable for displaying to the user. (Optional)</param>
    /// <param name="eTag">An ETag value to be associated with this instance.  It is used for upload, download and delete operations. (Optional)</param>
    /// <remarks>
    /// ClientTimestamp.UniversalTime is initialized to 0. Length is initialized to 0.
    /// TitleStorageBlobMetadata objects retrieved using TitleStorageService::GetBlobMetadataAsync will have current Length and ETag values.
    ///</remarks>
    TitleStorageBlobMetadata(
        _In_ Platform::String^ serviceConfigurationId,
        _In_ TitleStorageType storageType,
        _In_ Platform::String^ blobPath,
        _In_ TitleStorageBlobType blobType,
        _In_opt_ Platform::String^ xboxUserId,
        _In_opt_ Platform::String^ displayName,
        _In_opt_ Platform::String^ eTag
        );

    /// <summary>
    /// Initializes a new instance of the TitleStorageBlobMetadata class including support for all optional properties.
    /// </summary>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) of the title</param>
    /// <param name="storageType">The TitleStorageType to get blob metadata objects for.  Valid values are TrustedPlatormStorage, JsonStorage and GlobalStorage.</param>
    /// <param name="blobPath">The full path to to the blob.  examples: "gameconfig.json" and "user/settings/playerconfiguration.json".</param>
    /// <param name="blobType">The TitleStorageBlobType of this object.  Valid values are Binary, Json and Config.</param>
    /// <param name="xboxUserId">The Xbox User ID of the title storage to enumerate. Ignored when dealing with GlobalStorage, so passing nullptr is acceptable in that case. (Optional)</param>
    /// <param name="displayName">A display name suitable for displaying to the user. (Optional)</param>
    /// <param name="eTag">An ETag value to be associated with this instance.  It is used for upload, download and delete operations. (Optional)</param>
    /// <param name="clientTimestamp">A client provided timestamp value to be associated with this instance.</param>
    /// <remarks>
    /// Length is initialized to 0.
    /// TitleStorageBlobMetadata objects retrieved using TitleStorageService::GetBlobMetadataAsync will have current Length and ETag values.
    ///</remarks>
    TitleStorageBlobMetadata(
        _In_ Platform::String^ serviceConfigurationId,
        _In_ TitleStorageType storageType,
        _In_ Platform::String^ blobPath,
        _In_ TitleStorageBlobType blobType,
        _In_opt_ Platform::String^ xboxUserId,
        _In_opt_ Platform::String^ displayName,
        _In_opt_ Platform::String^ eTag,
        _In_ Windows::Foundation::DateTime clientTimestamp
        );

    /// <summary>
    /// Creates metadata for a blob for session storage, including support for all optional properties except ClientTimestamp.
    /// </summary>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) of the title</param>
    /// <param name="blobPath">The full path to to the blob.  examples: "gameconfig.json" and "user/settings/playerconfiguration.json"</param>
    /// <param name="blobType">The TitleStorageBlobType of this object.  Valid values are Binary, Json and Config.</param>
    /// <param name="multiplayerSessionTemplateName">The multiplayer session template name associated with this object</param>
    /// <param name="multiplayerSessionName">The multiplayer session name associated with this object</param>
    /// <param name="displayName">A display name suitable for displaying to the user. (Optional)</param>
    /// <param name="eTag">An ETag value to be associated with this instance.  It is used for upload, download and delete operations. (Optional)</param>
    /// <returns>
    /// A new instance of the TitleStorageBlobMetadata class for SessionStorage.
    /// </returns>
    /// <remarks>
    /// ClientTimestamp.UniversalTime is initialized to 0. Length is initialized to 0.
    /// TitleStorageBlobMetadata objects retrieved using TitleStorageService::GetBlobMetadataAsync will have current Length and ETag values.
    /// </remarks>
#if _MSC_VER >= 1800
    [Windows::Foundation::Metadata::Deprecated("Title Storage for sessions has been deprecated. Use universal instead.", Windows::Foundation::Metadata::DeprecationType::Deprecate, 0x0)]
#endif
    static TitleStorageBlobMetadata^ CreateTitleStorageBlobMetadataForSessionStorage(
        _In_ Platform::String^ serviceConfigurationId,
        _In_ Platform::String^ blobPath,
        _In_ TitleStorageBlobType blobType,
        _In_ Platform::String^ multiplayerSessionTemplateName,
        _In_ Platform::String^ multiplayerSessionName,
        _In_opt_ Platform::String^ displayName,
        _In_opt_ Platform::String^ eTag
        );

    /// <summary>
    /// The blob path is a unique string that conforms to a SubPath\file format.  For example: "alpha\beta\blob.txt".
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(BlobPath, blob_path);

    /// <summary>
    /// The type of storage used by the blob. Possible values are: Binary, Json, and Config.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(BlobType, blob_type, TitleStorageBlobType);

    /// <summary>
    /// The type of storage that contains the blob.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(StorageType, storage_type, TitleStorageType);

    /// <summary>
    /// [optional] The friendly display name to show in app UI.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(DisplayName, display_name);

    /// <summary>
    /// The ETag for the file used in read and write requests.
    /// Etags are used to determine if a file have been updated. Each time the file is changes, a new ETag is applied by the service.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(ETag, e_tag);

    /// <summary>
    /// [optional] The timestamp assigned for the blob by the client.
    /// </summary>
    DEFINE_PROP_GETSET_DATETIME_OBJ(ClientTimestamp, client_timestamp);

    /// <summary>
    /// Gets the number of bytes of the blob data.
    /// </summary>
    DEFINE_PROP_GET_OBJ(Length, length, uint64);

    /// <summary>
    /// The service configuration ID associated with this blob data.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(ServiceConfigurationId, service_configuration_id);

    /// <summary>
    /// The Xbox User ID of the player that this file belongs to.
    /// This value is null for Global and Session files.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(XboxUserId, xbox_user_id);

    /// <summary>
    /// The multiplayer session template name this file belongs to if the StorageType is SessionStorage, otherwise null.
    /// </summary>
#if _MSC_VER >= 1800
    [Windows::Foundation::Metadata::Deprecated("Title Storage for sessions has been deprecated. Use universal instead.", Windows::Foundation::Metadata::DeprecationType::Deprecate, 0x0)]
#endif
    DEFINE_PROP_GET_STR_OBJ(MultiplayerSessionTemplateName, multiplayer_session_template_name);

    /// <summary>
    /// The multiplayer session name this file belongs to if the StorageType is SessionStorage, otherwise null.
    /// </summary>
#if _MSC_VER >= 1800
    [Windows::Foundation::Metadata::Deprecated("Title Storage for sessions has been deprecated. Use universal instead.", Windows::Foundation::Metadata::DeprecationType::Deprecate, 0x0)]
#endif
    DEFINE_PROP_GET_STR_OBJ(MultiplayerSessionName, multiplayer_session_name);

internal:
    TitleStorageBlobMetadata(_In_ xbox::services::title_storage::title_storage_blob_metadata cppObj);
    xbox::services::title_storage::title_storage_blob_metadata GetCppObj() const;

private:
    xbox::services::title_storage::title_storage_blob_metadata m_cppObj;
};
NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_END