// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#if UWP_API

#include "xsapi/events.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_EVENTS_BEGIN

/// <summary>
/// Represents a service class that provides APIs that you can use to write in-game events
/// </summary>
public ref class EventsService sealed
{
public:
    /// <summary>
    /// Write a simple in-game event without providing any data fields.
    /// </summary>
    /// <param name="eventName">Event name</param>
    /// <remarks>
    /// The name of the event must match the event name declared in the title's service configuration.
    /// The names are case insensitive.
    /// If the API writes an event with a name that does not match a name in the service configuration, the
    /// service drops the event with no notification.
    /// </remarks>
    void WriteInGameEvent(_In_ Platform::String^ eventName);

    /// <summary>
    /// Write an in-game event that includes "dimension" and "measurement" data fields.
    ///
    /// Dimensions include event fields with a finite number of defined numeric or string values.
    /// Examples of dimensions: map id, difficulty level, character or weapon class, game mode, boolean settings, etc.
    /// 
    /// Measurements include event fields that represent scalar numeric metrics.
    /// Examples of measurements: score, time, counters, position, etc.
    ///
    /// Example: for an in-game event that tracks the highest match score for a particular difficulty level: 
    /// The difficulty level should be included in dimensions, and the score should be included in measurements.
    /// </summary>
    /// <param name="eventName">Event name</param>
    /// <param name="dimensions">Dimension data fields</param>
    /// <param name="measurements">Measurement data fields</param>
    /// <remarks>
    /// The name of the event, and the names of the event fields (both dimensions and measurements), must match
    /// the names declared in the title's service configuration. The names are case insensitive.
    /// If the API writes an event with a name that does not match a name in the service configuration, the
    /// service drops the event with no notification.
    /// </remarks>
    void WriteInGameEvent(
        _In_ Platform::String^ eventName,
        _In_ Windows::Foundation::Collections::PropertySet^ dimensions,
        _In_ Windows::Foundation::Collections::PropertySet^ measurements
        );

internal:
    EventsService( 
        _In_ xbox::services::events::events_service cppObj
        );

private:
    xbox::services::events::events_service m_cppObj;

};

NAMESPACE_MICROSOFT_XBOX_SERVICES_EVENTS_END

#endif //#if UWP_API