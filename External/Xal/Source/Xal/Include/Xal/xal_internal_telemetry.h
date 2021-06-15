#pragma once

#if !defined(__cplusplus)
#error C++11 required
#endif

// Attention: This file is intended for internal uses only.
// Its use is not recommended and not supported.

#include <Xal/xal_types.h>

extern "C"
{

//-----------------------------------------------------------------------------
// Telemetry
//-----------------------------------------------------------------------------

/// <summary>
/// Controls if the event is to be uploaded immediately or can be batched.
/// </summary>
/// <remarks>
/// This is a copy of cll::Latency and should be kept in sync with it.
/// </remarks>
typedef enum XalTelemetryLatency
{
    XalTelemetryLatency_Unspecified = 0,
    XalTelemetryLatency_Normal = 0x0100,
    XalTelemetryLatency_Realtime = 0x0200,
} XalTelemetryLatency;

/// <summary>
/// Controls the priority of keeping the event in case cll needs to evict some.
/// </summary>
/// <remarks>
/// This is a copy of cll::Persistence and should be kept in sync with it.
/// </remarks>
typedef enum XalTelemetryPersistence
{
    XalTelemetryPersistence_Unspecified = 0,
    XalTelemetryPersistence_Normal = 0x01,
    XalTelemetryPersistence_Critical = 0x02,
} XalTelemetryPersistence;

/// <summary>
/// Controls the sensitivity of the event.
/// </summary>
/// <remarks>
/// This is a copy of cll::Sensitivity and should be kept in sync with it.
/// </remarks>
typedef enum XalTelemetrySensitivity
{
    XalTelemetrySensitivityUnspecified = 0x000001,
    XalTelemetrySensitivity_None = 0x000000,
    XalTelemetrySensitivity_Mark = 0x080000,
    XalTelemetrySensitivity_Hash = 0x100000,
    XalTelemetrySensitivity_Drop = 0x200000,
} XalTelemetrySensitivity;

/// <summary>
/// Controls the sampling rate of the event.
/// </summary>
typedef enum XalTelemetrySampleRate
{
    XalTelemetrySampleRate_Unspecified = 0,
    XalTelemetrySampleRate_NoSampling = 1,
    XalTelemetrySampleRate_10_percent = 2,
    XalTelemetrySampleRate_0_percent = 3,
} XalTelemetrySampleRate;

/// <summary>
/// Describes the type of a ticket.
/// </summary>
typedef enum XalTelemetryTicketType
{
    //XalTelemetryTicketType_Unspecified = 0,   // currently unsupported
    //XalTelemetryTicketType_MsaUser = 0x01,    // currently unsupported
    //XalTelemetryTicketType_MsaDevice = 0x02,  // currently unsupported
    XalTelemetryTicketType_XauthUser = 0x03,
    XalTelemetryTicketType_XauthDevice = 0x04
} XalTelemetryTicketType;

/// <summary>
/// Describes a ticket that should be included with a telemetry event.
/// </summary>
typedef struct XalTelemetryTicket
{
    /// <summary>
    /// The url for the ticket audience.
    /// </summary>
    _Field_z_ char const* Url;

    /// <summary>
    /// The ticket id.
    /// </summary>
    uint32_t Id;

    /// <summary>
    /// The type of ticket.
    /// </summary>
    XalTelemetryTicketType Type;
} XalTelemetryTicket;

/// <summary>
/// A method to write a telemetry event and send it to vortex.
/// </summary>
/// <param name="user">The user to send this event for.</param>
/// <param name="iKey">The game's iKey.</param>
/// <param name="eventNameWithProvider">The null terminated event name string. This will be the 
/// full name of the event with the provider prefix.</param>
/// <param name="data">The null terminated event data string. The string should
/// be properly formatted JSON.</param>
/// <param name="ticketCount">The number of tickets to send with the event.</param>
/// <param name="tickets">Information about the tickets to send.</param>
/// <param name="latency">The cll latency for this event.</param>
/// <param name="persistence">The cll persistence for this event.</param>
/// <param name="sensitivity">The cll sensitivity for this event.</param>
/// <param name="sampleRate">The cll sampleRate for this event.</param>
STDAPI XalTelemetryWriteEvent(
    _In_ XalUserHandle user,
    _In_z_ char const* iKey,
    _In_z_ char const* eventNameWithProvider,
    _In_z_ char const* data,
    _In_ uint32_t ticketCount,
    _In_reads_(ticketCount) XalTelemetryTicket* tickets,
    _In_ XalTelemetryLatency latency,
    _In_ XalTelemetryPersistence persistence,
    _In_ XalTelemetrySensitivity sensitivity,
    _In_ XalTelemetrySampleRate sampleRate
) noexcept;

}
