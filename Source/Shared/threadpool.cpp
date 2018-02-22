// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "threadpool.h"
#include "httpClient/httpClient.h"

#define MAX_THREADS 64

using namespace xbox::services;

xbl_thread_pool::xbl_thread_pool() :
    m_numActiveThreads(0),
    m_targetNumThreads(2),
    m_defaultIdealProcessor(MAXIMUM_PROCESSORS)
{
    memset(m_hActiveThreads, 0, sizeof(HANDLE) * MAX_THREADS);
    m_stopRequestedHandle.set(CreateEvent(nullptr, true, false, nullptr));
    m_completeReadyHandle.set(CreateEvent(nullptr, false, false, nullptr));
    m_pendingReadyHandle.set(CreateEvent(nullptr, false, false, nullptr));
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

HANDLE xbl_thread_pool::get_pending_ready_handle()
{
    return m_pendingReadyHandle.get();
}

HANDLE xbl_thread_pool::get_complete_ready_handle()
{
    return m_completeReadyHandle.get();
}

HANDLE xbl_thread_pool::get_stop_handle()
{
    return m_stopRequestedHandle.get();
}

DWORD WINAPI xbox_live_thread_proc(LPVOID lpParam)
{
    auto threadPool = reinterpret_cast<xbl_thread_pool*>(lpParam);
    HANDLE hEvents[3] =
    {
        threadPool->get_pending_ready_handle(),
        threadPool->get_complete_ready_handle(),
        threadPool->get_stop_handle()
    };

    bool stop = false;
    while (!stop)
    {
        DWORD dwResult = WaitForMultipleObjectsEx(3, hEvents, false, INFINITE, false);
        switch (dwResult)
        {
        case WAIT_OBJECT_0:
            HCTaskProcessNextPendingTask(HC_SUBSYSTEM_ID_XSAPI);
            if (HCTaskGetPendingTaskQueueSize(HC_SUBSYSTEM_ID_XSAPI) > 0)
            {
                SetEvent(threadPool->get_pending_ready_handle());
            };

            break;
        case WAIT_OBJECT_0 + 1:
            HCTaskProcessNextCompletedTask(HC_SUBSYSTEM_ID_XSAPI, XSAPI_DEFAULT_TASKGROUP);
            if (HCTaskGetCompletedTaskQueueSize(HC_SUBSYSTEM_ID_XSAPI, XSAPI_DEFAULT_TASKGROUP) > 0)
            {
                SetEvent(threadPool->get_complete_ready_handle());
            }
            break;

        default:
            stop = true;
            break;
        }
    }

    return 0;
}

void libhttpclient_event_handler(
    _In_opt_ void* context,
    _In_ HC_TASK_EVENT_TYPE eventType,
    _In_ HC_TASK_HANDLE taskHandle
    )
{
    UNREFERENCED_PARAMETER(taskHandle);

    auto threadPool = reinterpret_cast<xbl_thread_pool*>(context);
    switch (eventType)
    {
    case HC_TASK_EVENT_TYPE::HC_TASK_EVENT_PENDING:
        SetEvent(threadPool->get_pending_ready_handle());
        break;

    case HC_TASK_EVENT_TYPE::HC_TASK_EVENT_EXECUTE_STARTED:
        break;

    case HC_TASK_EVENT_TYPE::HC_TASK_EVENT_EXECUTE_COMPLETED:
        SetEvent(threadPool->get_complete_ready_handle());
        break;
    }
}

void xbl_thread_pool::start_threads()
{
    HCAddTaskEventHandler(HC_SUBSYSTEM_ID_XSAPI, this, libhttpclient_event_handler, &m_hcEventHandle);

    for (int i = 0; i < m_targetNumThreads; i++)
    {
        m_hActiveThreads[i] = CreateThread(nullptr, 0, xbox_live_thread_proc, this, 0, nullptr);
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
