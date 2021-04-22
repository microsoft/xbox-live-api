// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#if !defined(__cplusplus)
   #error C++11 required
#endif

#pragma once

extern "C"
{

/// <summary>
/// Gets the connect, send, and receive timeouts for HTTP socket operations of long calls such as Title Storage calls.
/// </summary>
/// <param name="context">Xbox live context that the settings are associated with.</param>
/// <param name="timeoutInSeconds">Passes back the timeout for long HTTP calls in seconds.  
/// Default is 5 minutes (300 seconds).  </param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Calls that take longer than the given timeout are aborted.
/// </remarks>
STDAPI XblContextSettingsGetLongHttpTimeout(
    _In_ XblContextHandle context,
    _Out_ uint32_t* timeoutInSeconds
) XBL_NOEXCEPT;

/// <summary>
/// Sets the connect, send, and receive timeouts for HTTP socket operations of long calls such as Title Storage calls.
/// </summary>
/// <param name="context">Xbox live context that the settings are associated with.</param>
/// <param name="timeoutInSeconds">The timeout for long HTTP calls in seconds.  
/// Default is 5 minutes (300 seconds).</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Calls that take longer than the given timeout are aborted.  
/// Take care when setting this to smaller values as some calls like Title Storage may take a few minutes to complete.
/// </remarks>
STDAPI XblContextSettingsSetLongHttpTimeout(
    _In_ XblContextHandle context,
    _In_ uint32_t timeoutInSeconds
) XBL_NOEXCEPT;

/// <summary>
/// Gets the HTTP retry delay in seconds.
/// </summary>
/// <param name="context">Xbox live context that the settings are associated with.</param>
/// <param name="delayInSeconds">Passes back the retry delay in seconds.  
/// Retries are delayed using an exponential back off.  
/// By default, it will delay 2 seconds then the next retry will delay 4 seconds, then 8 seconds, 
/// and so on up to a max of 1 min until either the call succeeds or the http_timeout_window 
/// is reached, at which point the call will fail.  
/// The delay is also jittered between the current and next delay to spread out service load.  
/// The default for http_timeout_window is 20 seconds and can be changed using XblContextSettingsSetHttpTimeoutWindow.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// If the service returns an HTTP error with a "Retry-After" header, then all future calls to that API will 
/// immediately fail with the original error without contacting the service until the "Retry-After" time has been reached.  
/// <br/>
/// Idempotent service calls are retried when a network error occurs or the server responds with one of these HTTP status codes:<br/>
/// 408 (Request Timeout)<br/>
/// 429 (Too Many Requests)<br/>
/// 500 (Internal Server Error)<br/>
/// 502 (Bad Gateway)<br/>
/// 503 (Service Unavailable)<br/>
/// 504 (Gateway Timeout)<br/>
/// </remarks>
STDAPI XblContextSettingsGetHttpRetryDelay(
    _In_ XblContextHandle context,
    _Out_ uint32_t* delayInSeconds
) XBL_NOEXCEPT;

/// <summary>
/// Sets the HTTP retry delay in seconds.
/// </summary>
/// <param name="context">Xbox live context that the settings are associated with.</param>
/// <param name="delayInSeconds">The retry delay in seconds.  
/// Retries are delayed using an exponential back off.  
/// By default, it will delay 2 seconds then the next retry will delay 4 seconds, then 8 seconds, 
/// and so on up to a max of 1 min until either the call succeeds or the http_timeout_window 
/// is reached, at which point the call will fail.  
/// The delay is also jittered between the current and next delay to spread out service load.  
/// The default for http_timeout_window is 20 seconds and can be changed using XblContextSettingsSetHttpTimeoutWindow.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// If the service returns an HTTP error with a "Retry-After" header, then all future calls to that API will 
/// immediately fail with the original error without contacting the service until the "Retry-After" time has been reached.  
/// <br/>
/// Idempotent service calls are retried when a network error occurs or the server responds with one of these HTTP status codes:<br/>
/// 408 (Request Timeout)<br/>
/// 429 (Too Many Requests)<br/>
/// 500 (Internal Server Error)<br/>
/// 502 (Bad Gateway)<br/>
/// 503 (Service Unavailable)<br/>
/// 504 (Gateway Timeout)<br/>
/// </remarks>
STDAPI XblContextSettingsSetHttpRetryDelay(
    _In_ XblContextHandle context,
    _In_ uint32_t delayInSeconds
) XBL_NOEXCEPT;

/// <summary>
/// Gets the HTTP timeout window in seconds.
/// </summary>
/// <param name="context">Xbox live context that the settings are associated with.</param>
/// <param name="timeoutWindowInSeconds">Passes back the timeout window in seconds.  
/// The default is 20 seconds.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// This controls how long to spend attempting to retry idempotent service calls before failing.<br/>
/// <br/>
/// Idempotent service calls are retried when a network error occurs or the server responds with one of these HTTP status codes:<br/>
/// 408 (Request Timeout)<br/>
/// 429 (Too Many Requests)<br/>
/// 500 (Internal Server Error)<br/>
/// 502 (Bad Gateway)<br/>
/// 503 (Service Unavailable)<br/>
/// 504 (Gateway Timeout)<br/>
/// </remarks>
STDAPI XblContextSettingsGetHttpTimeoutWindow(
    _In_ XblContextHandle context,
    _Out_ uint32_t* timeoutWindowInSeconds
) XBL_NOEXCEPT;

/// <summary>
/// Sets the HTTP timeout window in seconds.
/// </summary>
/// <param name="context">Xbox live context that the settings are associated with.</param>
/// <param name="timeoutWindowInSeconds">The timeout window in seconds.  
/// The default is 20 seconds.  
/// Set to 0 to turn off retry.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// This controls how long to spend attempting to retry idempotent service calls before failing.<br/>
/// <br/>
/// Idempotent service calls are retried when a network error occurs or the server responds with one of these HTTP status codes:<br/>
/// 408 (Request Timeout)<br/>
/// 429 (Too Many Requests)<br/>
/// 500 (Internal Server Error)<br/>
/// 502 (Bad Gateway)<br/>
/// 503 (Service Unavailable)<br/>
/// 504 (Gateway Timeout)<br/>
/// </remarks>
STDAPI XblContextSettingsSetHttpTimeoutWindow(
    _In_ XblContextHandle context,
    _In_ uint32_t timeoutWindowInSeconds
) XBL_NOEXCEPT;

/// <summary>
/// Gets the web socket timeout window in seconds.
/// </summary>
/// <param name="context">Xbox live context that the settings are associated with.</param>
/// <param name="timeoutWindowInSeconds">Passes back the timeout window in seconds.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblContextSettingsGetWebsocketTimeoutWindow(
    _In_ XblContextHandle context,
    _Out_ uint32_t* timeoutWindowInSeconds
) XBL_NOEXCEPT;

