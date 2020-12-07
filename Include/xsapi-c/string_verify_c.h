// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#if !defined(__cplusplus)
#error C++11 required
#endif

#pragma once

extern "C"
{

/// <summary>
/// Enumeration values that indicate the result code from string verification.
/// </summary>
/// <remarks>
/// These values are defined on the service side and should not be modified.
/// </remarks>
/// <argof><see cref="XblVerifyStringResult"/></argof>
enum class XblVerifyStringResultCode
{
    /// <summary>
    /// No issues were found with the string.
    /// </summary>
    Success,

    /// <summary>
    /// The string contains offensive content.
    /// </summary>
    Offensive,

    /// <summary>
    /// The string is too long to verify.
    /// </summary>
    TooLong,

    /// <summary>
    /// An unknown error was encountered during string verification.
    /// </summary>
    UnknownError
};

/// <summary>
/// Contains information about the results of a string verification.
/// </summary>
/// <argof><see cref="XblStringVerifyStringResult"/></argof>
/// <argof><see cref="XblStringVerifyStringsResult"/></argof>
typedef struct XblVerifyStringResult
{
    /// <summary>
    /// The result code for the string verification.
    /// </summary>
    XblVerifyStringResultCode resultCode;

    /// <summary>
    /// Contains the first offending substring if the
    /// resultCode is XblVerifyStringResultCode::Offensive.
    /// </summary>
    char* firstOffendingSubstring;
} XblVerifyStringResult;

/// <summary>
/// Verifies if a string contains acceptable text for use with Xbox Live.
/// </summary>
/// <param name="xboxLiveContext">Xbox live context for the local user.</param>
/// <param name="stringToVerify">The string to verify.</param>
/// <param name="asyncBlock">The AsyncBlock for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Call <see cref="XblStringVerifyStringResultSize"/> to retrieve size of buffer needed for result.  
/// Call <see cref="XblStringVerifyStringResult"/> to retrieve the size of the results.
/// </remarks>
STDAPI XblStringVerifyStringAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_ const char* stringToVerify,
    _In_ XAsyncBlock* asyncBlock
) XBL_NOEXCEPT;

/// <summary>
/// Get the result size for an XblStringVerifyStringAsync call.
/// </summary>
/// <param name="asyncBlock">The AsyncBlock for this operation.</param>
/// <param name="resultSizeInBytes">Passes back the size in bytes required to store the XblVerifyStringResult result.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblStringVerifyStringResultSize(
    _In_ XAsyncBlock* asyncBlock,
    _Out_ size_t* resultSizeInBytes
) XBL_NOEXCEPT;

/// <summary>
/// Get the result for an XblStringVerifyStringAsync call.
/// </summary>
/// <param name="asyncBlock">The AsyncBlock for this operation.</param>
/// <param name="bufferSize">The size of the provided buffer.  
/// Use <see cref="XblStringVerifyStringResultSize"/> to get the size required.</param>
/// <param name="buffer">A caller allocated byte buffer that passes back the verify string result.</param>
/// <param name="ptrToBuffer">Passes back a strongly typed pointer that points into buffer.  
/// Do not free this as its lifecycle is tied to buffer.</param>
/// <param name="bufferUsed">Passes back the number of bytes written to the buffer.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblStringVerifyStringResult(
    _In_ XAsyncBlock* asyncBlock,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_(bufferSize, *bufferUsed) void* buffer,
    _Outptr_ XblVerifyStringResult** ptrToBuffer,
    _Out_opt_ size_t* bufferUsed
) XBL_NOEXCEPT;

/// <summary>
/// Verifies a collection of strings to see if each string contains acceptable text for use with Xbox Live.
/// </summary>
/// <param name="xboxLiveContext">Xbox live context for the local user.</param>
/// <param name="stringsToVerify">The string to verify.</param>
/// <param name="stringsCount">The number of strings being verified.</param>
/// <param name="asyncBlock">The AsyncBlock for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Call <see cref="XblStringVerifyStringsResultSize"/> to retrieve size of buffer needed for result.  
/// Call <see cref="XblStringVerifyStringsResult"/> to retrieve the size of the results.
///</remarks>
STDAPI XblStringVerifyStringsAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_ const char** stringsToVerify,
    _In_ const uint64_t stringsCount,
    _In_ XAsyncBlock* asyncBlock
) XBL_NOEXCEPT;

/// <summary>
/// Get the result size for an XblStringVerifyStringsAsync call.
/// </summary>
/// <param name="asyncBlock">The AsyncBlock for this operation.</param>
/// <param name="resultSizeInBytes">Passes back the size in bytes required to store the XblVerifyStringResult results.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblStringVerifyStringsResultSize(
    _In_ XAsyncBlock* asyncBlock,
    _Out_ size_t* resultSizeInBytes
) XBL_NOEXCEPT;

/// <summary>
/// Get the result for an XblStringVerifyStringsAsync call.
/// </summary>
/// <param name="asyncBlock">The AsyncBlock for this operation.</param>
/// <param name="bufferSize">The size of the provided buffer.  
/// Call <see cref="XblStringVerifyStringsResultSize"/> to get the size required.</param>
/// <param name="buffer">A caller allocated byte buffer that passes back the verify strings result.</param>
/// <param name="ptrToBufferStrings">Passes back a strongly typed array of XblVerifyStringResult that points into buffer.  
/// Do not free this as its lifecycle is tied to buffer.</param>
/// <param name="stringsCount">Passes back the number of XblVerifyStringResult results.</param>
/// <param name="bufferUsed">Passes back the number of bytes written to the buffer.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblStringVerifyStringsResult(
    _In_ XAsyncBlock* asyncBlock,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_(bufferSize, *bufferUsed) void* buffer,
    _Outptr_ XblVerifyStringResult** ptrToBufferStrings,
    _Out_ size_t* stringsCount,
    _Out_opt_ size_t* bufferUsed
) XBL_NOEXCEPT;

} // end extern C