// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#pragma once
#include "pch.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

class Win32Event
{
public:
    Win32Event();
    ~Win32Event();
    void Set();
    void WaitForever();

private:
    HANDLE m_event;
};

class win32_handle
{
public:
    win32_handle() : m_handle(nullptr)
    {
    }

    ~win32_handle()
    {
        if (m_handle != nullptr) CloseHandle(m_handle);
        m_handle = nullptr;
    }

    void set(HANDLE handle)
    {
        m_handle = handle;
    }

    HANDLE get() { return m_handle; }

private:
    HANDLE m_handle;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END