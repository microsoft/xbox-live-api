// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "pal.h"

/// <summary>
/// Enumeration values that define the Xbox Live API error conditions.
/// </summary>
/// <remarks>
/// A best practice is to test the returned HRESULT against these error conditions using
/// For more detail about std::error_code vs std::error_condition, see 
/// http://en.cppreference.com/w/cpp/error/error_condition 
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
    XblErrorCondition_GenericError = 1,

    /// <summary>
    /// An error condition related to an object being out of range.
    /// </summary>
    XblErrorCondition_GenericOutOfRange = 2,

    /// <summary>
    /// An error condition related to attempting to authenticate.
    /// </summary>
    XblErrorCondition_Auth = 3,

    /// <summary>
    /// An error condition related to network connectivity.
    /// </summary>
    XblErrorCondition_Network = 4,

    /// <summary>
    /// An error condition related to an HTTP method call.
    /// </summary>
    XblErrorCondition_Http = 5,

    // SHIPTODO add these and rename XblErrorCondition_Http to XblErrorCondition_Http_Generic
    ///// <summary>
    ///// The requested resource was not found.
    ///// </summary>
    //XBL_ERROR_CONDITION_HTTP_404_NOT_FOUND,

    ///// <summary>
    ///// The precondition given in one or more of the request-header fields evaluated
    ///// to false when it was tested on the server.
    ///// </summary>
    //XBL_ERROR_CONDITION_HTTP_412_PRECONDITION_FAILED,

    ///// <summary>
    ///// Client is sending too many requests
    ///// </summary>
    //XBL_ERROR_CONDITION_HTTP_429_TOO_MANY_REQUESTS,

    ///// <summary>
    ///// The service timed out while attempting to process the request.
    ///// </summary>
    //XBL_ERROR_CONDITION_HTTP_SERVICE_TIMEOUT,


    /// <summary>
    /// An error related to real time activity.
    /// </summary>
    XblErrorCondition_Rta = 10
} XblErrorCondition;

/// <summary>
/// Groups HRESULT values returned from Xbl APIs in to error condition buckets that are actionable.
/// </summary>
/// <param name="hr">HRESULT value returned from an Xbl API.</param>
/// <returns>The corresponding XblErrorCondition.</returns>
STDAPI_(XblErrorCondition) XblGetErrorCondition(
    _In_ HRESULT hr
    ) XBL_NOEXCEPT;
