// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi-c/events_c.h"
#include "xbox_live_context_internal.h"

using namespace xbox::services;

STDAPI XblEventsWriteInGameEvent(
    _In_ XblContextHandle xboxLiveContext,
    _In_z_ const char* eventName,
    _In_opt_z_ const char* dimensions,
    _In_opt_z_ const char* measurements
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(xboxLiveContext == nullptr || eventName == nullptr);
    VERIFY_XBL_INITIALIZED();

    return xboxLiveContext->EventsService()->WriteInGameEvent(eventName, dimensions, measurements);
}
CATCH_RETURN()

#if XSAPI_INTERNAL_EVENTS_SERVICE

STDAPI XblEventsSetStorageAllotment(
    uint64_t storageAllotmentInBytes
) XBL_NOEXCEPT
try
{
    return events::EventQueue::SetStorageAllotment(storageAllotmentInBytes);
}
CATCH_RETURN()

STDAPI XblEventsSetMaxFileSize(
    uint64_t maxFileSizeInByes
) XBL_NOEXCEPT
try
{
    return events::EventQueue::SetMaxFileSize(maxFileSizeInByes);
}
CATCH_RETURN()

#endif // !XSAPI_ETW_EVENTS_SERVICE