// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once


#if UWP_API || XSAPI_U
NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN
    /// <summary>
    /// Contains classes that let you send an in-game event.
    /// </summary>
    namespace events {

    /// <summary>
/// Represents a service class that provides APIs that you can use to write in-game events.
/// </summary>
class events_service
{
public:
    events_service() {};

    events_service(
        _In_ std::shared_ptr<xbox::services::user_context> userContext,
        _In_ std::shared_ptr<xbox::services::xbox_live_app_config> localConfig
        );

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
    _XSAPIIMP xbox_live_result<void> write_in_game_event(_In_ const string_t& eventName);

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
    _XSAPIIMP xbox_live_result<void> write_in_game_event(
        _In_ const string_t& eventName,
        _In_ const web::json::value& dimensions,
        _In_ const web::json::value& measurement
        );

private:
    std::shared_ptr<xbox::services::user_context> m_userContext;
    std::shared_ptr<xbox::services::xbox_live_app_config> m_appConfig;

    string_t m_playSession;
#if UWP_API
    string_t m_appInsightsKey;

    void add_value_pair(
        _In_ Windows::Foundation::Diagnostics::LoggingFields^ fields,
        _In_ const std::pair<utility::string_t, web::json::value>& pair
        );

    Windows::Foundation::Diagnostics::LoggingFields^ create_logging_field(
        _In_ const string_t& eventName,
        _In_ const web::json::value& properties,
        _In_ const web::json::value& measurement
        );

    string_t load_app_insights_key();

    void add_common_logging_field(_In_ Windows::Foundation::Diagnostics::LoggingFields^ fields);
    Windows::Foundation::Diagnostics::LoggingOptions^ m_loggingOptions;
    Windows::Foundation::Diagnostics::LoggingChannel^ m_loggingChannel;
#endif
};

} // namespace events
NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

#endif