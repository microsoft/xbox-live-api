// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#if XSAPI_WRL_EVENTS_SERVICE

#include <wrl.h>
#include <windows.foundation.diagnostics.h>
#include "events_service.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_EVENTS_CPP_BEGIN

class EventsService : public IEventsService, public std::enable_shared_from_this<EventsService>
{
public:
    EventsService(_In_ User&& user);
    ~EventsService();

    HRESULT Initialize();

    HRESULT WriteInGameEvent(
        _In_z_ const char* eventName,
        _In_opt_z_ const char* dimensions,
        _In_opt_z_ const char* measurements
    );

private:
    HRESULT WriteInGameEventHelper(
        _In_ const xsapi_internal_string& eventName,
        _In_ const JsonValue& dimensions,
        _In_ const JsonValue& measurement
    );

    CO_MTA_USAGE_COOKIE m_mtaUsageCookie = nullptr;
#if HC_PLATFORM != HC_PLATFORM_UWP
    HRESULT m_hrCoIncrementMTAUsage = E_FAIL;
#endif
    User m_user;

    xsapi_internal_string m_playSession;
    xsapi_internal_string m_scid;

    Microsoft::WRL::ComPtr<ABI::Windows::Foundation::Diagnostics::ILoggingChannel> m_loggingChannel;
    Microsoft::WRL::ComPtr<ABI::Windows::Foundation::Diagnostics::ILoggingOptions> m_loggingOptions;

    void AddValuePair(
        _Inout_ Microsoft::WRL::ComPtr<ABI::Windows::Foundation::Diagnostics::ILoggingFields> fields,
        _In_ const std::pair<xsapi_internal_string, JsonDocument>& pair
    );

    Microsoft::WRL::ComPtr<ABI::Windows::Foundation::Diagnostics::ILoggingFields> CreateLoggingFields(
        _In_ const xsapi_internal_string& eventName,
        _In_ const JsonValue& properties,
        _In_ const JsonValue& measurement
    );
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_EVENTS_CPP_END

#endif