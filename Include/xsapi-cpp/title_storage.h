// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include <mutex>
#include "xsapi-c/title_storage_c.h"

struct XblContext;

namespace xbox { namespace services {
    class http_call;
    class xbox_live_context;
}}

namespace xbox { namespace services {
    /// <summary>
    /// Classes for handling title data in the cloud. 
    /// </summary>
    namespace title_storage {

        class title_storage_service;

/// <summary>Defines values used to indicate title storage type.</summary>
enum class title_storage_type
{
    /// <summary>
    /// Per-user data storage such as game state or game settings that can be only be accessed by Xbox One.
    /// User restrictions can be configured to public or owner only in the service configuration.
    /// </summary>
    trusted_platform_storage = static_cast<int>(XblTitleStorageType::TrustedPlatformStorage),

    /// <summary>
    /// Global data storage.  This storage type is only writable via title configuration sites or Xbox Live developer tools.
    /// Any platform may read from this storage type. Data could be rosters, maps, challenges, art resources, etc.
    /// </summary>
    global_storage = static_cast<int>(XblTitleStorageType::GlobalStorage),

    /// <summary>
    /// Per-user data storage such as game state or game settings the can be accessed by Xbox One, Windows 10, and Windows Phone 10 devices
    /// User restrictions can be configured to public or owner only in the service configuration.
    /// </summary>
    universal = static_cast<int>(XblTitleStorageType::Universal)
};

/// <summary>Defines values used to indicate title storage blob type.</summary>
enum class title_storage_blob_type
{
    /// <summary>Unknown blob type.</summary>
    unknown = static_cast<int>(XblTitleStorageBlobType::Unknown),

    /// <summary>Binary blob type.</summary>
    binary = static_cast<int>(XblTitleStorageBlobType::Binary),

    /// <summary>JSON blob type.</summary>
    json = static_cast<int>(XblTitleStorageBlobType::Json),

    /// <summary>Config blob type. </summary>
    config = static_cast<int>(XblTitleStorageBlobType::Config)
};

/// <summary>Defines values used to indicate the ETag match condition used when downloading, uploading or deleting title storage data.</summary>
enum class title_storage_e_tag_match_condition
{
    /// <summary>There is no match condition.</summary>
    not_used = static_cast<int>(XblTitleStorageETagMatchCondition::NotUsed),

    /// <summary>Perform the request if the Etag value specified matches the service value.</summary>
    if_match = static_cast<int>(XblTitleStorageETagMatchCondition::IfMatch),

    /// <summary>Perform the request if the Etag value specified does not match the service value.</summary>
    if_not_match = static_cast<int>(XblTitleStorageETagMatchCondition::IfNotMatch)
};

/// <summary>
/// Returns the amount of storage space allocated and used.
/// </summary>
class title_storage_quota
{
public:
    /// <summary>
    /// The service configuration ID associated with the quota.
    /// </summary>
    inline const string_t& service_configuration_id() const;

    /// <summary>
    /// The TitleStorageType associated with the quota.
    /// </summary>
    inline title_storage_type storage_type() const;

    /// <summary>
    /// The Xbox User ID associated with the quota if StorageType is TrustedPlatformStorage or JsonStorage, otherwise null.
    /// </summary>
    inline string_t xbox_user_id() const;

    /// <summary>
    /// Number of bytes used in title storage of type StorageType.
    /// </summary>
    inline uint64_t used_bytes() const;

    /// <summary>
    /// Maximum number of bytes that can be used in title storage of type StorageType.
    /// Note that this is a soft limit and the used bytes may actually exceed this value.
    /// </summary>
    inline uint64_t quota_bytes() const;

