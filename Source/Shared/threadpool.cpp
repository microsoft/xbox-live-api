// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "threadpool.h"
#include "httpClient/httpClient.h"

#define MAX_THREADS 64

xbl_thread_pool::xbl_thread_pool() :
    m_numActiveThreads(0),
    m_targetNumThreads(2),
    m_defaultIdealProcessor(MAXIMUM_PROCESSORS)
{
    memset(m_hActiveThreads, 0, sizeof(HANDLE) * MAX_THREADS);
    m_stopRequestedHandle.set(CreateEvent(nullptr, true, false, nullptr));
}

long xbl_thread_pool::get_num_active_threads()
{
    return m_numActiveThreads;
}

void xbl_thread_pool::set_target_num_active_threads(long targetNumThreads)
{
    assert(m_targetNumThreads <= MAX_THREADS);
    if (m_targetNumThreads > MAX_THREADS) return;

    if (m_targetNumThreads != targetNumThreads)
    {
        if (m_numActiveThreads > 0) shutdown_active_threads();

        m_targetNumThreads = targetNumThreads;
        start_threads();
    }
}

HANDLE xbl_thread_pool::get_stop_handle()
{
    return m_stopRequestedHandle.get();
}

HANDLE xbl_thread_pool::get_ready_handle()
{
    return m_readyHandle.get();
}

void xbl_thread_pool::set_async_op_ready()
{
    SetEvent(get_ready_handle());
}

DWORD WINAPI xbox_live_thread_proc(LPVOID lpParam)
{
    HANDLE hEvents[2] =
    {
        HCTaskGetPendingHandle(),
        HCTaskGetCompletedHandle(0)
    };

    bool stop = false;
    while (!stop)
    {
        DWORD dwResult = WaitForMultipleObjectsEx(2, hEvents, false, INFINITE, false);
        switch (dwResult)
        {
        case WAIT_OBJECT_0: // pending 
            HCTaskProcessNextPendingTask();
            break;

        case WAIT_OBJECT_0 + 1: // completed
            HCTaskProcessNextCompletedTask(0);
            break;

        default:
            stop = true;
            break;
        }
    }

    return 0;
}

void xbl_thread_pool::start_threads()
{
    for (int i = 0; i < m_targetNumThreads; i++)
    {
        m_hActiveThreads[i] = CreateThread(nullptr, 0, xbox_live_thread_proc, nullptr, 0, nullptr);
        if (m_defaultIdealProcessor != MAXIMUM_PROCESSORS)
        {
            SetThreadIdealProcessor(m_hActiveThreads[i], m_defaultIdealProcessor);
        }
    }

    m_numActiveThreads = m_targetNumThreads;
}

void xbl_thread_pool::shutdown_active_threads()
{
    SetEvent(m_stopRequestedHandle.get());
    DWORD dwResult = WaitForMultipleObjectsEx(m_numActiveThreads, m_hActiveThreads, true, INFINITE, false);
    if (dwResult >= WAIT_OBJECT_0 && dwResult <= WAIT_OBJECT_0 + m_numActiveThreads - 1)
    {
        for (int i = 0; i < m_numActiveThreads; i++)
        {
            CloseHandle(m_hActiveThreads[i]);
            m_hActiveThreads[i] = nullptr;
        }
        m_numActiveThreads = 0;
        ResetEvent(m_stopRequestedHandle.get());
    }
}

void xbl_thread_pool::set_thread_ideal_processor(_In_ int threadIndex, _In_ DWORD dwIdealProcessor)
{
    if (threadIndex == -1)
    {
        for (int i = 0; i < m_numActiveThreads; i++)
        {
            HANDLE hThread = m_hActiveThreads[i];
            if (hThread != nullptr)
            {
                SetThreadIdealProcessor(hThread, dwIdealProcessor);
            }
        }

        m_defaultIdealProcessor = dwIdealProcessor;
    }
    else
    {
        if (threadIndex <= MAX_THREADS)
        {
            HANDLE hThread = m_hActiveThreads[threadIndex];
            if (hThread != nullptr)
            {
                SetThreadIdealProcessor(hThread, dwIdealProcessor);
            }
        }
    }
}
