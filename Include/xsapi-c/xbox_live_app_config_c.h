// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

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
    /// UTF-8 encoded service config id of the app.
    /// </summary>
    _Field_z_ const char* scid;

    /// <summary>
    /// UTF-8 encoded Xbox Live environment being used, it is empty before you sign in or using production.
    /// </summary>
    _Field_z_ const char* environment;

    /// <summary>
    /// The UTF-8 encoded sandbox such as "XDKS.1", it is empty until you sign in.
    /// </summary>
    _Field_z_ const char* sandbox;
} XblAppConfig;

/// <summary>
/// Gets the Xbox Live app config 
/// </summary>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblGetXboxLiveAppConfig(
    _Out_ const XblAppConfig** appConfig
    );
