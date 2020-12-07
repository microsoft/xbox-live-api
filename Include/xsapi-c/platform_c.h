// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#if !defined(__cplusplus)
#error C++11 required
#endif

#pragma once

extern "C"
{

/// <summary>
/// Handle to an operation XSAPI requested the client to perform.
/// </summary>
typedef struct XblClientOperation* XblClientOperationHandle;

/// <summary>
/// Enum defining the results of a client operation.
/// </summary>
enum class XblClientOperationResult : uint32_t
{
    /// <summary>
    /// Client completed the requested operation sucessfully.
    /// </summary>
    Success,

    /// <summary>
    /// Client was not able to complete the requested operation for any reason.
    /// </summary>
    Failure
};

//-----------------------------------------------------------------------------
// Types related to custom storage hooks. Local storage is needed on Win32,
// iOS, and Android platforms but default implementations are provided.

/// <summary>
/// Write mode for a local storage write operation.
/// </summary>
enum class XblLocalStorageWriteMode : uint32_t
{
    /// <summary>
    /// Appends new data to the end of any existing data.
    /// </summary>
    Append,

    /// <summary>
    /// Overwrite any existing data.
    /// </summary>
    Truncate
};

/// <summary>
/// Invoked by XSAPI to request the client to perform a local storage write operation.
/// </summary>
/// <param name="context">Optional pointer to data used by the event handler.</param>
/// <param name="operation">The handle for this operation.</param>
/// <param name="user">The user for which the data is being written.</param>
/// <param name="mode">The mode to be used for the write operation.</param>
/// <param name="key">Identifies the data being written.</param>
/// <param name="dataSize">The size (in bytes) of the data.</param>
/// <param name="data">The data to write.</param>
/// <returns></returns>
/// <remarks>
/// When the operation is complete, XblLocalStorageWriteComplete should be called.  
/// Apart from context, all parameters are owned by XSAPI and are guaranteed to be valid
/// until the operation is complete.
/// </remarks>
typedef void (*XblLocalStorageWriteHandler)(
    _In_opt_ void* context,
    _In_ XblClientOperationHandle operation,
    _In_ XblUserHandle user,
    _In_ XblLocalStorageWriteMode mode,
    _In_z_ char const* key,
    _In_ size_t dataSize,
    _In_reads_bytes_(dataSize) void const* data
);

/// <summary>
/// Invoked by XSAPI to request the client to perform a local storage read operation.
/// </summary>
/// <param name="context">Optional pointer to data used by the event handler.</param>
/// <param name="operation">The handle for this operation.</param>
/// <param name="user">The user for which XSAPI is requesting the read.</param>
/// <param name="key">Identifies the data being read.</param>
/// <returns></returns>
/// <remarks>
/// When the operation is complete, XblLocalStorageReadComplete should be called.  
/// Apart from context, all parameters are owned by XSAPI and are guaranteed to be valid
/// until the operation is complete.  
/// If the requested key is not found, the client should complete with 
/// XblClientOperationResult::Success and no data.
/// </remarks>
typedef void (*XblLocalStorageReadHandler)(
    _In_opt_ void* context,
    _In_ XblClientOperationHandle operation,
    _In_ XblUserHandle user,
    _In_z_ const char* key
);

/// <summary>
/// Invoked by XSAPI to request the client to perform a local storage clear operation.
/// </summary>
/// <param name="context">Optional pointer to data used by the event handler.</param>
/// <param name="operation">The handle for this operation.</param>
/// <param name="user">The user for which XSAPI is requesting the clear.</param>
/// <param name="key">Identifies the data being read.</param>
/// <returns></returns>
/// <remarks>
/// When the operation is complete, XblLocalStorageClearComplete should be called.  
/// Apart from context, all parameters are owned by XSAPI and are guaranteed to be valid
/// until the operation is complete.
/// </remarks>
typedef void (*XblLocalStorageClearHandler)(
    _In_opt_ void* context,
    _In_ XblClientOperationHandle operation,
    _In_ XblUserHandle user,
    _In_z_ const char* key
);

#if XSAPI_INTERNAL_EVENTS_SERVICE

/// <summary>
/// Completes a local storage write operation.
/// <see cref="XblLocalStorageWriteHandler"/>
/// </summary>
/// <param name="operation">The handle for this operation.</param>
/// <param name="result">The result of the operation.</param>
/// <param name="dataSize">The new size (in bytes) of the data associated with the requested key.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Should only be called by clients after completing (or failing to complete) a requested write operation.
/// </remarks>
STDAPI XblLocalStorageWriteComplete(
    _In_ XblClientOperationHandle operation,
    _In_ XblClientOperationResult result,
    _In_ size_t dataSize
) XBL_NOEXCEPT;

/// <summary>
/// Completes a local storage read operation.
/// <see cref="XblLocalStorageReadHandler"/>
/// </summary>
/// <param name="operation">The handle for this operation.</param>
/// <param name="result">The result of the operation.</param>
/// <param name="dataSize">The size (in bytes) of the data.</param>
/// <param name="data">The data read.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Should only be called by clients after completing (or failing to complete) a requested read operation.  
/// If the requested key cannot be found, the operation should be completed with 
/// XblClientOperationResult::Success and dataSize of 0.
/// </remarks>
STDAPI XblLocalStorageReadComplete(
    _In_ XblClientOperationHandle operation,
    _In_ XblClientOperationResult result,
    _In_ size_t dataSize,
    _In_reads_bytes_opt_(dataSize) void const* data
) XBL_NOEXCEPT;

/// <summary>
/// Completes a local storage clear operation.
/// <see cref="XblLocalStorageClearHandler"/>
/// </summary>
/// <param name="operation">The handle for this operation.</param>
/// <param name="result">The result of the operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Should only be called by clients after completing (or failing to complete) a requested clear operation.  
/// If the requested key cannot be found, the operation should be completed with XblClientOperationResult::Success.
/// </remarks>
STDAPI XblLocalStorageClearComplete(
    _In_ XblClientOperationHandle operation,
    _In_ XblClientOperationResult result
) XBL_NOEXCEPT;

/// <summary>
/// Sets the storage handlers.
/// </summary>
/// <param name="queue">The async queue the callbacks should be invoked on.</param>
/// <param name="writeHandler">Handler to be invoked when XSAPI needs to write to local storage.</param>
/// <param name="readHandler">Handler to be invoked when XSAPI needs to read from local storage.</param>
/// <param name="clearHandler">Handler to be invoked when XSAPI needs to clear local storage.</param>
/// <param name="context">Client context to be passed back to the handlers.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Must be called before XblInitialize and all three handlers must be set together.
/// </remarks>
STDAPI XblLocalStorageSetHandlers(
    _In_opt_ XTaskQueueHandle queue,
    _In_ XblLocalStorageWriteHandler writeHandler,
    _In_ XblLocalStorageReadHandler readHandler,
    _In_ XblLocalStorageClearHandler clearHandler,
    _In_opt_ void* context
) XBL_NOEXCEPT;

#endif

}