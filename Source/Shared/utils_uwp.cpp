// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "utils_uwp.h"

Win32Event::Win32Event()
{
    m_event = CreateEvent(NULL, TRUE, FALSE, nullptr);
}

Win32Event::~Win32Event()
{
    CloseHandle(m_event);
}

void Win32Event::Set()
{
    SetEvent(m_event);
}

void Win32Event::WaitForever()
{
    WaitForSingleObject(m_event, INFINITE);
}