    // Internal
    title_storage_quota() = default;
    inline title_storage_quota(string_t scid, title_storage_type storageType, uint64_t xuid, uint64_t usedByted, uint64_t quotaBytes);

private:
    string_t m_serviceConfigurationId;
    title_storage_type m_storageType{};
    uint64_t m_xboxUserId{ 0 };
    uint64_t m_usedBytes{ 0 };
    uint64_t m_quotaBytes{ 0 };
};

/// <summary>
/// Metadata about a blob.
/// </summary>
class title_storage_blob_metadata
{
public:
    /// <summary>
    /// Initializes a new instance of the title_storage_blob_metadata class.
    /// </summary>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) of the title</param>
    /// <param name="storageType">The TitleStorageType to get blob metadata objects for.  Valid values are TrustedPlatormStorage, JsonStorage and GlobalStorage.</param>
    /// <param name="blobPath">The full path to to the blob.  examples: "gameconfig.json" and "user/settings/playerconfiguration.json".</param>
    /// <param name="blobType">The TitleStorageBlobType of this object.  Valid values are Binary, Json and Config.</param>
    /// <param name="xboxUserId">The Xbox User ID of the title storage to enumerate. Ignored when dealing with GlobalStorage, so passing nullptr is acceptable in that case. (Optional)</param>
    /// <remarks>
    /// All other properties of this class are optional.  To initialize optional properties, use the other constructors.
    /// ClientTimestamp.UniversalTime will be initialized to 0. Length is initialized to 0.
    /// title_storage_blob_metadata objects retrieved using TitleStorageService::GetBlobMetadataAsync will have current Length and ETag values.
    /// </remarks>
    inline title_storage_blob_metadata(
        _In_ string_t serviceConfigurationId,
        _In_ title_storage_type storageType,
        _In_ string_t blobPath,
        _In_ title_storage_blob_type blobType,
        _In_ string_t xboxUserId
        );

    /// <summary>
    /// Initializes a new instance of the title_storage_blob_metadata class including support for all optional properties except ClientTimestamp.
    /// </summary>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) of the title</param>
    /// <param name="storageType">The TitleStorageType to get blob metadata objects for.  Valid values are TrustedPlatormStorage, JsonStorage and GlobalStorage.</param>
    /// <param name="blobPath">The full path to to the blob.  examples: "gameconfig.json" and "user/settings/playerconfiguration.json".</param>
    /// <param name="blobType">The TitleStorageBlobType of this object.  Valid values are Binary, Json and Config.</param>
    /// <param name="xboxUserId">The Xbox User ID of the title storage to enumerate. Ignored when dealing with GlobalStorage, so passing nullptr is acceptable in that case. (Optional)</param>
    /// <param name="displayName">A display name suitable for displaying to the user. (Optional)</param>
    /// <param name="eTag">An ETag value to be associated with this instance.  It is used for upload, download and delete operations. (Optional)</param>
    /// <remarks>
    /// ClientTimestamp.UniversalTime will be initialized to 0. Length is initialized to 0.
    /// title_storage_blob_metadata objects retrieved using TitleStorageService::GetBlobMetadataAsync will have current Length and ETag values.
    /// </remarks>
    inline title_storage_blob_metadata(
        _In_ string_t serviceConfigurationId,
        _In_ title_storage_type storageType,
        _In_ string_t blobPath,
        _In_ title_storage_blob_type blobType,
        _In_ string_t xboxUserId,
        _In_ string_t displayName,
        _In_ string_t eTag
        );

    /// <summary>
    /// Initializes a new instance of the title_storage_blob_metadata class including support for all optional properties.
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
    /// title_storage_blob_metadata objects retrieved using TitleStorageService::GetBlobMetadataAsync will have current Length and ETag values.
    /// </remarks>
    inline title_storage_blob_metadata(
        _In_ string_t serviceConfigurationId,
        _In_ title_storage_type storageType,
        _In_ string_t blobPath,
        _In_ title_storage_blob_type blobType,
        _In_ string_t xboxUserId,
        _In_ string_t displayName,
        _In_ string_t eTag,
        _In_ utility::datetime clientTimestamp
        );

    /// <summary>
    /// Blob path is a unique string that conforms to a SubPath\file format (example: "foo\bar\blob.txt").
    /// </summary>
    inline string_t blob_path() const;

    /// <summary>
    /// Type of blob data. Possible values are: Binary, Json, and Config.
    /// </summary>
    inline title_storage_blob_type blob_type() const;

    /// <summary>
    /// Type of storage.
    /// </summary>
    inline title_storage_type storage_type() const;

    /// <summary>
    /// [optional] Friendly display name to show in app UI.
    /// </summary>
    inline string_t display_name() const;

    /// <summary>
    /// ETag for the file used in read and write requests.
    /// </summary>
    inline string_t e_tag() const;

