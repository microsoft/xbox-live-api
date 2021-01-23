// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "events_service.h"

#if XSAPI_BUILD_WITH_1910_GRTS

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
    User m_user;
    xsapi_internal_string m_playSession;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_EVENTS_CPP_END

#endif XSAPI_BUILD_WITH_1910_GRTS
