// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include <stdint.h>
#include "types_c.h"

#if defined(__cplusplus)
extern "C" {
#endif

/// <summary>
/// Represents the configuration of an Xbox Live application.
/// </summary>
typedef struct XBL_XBOX_LIVE_APP_CONFIG
{
    /// <summary>
    /// Title id of the app.
    /// </summary>
    uint32_t titleId;

    /// <summary>
    /// Service config id of the app.
    /// </summary>
    PCSTR scid;

    /// <summary>
    /// Xbox Live environment being used, it is empty before you sign in or using production.
    /// </summary>
    PCSTR environment;

    /// <summary>
    /// The sandbox such as "XDKS.1", it is empty until you sign in.
    /// </summary>
    PCSTR sandbox;
} XBL_XBOX_LIVE_APP_CONFIG;

/// <summary>
/// Gets the app config singleton.
/// </summary>
XBL_API XBL_RESULT XBL_CALLING_CONV
XblGetXboxLiveAppConfigSingleton(
    _Out_ CONST XBL_XBOX_LIVE_APP_CONFIG** appConfig
    );

#if defined(__cplusplus)
} // end extern "C"
#endif // defined(__cplusplus)