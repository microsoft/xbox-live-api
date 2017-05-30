// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "Event_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_BEGIN

Event::Event()
{
    m_event = ::CreateEventExW(
        nullptr,
        nullptr,
        CREATE_EVENT_MANUAL_RESET,
        DELETE | SYNCHRONIZE | EVENT_MODIFY_STATE
        );

    if (m_event == nullptr)
    {
        HRESULT hr = HRESULT_FROM_WIN32(GetLastError());
        throw ref new Platform::COMException(hr);
    }
}

Event::~Event()
{
    if (m_event != nullptr)
    {
        ::CloseHandle(m_event);
    }
}

void Event::Reset()
{
    ::ResetEvent(m_event);
}

void Event::Set()
{
    ::SetEvent(m_event);
}

bool Event::Wait(_In_ unsigned int timeoutInMilliseconds)
{
    UINT waitRes = ::WaitForSingleObjectEx(m_event, timeoutInMilliseconds, FALSE);

    switch (waitRes)
    {
    case WAIT_OBJECT_0:
        return true;

    case WAIT_TIMEOUT:
        return false;

    default:
        HRESULT hr = HRESULT_FROM_WIN32(GetLastError());
        throw ref new Platform::COMException(hr);
    }
}

void Event::Wait()
{
    Wait(INFINITE);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_END
