// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include <mutex>

namespace xbox { namespace services {
    class http_call;
    class xbox_live_context_impl;
}}

namespace xbox { namespace services {
    /// <summary>
    /// Classes for handling title data in the cloud. 
    /// </summary>
    namespace title_storage {

/// <summary>Defines values used to indicate title storage type.</summary>
enum class title_storage_type
{
    /// <summary>
    /// Per-user data storage such as game state or game settings that can be only be accessed by Xbox One.
    /// User restrictions can be configured to public or owner only in the service configuration.
    /// </summary>
    trusted_platform_storage,

    /// <summary>
    /// DEPRECATED
    /// JSON storage has been deprecated.  Use universal instead.
    ///
    /// Per-user JSON data storage such as game state, game settings, and user generated content for any platforms
    /// Data type is restricted to TitleStorageBlobType::Json
    /// User restrictions can be configured to public or owner only in the service configuration.
    /// </summary>
    json_storage,

    /// <summary>
    /// Global data storage.  This storage type is only writable via title configuration sites or Xbox Live developer tools.
    /// Any platform may read from this storage type. Data could be rosters, maps, challenges, art resources, etc.
    /// </summary>
    global_storage,

    /// <summary>
    /// DEPRECATED
    /// Title Storage for sessions has been deprecated. Use universal instead.
    ///
    /// Per-session data storage for multiplayer game sessions.  This storage type is only writable to users joined
    /// to the session.  Read access is configurable to public or session-joined only in the service configuration.
    /// </summary>
    session_storage,

    /// <summary>
    /// Untrusted platform storage has been deprecated.  Use universal instead
    ///
    /// Per-user data storage such as game state or game settings for untrusted platforms
    /// User restrictions can be configured to public or owner only in the service configuration.
    /// </summary>
    untrusted_platform_storage,

    /// <summary>
    /// Per-user data storage such as game state or game settings the can be accessed by Xbox One, Windows 10, and Windows Phone 10 devices
    /// User restrictions can be configured to public or owner only in the service configuration.
    /// </summary>
    universal
};

/// <summary>Defines values used to indicate title storage blob type.</summary>
enum class title_storage_blob_type
{
    /// <summary>Unknown blob type.</summary>
    unknown,

    /// <summary>Binary blob type.</summary>
    binary,

    /// <summary>JSON blob type.</summary>
    json,

    /// <summary>Config blob type. </summary>
    config
};

/// <summary>Defines values used to indicate the ETag match condition used when downloading, uploading or deleting title storage data.</summary>
enum class title_storage_e_tag_match_condition
{
    /// <summary>There is no match condition.</summary>
    not_used,

    /// <summary>Perform the request if the Etag value specified matches the service value.</summary>
    if_match,

    /// <summary>Perform the request if the Etag value specified does not match the service value.</summary>
    if_not_match
};

/// <summary>
/// Returns the amount of storage space allocated and used.
/// </summary>
class title_storage_quota
{
public:
    /// <summary>
    /// Internal function
    /// </summary>
    _XSAPIIMP title_storage_quota();

    /// <summary>
    /// The service configuration ID associated with the quota.
    /// </summary>
    _XSAPIIMP const string_t& service_configuration_id() const;

    /// <summary>
    /// The TitleStorageType associated with the quota.
    /// </summary>
    _XSAPIIMP title_storage_type storage_type() const;

    /// <summary>
    /// The Xbox User ID associated with the quota if StorageType is TrustedPlatformStorage or JsonStorage, otherwise null.
    /// </summary>
    _XSAPIIMP const string_t& xbox_user_id() const;

    /// <summary>
    /// DEPRECATED
    /// Title Storage for sessions has been deprecated. Use universal instead.
    /// The multiplayer session template name associated with the quota if StorageType is SessionStorage, otherwise null.
    /// </summary>
#if XSAPI_CPP
    _XSAPIIMP_DEPRECATED const string_t& multiplayer_session_template_name() const;
#else
    _XSAPIIMP const string_t& multiplayer_session_template_name() const;
#endif

    /// <summary>
    /// DEPRECATED
    /// Title Storage for sessions has been deprecated. Use universal instead.
    /// The multiplayer session name associated with the quota if StorageType is SessionStorage, otherwise null.
    /// </summary>
#if XSAPI_CPP
    _XSAPIIMP_DEPRECATED const string_t& multiplayer_session_name() const;
#else
    _XSAPIIMP const string_t& multiplayer_session_name() const;
#endif

