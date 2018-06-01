// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

/// <summary>
/// Enumeration values that define the Xbox Live API error conditions.
/// </summary>
/// <remarks>
/// A best practice is to test the returned HRESULT against these error conditions using
/// </remarks>
typedef enum XblErrorCondition
{
    /// <summary>
    /// No error.
    /// </summary>
    XblErrorCondition_NoError = 0,

    /// <summary>
    /// A generic error condition.
    /// </summary>
    XblErrorCondition_GenericError,

    /// <summary>
    /// An error condition related to an object being out of range.
    /// </summary>
    XblErrorCondition_GenericOutOfRange,

    /// <summary>
    /// An error condition related to attempting to authenticate.
    /// </summary>
    XblErrorCondition_Auth,

    /// <summary>
    /// An error condition related to network connectivity.
    /// </summary>
    XblErrorCondition_Network,

    /// <summary>
    /// An error condition related to an HTTP method call.
    /// </summary>
    XblErrorCondition_HttpGeneric,

    /// <summary>
    /// The requested resource was not found.
    /// </summary>
    XblErrorCondition_Http404NotFound,

    /// <summary>
    /// The precondition given in one or more of the request-header fields evaluated
    /// to false when it was tested on the server.
    /// </summary>
    XblErrorCondition_Http412PreconditionFailed,

    /// <summary>
    /// Client is sending too many requests
    /// </summary>
    XblErrorCondition_Http429TooManyRequests,
    
    /// <summary>
    /// The service timed out while attempting to process the request.
    /// </summary>
    XblErrorCondition_HttpServiceTimeout,

    /// <summary>
    /// An error related to real time activity.
    /// </summary>
    XblErrorCondition_Rta
} XblErrorCondition;

/// <summary>
/// Groups HRESULT values returned from Xbl APIs in to error condition buckets that are actionable.
/// </summary>
/// <param name="hr">HRESULT value returned from an Xbl API.</param>
/// <returns>The corresponding XblErrorCondition.</returns>
STDAPI_(XblErrorCondition) XblGetErrorCondition(
    _In_ HRESULT hr
    ) XBL_NOEXCEPT;