/// <summary>
/// Sets the web socket timeout window in seconds.
/// </summary>
/// <param name="context">Xbox live context that the settings are associated with.</param>
/// <param name="timeoutWindowInSeconds">The timeout window in seconds.  
/// Default is 300 seconds.  
/// Set to 0 to turn off retry.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Controls how long to spend attempting to retry establishing a web socket connection before failing.
/// </remarks>
STDAPI XblContextSettingsSetWebsocketTimeoutWindow(
    _In_ XblContextHandle context,
    _In_ uint32_t timeoutWindowInSeconds
) XBL_NOEXCEPT;

// TODO these probably should be moved somewhere else, they aren't really related to context settings
/// <summary>
/// Gets whether to use the xplatqos server for QoS calls.
/// </summary>
/// <param name="context">Xbox live context that the settings are associated with.</param>
/// <param name="value">Passes back true if the cross platform QoS servers should be used, false otherwise.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblContextSettingsGetUseCrossPlatformQosServers(
    _In_ XblContextHandle context,
    _Out_ bool* value
) XBL_NOEXCEPT;

/// <summary>
/// Controls whether we use cross platform qos endpoints or not.  
/// In some case if you are shipping with TV_API enabled, you want to be able to choose.
/// </summary>
/// <param name="context">Xbox live context that the settings are associated with.</param>
/// <param name="value">True if the cross platform QoS servers should be used, false otherwise.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblContextSettingsSetUseCrossPlatformQosServers(
    _In_ XblContextHandle context,
    _In_ bool value
) XBL_NOEXCEPT;

}