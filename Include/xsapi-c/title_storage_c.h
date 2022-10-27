// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#if !defined(__cplusplus)
#error C++11 required
#endif

#pragma once

extern "C"
{

#define XBL_TITLE_STORAGE_MIN_UPLOAD_BLOCK_SIZE 1024
#define XBL_TITLE_STORAGE_MAX_UPLOAD_BLOCK_SIZE (4 * 1024 * 1024)
#define XBL_TITLE_STORAGE_DEFAULT_UPLOAD_BLOCK_SIZE (256 * 1024)
#define XBL_TITLE_STORAGE_MIN_DOWNLOAD_BLOCK_SIZE 1024
#define XBL_TITLE_STORAGE_DEFAULT_DOWNLOAD_BLOCK_SIZE (1024 * 1024)

#define XBL_TITLE_STORAGE_BLOB_PATH_MAX_LENGTH (257 * 3)
#define XBL_TITLE_STORAGE_BLOB_DISPLAY_NAME_MAX_LENGTH (129 * 3)
#define XBL_TITLE_STORAGE_BLOB_ETAG_MAX_LENGTH (18 * 3) 

/// <summary>
/// Defines values used to indicate title storage type.
/// </summary>
enum class XblTitleStorageType : uint32_t
{
    /// <summary>
    /// Per-user data storage such as game state or game settings that can be only be accessed by Xbox consoles.  
    /// User restrictions can be configured to public or owner only in the service configuration.
    /// </summary>
    TrustedPlatformStorage,

    /// <summary>
    /// Global data storage.  This storage type is only writable via title configuration sites or Xbox Live developer tools.  
    /// Any platform may read from this storage type.  Data could be rosters, maps, challenges, art resources, etc.
    /// </summary>
    GlobalStorage,

    /// <summary>
    /// Per-user data storage such as game state or game settings that can be accessed by Xbox consoles, Windows 10, and mobile devices.  
    /// User restrictions can be configured to public or owner only in the service configuration.
    /// </summary>
    Universal
};

/// <summary>
/// Defines values used to indicate title storage blob type.
/// </summary>
enum class XblTitleStorageBlobType : uint32_t
{
    /// <summary>
    /// Unknown blob type.
    /// </summary>
    Unknown,

    /// <summary>
    /// Binary blob type.
    /// </summary>
    Binary,

    /// <summary>
    /// JSON blob type.
    /// </summary>
    Json,

    /// <summary>
    /// Config blob type.
    /// </summary>
    Config
};

/// <summary>
/// Defines values used to indicate the ETag match condition used when downloading, uploading or deleting title storage data.
/// </summary>
enum class XblTitleStorageETagMatchCondition : uint32_t
{
    /// <summary>
    /// There is no match condition.
    /// </summary>
    NotUsed,

    /// <summary>
    /// Perform the request if the Etag value specified matches the service value.
    /// </summary>
    IfMatch,

    /// <summary>
    /// Perform the request if the Etag value specified does not match the service value.
    /// </summary>
    IfNotMatch
};

/// <summary>
/// Metadata about a blob.
/// </summary>
typedef struct XblTitleStorageBlobMetadata
{
    /// <summary>
    /// Blob path is a unique string that conforms to a SubPath\file format (example: "foo\bar\blob.txt").
    /// </summary>
    _Null_terminated_ char blobPath[XBL_TITLE_STORAGE_BLOB_PATH_MAX_LENGTH];

    /// <summary>
    /// Type of blob data. Possible values are: Binary, Json, and Config.
    /// </summary>
    XblTitleStorageBlobType blobType;

    /// <summary>
    /// Type of storage.
    /// </summary>
    XblTitleStorageType storageType;

    /// <summary>
    /// [optional] Friendly display name to show in app UI.
    /// </summary>
    _Null_terminated_ char displayName[XBL_TITLE_STORAGE_BLOB_DISPLAY_NAME_MAX_LENGTH];

    /// <summary>
    /// ETag for the file used in read and write requests.
    /// </summary>
    _Null_terminated_ char eTag[XBL_TITLE_STORAGE_BLOB_ETAG_MAX_LENGTH];

    /// <summary>
    /// [optional] Timestamp assigned by the client.
    /// </summary>
    time_t clientTimestamp;

    /// <summary>
    /// Gets the number of bytes of the blob data.
    /// </summary>
    size_t length;

    /// <summary>
    /// The service configuration ID of the title
    /// </summary>
    _Null_terminated_ char serviceConfigurationId[XBL_SCID_LENGTH];

    /// <summary>
    /// The Xbox User ID of the player this file belongs to.  
    /// This value will be null for Global and Session files.
    /// </summary>
    uint64_t xboxUserId;
} XblTitleStorageBlobMetadata;

/// <summary>
/// A handle that represents metadata about blob data returned from the cloud.
/// </summary>
typedef struct XblTitleStorageBlobMetadataResult* XblTitleStorageBlobMetadataResultHandle;

/// <summary>
/// Get a list of XblTitleStorageBlobMetadata objects.
/// </summary>
/// <param name="resultHandle">Title storage blob metadata result handle.</param>
/// <param name="items">Passes back a pointer to an array of XblTitleStorageBlobMetadata objects.  
/// The memory for the returned pointer remains valid for the life of 
/// the XblTitleStorageBlobMetadataResultHandle object until it is closed.</param>
/// <param name="itemsCount">Passes back the number of objects in the items array.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// This memory of the list is freed when the XblTitleStorageBlobMetadataResultHandle is closed 
/// with <see cref="XblTitleStorageBlobMetadataResultCloseHandle"/>.
/// </remarks>
STDAPI XblTitleStorageBlobMetadataResultGetItems(
    _In_ XblTitleStorageBlobMetadataResultHandle resultHandle,
    _Out_ const XblTitleStorageBlobMetadata** items,
    _Out_ size_t* itemsCount
) XBL_NOEXCEPT;

/// <summary>
/// Checks if there are more pages of XblTitleStorageBlobMetadata to retrieve from the service.
/// </summary>
/// <param name="resultHandle">Title storage blob metadata result handle.</param>
/// <param name="hasNext">Passes back true if there are more results to retrieve, false otherwise.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// To retrieve the next page of items, call <see cref="XblTitleStorageBlobMetadataResultGetNextAsync"/>.
/// </remarks>
STDAPI XblTitleStorageBlobMetadataResultHasNext(
    _In_ XblTitleStorageBlobMetadataResultHandle resultHandle,
    _Out_ bool* hasNext
) XBL_NOEXCEPT;

/// <summary>
/// Retrieves the next page of XblTitleStorageBlobMetadata objects.
/// </summary>
/// <param name="resultHandle">Title storage blob metadata result handle.</param>
/// <param name="maxItems">The maximum number of items the result can contain. Pass 0 to attempt to retrieve all items.</param>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// To get the result, call <see cref="XblTitleStorageBlobMetadataResultGetNextResult"/> inside 
/// the AsyncBlock callback or after the AsyncBlock is complete.
/// </remarks>
STDAPI XblTitleStorageBlobMetadataResultGetNextAsync(
    _In_ XblTitleStorageBlobMetadataResultHandle resultHandle,
    _In_ uint32_t maxItems,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Get the result for a completed XblTitleStorageBlobMetadataResultGetNextAsync operation.
/// </summary>
/// <param name="async">The same AsyncBlock that passed to XblTitleStorageBlobMetadataResultGetNextAsync.</param>
/// <param name="result">Passes back the next XblTitleStorageBlobMetadataResultHandle.  
/// Note that this is a separate handle than the one passed to the XblTitleStorageBlobMetadataResultGetNextAsync API.  
/// Each result handle must be closed separately.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblTitleStorageBlobMetadataResultGetNextResult(
    _In_ XAsyncBlock* async,
    _Out_ XblTitleStorageBlobMetadataResultHandle* result
) XBL_NOEXCEPT;

/// <summary>
/// Duplicates a XblTitleStorageBlobMetadataResultHandle.
/// </summary>
/// <param name="handle">The XblTitleStorageBlobMetadataResultHandle to duplicate.</param>
/// <param name="duplicatedHandle">Passes back the duplicated handle.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblTitleStorageBlobMetadataResultDuplicateHandle(
    _In_ XblTitleStorageBlobMetadataResultHandle handle,
    _Out_ XblTitleStorageBlobMetadataResultHandle* duplicatedHandle
) XBL_NOEXCEPT;

