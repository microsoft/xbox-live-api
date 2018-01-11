// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "shared_macros.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_BEGIN

#if UWP_API || TV_API || UNIT_TEST_SERVICES
/// <summary>
/// Wrapper for Win32 event object.
/// All events are manual-reset, unnamed, and use the application's default security context.
/// </summary>
private ref class Event sealed
{
public:
    Event();
    virtual ~Event();

public:
    void Reset();
    void Set();
    void Wait();
    bool Wait(_In_ unsigned int timeoutInMilliseconds);

private:
    HANDLE m_event;
};
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_END
