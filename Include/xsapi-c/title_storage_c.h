// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi-c/types_c.h"
#include "xsapi-c/errors_c.h"

#if defined(__cplusplus)
extern "C" {
#endif

struct XSAPI_TITLE_STORAGE_BLOB_METADATA_IMPL;

typedef enum XSAPI_TITLE_STORAGE_TYPE
{
    /// <summary>
    /// Per-user data storage such as game state or game settings that can be only be accessed by Xbox One.
    /// User restrictions can be configured to public or owner only in the service configuration.
    /// </summary>
    XSAPI_TITLE_STORAGE_TYPE_TRUSTED_PLATFORM_STORAGE = 0,

    /// <summary>
    /// DEPRECATED
    /// JSON storage has been deprecated.  Use universal instead.
    ///
    /// Per-user JSON data storage such as game state, game settings, and user generated content for any platforms
    /// Data type is restricted to TitleStorageBlobType::Json
    /// User restrictions can be configured to public or owner only in the service configuration.
    /// </summary>
    XSAPI_TITLE_STORAGE_TYPE_JSON_STORAGE,

    /// <summary>
    /// Global data storage.  This storage type is only writable via the Xbox Developer Portal (XDP).
    /// Any platform may read from this storage type. Data could be rosters, maps, challenges, art resources, etc.
    /// </summary>
    XSAPI_TITLE_STORAGE_TYPE_GLOBAL_STORAGE,

    /// <summary>
    /// DEPRECATED
    /// Title Storage for sessions has been deprecated. Use universal instead.
    ///
    /// Per-session data storage for multiplayer game sessions.  This storage type is only writable to users joined
    /// to the session.  Read access is configurable to public or session-joined only in the service configuration.
    /// </summary>
    XSAPI_TITLE_STORAGE_TYPE_SESSION_STORAGE,

    /// <summary>
    /// Untrusted platform storage has been deprecated.  Use universal instead
    ///
    /// Per-user data storage such as game state or game settings for untrusted platforms
    /// User restrictions can be configured to public or owner only in the service configuration.
    /// </summary>
    XSAPI_TITLE_STORAGE_TYPE_UNTRUSTED_PLATFORM_STORAGE,

    /// <summary>
    /// Per-user data storage such as game state or game settings the can be accessed by Xbox One, Windows 10, and Windows Phone 10 devices
    /// User restrictions can be configured to public or owner only in the service configuration.
    /// </summary>
    XSAPI_TITLE_STORAGE_TYPE_UNIVERSAL
} XSAPI_TITLE_STORAGE_TYPE;

typedef enum XSAPI_TITLE_STORAGE_BLOB_TYPE
{
    XSAPI_TITLE_STORAGE_BLOB_TYPE_UNKNOWN,
    XSAPI_TITLE_STORAGE_BLOB_TYPE_BINARY,
    XSAPI_TITLE_STORAGE_BLOB_TYPE_JSON,
    XSAPI_TITLE_STORAGE_BLOB_TYPE_CONFIG
} XSAPI_TITLE_STORAGE_BLOB_TYPE;

typedef enum XSAPI_TITLE_STORAGE_E_TAG_MATCH_CONDITION
{
    XSAPI_TITLE_STORAGE_E_TAG_MATCH_CONDITION_NOT_USED,
    XSAPI_TITLE_STORAGE_E_TAG_MATCH_CONDITION_IF_MATCH,
    XSAPI_TITLE_STORAGE_E_TAG_MATCH_CONDITION_IF_NOT_MATCH
} XSAPI_TITLE_STORAGE_E_TAG_MATCH_CONDITION;

typedef struct XSAPI_TITLE_STORAGE_QUOTA
{
    PCSTR serviceConfigurationId;
    XSAPI_TITLE_STORAGE_TYPE storageType;
    PCSTR xboxUserId;
    uint64_t usedBytes;
    uint64_t quotaBytes;
} XSAPI_TITLE_STORAGE_QUOTA;

typedef struct XSAPI_TITLE_STORAGE_BLOB_METADATA
{
    PCSTR blobPath;
    XSAPI_TITLE_STORAGE_BLOB_TYPE blobType;
    XSAPI_TITLE_STORAGE_TYPE storageType;
    PCSTR displayName;
    PCSTR ETag;
    time_t clientTimestamp;
    uint64_t length;
    PCSTR serviceConfigurationId;
    PCSTR xboxUserId;
    XSAPI_TITLE_STORAGE_BLOB_METADATA_IMPL *pImpl;
} XSAPI_TITLE_STORAGE_BLOB_METADATA;

typedef struct XSAPI_TITLE_STORAGE_BLOB_METADATA_RESULT
{
    CONST XSAPI_TITLE_STORAGE_BLOB_METADATA* items;
    uint64_t itemCount;
    bool hasNext;
} XSAPI_TITLE_STORAGE_BLOB_METADATA_RESULT;

typedef struct XSAPI_TITLE_STORAGE_BLOB_RESULT
{
    PBYTE blobBuffer;
    uint32_t cbBlobBuffer;
    CONST XSAPI_TITLE_STORAGE_BLOB_METADATA* pMetadata;
} XSAPI_TITLE_STORAGE_BLOB_RESULT;

typedef void(*XSAPI_GET_QUOTA_COMPLETION_ROUTINE)(
    _In_ XBL_RESULT result,
    _In_ XSAPI_TITLE_STORAGE_QUOTA quota,
    _In_opt_ void* context
    );

XBL_API XBL_RESULT XBL_CALLING_CONV
TitleStorageGetQuota(
    _In_ XBL_XBOX_LIVE_CONTEXT_HANDLE xboxLiveContext,
    _In_ PCSTR serviceConfigurationId,
    _In_ XSAPI_TITLE_STORAGE_TYPE storageType,
    _In_ XSAPI_GET_QUOTA_COMPLETION_ROUTINE completionRoutine,
    _In_opt_ void* completionRoutineContext,
    _In_ uint64_t taskGroupId
    ) XBL_NOEXCEPT;

///<summary>
/// The returned blob metadata objects are valid until either of the following APIs are subsequently called
/// 1) TitleStorageGetBlobMetadata
/// 2) TitleStorageBlobMetadataResultGetNext
/// 3) TitleStorageReleaseBlobMetadata
///</summary>
typedef void(*XSAPI_GET_BLOB_METADATA_COMPLETION_ROUTINE)(
    _In_ XBL_RESULT result,
    _In_ XSAPI_TITLE_STORAGE_BLOB_METADATA_RESULT payload,
    _In_opt_ void* context
    );

XBL_API XBL_RESULT XBL_CALLING_CONV
TitleStorageGetBlobMetadata(
    _In_ XBL_XBOX_LIVE_CONTEXT_HANDLE xboxLiveContext,
    _In_ PCSTR serviceConfigurationId,
    _In_ XSAPI_TITLE_STORAGE_TYPE storageType,
    _In_opt_ PCSTR blobPath,
    _In_opt_ PCSTR xboxUserId,
    _In_opt_ uint32_t skipItems,
    _In_opt_ uint32_t maxItems,
    _In_ XSAPI_GET_BLOB_METADATA_COMPLETION_ROUTINE completionRoutine,
    _In_opt_ void* completionRoutineContext,
    _In_ uint64_t taskGroupId
    ) XBL_NOEXCEPT;

XBL_API XBL_RESULT XBL_CALLING_CONV
TitleStorageBlobMetadataResultGetNext(
    _In_ XSAPI_TITLE_STORAGE_BLOB_METADATA_RESULT metadataResult,
    _In_ uint32_t maxItems,
    _In_ XSAPI_GET_BLOB_METADATA_COMPLETION_ROUTINE completionRoutine,
    _In_opt_ void* completionRoutineContext,
    _In_ uint64_t taskGroupId
    ) XBL_NOEXCEPT;

///<summary>
/// The returned blob metadata object is valid until TitleStorageReleaseBlobMetadata is called
///</summary>
XBL_API XBL_RESULT XBL_CALLING_CONV
TitleStorageCreateBlobMetadata(
    _In_ PCSTR serviceConfigurationId,
    _In_ XSAPI_TITLE_STORAGE_TYPE storageType,
    _In_ PCSTR blobPath,
    _In_ XSAPI_TITLE_STORAGE_BLOB_TYPE blobType,
    _In_opt_ PCSTR xboxUserId,
    _In_opt_ PCSTR displayName,
    _In_opt_ PCSTR etag,
    _In_opt_ time_t* pClientTimeStamp,
    _Out_ CONST XSAPI_TITLE_STORAGE_BLOB_METADATA** ppMetadata
    ) XBL_NOEXCEPT;

XBL_API XBL_RESULT XBL_CALLING_CONV
TitleStorageReleaseBlobMetadata(
    _In_ CONST XSAPI_TITLE_STORAGE_BLOB_METADATA* pMetadata
    ) XBL_NOEXCEPT;

typedef void(*XSAPI_DELETE_BLOB_COMPLETION_ROUTINE)(
    _In_ XBL_RESULT result,
    _In_opt_ void* context
    );

XBL_API XBL_RESULT XBL_CALLING_CONV
TitleStorageDeleteBlob(
    _In_ XBL_XBOX_LIVE_CONTEXT_HANDLE xboxLiveContext,
    _In_ CONST XSAPI_TITLE_STORAGE_BLOB_METADATA* pMetadata,
    _In_ bool deleteOnlyIfEtagMatches,
    _In_ XSAPI_DELETE_BLOB_COMPLETION_ROUTINE completionRoutine,
    _In_opt_ void* completionRoutineContext,
    _In_ uint64_t taskGroupId
    ) XBL_NOEXCEPT;

typedef void(*XSAPI_DOWNLOAD_BLOB_COMPLETION_ROUTINE)(
    _In_ XBL_RESULT result,
    _In_ XSAPI_TITLE_STORAGE_BLOB_RESULT payload,
    _In_opt_ void* context
    );

XBL_API XBL_RESULT XBL_CALLING_CONV
TitleStorageDownloadBlob(
    _In_ XBL_XBOX_LIVE_CONTEXT_HANDLE xboxLiveContext,
    _In_ CONST XSAPI_TITLE_STORAGE_BLOB_METADATA* pMetadata,
    _Out_ PBYTE blobBuffer,
    _In_ uint32_t cbBlobBuffer,
    _In_ XSAPI_TITLE_STORAGE_E_TAG_MATCH_CONDITION etagMatchCondition,
    _In_opt_ PCSTR selectQuery,
    _In_opt_ uint32_t* preferredDownloadBlockSize,
    _In_ XSAPI_DOWNLOAD_BLOB_COMPLETION_ROUTINE completionRoutine,
    _In_opt_ void* completionRoutineContext,
    _In_ uint64_t taskGroupId
    ) XBL_NOEXCEPT;

typedef void(*XSAPI_UPLOAD_BLOB_COMPLETION_ROUTINE)(
    _In_ XBL_RESULT result,
    _In_ CONST XSAPI_TITLE_STORAGE_BLOB_METADATA* pMetadata,
    _In_opt_ void* context
    );

XBL_API XBL_RESULT XBL_CALLING_CONV
TitleStorageUploadBlob(
    _In_ XBL_XBOX_LIVE_CONTEXT_HANDLE xboxLiveContext,
    _In_ CONST XSAPI_TITLE_STORAGE_BLOB_METADATA* pMetadata,
    _In_ PBYTE blobBuffer,
    _In_ uint32_t cbBlobBuffer,
    _In_ XSAPI_TITLE_STORAGE_E_TAG_MATCH_CONDITION etagMatchCondition,
    _In_opt_ uint32_t* preferredUploadBlockSize,
    _In_ XSAPI_UPLOAD_BLOB_COMPLETION_ROUTINE completionRoutine,
    _In_opt_ void* completionRoutineContext,
    _In_ uint64_t taskGroupId
    ) XBL_NOEXCEPT;

#if defined(__cplusplus)
} // end extern "C"
#endif // defined(__cplusplus)