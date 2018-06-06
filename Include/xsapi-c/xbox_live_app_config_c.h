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
    ) XBL_NOEXCEPT;

/// <summary>
/// Enum used with XblContextSettings APIs
/// </summary>
typedef enum XblConfigSetting
{
    /// <summary>
    /// Only passed to the below API's to warn code reviewers that 
    /// there's an outstanding Xbox Live calling pattern issue that needs to be addressed.
    /// </summary>
    XblConfigSetting_ThisCodeNeedsToBeChanged
} XblConfigSetting;

/// <summary>
/// Disables asserts for Xbox Live throttling in dev sandboxes.
/// The asserts will not fire in RETAIL sandbox, and this setting has has no affect in RETAIL sandboxes.
/// It is best practice to not call this API, and instead adjust the calling pattern but this is provided
/// as a temporary way to get unblocked while in early stages of game development.
/// </summary>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblDisableAssertsForXboxLiveThrottlingInDevSandboxes(
    _In_ XblConfigSetting setting
    ) XBL_NOEXCEPT;

/// <summary>
/// Disables asserts for having maximum number of websockets being activated 
/// i.e. XBL_MAXIMUM_WEBSOCKETS_ACTIVATIONS_ALLOWED_PER_USER (5) per user per title instance.
/// It is best practice to not call this API, and instead adjust the calling pattern but this is provided
/// as a temporary way to get unblocked while in early stages of game development.
/// </summary>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblDisableAssertsForMaximumNumberOfWebsocketsActivated(
    _In_ XblConfigSetting setting
    ) XBL_NOEXCEPT;
