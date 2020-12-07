// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#if XSAPI_EVENTS_SERVICE

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

class xbox_live_context;

namespace events
{

/// <summary>
/// Represents a service class that provides APIs that you can use to write in-game events.
/// </summary>
class events_service : public std::enable_shared_from_this<events_service>
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
    inline xbox_live_result<void> write_in_game_event(_In_ const string_t& eventName);

    /// <summary>
    /// Write an in-game event that includes "dimensions" and "measurement" data fields.
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
    /// <param name="dimensions">Dimensions data fields</param>
    /// <param name="measurements">Measurement data fields</param>
    /// <remarks>
    /// The name of the event, and the names of the event fields (both dimensions and measurements), must match
    /// the names declared in the title's service configuration. The names are case insensitive.
    /// If the API writes an event with a name that does not match a name in the service configuration, the
    /// service drops the event with no notification.
    /// </remarks>
    inline xbox_live_result<void> write_in_game_event(
        _In_ const string_t& eventName,
        _In_ const web::json::value& dimensions,
        _In_ const web::json::value& measurement
        );

    inline events_service(const events_service& other);
    inline events_service& operator=(events_service other);
    inline ~events_service();

private:
    inline events_service(_In_ XblContextHandle contextHandle);

    XblContextHandle m_xblContext;

    friend xbox_live_context;
};

}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

#include "impl/events.hpp"

#endif