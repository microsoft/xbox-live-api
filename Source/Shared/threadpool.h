// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#pragma once

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

#ifdef _WIN32 // TODO add threadpool implementation for non Windows platforms
class Win32Event
{
public:
    Win32Event()
    {
        m_event = CreateEvent(NULL, TRUE, FALSE, nullptr);
    }
    ~Win32Event()
    {
        CloseHandle(m_event);
    }
    void Set()
    {
        SetEvent(m_event);
    }
    void WaitForever()
    {
        WaitForSingleObject(m_event, INFINITE);
    }

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

class xbl_thread_pool
{
public:
    xbl_thread_pool();

    void start_threads();

    void set_target_num_active_threads(long targetNumThreads);
    void shutdown_active_threads();
    long get_num_active_threads();
    void set_thread_ideal_processor(_In_ int threadIndex, _In_ BYTE dwIdealProcessor);

    HANDLE get_pending_ready_handle();
    HANDLE get_complete_ready_handle();
    HANDLE get_stop_handle();

private:
    long m_targetNumThreads;
    xbox::services::win32_handle m_stopRequestedHandle;
    xbox::services::win32_handle m_pendingReadyHandle;
    xbox::services::win32_handle m_completeReadyHandle;

    long m_numActiveThreads;
    HANDLE m_hActiveThreads[64];
    PROCESSOR_NUMBER m_defaultIdealProcessor;

    HC_TASK_EVENT_HANDLE m_hcEventHandle;
};

#endif // WIN32

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END