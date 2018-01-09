// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "types_c.h"

#if defined(__cplusplus)
extern "C" {
#endif

// TODO: rename to XSAPI_ERROR_CONDITION
typedef enum XSAPI_RESULT
{
    XSAPI_RESULT_OK = 0,

    // TODO: remove these generic errors and into platformSpecificErrorCode below
    //////////////////////////////////////////////////////////////////////////
    // Generic errors
    //////////////////////////////////////////////////////////////////////////
    XSAPI_RESULT_E_HC_FAIL = -1,
    XSAPI_RESULT_E_HC_POINTER = -2,
    XSAPI_RESULT_E_HC_INVALIDARG = -3,
    XSAPI_RESULT_E_HC_OUTOFMEMORY = -4,
    XSAPI_RESULT_E_HC_BUFFERTOOSMALL = -5,
    XSAPI_RESULT_E_HC_NOTINITIALIZED = -6,
    XSAPI_RESULT_E_HC_FEATURENOTPRESENT = -7,

    //////////////////////////////////////////////////////////////////////////
    // XSAPI error conditions
    //////////////////////////////////////////////////////////////////////////
    XSAPI_RESULT_E_GENERIC_ERROR = -1000,
    XSAPI_RESULT_E_OUT_OF_RANGE = -1001,
    XSAPI_RESULT_E_AUTH = -1002,
    XSAPI_RESULT_E_NETWORK = -1003,
    XSAPI_RESULT_E_HTTP = -1004,
    XSAPI_RESULT_E_HTTP_404_NOT_FOUND = -1005,
    XSAPI_RESULT_E_HTTP_412_PRECONDITION_FAILED = -1006,
    XSAPI_RESULT_E_HTTP_429_TOO_MANY_REQUESTS = -1007,
    XSAPI_RESULT_E_HTTP_SERVICE_TIMEOUT = -1008,
    XSAPI_RESULT_E_RTA = -1008

} XSAPI_RESULT;

typedef struct XSAPI_RESULT_INFO
{
    // TODO: add platformSpecificErrorCode and change errorCode to errorCondition
    XSAPI_RESULT errorCode;
    PCSTR errorMessage;
} XSAPI_RESULT_INFO;

#if defined(__cplusplus)
} // end extern "C"
#endif // defined(__cplusplus)