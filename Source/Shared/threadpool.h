// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#pragma once
#include "utils_uwp.h"

class xbl_thread_pool
{
public:
    xbl_thread_pool();

    void start_threads();

    void set_target_num_active_threads(long targetNumThreads);
    void shutdown_active_threads();
    long get_num_active_threads();
    void set_thread_ideal_processor(_In_ int threadIndex, _In_ DWORD dwIdealProcessor);

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
    DWORD m_defaultIdealProcessor;

    HC_TASK_EVENT_HANDLE m_hcEventHandle;
};