    /// <summary>
    /// [optional] Timestamp assigned by the client.
    /// </summary>
    inline utility::datetime client_timestamp() const;

    /// <summary>
    /// [optional] Timestamp assigned by the client.
    /// </summary>
    inline void set_client_timestamp(_In_ utility::datetime value);

    /// <summary>
    /// Gets the number of bytes of the blob data.
    /// </summary>
    inline uint64_t length() const;

    /// <summary>
    /// The service configuration ID of the title
    /// </summary>
    inline string_t service_configuration_id() const;

    /// <summary>
    /// The Xbox User ID of the player this file belongs to.
    /// This value will be null for Global and Session files.
    /// </summary>
    inline string_t xbox_user_id() const;

    // Internal
    title_storage_blob_metadata() = default;
    inline title_storage_blob_metadata(XblTitleStorageBlobMetadata blobMetadata);

private:
    XblTitleStorageBlobMetadata m_blobMetadata{};

    friend title_storage_service;
};

/// <summary>
/// Metadata about blob data returned from the cloud.
/// </summary>
class title_storage_blob_metadata_result
{
public:
    /// <summary>
    /// Collection of title_storage_blob_metadata objects returned by a service request
    /// </summary>
    inline const std::vector<title_storage_blob_metadata>& items() const;

    /// <summary>
    /// Returns an title_storage_blob_metadata_result object containing the next page of title_storage_blob_metadata objects
    /// </summary>
    /// <param name="maxItems">The maximum number of items the result can contain.  Pass 0 to attempt to retrieve all items.</param>
    /// <returns>title_storage_blob_metadata_result object for the next page.</returns>
    inline pplx::task<xbox_live_result<title_storage_blob_metadata_result>> get_next(
        _In_ uint32_t maxItems
        ) const;

    /// <summary>
    /// Indicates if there is additional data to retrieve from a get_next call
    /// </summary>
    inline bool has_next() const;

    // Internal
    title_storage_blob_metadata_result() = default;
    inline title_storage_blob_metadata_result(XblTitleStorageBlobMetadataResultHandle handle);
    inline title_storage_blob_metadata_result(_In_ const title_storage_blob_metadata_result& other);
    inline title_storage_blob_metadata_result& operator=(title_storage_blob_metadata_result other);
    inline ~title_storage_blob_metadata_result();

private:
    XblTitleStorageBlobMetadataResultHandle m_handle{ nullptr };
    std::vector<title_storage_blob_metadata> m_items;
};

/// <summary>
/// Blob data returned from the cloud.
/// </summary>
class title_storage_blob_result
{
public:
    /// <summary>
    /// The contents of the title storage blob.
    /// </summary>
    inline std::shared_ptr<std::vector<unsigned char>> const blob_buffer() const;

    /// <summary>
    /// Updated title_storage_blob_metadata object following an upload or download.
    /// </summary>
    inline const title_storage_blob_metadata& blob_metadata() const;

    // Internal
    title_storage_blob_result() = default;
    inline title_storage_blob_result(std::shared_ptr<std::vector<unsigned char>> blobBuffer, title_storage_blob_metadata blobMetadata);
private:
    std::shared_ptr<std::vector<unsigned char>> m_blobBuffer;
    title_storage_blob_metadata m_blobMetadata;
};

/// <summary>
/// Services that manage title storage.
/// </summary>
class title_storage_service
{
public:
    /// <summary>
    /// Gets title storage quota information for the specified service configuration and storage type.
    /// For user storage types (TrustedPlatform and Json) the request will be made for the calling user's
    /// Xbox user Id.
    /// </summary>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) of the title</param>
    /// <param name="storageType">The storage type to get quota information for.</param>
    /// <returns>title_storage_quota object containing the quota information.</returns>
    /// <remarks>Calls
    /// V1 GET trustedplatform/users/xuid({xuid})/scids/{scid} or
    /// V1 GET json/users/xuid({xuid})/scids/{scid} or
    /// V1 GET global/scids/{scid}
    /// </remarks>
    inline pplx::task<xbox_live_result<title_storage_quota>> get_quota(
        _In_ string_t serviceConfigurationId,
        _In_ title_storage_type storageType
        );

