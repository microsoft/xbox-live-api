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

    HANDLE get_stop_handle();
    HANDLE get_ready_handle();
    void set_async_op_ready();

private:
    long m_targetNumThreads;
    win32_handle m_stopRequestedHandle;
    win32_handle m_readyHandle;

    long m_numActiveThreads;
    HANDLE m_hActiveThreads[64];
    DWORD m_defaultIdealProcessor;
};
