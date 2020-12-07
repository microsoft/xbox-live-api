// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#if !defined(__cplusplus)
#error C++11 required
#endif

#pragma once

extern "C"
{

/// <summary>
/// Defines the response body type when reading the results of an HTTP call.
/// </summary>
enum class XblHttpCallResponseBodyType : uint32_t
{
    /// <summary>
    /// Read results as string.
    /// </summary>
    String,

    /// <summary>
    /// Read results as vector.
    /// </summary>
    Vector
};

/// <summary>
/// A handle to an HTTP call.
/// </summary>
typedef struct XblHttpCall* XblHttpCallHandle;

// Http APIs
//

/// <summary>
/// Creates an HTTP call handle.
/// </summary>
/// <param name="xblContext">Xbox Live context that provides user context for authorizing the call.</param>
/// <param name="method">UTF-8 encoded method for the HTTP call.</param>
/// <param name="url">UTF-8 encoded URL for the HTTP call.</param>
/// <param name="call">The handle of the HTTP call.</param>
/// <returns>Result code for this API operation.  Possible values are S_OK, E_INVALIDARG, or E_FAIL.</returns>
/// <remarks>
/// First create an HTTP handle using XblHttpCallCreate().  
/// Then call XblHttpCallRequestSet*() to prepare the XblHttpCallHandle.  
/// Then call XblHttpCallPerformAsync() to perform HTTP call using the XblHttpCallHandle.  
/// This call is asynchronous, so the work will be done on a background thread and will return via the callback.  
///
/// The perform call is asynchronous, so the work will be done on a background thread which calls 
/// XTaskQueueDispatch( ..., XTaskQueuePort::Work ).  
///
/// The results will return to the callback on the thread that calls 
/// XTaskQueueDispatch( ..., XTaskQueuePort::Completion ), then get the result of the HTTP call by calling 
/// XblHttpCallResponseGet*() to get the HTTP response of the XblHttpCallHandle.  
/// 
/// When the XblHttpCallHandle is no longer needed, call <see cref="XblHttpCallCloseHandle"/> to free the 
/// memory associated with the XblHttpCallHandle.
/// </remarks>
STDAPI XblHttpCallCreate(
    _In_ XblContextHandle xblContext,
    _In_z_ const char* method,
    _In_z_ const char* url,
    _Out_ XblHttpCallHandle* call
    ) XBL_NOEXCEPT;

/// <summary>
/// Perform an HTTP call using the XblHttpCallHandle.
/// </summary>
/// <param name="call">The handle of the HTTP call.</param>
/// <param name="type">The response body type to read the results of this HTTP call.  
/// Note: this does not influence the content-type header, which must be 
/// supplied by calling <see cref="XblHttpCallRequestSetHeader"/>.</param>
/// <param name="asyncBlock">The XAsyncBlock that defines the async operation.</param>
/// <returns>Result code for this API operation.  Possible values are S_OK, E_INVALIDARG, E_OUTOFMEMORY, or E_FAIL.</returns>
/// <remarks>
/// First create a HTTP handle using XblHttpCallCreate().  
/// Then call XblHttpCallRequestSet*() to prepare the XblHttpCallHandle.  
/// Then call XblHttpCallPerformAsync() to perform HTTP call using the XblHttpCallHandle.  
/// This call is asynchronous, so the work will be done on a background thread and will return via the callback.  
///
/// The perform call is asynchronous, so the work will be done on a background thread which calls 
/// XTaskQueueDispatch( ..., XTaskQueuePort::Work ).  
///
/// The results will return to the callback on the thread that calls 
/// XTaskQueueDispatch( ..., XTaskQueuePort::Completion ), then get the result of the HTTP call by calling 
/// XblHttpCallResponseGet*() to get the HTTP response of the XblHttpCallHandle.  
/// 
/// When the XblHttpCallHandle is no longer needed, call XblHttpCallCloseHandle() to free the 
/// memory associated with the XblHttpCallHandle.  
///
/// XblHttpCallPerformAsync can only be called once.  Create new XblHttpCallHandle to repeat the call.
/// </remarks>
STDAPI XblHttpCallPerformAsync(
    _In_ XblHttpCallHandle call,
    _In_ XblHttpCallResponseBodyType type,
    _Inout_ XAsyncBlock* asyncBlock
    ) XBL_NOEXCEPT;

/// <summary>
/// Duplicates the XblHttpCallHandle object.
/// </summary>
/// <param name="call">The handle of the HTTP call.</param>
/// <param name="duplicateHandle">The duplicated handle.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// Use XblHttpCallCloseHandle to close it.
/// </remarks>
STDAPI XblHttpCallDuplicateHandle(
    _In_ XblHttpCallHandle call,
    _Out_ XblHttpCallHandle* duplicateHandle
    ) XBL_NOEXCEPT;

/// <summary>
/// Decrements the reference count on the call object.
/// </summary>
/// <param name="call">The handle of the HTTP call.</param>
/// <returns></returns>
/// <remarks>
/// When the XblHttpCallHandle ref count is 0, XblHttpCallCloseHandle() will 
/// free the memory associated with the XblHttpCallHandle.
/// </remarks>
STDAPI_(void) XblHttpCallCloseHandle(
    _In_ XblHttpCallHandle call
    ) XBL_NOEXCEPT;

/// <summary>
/// Enables or disables tracing for this specific HTTP call.
/// </summary>
/// <param name="call">The handle of the HTTP call.</param>
/// <param name="traceCall">Trace this call.</param>
/// <returns>Result code for this API operation.  Possible values are S_OK, E_INVALIDARG, or E_FAIL.</returns>
/// <remarks>
/// Defaults to true.
/// </remarks>
STDAPI XblHttpCallSetTracing(
    _In_ XblHttpCallHandle call,
    _In_ bool traceCall
    ) XBL_NOEXCEPT;

/// <summary>
/// Gets the request url for the HTTP call.
/// </summary>
/// <param name="call">The handle of the HTTP call.</param>
/// <param name="url">The UTF-8 encoded url body string of the HTTP call.  
/// The memory for the returned string pointer remains valid for the life of 
/// the XblHttpCallHandle object until XblHttpCallCloseHandle() is called on it.</param>
/// <returns>Result code for this API operation.</returns>
STDAPI XblHttpCallGetRequestUrl(
    _In_ XblHttpCallHandle call,
    _Out_ const char** url
    ) XBL_NOEXCEPT;

/////////////////////////////////////////////////////////////////////////////////////////
// HttpCallRequest Set APIs
//

/// <summary>
/// Set the request body bytes of the HTTP call.
/// </summary> 
/// <param name="call">The handle of the HTTP call.</param>
/// <param name="requestBodyBytes">The request body bytes of the HTTP call.</param>
/// <param name="requestBodySize">The length in bytes of the body being set.</param>
/// <returns>Result code for this API operation.  Possible values are S_OK, E_INVALIDARG, E_OUTOFMEMORY, or E_FAIL.</returns>
/// <remarks>
/// This must be called prior to calling <see cref="XblHttpCallPerformAsync"/>.
/// </remarks>
STDAPI XblHttpCallRequestSetRequestBodyBytes(
    _In_ XblHttpCallHandle call,
    _In_reads_bytes_(requestBodySize) const uint8_t* requestBodyBytes,
    _In_ uint32_t requestBodySize
    ) XBL_NOEXCEPT;

/// <summary>
/// Set the request body string of the HTTP call.
/// </summary> 
/// <param name="call">The handle of the HTTP call.</param>
/// <param name="requestBodyString">The UTF-8 encoded request body string of the HTTP call.</param>
/// <returns>Result code for this API operation.  Possible values are S_OK, E_INVALIDARG, E_OUTOFMEMORY, or E_FAIL.</returns>
/// <remarks>
/// This must be called prior to calling <see cref="XblHttpCallPerformAsync"/>.
/// </remarks>
STDAPI XblHttpCallRequestSetRequestBodyString(
    _In_ XblHttpCallHandle call,
    _In_z_ const char* requestBodyString
    ) XBL_NOEXCEPT;

/// <summary>
/// Set a request header for the HTTP call.
/// </summary>
/// <param name="call">The handle of the HTTP call.</param>
/// <param name="headerName">UTF-8 encoded request header name for the HTTP call.</param>
/// <param name="headerValue">UTF-8 encoded request header value for the HTTP call.</param>
/// <param name="allowTracing">Set to false to skip tracing this request header, for example if it contains private information.</param>
/// <returns>Result code for this API operation.  Possible values are S_OK, E_INVALIDARG, E_OUTOFMEMORY, or E_FAIL.</returns>
/// <remarks>
/// This must be called prior to calling <see cref="XblHttpCallPerformAsync"/>.
/// </remarks>
STDAPI XblHttpCallRequestSetHeader(
    _In_ XblHttpCallHandle call,
    _In_z_ const char* headerName,
    _In_z_ const char* headerValue,
    _In_ bool allowTracing
    ) XBL_NOEXCEPT;

/// <summary>
/// Sets if retry is allowed for this HTTP call.
/// </summary>
/// <param name="call">The handle of the HTTP call. 
/// Pass nullptr to set the default for future calls.</param>
/// <param name="retryAllowed">If retry is allowed for this HTTP call.</param>
/// <returns>Result code for this API operation. Possible values are S_OK, or E_FAIL.</returns>
/// <remarks>
/// Defaults to true.  
/// This must be called prior to calling <see cref="XblHttpCallPerformAsync"/>.
/// </remarks>
STDAPI XblHttpCallRequestSetRetryAllowed(
    _In_ XblHttpCallHandle call,
    _In_ bool retryAllowed
    ) XBL_NOEXCEPT;

/// <summary>
/// ID number of this REST endpoint used to cache the Retry-After header for fast fail.
/// </summary>
/// <param name="call">The handle of the HTTP call.  
/// Pass nullptr to set the default for future calls.</param>
/// <param name="retryAfterCacheId">ID number of this REST endpoint used to cache the Retry-After header for fast fail.  1-1000 are reserved for XSAPI.</param>
/// <returns>Result code for this API operation.  Possible values are S_OK, E_INVALIDARG, or E_FAIL.</returns>
/// <remarks>
/// This must be called prior to calling <see cref="XblHttpCallPerformAsync"/>.
/// </remarks>
STDAPI XblHttpCallRequestSetRetryCacheId(
    _In_ XblHttpCallHandle call,
    _In_ uint32_t retryAfterCacheId
    ) XBL_NOEXCEPT;

/// <summary>
/// Sets a flag which defines the HTTP call as long or not.
/// </summary>
/// <param name="call">The handle of the HTTP call.  
/// Pass nullptr to set the default for future calls.</param>
/// <param name="longHttpCall">The boolean to set the HTTP call to long or not.</param>
/// <returns>Result code for this API operation.  Possible values are S_OK, or E_FAIL.</returns>
/// <remarks>
/// Defaults to false.  
/// This must be called prior to calling <see cref="XblHttpCallPerformAsync"/>.
/// </remarks>
STDAPI XblHttpCallRequestSetLongHttpCall(
    _In_ XblHttpCallHandle call,
    _In_ bool longHttpCall
    ) XBL_NOEXCEPT;

/////////////////////////////////////////////////////////////////////////////////////////
// HttpCallResponse Get APIs
// 

/// <summary>
/// Get the response body string of the HTTP call.
/// </summary>
/// <param name="call">The handle of the HTTP call.</param>
/// <param name="responseString">
/// The UTF-8 encoded response body string of the HTTP call.  
/// The memory for the returned string pointer remains valid for the life of the XblHttpCallHandle object 
/// until XblHttpCallCloseHandle() is called on it.
/// </param>
/// <returns>Result code for this API operation.  Possible values are S_OK, E_INVALIDARG, or E_FAIL.</returns>
/// <remarks>
/// This can only be called after calling <see cref="XblHttpCallPerformAsync"/> when the HTTP task is completed.  
/// This will only be valid if the responsetype passed to PerformAsync was String.
/// </remarks>
STDAPI XblHttpCallGetResponseString(
    _In_ XblHttpCallHandle call,
    _Out_ const char** responseString
    ) XBL_NOEXCEPT;

/// <summary>
/// Get the response body buffer size of the HTTP call.
/// </summary>
/// <param name="call">The handle of the HTTP call.</param>
/// <param name="bufferSize">The response body buffer size of the HTTP call.</param>
/// <returns>Result code for this API operation. Possible values are S_OK, E_INVALIDARG, or E_FAIL.</returns>
/// <remarks>
/// This can only be called after calling <see cref="XblHttpCallPerformAsync"/> when the HTTP task is completed.  
/// This will only be valid if the responsetype passed to PerformAsync was Vector.
/// </remarks>
STDAPI XblHttpCallGetResponseBodyBytesSize(
    _In_ XblHttpCallHandle call,
    _Out_ size_t* bufferSize
    ) XBL_NOEXCEPT;

/// <summary>
/// Get the response body buffer of the HTTP call.
/// </summary>
/// <param name="call">The handle of the HTTP call.</param>
/// <param name="bufferSize">The response body buffer size being passed in.</param>
/// <param name="buffer">The buffer to be written to.</param>
/// <param name="bufferUsed">The actual number of bytes written to the buffer.</param>
/// <returns>Result code for this API operation.  Possible values are S_OK, E_INVALIDARG, or E_FAIL.</returns>
/// <remarks>
/// This can only be called after calling <see cref="XblHttpCallPerformAsync"/> when the HTTP task is completed.  
/// This will only be valid if the responsetype passed to PerformAsync was Vector.
/// </remarks>
STDAPI XblHttpCallGetResponseBodyBytes(
    _In_ XblHttpCallHandle call,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_(bufferSize, *bufferUsed) uint8_t* buffer,
    _Out_opt_ size_t* bufferUsed
    ) XBL_NOEXCEPT;

/// <summary>
/// Get the HTTP status code of the HTTP call response.
/// </summary>
/// <param name="call">The handle of the HTTP call.</param>
/// <param name="statusCode">the HTTP status code of the HTTP call response.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// This can only be called after calling <see cref="XblHttpCallPerformAsync"/> when the HTTP task is completed.
/// </remarks>
STDAPI XblHttpCallGetStatusCode(
    _In_ XblHttpCallHandle call,
    _Out_ uint32_t* statusCode
    ) XBL_NOEXCEPT;

/// <summary>
/// Get the network error code of the HTTP call.
/// </summary>
/// <param name="call">The handle of the HTTP call.</param>
/// <param name="networkErrorCode">The network error code of the HTTP call. Possible values are S_OK, or E_FAIL.</param>
/// <param name="platformNetworkErrorCode">The platform specific network error code of the HTTP call to be used for tracing / debugging.</param>
/// <returns>Result code for this API operation.  Possible values are S_OK, E_INVALIDARG, or E_FAIL.</returns>
/// <remarks>
/// This can only be called after calling <see cref="XblHttpCallPerformAsync"/> when the HTTP task is completed.
/// </remarks>
STDAPI XblHttpCallGetNetworkErrorCode(
    _In_ XblHttpCallHandle call,
    _Out_ HRESULT* networkErrorCode,
    _Out_ uint32_t* platformNetworkErrorCode
    ) XBL_NOEXCEPT;

/// <summary>
/// Get the platform network error message of the HTTP call.
/// </summary>
/// <param name="call">The handle of the HTTP call.</param>
/// <param name="platformNetworkErrorMessage">The platform specific network error message of the HTTP call to be used for tracing / debugging.</param>
/// <returns>Result code for this API operation.  Possible values are S_OK, E_INVALIDARG, or E_FAIL.</returns>
/// <remarks>
/// This can only be called after calling <see cref="XblHttpCallPerformAsync"/> when the HTTP task is completed.
/// </remarks>
STDAPI XblHttpCallGetPlatformNetworkErrorMessage(
    _In_ XblHttpCallHandle call,
    _Out_ const char** platformNetworkErrorMessage
    ) XBL_NOEXCEPT;

/// <summary>
/// Get a response header for the HTTP call for a given header name.
/// </summary>
/// <param name="call">The handle of the HTTP call.</param>
/// <param name="headerName">UTF-8 encoded response header name for the HTTP call.  
/// The memory for the returned string pointer remains valid for the life of 
/// the XblHttpCallHandle object until XblHttpCallCloseHandle() is called on it.</param>
/// <param name="headerValue">UTF-8 encoded response header value for the HTTP call.  
/// Returns nullptr if the header doesn't exist.  
/// The memory for the returned string pointer remains valid for the life of 
/// the XblHttpCallHandle object until XblHttpCallCloseHandle() is called on it.</param>
/// <returns>Result code for this API operation. Possible values are S_OK, E_INVALIDARG, or E_FAIL.</returns>
/// <remarks>
/// This can only be called after calling <see cref="XblHttpCallPerformAsync"/> when the HTTP task is completed.
/// </remarks>
STDAPI XblHttpCallGetHeader(
    _In_ XblHttpCallHandle call,
    _In_z_ const char* headerName,
    _Out_ const char** headerValue
    ) XBL_NOEXCEPT;

/// <summary>
/// Gets the number of response headers in the HTTP call.
/// </summary>
/// <param name="call">The handle of the HTTP call.</param>
/// <param name="numHeaders">The number of response headers in the HTTP call.</param>
/// <returns>Result code for this API operation.  Possible values are S_OK, E_INVALIDARG, or E_FAIL.</returns>
/// <remarks>
/// This can only be called after calling <see cref="XblHttpCallPerformAsync"/> when the HTTP task is completed.
/// </remarks>
STDAPI XblHttpCallGetNumHeaders(
    _In_ XblHttpCallHandle call,
    _Out_ uint32_t* numHeaders
    ) XBL_NOEXCEPT;

/// <summary>
/// Gets the response headers at specific zero based index in the HTTP call.
/// </summary>
/// <param name="call">The handle of the HTTP call.</param>
/// <param name="headerIndex">Specific zero based index of the response header.</param>
/// <param name="headerName">UTF-8 encoded response header name for the HTTP call.  
/// The memory for the returned string pointer remains valid for the life of 
/// the XblHttpCallHandle object until XblHttpCallCloseHandle() is called on it.</param>
/// <param name="headerValue">UTF-8 encoded response header value for the HTTP call.  
/// The memory for the returned string pointer remains valid for the life of 
/// the XblHttpCallHandle object until XblHttpCallCloseHandle() is called on it.</param>
/// <returns>Result code for this API operation.  Possible values are S_OK, E_INVALIDARG, or E_FAIL.</returns>
/// <remarks>
/// This can only be called after calling <see cref="XblHttpCallPerformAsync"/> when the HTTP task is completed.  
/// Use <see cref="XblHttpCallGetNumHeaders"/> to know how many response headers there are in the HTTP call.
/// </remarks>
STDAPI XblHttpCallGetHeaderAtIndex(
    _In_ XblHttpCallHandle call,
    _In_ uint32_t headerIndex,
    _Out_ const char** headerName,
    _Out_ const char** headerValue
    ) XBL_NOEXCEPT;
}