    /// <summary>
    /// Gets a list of blob metadata objects under a given path for the specified service configuration, storage type and storage ID.
    /// </summary>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) of the title</param>
    /// <param name="storageType">The storage type to get blob metadata objects for.</param>
    /// <param name="blobPath">The root path to enumerate.  Results will be for blobs contained in this path and all subpaths. (Optional)</param>
    /// <param name="xboxUserId">The Xbox User ID of the title storage to enumerate. Ignored when enumerating GlobalStorage, so passing nullptr is acceptable. (Optional)</param>
    /// <param name="skipItems">The number of items to skip before returning results. (Optional)</param>
    /// <param name="maxItems">The maximum number of items to return. (Optional)</param>
    /// <returns>title_storage_blob_metadata_result object containing the list of enumerated blob metadata objects.</returns>
    /// <remarks>Calls
    /// V1 GET trustedplatform/users/xuid({xuid})/scids/{scid}/data/{path}?maxItems={maxItems}[skipItems={skipItems}] or
    /// V1 GET json/users/xuid({xuid})/scids/{scid}/data/{path}?maxItems={maxItems}[skipItems={skipItems}] or
    /// V1 GET global/scids/{scid}/data/{path}?maxItems={maxItems}[skipItems={skipItems}]
    /// </remarks>
    inline pplx::task<xbox_live_result<title_storage_blob_metadata_result>> get_blob_metadata(
        _In_ string_t serviceConfigurationId,
        _In_ title_storage_type storageType,
        _In_ string_t blobPath = string_t(),
        _In_ string_t xboxUserId = string_t(),
        _In_ uint32_t skipItems = 0,
        _In_ uint32_t maxItems = 0
        );

    /// <summary>
    /// Deletes a blob from title storage.
    /// </summary>
    /// <param name="blobMetadata">The blob metadata for the title storage blob to delete.</param>
    /// <param name="deleteOnlyIfEtagMatches">Specifies whether or not to have the delete operation check that the ETag matches before deleting the blob.</param>
    /// <remarks>Calls
    /// V1 DELETE trustedplatform/users/xuid({xuid})/scids/{scid}/data/{path},{type} or
    /// V1 DELETE json/users/xuid({xuid})/scids/{scid}/data/{path},{type} or
    /// V1 DELETE sessions/{sessionId}/scids/{scid}/data/{path},{type}
    /// </remarks>
    inline pplx::task<xbox_live_result<void>> delete_blob(
        _In_ const title_storage_blob_metadata& blobMetadata,
        _In_ bool deleteOnlyIfEtagMatches
        );

    /// <summary>
    /// Downloads blob data from title storage.
    /// </summary>
    /// <param name="blobMetadata">The blob metadata for the title storage blob to download.</param>
    /// <param name="blobBuffer">The client provided buffer to store the downloaded blob data in.  This buffer needs to be large enough to store the blob being downloaded.
    /// If necessary, the length required for the buffer can be retrieved by getting the blob metadata.</param>
    /// <param name="etagMatchCondition">The ETag match condition used to determine if the blob should be downloaded.</param>
    /// <param name="selectQuery">ConfigStorage filter string or JSONStorage json property name string to filter. (Optional)</param>
    /// <returns>TitleStorageBlobResult object containing the client provided blob buffer and an updated title_storage_blob_metadata object.
    /// The metadata object will contain updated ETag and Length properties.</returns>
    /// <remarks>
    /// This method will throw ERROR_INSUFFICIENT_BUFFER (0x8007007A) if the blobBuffer doesn't have enough capacity to hold the blob data.
    /// Calls V1 GET trustedplatform/users/xuid({xuid})/scids/{scid}/data/{path},{type} or
    /// V1 GET json/users/xuid({xuid})/scids/{scid}/data/{path},{type} or
    /// V1 GET global/scids/{scid}/data/{path},{type} or
    /// V1 GET sessions/{sessionId}/scids/{scid}/data/{path},{type}
    /// </remarks>
    inline pplx::task<xbox_live_result<title_storage_blob_result>> download_blob(
        _In_ title_storage_blob_metadata blobMetadata,
        _In_ std::shared_ptr<std::vector<unsigned char>> blobBuffer,
        _In_ title_storage_e_tag_match_condition etagMatchCondition,
        _In_ string_t selectQuery = string_t()
        );

