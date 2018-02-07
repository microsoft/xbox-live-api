// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "threadpool.h"

using namespace xbox::services;
using namespace xbox::services::system;

XBL_API XBL_RESULT XBL_CALLING_CONV
XblAddTaskEventHandler(
    _In_opt_ void* context,
    _In_opt_ XBL_TASK_EVENT_FUNC taskEventFunc,
    _Out_opt_ XBL_TASK_EVENT_HANDLE* eventHandle
    ) XBL_NOEXCEPT
{
    XBL_TASK_EVENT_HANDLE tempEventHandle;

    auto hcAddTaskEventHandlerContext = xsapi_allocate_shared<std::pair<XBL_TASK_EVENT_FUNC, void*>>(taskEventFunc, context);

    HC_RESULT hcResult = HCAddTaskEventHandler(HC_SUBSYSTEM_ID_XSAPI, hcAddTaskEventHandlerContext.get(),
        [](void* context, HC_TASK_EVENT_TYPE eventType, HC_TASK_HANDLE taskHandle)
    {
        auto hcAddTaskEventHandlerContext = reinterpret_cast<std::pair<XBL_TASK_EVENT_FUNC, void *>*>(context);

        hcAddTaskEventHandlerContext->first(
            hcAddTaskEventHandlerContext->second, 
            static_cast<XBL_TASK_EVENT_TYPE>(eventType),
            taskHandle);

        return HC_OK;

    }, &tempEventHandle);

    if (hcResult == HC_OK)
    {
        if (eventHandle != nullptr)
        {
            *eventHandle = tempEventHandle;
        }
        auto singleton = get_xsapi_singleton();
        std::lock_guard<std::mutex> lock(singleton->m_eventHandlerContextsLock);

        singleton->m_eventHandlerContexts[tempEventHandle] = hcAddTaskEventHandlerContext;
    }

    return utils::create_xbl_result(hcResult);
}

XBL_API XBL_RESULT XBL_CALLING_CONV
XblRemoveTaskEventHandler(
    _In_ XBL_TASK_EVENT_HANDLE eventHandle
    ) XBL_NOEXCEPT
{
    auto singleton = get_xsapi_singleton();
    std::lock_guard<std::mutex> lock(singleton->m_eventHandlerContextsLock);

    singleton->m_eventHandlerContexts.erase(eventHandle);

    return utils::create_xbl_result(HCRemoveTaskEventHandler(eventHandle));
}

XBL_API XBL_RESULT XBL_CALLING_CONV
XblTaskProcessNextPendingTask() XBL_NOEXCEPT
{
    return utils::create_xbl_result(HCTaskProcessNextPendingTask(HC_SUBSYSTEM_ID_XSAPI));
}

XBL_API XBL_RESULT XBL_CALLING_CONV
XblTaskProcessNextCompletedTask(_In_ uint64_t taskGroupId) XBL_NOEXCEPT
{
    return utils::create_xbl_result(HCTaskProcessNextCompletedTask(HC_SUBSYSTEM_ID_XSAPI, taskGroupId));
}

XBL_API uint64_t XBL_CALLING_CONV
XblTaskGetPendingTaskQueueSize() XBL_NOEXCEPT
{
    return HCTaskGetPendingTaskQueueSize(HC_SUBSYSTEM_ID_XSAPI);
}

XBL_API uint64_t XBL_CALLING_CONV
XblTaskGetCompletedTaskQueueSize(
    _In_ uint64_t taskGroupId
    ) XBL_NOEXCEPT
{
    return HCTaskGetCompletedTaskQueueSize(HC_SUBSYSTEM_ID_XSAPI, taskGroupId);
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