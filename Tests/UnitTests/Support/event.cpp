// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "event.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

Event::Event() noexcept
{
    m_handle = CreateEvent(nullptr, false, false, nullptr);
    assert(m_handle);
}

Event::~Event() noexcept
{
    CloseHandle(m_handle);
}

void Event::Set() noexcept
{
    auto result = SetEvent(m_handle);
    assert(result);
    UNREFERENCED_PARAMETER(result);
}

void Event::Wait() noexcept
{
    WaitForSingleObject(m_handle, INFINITE);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END