/// <summary>
/// Closes the XblTitleStorageBlobMetadataResultHandle.
/// </summary>
/// <param name="handle">The XblTitleStorageBlobMetadataResultHandle to close.</param>
/// <returns></returns>
/// <remarks>
/// When all outstanding handles have been closed, the memory associated with the title storage blob metadata result will be freed.
/// </remarks>
STDAPI_(void) XblTitleStorageBlobMetadataResultCloseHandle(
    _In_ XblTitleStorageBlobMetadataResultHandle handle
) XBL_NOEXCEPT;
    
/// <summary>
/// Gets title storage quota information for the specified service configuration and storage type.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="serviceConfigurationId">The Service Configuration ID (SCID) for the title. The SCID is considered case sensitive so paste it directly from the Partner Center.</param>
/// <param name="storageType">The storage type to get quota information for.</param>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// For TrustedPlatform storage types, the request will be made for the calling user's Xbox user Id.  
/// To get the result, call XblTitleStorageGetQuotaResult inside the AsyncBlock callback
/// or after the AsyncBlock is complete.
/// </remarks>
/// <rest>V1 GET trustedplatform/users/xuid({xuid})/scids/{scid}</rest>
/// <rest>V1 GET trustedplatform/users/xuid({xuid})/scids/{scid}</rest>
/// <rest>V1 GET json/users/xuid({xuid})/scids/{scid}</rest>
/// <rest>V1 GET global/scids/{scid}</rest>
STDAPI XblTitleStorageGetQuotaAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_z_ const char* serviceConfigurationId,
    _In_ XblTitleStorageType storageType,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Get the result for a completed XblTitleStorageGetQuotaAsync operation.
