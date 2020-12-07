// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#if !defined(__cplusplus)
    #error C++11 required
#endif

#pragma once

extern "C"
{

/// <summary>
/// Enumeration values that define the Xbox Live API error conditions.
/// </summary>
/// <remarks>
/// A best practice is to test the returned HRESULT against these error conditions using <see cref="XblGetErrorCondition"/>.
/// </remarks>
enum class XblErrorCondition : uint32_t
{
    /// <summary>
    /// No error.
    /// </summary>
    NoError = 0,

    /// <summary>
    /// A generic error condition.
    /// </summary>
    GenericError,

    /// <summary>
    /// An error condition related to an object being out of range.
    /// </summary>
    GenericOutOfRange,

    /// <summary>
    /// An error condition related to attempting to authenticate.
    /// </summary>
    Auth,

    /// <summary>
    /// An error condition related to network connectivity.
    /// </summary>
    Network,

    /// <summary>
    /// An error condition related to an HTTP method call.
    /// </summary>
    HttpGeneric,

    /// <summary>
    /// The requested resource was not modified.
    /// </summary>
    Http304NotModified,

    /// <summary>
    /// The requested resource was not found.
    /// </summary>
    Http404NotFound,

    /// <summary>
    /// The precondition given in one or more of the request-header fields evaluated
    /// to false when it was tested on the server.
    /// </summary>
    Http412PreconditionFailed,

    /// <summary>
    /// Client is sending too many requests
    /// </summary>
    Http429TooManyRequests,
    
    /// <summary>
    /// The service timed out while attempting to process the request.
    /// </summary>
    HttpServiceTimeout,

    /// <summary>
    /// An error related to real time activity.
    /// </summary>
    Rta
};

/// <summary>
/// Groups HRESULT values returned from Xbl APIs in to error condition buckets that are actionable.
/// </summary>
/// <param name="hr">HRESULT value returned from an Xbl API.</param>
/// <returns>The corresponding XblErrorCondition.</returns>
STDAPI_(XblErrorCondition) XblGetErrorCondition(
    _In_ HRESULT hr
    ) XBL_NOEXCEPT;

}