    /// <summary>
    /// Number of bytes used in title storage of type StorageType.
    /// </summary>
    _XSAPIIMP uint64_t used_bytes() const;

    /// <summary>
    /// Maximum number of bytes that can be used in title storage of type StorageType.
    /// Note that this is a soft limit and the used bytes may actually exceed this value.
    /// </summary>
    _XSAPIIMP uint64_t quota_bytes() const;

    /// <summary>
    /// Internal function
    /// </summary>
    void _Initialize(
        _In_ string_t serviceConfigurationId,
        _In_ title_storage_type storageType,
        _In_ string_t xboxUserId,
        _In_ string_t multiplayerSessionTemplateName,
        _In_ string_t multiplayerSessionName
        );

    /// <summary>
    /// Internal function
    /// </summary>
    static xbox_live_result<title_storage_quota> _Deserialize(_In_ const web::json::value& json);

private:
    string_t m_serviceConfigurationId;
    title_storage_type m_storageType;
    string_t m_xboxUserId;
    string_t m_multiplayerSessionTemplateName;
    string_t m_multiplayerSessionName;
    uint64_t m_usedBytes;
    uint64_t m_quotaBytes;
};

/// <summary>
/// Metadata about a blob.
/// </summary>
class title_storage_blob_metadata
{
public:
    /// <summary>
    /// Internal function
    /// </summary>
    title_storage_blob_metadata();

