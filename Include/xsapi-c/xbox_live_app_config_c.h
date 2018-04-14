// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include <stdint.h>
#include "pal.h"

#if defined(__cplusplus)
extern "C" {
#endif

/// <summary>
/// Represents the configuration of an Xbox Live application.
/// </summary>
typedef struct XblAppConfig
{
    /// <summary>
    /// Title id of the app.
    /// </summary>
    uint32_t titleId;

    /// <summary>
    /// Service config id of the app.
    /// </summary>
    UTF8CSTR scid;

    /// <summary>
    /// Xbox Live environment being used, it is empty before you sign in or using production.
    /// </summary>
    UTF8CSTR environment;

    /// <summary>
    /// The sandbox such as "XDKS.1", it is empty until you sign in.
    /// </summary>
    UTF8CSTR sandbox;
} XblAppConfig;

/// <summary>
/// Gets the Xbox Live app config 
/// </summary>
STDAPI
XblGetXboxLiveAppConfig(
    _Out_ const XblAppConfig** appConfig
    );

#if defined(__cplusplus)
} // end extern "C"
#endif // defined(__cplusplus)