/// </summary>
/// <param name="async">The same AsyncBlock that passed to XblTitleStorageGetQuotaAsync.</param>
/// <param name="usedBytes">Passes back the number of bytes used in title storage of type StorageType.</param>
/// <param name="quotaBytes">Passes back the maximum number of bytes that can be used in title storage of type StorageType.  
/// Note that this is a soft limit and the used bytes may actually exceed this value.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblTitleStorageGetQuotaResult(
    _In_ XAsyncBlock* async,
    _Out_ size_t* usedBytes,
    _Out_ size_t* quotaBytes
) XBL_NOEXCEPT;

/// <summary>
/// Gets a list of blob metadata objects under a given path for the specified service configuration, storage type and storage ID.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="serviceConfigurationId">The Service Configuration ID (SCID) for the title. The SCID is considered case sensitive so paste it directly from the Partner Center.</param>
/// <param name="storageType">The storage type to get blob metadata objects for.</param>
/// <param name="blobPath">The root path to enumerate.  Results will be for blobs contained in this path and all subpaths. (Optional)</param>
/// <param name="xboxUserId">The Xbox User ID of the title storage to enumerate. Ignored when enumerating GlobalStorage, so passing 0 is acceptable. (Optional)</param>
/// <param name="skipItems">The number of items to skip before returning results. (Optional)</param>
/// <param name="maxItems">The maximum number of items to return. Pass 0 to attempt to retrieve all items. (Optional)</param>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// To get the result, call XblTitleStorageGetBlobMetadataResult inside the AsyncBlock callback
/// or after the AsyncBlock is complete.
/// </remarks>
/// <rest>V1 GET trustedplatform/users/xuid({xuid})/scids/{scid}/data/{path}?maxItems={maxItems}[skipItems={skipItems}]</rest>
/// <rest>V1 GET json/users/xuid({xuid})/scids/{scid}/data/{path}?maxItems={maxItems}[skipItems={skipItems}]</rest>
/// <rest>V1 GET global/scids/{scid}/data/{path}?maxItems={maxItems}[skipItems={skipItems}]</rest>
STDAPI XblTitleStorageGetBlobMetadataAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_z_ const char* serviceConfigurationId,
    _In_ XblTitleStorageType storageType,
    _In_z_ const char* blobPath,
    _In_ uint64_t xboxUserId,
    _In_ uint32_t skipItems,
    _In_ uint32_t maxItems,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;
    
/// <summary>
/// Get the result for a completed XblTitleStorageGetBlobMetadataAsync operation.
/// </summary>
/// <param name="async">The same AsyncBlock that passed to XblTitleStorageGetBlobMetadataAsync.</param>
/// <param name="result">Passes back the title storage blob metadata result handle.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblTitleStorageGetBlobMetadataResult(
    _In_ XAsyncBlock* async,
    _Out_ XblTitleStorageBlobMetadataResultHandle* result
) XBL_NOEXCEPT;
    
/// <summary>
/// Deletes a blob from title storage.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="blobMetadata">The blob metadata for the title storage blob to delete.</param>
/// <param name="deleteOnlyIfEtagMatches">Specifies whether or not to have the delete operation check that the ETag matches before deleting the blob.</param>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// The result of the asynchronous operation can be obtained by calling <see cref="XAsyncGetStatus"/>
/// inside the AsyncBlock callback or after the AsyncBlock is complete.
/// </remarks>
/// <rest>V1 DELETE trustedplatform/users/xuid({xuid})/scids/{scid}/data/{path},{type}</rest>
/// <rest>V1 DELETE json/users/xuid({xuid})/scids/{scid}/data/{path},{type}</rest>
/// <rest>V1 DELETE sessions/{sessionId}/scids/{scid}/data/{path},{type}</rest>
STDAPI XblTitleStorageDeleteBlobAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_ XblTitleStorageBlobMetadata blobMetadata,
    _In_ bool deleteOnlyIfEtagMatches,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Downloads blob data from title storage.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="blobMetadata">The blob metadata for the title storage blob to download.</param>
