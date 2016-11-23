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
#include "xsapi/title_storage.h"
#include "TitleStorageQuota_WinRT.h"
#include "TitleStorageType_WinRT.h"
#include "TitleStorageBlobMetadataResult_WinRT.h"
#include "TitleStorageBlobResult_WinRT.h"
#include "TitleStorageETagMatchCondition_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_BEGIN

/// <summary>
/// Represents an endpoint that provides methods that you can use to access the Title Storage service.
/// </summary>
public ref class TitleStorageService sealed
{
public:
    /// <summary>
    /// Gets title storage quota information for the specified service configuration and storage type.
    /// For user storage types (TrustedPlatform and Json) the request is made for the calling user's
    /// Xbox user Id.
    /// </summary>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) of the title</param>
    /// <param name="storageType">The storage type to get quota information for.</param>
    /// <returns>
    /// An interface for tracking the process of the asynchronous call.
    /// The result is a TitleStorageQuota object containing the quota information.
    /// </returns>
    /// <remarks>Calls
    /// V1 GET trustedplatform/users/xuid({xuid})/scids/{scid} or
    /// V1 GET json/users/xuid({xuid})/scids/{scid} or
    /// V1 GET global/scids/{scid}
    /// </remarks>
    Windows::Foundation::IAsyncOperation<TitleStorageQuota^>^ GetQuotaAsync(
        _In_ Platform::String^ serviceConfigurationId,
        _In_ TitleStorageType storageType
        );

    /// <summary>
    /// Gets title storage quota information for the SessionStorage associated with the specified multiplayer session.
    /// </summary>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) of the title</param>
    /// <param name="multiplayerSessionTemplateName">The multiplayer session template name for the session to get quota information for.</param>
    /// <param name="multiplayerSessionName">The multiplayer session name for the session to get quota information for.</param>
    /// <returns>
    /// An interface for tracking the process of the asynchronous call.
    /// The result is a TitleStorageQuota object containing the quota information.
    /// </returns>
    /// <remarks>Calls
    /// V1 GET sessions/{sessionId}/scids/{scid}
    /// </remarks>
    Windows::Foundation::IAsyncOperation<TitleStorageQuota^>^ GetQuotaForSessionStorageAsync(
        _In_ Platform::String^ serviceConfigurationId,
        _In_ Platform::String^ multiplayerSessionTemplateName,
        _In_ Platform::String^ multiplayerSessionName
        );

    /// <summary>
    /// Gets a list of metadata information about a file in storage, given a specified service configuration, storage type, and storage ID.
    /// </summary>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) of the title</param>
    /// <param name="storageType">The storage type to get blob metadata objects for.</param>
    /// <param name="blobPath">The root path to search.  The results include all blobs contained in this path and all subpaths. (Optional)</param>
    /// <param name="xboxUserId">The Xbox User ID of the title storage to enumerate. Pass nullptr when searching for GlobalStorage type data. (Optional)</param>
    /// <param name="skipItems">The number of items to skip before returning results.</param>
    /// <param name="maxItems">The maximum number of items to return.</param>
    /// <returns>
    /// An interface for tracking the process of the asynchronous call.
    /// The result is a TitleStorageBlobMetadataResult object containing the list of enumerated blob metadata objects.
    /// </returns>
    /// <remarks>Calls
    /// V1 GET trustedplatform/users/xuid({xuid})/scids/{scid}/data/{path}?maxItems={maxItems}[skipItems={skipItems}] or
    /// V1 GET json/users/xuid({xuid})/scids/{scid}/data/{path}?maxItems={maxItems}[skipItems={skipItems}] or
    /// V1 GET global/scids/{scid}/data/{path}?maxItems={maxItems}[skipItems={skipItems}]
    /// </remarks>
    Windows::Foundation::IAsyncOperation<TitleStorageBlobMetadataResult^>^ GetBlobMetadataAsync(
        _In_ Platform::String^ serviceConfigurationId,
        _In_ TitleStorageType storageType,
        _In_opt_ Platform::String^ blobPath,
        _In_opt_ Platform::String^ xboxUserId,
        _In_ uint32 skipItems,
        _In_ uint32 maxItems
        );

    /// <summary>
    /// Gets a list of metadata information about a file in session storage under a given path for the specified multiplayer session.
    /// </summary>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) of the title</param>
    /// <param name="blobPath">The root path to search.  The results include all blobs contained in this path and all subpaths. (Optional)</param>
    /// <param name="multiplayerSessionTemplateName">The multiplayer session template name for the session to get blob metadata objects for.</param>
    /// <param name="multiplayerSessionName">The multiplayer session name for the session to get blob metadata objects for.</param>
    /// <param name="skipItems">The number of items to skip before returning results.</param>
    /// <param name="maxItems">The maximum number of items to return.</param>
    /// <returns>
    /// An interface for tracking the process of the asynchronous call.
    /// The result is a TitleStorageBlobMetadataResult object containing the list of enumerated blob metadata objects.
    /// </returns>
    /// <remarks>Calls
    /// V1 GET sessions/{sessionId}/scids/{scid}/data/{path}?maxItems={maxItems}[skipItems={skipItems}]
    /// </remarks>
    Windows::Foundation::IAsyncOperation<TitleStorageBlobMetadataResult^>^ GetBlobMetadataForSessionStorageAsync(
        _In_ Platform::String^ serviceConfigurationId,
        _In_opt_ Platform::String^ blobPath,
        _In_ Platform::String^ multiplayerSessionTemplateName,
        _In_ Platform::String^ multiplayerSessionName,
        _In_ uint32 skipItems,
        _In_ uint32 maxItems
        );

    /// <summary>
    /// Deletes a blob from title storage.
    /// </summary>
    /// <param name="blobMetadata">The blob metadata for the title storage blob to delete.</param>
    /// <param name="deleteOnlyIfEtagMatches">Specifies whether or not to have the delete operation check that the ETag matches before deleting the blob.</param>
    /// <returns>
    /// An interface for tracking the progress of the asynchronous call.
    /// </returns>
    /// <remarks>Calls
    /// V1 DELETE trustedplatform/users/xuid({xuid})/scids/{scid}/data/{path},{type} or
    /// V1 DELETE json/users/xuid({xuid})/scids/{scid}/data/{path},{type} or
    /// V1 DELETE sessions/{sessionId}/scids/{scid}/data/{path},{type}
    /// </remarks>
    Windows::Foundation::IAsyncAction^ DeleteBlobAsync(
        _In_ TitleStorageBlobMetadata^ blobMetadata,
        _In_ bool deleteOnlyIfETagMatches
        );

    /// <summary>
    /// Dowloads blob data from title storage.
    /// </summary>
    /// <param name="blobMetadata">The blob metadata for the title storage blob to download.</param>
    /// <param name="blobBuffer">The client provided buffer to contain the downloaded blob data.
    /// The buffer must be large enough to store the downloaded blob. You can retrieve the required buffer size by getting the blob metadata.</param>
    /// <param name="etagMatchCondition">The ETag match condition used to determine if the blob should be downloaded.</param>
    /// <param name="selectQuery">A query string that contains a ConfigStorage filter string or JSONStorage json property name string to filter. (Optional)</param>
    /// <returns>
    /// An interface for tracking the progress of the asynchronous call.
    /// The result is a TitleStorageBlobResult object containing the client provided blob buffer and an updated TitleStorageBlobMetadata object.
    /// The metadata object contains updated ETag and Length properties.
    /// </returns>
    /// <remarks>
    /// This method throws ERROR_INSUFFICIENT_BUFFER (0x8007007A) if the blobBuffer doesn't have enough capacity to hold the blob data.
    ///
    /// Calls V1 GET trustedplatform/users/xuid({xuid})/scids/{scid}/data/{path},{type} or
    /// V1 GET json/users/xuid({xuid})/scids/{scid}/data/{path},{type} or
    /// V1 GET global/scids/{scid}/data/{path},{type} or
    /// V1 GET sessions/{sessionId}/scids/{scid}/data/{path},{type}
    /// </remarks>
    Windows::Foundation::IAsyncOperation<TitleStorageBlobResult^>^ DownloadBlobAsync(
        _In_ TitleStorageBlobMetadata^ blobMetadata,
        _In_ Windows::Storage::Streams::IBuffer^ blobBuffer,
        _In_ TitleStorageETagMatchCondition etagMatchCondition,
        _In_opt_ Platform::String^ selectQuery
        );

    /// <summary>
    /// Dowloads blob data from title storage.
    /// </summary>
    /// <param name="blobMetadata">The blob metadata for the title storage blob to download.</param>
    /// <param name="blobBuffer">The client provided buffer to contain the downloaded blob data.
    /// The buffer must be large enough to store the downloaded blob. You can retrieve the required buffer size by getting the blob metadata.</param>
    /// <param name="etagMatchCondition">The ETag match condition used to determine if the blob should be downloaded.</param>
    /// <param name="selectQuery">A query string that contains a ConfigStorage filter string or JSONStorage json property name string to filter. (Optional)</param>
    /// <param name="preferredDownloadBlockSize">The preferred download block size in bytes for binary blobs. </param>
    /// <returns>
    /// An interface for tracking the progress of the asynchronous call.
    /// The result is a TitleStorageBlobResult object containing the client provided blob buffer and an updated TitleStorageBlobMetadata object.
    /// The metadata object will contain updated ETag and Length properties</returns>
    /// <remarks>
    /// This method will throw ERROR_INSUFFICIENT_BUFFER (0x8007007A) if the blobBuffer doesn't have enough capacity to hold the blob data.
    /// Calls V1 GET trustedplatform/users/xuid({xuid})/scids/{scid}/data/{path},{type} or
    /// V1 GET json/users/xuid({xuid})/scids/{scid}/data/{path},{type} or
    /// V1 GET global/scids/{scid}/data/{path},{type} or
    /// V1 GET sessions/{sessionId}/scids/{scid}/data/{path},{type}
    /// </remarks>
    Windows::Foundation::IAsyncOperation<TitleStorageBlobResult^>^ DownloadBlobAsync(
        _In_ TitleStorageBlobMetadata^ blobMetadata,
        _In_ Windows::Storage::Streams::IBuffer^ blobBuffer,
        _In_ TitleStorageETagMatchCondition etagMatchCondition,
        _In_opt_ Platform::String^ selectQuery,
        _In_ uint32 preferredDownloadBlockSize 
        );

    /// <summary>
    /// Uploads a blob to title storage.
    /// </summary>
    /// <param name="blobMetadata">Contains properties required to upload the buffer to title storage.  Uploads require a service configuration ID, blob path, blob type and storage type at a minimum.</param>
    /// <param name="blobBuffer">The buffer containing the blob data to upload.  This buffer must be available for the duration of the async operation.  Clients should not modify the buffer while an upload is in progress.</param>
    /// <param name="etagMatchCondition">The ETag match condition used to determine if the blob data should be uploaded.</param>
    /// <param name="preferredUploadBlockSize">The preferred upload block size in bytes for binary blobs. Binary blobs are
    /// uploaded in multiple chunks of this size if they exceed it.  Larger sizes are preferred by the service.
    /// If timeouts occur, the app should retry with a smaller size.  Block size must be within the 1K to 4MB range.  This method
    /// will use a default size if this parameter is not within the acceptable range.  The current minimum size is 1024 bytes,
    /// the maximum size is 4194304 bytes, and the default size is 262144 bytes.
    /// </param>
    /// <returns>
    /// An interface for tracking the progress of the asynchronous call.
    /// The result is a TitleStorageBlobMetadata object with updated Etag and Length properties.
    /// </returns>
    /// <remarks>
    /// V1 PUT json/users/xuid({xuid})/scids/{scid}/data/{path},{type} or
    /// V1 PUT global/scids/{scid}/data/{path},{type} or
    /// V1 PUT sessions/{sessionId}/scids/{scid}/data/{path},{type}
    /// </remarks>
    Windows::Foundation::IAsyncOperation<TitleStorageBlobMetadata^>^ UploadBlobAsync(
        _In_ TitleStorageBlobMetadata^ blobMetadata,
        _In_ Windows::Storage::Streams::IBuffer^ blobBuffer,
        _In_ TitleStorageETagMatchCondition etagMatchCondition,
        _In_ uint32 preferredUploadBlockSize
        );

internal:
    TitleStorageService(
        _In_ xbox::services::title_storage::title_storage_service cppObj
        );

private:
    xbox::services::title_storage::title_storage_service m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_END