// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "types_c.h"

#if defined(__cplusplus)
extern "C" {
#endif

// TODO: rename to XSAPI_ERROR_CONDITION
typedef enum XBL_RESULT
{
    XBL_RESULT_OK = 0,
    XBL_RESULT_E_INVALIDARG = 1,
    XBL_RESULT_E_ALREADYINITIALISED = 2,

    // TODO: remove these generic errors and into platformSpecificErrorCode below
    //////////////////////////////////////////////////////////////////////////
    // Generic errors
    //////////////////////////////////////////////////////////////////////////
    XBL_RESULT_E_HC_FAIL = -1,
    XBL_RESULT_E_HC_POINTER = -2,
    XBL_RESULT_E_HC_INVALIDARG = -3,
    XBL_RESULT_E_HC_OUTOFMEMORY = -4,
    XBL_RESULT_E_HC_BUFFERTOOSMALL = -5,
    XBL_RESULT_E_HC_NOTINITIALIZED = -6,
    XBL_RESULT_E_HC_FEATURENOTPRESENT = -7,
    XBL_RESULT_E_HC_PERFORMALREADYCALLED = -8,
    XBL_RESULT_E_HC_ALREADYINITIALISED = -9,

    //////////////////////////////////////////////////////////////////////////
    // XSAPI error conditions
    //////////////////////////////////////////////////////////////////////////
    XBL_RESULT_E_GENERIC_ERROR = -1000,
    XBL_RESULT_E_OUT_OF_RANGE = -1001,
    XBL_RESULT_E_AUTH = -1002,
    XBL_RESULT_E_NETWORK = -1003,
    XBL_RESULT_E_HTTP = -1004,
    XBL_RESULT_E_HTTP_404_NOT_FOUND = -1005,
    XBL_RESULT_E_HTTP_412_PRECONDITION_FAILED = -1006,
    XBL_RESULT_E_HTTP_429_TOO_MANY_REQUESTS = -1007,
    XBL_RESULT_E_HTTP_SERVICE_TIMEOUT = -1008,
    XBL_RESULT_E_RTA = -1008

} XBL_RESULT;

typedef struct XBL_RESULT_INFO
{
    // TODO: add platformSpecificErrorCode and change errorCode to errorCondition
    XBL_RESULT errorCode;
    PCSTR errorMessage;
} XBL_RESULT_INFO;

#if defined(__cplusplus)
} // end extern "C"
#endif // defined(__cplusplus)