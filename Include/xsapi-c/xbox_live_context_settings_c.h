// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#define XBL_MAXIMUM_WEBSOCKETS_ACTIVATIONS_ALLOWED_PER_USER 5

/// <summary>
/// Gets the HTTP retry delay in seconds.
///
/// Retries are delayed using a exponential back off.  By default, it will delay 2 seconds then the 
/// next retry will delay 4 seconds, then 8 seconds, and so on up to a max of 1 min until either
/// the call succeeds or the http_timeout_window is reached, at which point the call will fail.
/// The delay is also jittered between the current and next delay to spread out service load.
/// The default for http_timeout_window is 20 seconds and can be changed using XblContextSettingsSetHttpTimeoutWindow
/// 
/// If the service returns an an HTTP error with a "Retry-After" header, then all future calls to that API 
/// will immediately fail with the original error without contacting the service until the "Retry-After" 
/// time has been reached.
///
/// Idempotent service calls are retried when a network error occurs or the server responds with one of these HTTP status codes:
/// 408 (Request Timeout)
/// 429 (Too Many Requests)
/// 500 (Internal Server Error)
/// 502 (Bad Gateway)
/// 503 (Service Unavailable)
/// 504 (Gateway Timeout)
/// </summary>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblContextSettingsGetHttpRetryDelay(
    _In_ xbl_context_handle context,
    _Out_ uint64_t* delayInSeconds
    ) XBL_NOEXCEPT;

/// <summary>
/// Sets the HTTP retry delay in seconds. The default and minimum delay is 2 seconds.
/// 
/// Retries are delayed using a exponential back off.  By default, it will delay 2 seconds then the 
/// next retry will delay 4 seconds, then 8 seconds, and so on up to a max of 1 min until either
/// the call succeeds or the http_timeout_window is reached, at which point the call will fail.
/// The delay is also jittered between the current and next delay to spread out service load.
/// The default for http_timeout_window is 20 seconds and can be changed using XblContextSettingsSetHttpTimeoutWindow
/// 
/// If the service returns an an HTTP error with a "Retry-After" header, then all future calls to that API 
/// will immediately fail with the original error without contacting the service until the "Retry-After" 
/// time has been reached.
///
/// Idempotent service calls are retried when a network error occurs or the server responds with one of these HTTP status codes:
/// 408 (Request Timeout)
/// 429 (Too Many Requests)
/// 500 (Internal Server Error)
/// 502 (Bad Gateway)
/// 503 (Service Unavailable)
/// 504 (Gateway Timeout)
/// </summary>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblContextSettingsSetHttpRetryDelay(
    _In_ xbl_context_handle context,
    _In_ uint64_t delayInSeconds
    ) XBL_NOEXCEPT;

/// <summary>
/// Gets the HTTP timeout window in seconds.
///
/// This controls how long to spend attempting to retry idempotent service calls before failing.
/// The default is 20 seconds
///
/// Idempotent service calls are retried when a network error occurs or the server responds with one of these HTTP status codes:
/// 408 (Request Timeout)
/// 429 (Too Many Requests)
/// 500 (Internal Server Error)
/// 502 (Bad Gateway)
/// 503 (Service Unavailable)
/// 504 (Gateway Timeout)
/// </summary>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblContextSettingsGetHttpTimeoutWindow(
    _In_ xbl_context_handle context,
    _Out_ uint64_t* timeoutWindowInSeconds
    ) XBL_NOEXCEPT;

/// <summary>
/// Sets the HTTP timeout window in seconds.
///
/// This controls how long to spend attempting to retry idempotent service calls before failing.
/// The default is 20 seconds.  Set to 0 to turn off retry.
///
/// Idempotent service calls are retried when a network error occurs or the server responds with one of these HTTP status codes:
/// 408 (Request Timeout)
/// 429 (Too Many Requests)
/// 500 (Internal Server Error)
/// 502 (Bad Gateway)
/// 503 (Service Unavailable)
/// 504 (Gateway Timeout)
/// </summary>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblContextSettingsSetHttpTimeoutWindow(
    _In_ xbl_context_handle context,
    _In_ uint64_t timeoutWindowInSeconds
    ) XBL_NOEXCEPT;

/// <summary>
/// Gets the web socket timeout window in seconds.
/// </summary>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblContextSettingsGetWebsocketTimeoutWindow(
    _In_ xbl_context_handle context,
    _Out_ uint64_t* timeoutWindowInSeconds
    ) XBL_NOEXCEPT;

/// <summary>
/// Sets the web socket timeout window in seconds.
/// Controls how long to spend attempting to retry establishing a websocket connection before failing. 
/// Default is 300 seconds.  Set to 0 to turn off retry.
/// </summary>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblContextSettingsSetWebsocketTimeoutWindow(
    _In_ xbl_context_handle context,
    _In_ uint64_t timeoutWindowInSeconds
    ) XBL_NOEXCEPT;