    /// <summary>
    /// Downloads blob data from title storage.
    /// </summary>
    /// <param name="blobMetadata">The blob metadata for the title storage blob to download.</param>
    /// <param name="blobBuffer">The client provided buffer to store the downloaded blob data in.  This buffer needs to be large enough to store the blob being downloaded.
    /// If necessary, the length required for the buffer can be retrieved by getting the blob metadata.</param>
    /// <param name="etagMatchCondition">The ETag match condition used to determine if the blob should be downloaded.</param>
    /// <param name="selectQuery">ConfigStorage filter string or JSONStorage json property name string to filter. (Optional)</param>
    /// <param name="preferredDownloadBlockSize">The preferred download block size in bytes for binary blobs. </param>
    /// <returns>TitleStorageBlobResult object containing the client provided blob buffer and an updated title_storage_blob_metadata object.
    /// The metadata object will contain updated ETag and Length properties.</returns>
    /// <remarks>
    /// This method will throw ERROR_INSUFFICIENT_BUFFER (0x8007007A) if the blobBuffer doesn't have enough capacity to hold the blob data.
    /// Calls V1 GET trustedplatform/users/xuid({xuid})/scids/{scid}/data/{path},{type} or
    /// V1 GET json/users/xuid({xuid})/scids/{scid}/data/{path},{type} or
    /// V1 GET global/scids/{scid}/data/{path},{type} or
    /// V1 GET sessions/{sessionId}/scids/{scid}/data/{path},{type}
    /// </remarks>
    inline pplx::task<xbox_live_result<title_storage_blob_result>> download_blob(
        _In_ title_storage_blob_metadata blobMetadata,
        _In_ std::shared_ptr<std::vector<unsigned char>> blobBuffer,
        _In_ title_storage_e_tag_match_condition etagMatchCondition,
        _In_ string_t selectQuery,
        _In_ uint32_t preferredDownloadBlockSize
        );

    /// <summary>
    /// Uploads blob data to title storage.
    /// </summary>
    /// <param name="blobMetadata">Contains properties required to upload the buffer to title storage.  Uploads require a service configuration Id, blob path, blob type and storage type at a minimum.</param>
    /// <param name="blobBuffer">The buffer containing the blob data to upload.  This buffer must be available for the duration of the async operation.  Clients should not modify the buffer while an upload is in progress.</param>
    /// <param name="etagMatchCondition">The ETag match condition used to determine if the blob data should be uploaded.</param>
    /// <param name="preferredUploadBlockSize">The preferred upload block size in bytes for binary blobs. Binary blobs will be
    /// uploaded in multiple chunks of this size if they exceed it.  Larger sizes are preferred by the service.
    /// If timeouts occur, the app should retry with a smaller size.  Block size must be within the 1K to 4MB range.  This method
    /// will use a default size if this parameter is not within the acceptable range.  The current minimum size is 1024 bytes,
    /// maximum size is 4194304 bytes and the default size is 262144 bytes.
    /// </param>
    /// <returns>title_storage_blob_metadata object with updated Etag and Length properties.</returns>
    /// <remarks>
    /// V1 PUT json/users/xuid({xuid})/scids/{scid}/data/{path},{type} or
    /// V1 PUT global/scids/{scid}/data/{path},{type} or
    /// V1 PUT sessions/{sessionId}/scids/{scid}/data/{path},{type}
    /// </remarks>
    inline pplx::task<xbox_live_result<title_storage_blob_metadata>> upload_blob(
        _In_ title_storage_blob_metadata blobMetadata,
        _In_ std::shared_ptr<std::vector<unsigned char>> blobBuffer,
        _In_ title_storage_e_tag_match_condition etagMatchCondition,
        _In_ uint32_t preferredUploadBlockSize = XBL_TITLE_STORAGE_MIN_UPLOAD_BLOCK_SIZE
        );

    inline title_storage_service(const title_storage_service& other);
    inline title_storage_service& operator=(title_storage_service other);
    inline ~title_storage_service();

private:
    inline title_storage_service(_In_ XblContextHandle contextHandle);

    XblContextHandle m_xblContext{ nullptr };

    friend xbox_live_context;
};

}}}

#include "impl/title_storage.hpp"