    /// <summary>
    /// Internal function
    /// </summary>
    title_storage_blob_metadata(
        _In_ string_t serviceConfigurationId,
        _In_ title_storage_type storageType,
        _In_ string_t blobPath,
        _In_ title_storage_blob_type blobType,
        _In_ string_t xboxUserId,
        _In_ string_t multiplayerSessionTemplateName,
        _In_ string_t multiplayerSessionName,
        _In_ string_t displayName,
        _In_ string_t eTag
        );

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
    _XSAPIIMP title_storage_blob_metadata(
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
    _XSAPIIMP title_storage_blob_metadata(
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
    _XSAPIIMP title_storage_blob_metadata(
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
    /// DEPRECATED
    /// Title Storage for sessions has been deprecated. Use universal instead.
    /// Initializes a new instance of the title_storage_blob_metadata class for SessionStorage including support for all optional properties except ClientTimestamp.
    /// ClientTimestamp.UniversalTime will be initialized to 0. Length is initialized to 0.
    /// title_storage_blob_metadata objects retrieved using TitleStorageService::GetBlobMetadataAsync will have current Length and ETag values.
    /// </summary>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) of the title</param>
    /// <param name="blobPath">The full path to to the blob.  examples: "gameconfig.json" and "user/settings/playerconfiguration.json".</param>
    /// <param name="blobType">The TitleStorageBlobType of this object.  Valid values are Binary, Json and Config.</param>
    /// <param name="multiplayerSessionTemplateName">The multiplayer session template name associated with this object.</param>
    /// <param name="multiplayerSessionName">The multiplayer session name associated with this object.</param>
    /// <param name="displayName">A display name suitable for displaying to the user. (Optional)</param>
    /// <param name="eTag">An ETag value to be associated with this instance.  It is used for upload, download and delete operations. (Optional)</param>
    _XSAPIIMP_DEPRECATED static title_storage_blob_metadata create_title_storage_blob_metadata_for_session_storage(
        _In_ string_t serviceConfigurationId,
        _In_ string_t blobPath,
        _In_ title_storage_blob_type blobType,
        _In_ string_t multiplayerSessionTemplateName,
        _In_ string_t multiplayerSessionName,
        _In_ string_t displayName = string_t(),
        _In_ string_t eTag = string_t()
        );

    /// <summary>
    /// Blob path is a unique string that conforms to a SubPath\file format (example: "foo\bar\blob.txt").
    /// </summary>
    _XSAPIIMP const string_t& blob_path() const;

    /// <summary>
    /// Type of blob data. Possible values are: Binary, Json, and Config.
    /// </summary>
    _XSAPIIMP title_storage_blob_type blob_type() const;

    /// <summary>
    /// Type of storage.
    /// </summary>
    _XSAPIIMP title_storage_type storage_type() const;

    /// <summary>
    /// [optional] Friendly display name to show in app UI.
    /// </summary>
    _XSAPIIMP const string_t& display_name() const;

    /// <summary>
    /// ETag for the file used in read and write requests.
    /// </summary>
    _XSAPIIMP const string_t& e_tag() const;

    /// <summary>
    /// [optional] Timestamp assigned by the client.
    /// </summary>
    _XSAPIIMP const utility::datetime& client_timestamp() const;

    /// <summary>
    /// [optional] Timestamp assigned by the client.
    /// </summary>
    _XSAPIIMP void set_client_timestamp(_In_ utility::datetime value);

    /// <summary>
    /// Gets the number of bytes of the blob data.
    /// </summary>
    _XSAPIIMP uint64_t length() const;

    /// <summary>
    /// The service configuration ID of the title
    /// </summary>
    _XSAPIIMP const string_t& service_configuration_id() const;

    /// <summary>
    /// The Xbox User ID of the player this file belongs to.
    /// This value will be null for Global and Session files.
    /// </summary>
    _XSAPIIMP const string_t& xbox_user_id() const;

    /// <summary>
    /// DEPRECATED
    /// Title Storage for sessions has been deprecated. Use universal instead.
    /// The multiplayer session template name this file belongs to if StorageType is SessionStorage, otherwise null.
    /// </summary>
#if XSAPI_CPP
    _XSAPIIMP_DEPRECATED const string_t& multiplayer_session_template_name() const;
#else
    _XSAPIIMP const string_t& multiplayer_session_template_name() const;
#endif

    /// <summary>
    /// DEPRECATED
    /// Title Storage for sessions has been deprecated. Use universal instead.
    /// The multiplayer session name this file belongs to if StorageType is SessionStorage, otherwise null.
    /// </summary>
#if XSAPI_CPP
    _XSAPIIMP_DEPRECATED const string_t& multiplayer_session_name() const;
#else
    _XSAPIIMP const string_t& multiplayer_session_name() const;
#endif

    /// <summary>
    /// Internal function
    /// </summary>
    void _Set_e_tag_and_length(
        _In_ string_t etag,
        _In_ uint64_t length
        );

    /// <summary>
    /// Internal function
    /// </summary>
    void _Initialize(
        _In_ string_t xboxUserId,
        _In_ string_t multiplayerSessionTemplateName,
        _In_ string_t multiplayerSessionName,
        _In_ string_t serviceConfigurationId,
        _In_ title_storage_type storageType
        );

    /// <summary>
    /// Internal function
    /// </summary>
    static const xbox_live_result<string_t> _Convert_title_storage_blob_type_to_string(_In_ title_storage_blob_type blobType);

    /// <summary>
    /// Internal function
    /// </summary>
    static title_storage_blob_type _Convert_string_to_title_storage_blob_type(_In_ const string_t& value);

    /// <summary>
    /// Internal function
    /// </summary>
    static xbox_live_result<title_storage_blob_metadata> _Deserialize(_In_ const web::json::value& json);

private:
    string_t m_serviceConfigurationId;
    title_storage_type m_storageType;
    string_t m_blobPath;
    title_storage_blob_type m_blobType;
    string_t m_displayName;
    string_t m_eTag;
    utility::datetime m_clientTimestamp;
    uint64_t m_lengthBytes;
    string_t m_xboxUserId;
    string_t m_multiplayerSessionTemplateName;
    string_t m_multiplayerSessionName;
};

/// <summary>
/// Metadata about blob data returned from the cloud.
/// </summary>
class title_storage_blob_metadata_result
{
public:
    /// <summary>
    /// Internal function
    /// </summary>
    _XSAPIIMP title_storage_blob_metadata_result();

    /// <summary>
    /// Collection of title_storage_blob_metadata objects returned by a service request
    /// </summary>
    _XSAPIIMP const std::vector<title_storage_blob_metadata>& items() const;

    /// <summary>
    /// Returns an title_storage_blob_metadata_result object containing the next page of title_storage_blob_metadata objects
    /// </summary>
    /// <param name="maxItems">The maximum number of items the result can contain.  Pass 0 to attempt retrieving all items.</param>
    /// <returns>title_storage_blob_metadata_result object for the next page.</returns>
    _XSAPIIMP pplx::task<xbox_live_result<title_storage_blob_metadata_result>> get_next(
        _In_ uint32_t maxItems
        ) const;

    /// <summary>
    /// Indicates if there is additional data to retrieve from a get_next call
    /// </summary>
    _XSAPIIMP bool has_next() const;

    /// <summary>
    /// Internal function
    /// </summary>
    void _Initialize(
        _In_ std::shared_ptr<xbox::services::user_context> userContext,
        _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
        _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig,
        _In_ string_t serviceConfigurationId,
        _In_ title_storage_type storageType,
        _In_ string_t blobPath,
        _In_ string_t xboxUserId,
        _In_ string_t multiplayerSessionTemplateName,
        _In_ string_t multiplayerSessionName
        );

    /// <summary>
    /// Internal function
    /// </summary>
    static xbox_live_result<title_storage_blob_metadata_result> _Deserialize(_In_ const web::json::value& json);

private:
    std::vector<title_storage_blob_metadata> m_items;
    std::shared_ptr<xbox::services::user_context> m_userContext;
    std::shared_ptr<xbox::services::xbox_live_context_settings> m_xboxLiveContextSettings;
    std::shared_ptr<xbox::services::xbox_live_app_config> m_appConfig;
    string_t m_serviceConfigurationId;
    title_storage_type m_storageType;
    string_t m_blobPath;
    string_t m_xboxUserId;
    string_t m_continuationToken;
    string_t m_multiplayerSessionTemplateName;
    string_t m_multiplayerSessionName;
};

/// <summary>
/// Blob data returned from the cloud.
/// </summary>
class title_storage_blob_result
{
public:
    /// <summary>
    /// Internal function
    /// </summary>
    title_storage_blob_result();

    /// <summary>
    /// Internal function
    /// </summary>
    title_storage_blob_result(
        _In_ std::shared_ptr<std::vector<unsigned char>> blobBuffer,
        _In_ title_storage_blob_metadata blobMetadata
        );

    /// <summary>
    /// The contents of the title storage blob.
    /// </summary>
    _XSAPIIMP std::shared_ptr<std::vector<unsigned char>> const blob_buffer() const;

    /// <summary>
    /// Updated title_storage_blob_metadata object following an upload or download.
    /// </summary>
    _XSAPIIMP const title_storage_blob_metadata& blob_metadata() const;

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
    _XSAPIIMP pplx::task<xbox_live_result<title_storage_quota>> get_quota(
        _In_ string_t serviceConfigurationId,
        _In_ title_storage_type storageType
        );

    /// <summary>
    /// DEPRECATED
    /// Title Storage for sessions has been deprecated. Use universal instead.
    /// Gets title storage quota information for the SessionStorage associated with the specified multiplayer session.
    /// </summary>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) of the title</param>
    /// <param name="multiplayerSessionTemplateName">The multiplayer session template name for the session to get quota information for.</param>
    /// <param name="multiplayerSessionName">The multiplayer session name for the session to get quota information for.</param>
    /// <returns>title_storage_quota object containing the quota information.</returns>
    /// <remarks>Calls
    /// V1 GET sessions/{sessionId}/scids/{scid}
    /// </remarks>
    _XSAPIIMP_DEPRECATED pplx::task<xbox_live_result<title_storage_quota>> get_quota_for_session_storage(
        _In_ const string_t& serviceConfigurationId,
        _In_ const string_t& multiplayerSessionTemplateName,
        _In_ const string_t& multiplayerSessionName
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
    _XSAPIIMP pplx::task<xbox_live_result<title_storage_blob_metadata_result>> get_blob_metadata(
        _In_ string_t serviceConfigurationId,
        _In_ title_storage_type storageType,
        _In_ string_t blobPath = string_t(),
        _In_ string_t xboxUserId = string_t(),
        _In_ uint32_t skipItems = 0,
        _In_ uint32_t maxItems = 0
        );

    /// <summary>
    /// DEPRECATED
    /// Title Storage for sessions has been deprecated. Use universal instead.
    /// Gets a list of blob metadata objects in SessionStorage under a given path for the specified multiplayer session.
    /// </summary>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) of the title</param>
    /// <param name="blobPath">The root path to enumerate.  Results will be for blobs contained in this path and all subpaths. (Optional)</param>
    /// <param name="multiplayerSessionTemplateName">The multiplayer session template name for the session to get blob metadata objects for.</param>
    /// <param name="multiplayerSessionName">The multiplayer session name for the session to get blob metadata objects for.</param>
    /// <param name="skipItems">The number of items to skip before returning results.</param>
    /// <param name="maxItems">The maximum number of items to return.</param>
    /// <returns>title_storage_blob_metadata_result object containing the list of enumerated blob metadata objects.</returns>
    /// <remarks>Calls
    /// V1 GET sessions/{sessionId}/scids/{scid}/data/{path}?maxItems={maxItems}[skipItems={skipItems}]
    /// </remarks>
    _XSAPIIMP_DEPRECATED pplx::task<xbox_live_result<title_storage_blob_metadata_result>> get_blob_metadata_for_session_storage(
        _In_ string_t serviceConfigurationId,
        _In_ string_t blobPath,
        _In_ string_t multiplayerSessionTemplateName,
        _In_ string_t multiplayerSessionName,
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
    _XSAPIIMP pplx::task<xbox_live_result<void>> delete_blob(
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
    _XSAPIIMP pplx::task<xbox_live_result<title_storage_blob_result>> download_blob(
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
    _XSAPIIMP pplx::task<xbox_live_result<title_storage_blob_result>> download_blob(
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
    _XSAPIIMP pplx::task<xbox_live_result<title_storage_blob_metadata>> upload_blob(
        _In_ title_storage_blob_metadata blobMetadata,
        _In_ std::shared_ptr<std::vector<unsigned char>> blobBuffer,
        _In_ title_storage_e_tag_match_condition etagMatchCondition,
        _In_ uint32_t preferredUploadBlockSize = DEFAULT_UPLOAD_BLOCK_SIZE
        );

    _XSAPIIMP static const uint32_t MIN_UPLOAD_BLOCK_SIZE;
    _XSAPIIMP static const uint32_t MAX_UPLOAD_BLOCK_SIZE;
    _XSAPIIMP static const uint32_t DEFAULT_UPLOAD_BLOCK_SIZE;
    _XSAPIIMP static const uint32_t MIN_DOWNLOAD_BLOCK_SIZE;
    _XSAPIIMP static const uint32_t DEFAULT_DOWNLOAD_BLOCK_SIZE;

private:
    title_storage_service() {}

    title_storage_service(
        _In_ std::shared_ptr<xbox::services::user_context> userContext,
        _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
        _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
        );

    pplx::task<xbox_live_result<title_storage_blob_metadata_result>> internal_get_blob_metadata(
        _In_ const string_t& serviceConfigurationId,
        _In_ title_storage_type storageType,
        _In_ const string_t& blobPath,
        _In_ const string_t& xboxUserId,
        _In_ const string_t& multiplayerSessionTemplateName,
        _In_ const string_t& multiplayerSessionName,
        _In_ uint32_t skipItems,
        _In_ uint32_t maxItems,
        _In_ const string_t& continuationToken
        );

    static void set_e_tag_header(
        _In_ std::shared_ptr<xbox::services::http_call> httpCall,
        _In_ string_t etag,
        _In_ title_storage_e_tag_match_condition eTagMatchCondition
        );

    static void set_range_header(
        _In_ std::shared_ptr<xbox::services::http_call> httpCall,
        _In_ uint32_t startByte,
        _In_ uint32_t endByte
        );

    static xbox_live_result<string_t> title_storage_quota_subpath(
        _In_ title_storage_type storageType,
        _In_ const string_t& serviceConfigurationId,
        _In_ const string_t& xboxUserId,
        _In_ const string_t& multiplayerSessionTemplateName,
        _In_ const string_t& multiplayerSessionName
        );

    static xbox_live_result<string_t> title_storage_blob_metadata_subpath(
        _In_ title_storage_type storageType,
        _In_ const string_t& serviceConfigurationId,
        _In_ const string_t& xboxUserId,
        _In_ const string_t& multiplayerSessionTemplateName,
        _In_ const string_t& multiplayerSessionName,
        _In_ const string_t& blobPath,
        _In_ uint32_t skipItems,
        _In_ uint32_t maxItems,
        _In_ const string_t& continuationToken
        );

    static xbox_live_result<string_t> title_storage_download_blob_subpath(
        _In_ const title_storage_blob_metadata& blobMetadata,
        _In_ const string_t& selectQuery
        );

    static xbox_live_result<string_t> title_storage_upload_blob_subpath(
        _In_ const title_storage_blob_metadata& blobMetadata,
        _In_ const string_t& continuationToken,
        _In_ bool finalBlock
        );

    std::shared_ptr<xbox::services::user_context> m_userContext;
    std::shared_ptr<xbox::services::xbox_live_context_settings> m_xboxLiveContextSettings;
    std::shared_ptr<xbox::services::xbox_live_app_config> m_appConfig;

    friend xbox_live_context_impl;
    friend class title_storage_blob_metadata_result;
};

}}}