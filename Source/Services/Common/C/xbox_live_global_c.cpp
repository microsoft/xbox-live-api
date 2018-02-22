// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "threadpool.h"
#include "async_queue.h"

using namespace xbox::services;
using namespace xbox::services::system;

XBL_API XBL_RESULT XblCreateAsyncQueue(
    _Out_ XBL_ASYNC_QUEUE* queue
    ) XBL_NOEXCEPT
{
    auto buffer = xsapi_memory::mem_alloc(sizeof(xbl_async_queue));
    *queue = new (buffer) xbl_async_queue();
    return XBL_RESULT_OK;
}

XBL_API XBL_RESULT XblCloseAsyncQueue(
    _In_ XBL_ASYNC_QUEUE queue
    ) XBL_NOEXCEPT
{
    xsapi_memory::mem_free(queue);
    return XBL_RESULT_OK;
}

XBL_API XBL_RESULT XBL_CALLING_CONV
XblAddTaskEventHandler(
    _In_opt_ void* context,
    _In_opt_ XBL_TASK_EVENT_FUNC taskEventFunc,
    _Out_opt_ XBL_ASYNC_EVENT_HANDLE* eventHandle
    ) XBL_NOEXCEPT
{
    return utils::create_xbl_result(HCAddTaskEventHandler(HC_SUBSYSTEM_ID_XSAPI, context, reinterpret_cast<HC_TASK_EVENT_FUNC>(taskEventFunc), eventHandle));
}

XBL_API XBL_RESULT XBL_CALLING_CONV
XblRemoveTaskEventHandler(
    _In_ XBL_ASYNC_EVENT_HANDLE eventHandle
    ) XBL_NOEXCEPT
{
    return utils::create_xbl_result(HCRemoveTaskEventHandler(eventHandle));
}

XBL_API XBL_RESULT XBL_CALLING_CONV
XblDispatchAsyncQueue(
    _In_ XBL_ASYNC_QUEUE queue,
    _In_ XBL_ASYNC_QUEUE_CALLBACK_TYPE type
    ) XBL_NOEXCEPT
{
    if (type == XBL_ASYNC_QUEUE_CALLBACK_TYPE_WORK)
    {
        return utils::create_xbl_result(HCTaskProcessNextPendingTask(HC_SUBSYSTEM_ID_XSAPI));
    }
    else if (type == XBL_ASYNC_QUEUE_CALLBACK_TYPE_COMPLETION)
    {
        return utils::create_xbl_result(HCTaskProcessNextCompletedTask(HC_SUBSYSTEM_ID_XSAPI, queue->taskGroupId));
    }
    return XBL_RESULT_OK;
}

XBL_API bool XBL_CALLING_CONV
XblIsAsyncQueueEmpty(
    _In_ XBL_ASYNC_QUEUE queue
    ) XBL_NOEXCEPT
{
    return !(HCTaskGetCompletedTaskQueueSize(HC_SUBSYSTEM_ID_XSAPI, queue->taskGroupId) > 0);
}

XBL_API XBL_RESULT XBL_CALLING_CONV
XblMemSetFunctions(
    _In_opt_ XBL_MEM_ALLOC_FUNC memAllocFunc,
    _In_opt_ XBL_MEM_FREE_FUNC memFreeFunc
    ) XBL_NOEXCEPT
{
    if (get_xsapi_singleton(false) != nullptr)
    {
        return XBL_RESULT{ XBL_ERROR_CONDITION_GENERIC_ERROR, XBL_ERROR_CODE_ALREADYINITITIALIZED };
    }

    auto hcResult = HCMemSetFunctions(memAllocFunc, memFreeFunc);
    if (hcResult != HC_OK)
    {
        return utils::create_xbl_result(hcResult);
    }

    if (memAllocFunc != nullptr)
    {
        g_pMemAllocHook = memAllocFunc;
    }
    if (memFreeFunc != nullptr)
    {
        g_pMemFreeHook = memFreeFunc;
    }
    return XBL_RESULT_OK;
}

XBL_API XBL_RESULT XBL_CALLING_CONV
XblMemGetFunctions(
    _Out_ XBL_MEM_ALLOC_FUNC* memAllocFunc,
    _Out_ XBL_MEM_FREE_FUNC* memFreeFunc
    ) XBL_NOEXCEPT
{
    if (memAllocFunc == nullptr || memFreeFunc == nullptr)
    {
        return XBL_RESULT{ XBL_ERROR_CONDITION_GENERIC_ERROR, XBL_ERROR_CODE_INVALID_ARGUMENT };
    }

    *memAllocFunc = g_pMemAllocHook;
    *memFreeFunc = g_pMemFreeHook;
    return XBL_RESULT_OK;
}

XBL_API XBL_RESULT XBL_CALLING_CONV
XblGlobalInitialize() XBL_NOEXCEPT
try
{
    (void) get_xsapi_singleton();
    return XBL_RESULT_OK;
}
CATCH_RETURN()

XBL_API void XBL_CALLING_CONV
XblGlobalCleanup() XBL_NOEXCEPT
try
{
    auto singleton = get_xsapi_singleton();
    if (singleton != nullptr)
    {
        singleton->m_threadpool->shutdown_active_threads();
    }
    HCGlobalCleanup();
}
CATCH_RETURN_WITH(;)