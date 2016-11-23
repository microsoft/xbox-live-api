#pragma once
#include "shared_macros.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_BEGIN

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

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_END
