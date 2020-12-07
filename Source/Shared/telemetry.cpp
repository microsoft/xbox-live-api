// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "telemetry.h"
#include "xbox_system_factory.h"
#include "xbox_live_app_config_internal.h"
#include "Xal/xal_internal_telemetry.h"

#if defined(_WIN32_WINNT) && _WIN32_WINNT >= _WIN32_WINNT_WIN10

#define TraceLoggingOptionMicrosoftTelemetry() \
    TraceLoggingOptionGroup(0x4f50731a, 0x89cf, 0x4782, 0xb3, 0xe0, 0xdc, 0xe8, 0xc9, 0x4, 0x76, 0xba)

TRACELOGGING_DEFINE_PROVIDER(
    g_hTraceLoggingProvider,
    "Microsoft.Xbox.Services",
    (0xe8f7748f, 0xd38f, 0x4f0d, 0x8f, 0x5d, 0x9, 0xe, 0x39, 0x18, 0xfc, 0xd6), // {E8F7748F-D38F-4F0D-8F5D-090E3918FCD6}
    TraceLoggingOptionMicrosoftTelemetry());

TRACELOGGING_DEFINE_PROVIDER(
    g_hUnitTestTraceLoggingProvider,
    "Microsoft.Xbox.Services.Tracing",
    (0x9594a560, 0xe985, 0x4ee6, 0xb0, 0xb5, 0xd, 0xac, 0x4f, 0x92, 0x41, 0x44), // {9594A560-E985-4EE6-B0B5-0DAC4F924144}
    );

#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

std::atomic<uint32_t> xsapi_telemetry::s_ticketId(0);

std::shared_ptr<xsapi_telemetry> xsapi_telemetry::get_singleton()
{
    auto xsapiSingleton = xbox::services::get_xsapi_singleton();
    if (xsapiSingleton)
    {
        std::lock_guard<std::mutex> guard(xsapiSingleton->m_telemetryLock);
        if (xsapiSingleton->m_telemetrySingleton == nullptr)
        {
            auto buffer = Alloc(sizeof(xsapi_telemetry));

            xsapiSingleton->m_telemetrySingleton = std::shared_ptr<xsapi_telemetry>(
                new (buffer) xsapi_telemetry(),
                Deleter<xsapi_telemetry>()
                );
        }
        return xsapiSingleton->m_telemetrySingleton;
    }
    else
    {
        return nullptr;
    }
}

void xsapi_telemetry::write_event(xbox_live_user_t user, xsapi_internal_string eventName, xsapi_internal_string jsonPayload, bool isRealtimeEvent)
{
#if HC_PLATFORM != HC_PLATFORM_GDK && !XSAPI_UNIT_TESTS // TODO: GDK
    try
    {
        // Check event name
        regex_t regex(_T("[A-Za-z]+[A-Za-z0-9_]*"));
        bool matchFound = std::regex_match(utils::string_t_from_internal_string(eventName), regex);
        if (!matchFound)
        {
            throw std::invalid_argument("Invalid event name!");
        }
        
        auto fullEventName =
            "Microsoft.XboxLive.T" +
            std::to_string( AppConfig::Instance()->TitleId() ) +
            "." +
            eventName.data();

        XalTelemetryTicket tickets[] =
        {
            { "https://vortex-win.data.microsoft.com", 0, XalTelemetryTicketType_XauthDevice },
            { "https://vortex-events.xboxlive.com", s_ticketId++, XalTelemetryTicketType_XauthUser }
        };

        XalTelemetryWriteEvent(
            user,
            m_iKey.c_str(),
            fullEventName.c_str(),
            jsonPayload.c_str(),
            sizeof(tickets) / sizeof(XalTelemetryTicket),
            tickets,
            isRealtimeEvent ? XalTelemetryLatency_Realtime : XalTelemetryLatency_Normal,
            XalTelemetryPersistence_Normal,
            XalTelemetrySensitivity_None,
            XalTelemetrySampleRate_Unspecified
        );
    }
    catch (const std::exception&)
    {
        xbl_error_code err = utils::convert_exception_to_xbox_live_error_code();
        xsapi_internal_stringstream ss;
        ss << "Exception in " << __FUNCTION__ << ": " << err;
        LOG_ERROR(ss.str().data());
    }
#else
    UNREFERENCED_PARAMETER(user);
    UNREFERENCED_PARAMETER(eventName);
    UNREFERENCED_PARAMETER(jsonPayload);
    UNREFERENCED_PARAMETER(isRealtimeEvent);
#endif
}

xsapi_telemetry::xsapi_telemetry()
{
#if !(HC_PLATFORM == HC_PLATFORM_XDK)
    xsapi_internal_stringstream defaultIKey;
    defaultIKey << "P-XBL-T" << AppConfig::Instance()->TitleId();
    m_iKey = defaultIKey.str();
#endif
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

