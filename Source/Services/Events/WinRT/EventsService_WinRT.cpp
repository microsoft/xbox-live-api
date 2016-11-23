//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#if !UNIT_TEST_SERVICES
#include "EventsService_WinRT.h"
#include "Utils_WinRT.h"

using namespace Concurrency;
using namespace xbox::services::events;
using namespace Microsoft::Xbox::Services::System;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace xbox::services;

NAMESPACE_MICROSOFT_XBOX_SERVICES_EVENTS_BEGIN

EventsService::EventsService(
    _In_ xbox::services::events::events_service cppObj
    ):
    m_cppObj(std::move(cppObj))
{
}

void 
EventsService::WriteInGameEvent(_In_ Platform::String^ eventName)
{
    xbox_live_result<void> result = m_cppObj.write_in_game_event(STRING_T_FROM_PLATFORM_STRING(eventName));
    THROW_IF_ERR(result);
}

void EventsService::WriteInGameEvent(
    _In_ Platform::String^ eventName,
    _In_ Windows::Foundation::Collections::PropertySet^ dimensions,
    _In_ Windows::Foundation::Collections::PropertySet^ measurements
    )
{
    xbox_live_result<void> result = m_cppObj.write_in_game_event(
        STRING_T_FROM_PLATFORM_STRING(eventName),
        UtilsWinRT::JsonValueFromPropertySet(dimensions),
        UtilsWinRT::JsonValueFromPropertySet(measurements)
        );
    THROW_IF_ERR(result);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_EVENTS_END
#endif