// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#if !defined(__cplusplus)
#error C++11 required
#endif

#pragma once

#if XSAPI_EVENTS_SERVICE

extern "C"
{

/// <summary>
/// Write an in-game event that includes "dimensions" and "measurement" data fields.
/// </summary>
/// <param name="xboxLiveContext">Xbox Live context handle.</param>
/// <param name="eventName">Event name. Must be contain only alphanumeric characters.</param>
/// <param name="dimensionsJson">Dimensions data fields.</param>
/// <param name="measurementsJson">Measurement data fields.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Dimensions include event fields with a finite number of defined numeric or string values.  
/// Examples of dimensions: map id, difficulty level, character or weapon class, game mode, boolean settings, etc.  
///
/// Measurements include event fields that represent scalar numeric metrics.  
/// Examples of measurements: score, time, counters, position, etc.  
///
/// Example: for an in-game event that tracks the highest match score for a particular difficulty level:  
/// The difficulty level should be included in dimensions, and the score should be included in measurements.  
///
/// The name of the event, and the names of the event fields (both dimensions and measurements), must match 
/// the names declared in the title's service configuration.  The names are case insensitive.  
/// If the API writes an event with a name that does not match a name in the service configuration, the 
/// service drops the event with no notification.  
///
/// When using the GDK PC version, a GRTS runtime with the XGameEvent feature must be installed or this will E_NOTIMPL.
/// </remarks>
STDAPI XblEventsWriteInGameEvent(
    _In_ XblContextHandle xboxLiveContext,
    _In_z_ const char* eventName,
    _In_opt_z_ const char* dimensionsJson,
    _In_opt_z_ const char* measurementsJson
) XBL_NOEXCEPT;

#if XSAPI_INTERNAL_EVENTS_SERVICE

/// <summary>
/// Set the maximum amount of disk space that Xsapi can use to store the events pending retry and upload before 
/// it starts deleting the oldest files.
/// </summary>
/// <param name="storageAllotmentInBytes">Maximum storage space (in bytes) that will be used to store pending events.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// When the maximum storage space is exceeded, the oldest file will be silently deleted.  
/// Note that this is a global setting and will apply to all Xbox Live contexts.  
/// The default value is approximately 20MB.
/// </remarks>
STDAPI XblEventsSetStorageAllotment(
    uint64_t storageAllotmentInBytes
) XBL_NOEXCEPT;

/// <summary>
/// Set the maximum file size for pending events files.
/// </summary>
/// <param name="maxFileSizeInByes">The maximum size (in bytes) for pending events files.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// The files will be read into memory all at once.  
/// Note that this is a global setting and will apply to all Xbox Live contexts.  
/// The default value is 128KB.
/// </remarks>
STDAPI XblEventsSetMaxFileSize(
    uint64_t maxFileSizeInByes
) XBL_NOEXCEPT;

#endif // XSAPI_INTERNAL_EVENTS_SERVICE
}

#endif // XSAPI_EVENTS_SERVICE