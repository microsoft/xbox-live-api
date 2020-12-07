// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

NAMESPACE_MICROSOFT_XBOX_SERVICES_EVENTS_CPP_BEGIN

class IEventsService
{
public:
    virtual HRESULT WriteInGameEvent(
        _In_z_ const char* eventName,
        _In_opt_z_ const char* dimensions,
        _In_opt_z_ const char* measurements
    ) = 0;

    virtual HRESULT Initialize() = 0;

    virtual ~IEventsService() = default;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_EVENTS_CPP_END

#if XSAPI_EVENTS_SERVICE
    #if XSAPI_GRTS_EVENTS_SERVICE
        #include "events_service_gdk.h"
    #elif XSAPI_WRL_EVENTS_SERVICE
        #include "events_service_etw.h"
    #elif XSAPI_INTERNAL_EVENTS_SERVICE
        #include "events_service_xsapi.h"
    #endif
#endif