/// <param name="blobBuffer">A caller allocated buffer that passes back the downloaded blob data.  
/// This buffer needs to be large enough to store the blob being downloaded.  
/// If necessary, the length required for the buffer can be retrieved by getting the blob metadata.</param>
/// <param name="blobBufferCount">The length of blobBuffer.</param>
/// <param name="etagMatchCondition">The ETag match condition used to determine if the blob should be downloaded.</param>
/// <param name="selectQuery">ConfigStorage filter string or JSONStorage json property name string to filter. (Optional)</param>
/// <param name="preferredDownloadBlockSize">The preferred download block size in bytes for binary blobs. Pass 0 to use the default size. (Optional)</param>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// To get the result, call <see cref="XblTitleStorageDownloadBlobResult"/> inside the AsyncBlock callback
/// or after the AsyncBlock is complete.  
/// This method will return E_NOT_SUFFICIENT_BUFFER (0x8007007A) if the blobBuffer doesn't have enough capacity to hold the blob data.
/// </remarks>
/// <rest>V1 GET trustedplatform/users/xuid({xuid})/scids/{scid}/data/{path},{type}</rest>
/// <rest>V1 GET json/users/xuid({xuid})/scids/{scid}/data/{path},{type}</rest>
/// <rest>V1 GET global/scids/{scid}/data/{path},{type}</rest>
/// <rest>V1 GET sessions/{sessionId}/scids/{scid}/data/{path},{type}</rest>
STDAPI XblTitleStorageDownloadBlobAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_ XblTitleStorageBlobMetadata blobMetadata,
    _Out_writes_(blobBufferCount) uint8_t* blobBuffer,
    _In_ size_t blobBufferCount,
    _In_ XblTitleStorageETagMatchCondition etagMatchCondition,
    _In_opt_z_ const char* selectQuery,
    _In_ size_t preferredDownloadBlockSize,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Get the result for a completed XblTitleStorageDownloadBlobAsync operation.
/// </summary>
/// <param name="async">The same AsyncBlock that passed to XblTitleStorageDownloadBlobAsync.</param>
/// <param name="blobMetadata">A caller allocated XblTitleStorageBlobMetadata that passes back the downloaded blob metadata.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblTitleStorageDownloadBlobResult(
    _In_ XAsyncBlock* async,
    _Out_ XblTitleStorageBlobMetadata* blobMetadata
) XBL_NOEXCEPT;
    
/// <summary>
/// Uploads blob data to title storage.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="blobMetadata">Contains properties required to upload the buffer to title storage.  
/// Uploads require a service configuration Id, blob path, blob type and storage type at a minimum.</param>
/// <param name="blobBuffer">The buffer containing the blob data to upload.  
/// This buffer must be available for the duration of the async operation.  
/// Clients should not modify the buffer while an upload is in progress.</param>
/// <param name="blobBufferCount">The length of blobBuffer.</param>
/// <param name="etagMatchCondition">The ETag match condition used to determine if the blob data should be uploaded.</param>
/// <param name="preferredUploadBlockSize">The preferred upload block size in bytes for binary blobs.  
/// Binary blobs will be uploaded in multiple chunks of this size if they exceed it.  
/// Larger sizes are preferred by the service.  
/// If timeouts occur, the app should retry with a smaller size.  
/// Block size must be within the 1K to 4MB range.  
/// This method will use a default size if this parameter is not within the acceptable range.  
/// The current minimum size is 1024 bytes, maximum size is 4194304 bytes and the default size is 262144 bytes. (Optional)</param>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <rest>V1 PUT json/users/xuid({xuid})/scids/{scid}/data/{path},{type}</rest>
/// <rest>V1 PUT global/scids/{scid}/data/{path},{type}</rest>
/// <rest>V1 PUT sessions/{sessionId}/scids/{scid}/data/{path},{type}</rest>
STDAPI XblTitleStorageUploadBlobAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_ XblTitleStorageBlobMetadata blobMetadata,
    _In_ const uint8_t* blobBuffer,
    _In_ size_t blobBufferCount,
    _In_ XblTitleStorageETagMatchCondition etagMatchCondition,
    _In_ size_t preferredUploadBlockSize,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;
    
/// <summary>
/// Get the result for a completed XblTitleStorageUploadBlobAsync operation.
/// </summary>
/// <param name="async">The same AsyncBlock that passed to XblTitleStorageUploadBlobAsync.</param>
/// <param name="blobMetadata">A caller allocated XblTitleStorageBlobMetadata that passes back the uploaded blob metadata.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblTitleStorageUploadBlobResult(
    _In_ XAsyncBlock* async,
    _Out_ XblTitleStorageBlobMetadata* blobMetadata
) XBL_NOEXCEPT;
    
}
