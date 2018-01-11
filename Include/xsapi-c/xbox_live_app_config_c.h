// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include <stdint.h>
#include "types_c.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct XSAPI_XBOX_LIVE_APP_CONFIG
{
    uint32_t titleId;
    PCSTR scid;
    PCSTR environment;
    PCSTR sandbox;
} XSAPI_XBOX_LIVE_APP_CONFIG;

XBL_API XSAPI_RESULT XBL_CALLING_CONV
GetXboxLiveAppConfigSingleton(
    _Out_ CONST XSAPI_XBOX_LIVE_APP_CONFIG** ppConfig
    );

#if defined(__cplusplus)
} // end extern "C"
#endif // defined(__cplusplus)