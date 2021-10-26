// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"

int XTaskQueueCreate_Lua(lua_State *L)
{
    // CODE SNIPPET START: XTaskQueueCreate
    XTaskQueueHandle queue = nullptr;
    HRESULT hr = XTaskQueueCreate(
        XTaskQueueDispatchMode::Manual,
        XTaskQueueDispatchMode::Manual,
        &queue);
    // CODE SNIPPET END

    Data()->queue = queue;
    LogToFile("XTaskQueueCreate: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XTaskQueueDuplicateHandle_Lua(lua_State *L)
{
    XTaskQueueHandle queue = Data()->queue;

    // CODE SNIPPET START: XTaskQueueDuplicateHandle
    XTaskQueueHandle newQueue = nullptr;
    HRESULT hr = XTaskQueueDuplicateHandle(
        queue,
        &newQueue);
    // CODE SNIPPET END
    XTaskQueueCloseHandle(newQueue);
    return LuaReturnHR(L, hr);
}

int XTaskQueueDispatch_Lua(lua_State *L)
{
    XTaskQueueHandle queue = Data()->queue;

    // CODE SNIPPET START: XTaskQueueDispatch
    HRESULT hr = XTaskQueueDispatch(queue, XTaskQueuePort::Completion, 0);
    // CODE SNIPPET END
    LogToFile("XTaskQueueDispatch: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XTaskQueueCloseHandle_Lua(lua_State *L)
{
    XTaskQueueHandle queue = Data()->queue;

    // CODE SNIPPET START: XTaskQueueCloseHandle
    if (queue != nullptr) 
    {
        XTaskQueueCloseHandle(queue);
    }
    // CODE SNIPPET END
    Data()->queue = nullptr;

    return LuaReturnHR(L, S_OK);
}

int XTaskQueueTerminate_Lua(lua_State *L)
{
    XTaskQueueHandle queue = Data()->queue;

    // CODE SNIPPET START: XTaskQueueTerminate
    HRESULT hr = XTaskQueueTerminate(queue, true, nullptr, nullptr);
    // CODE SNIPPET END

    LogToFile("XTaskQueueTerminate: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, S_OK);
}

int XTaskQueueTerminateWithAsyncWait_Lua(lua_State *L)
{
    XTaskQueueHandle queue = Data()->queue;
    HRESULT hr = S_OK;
    if (queue == nullptr)
    {
        Data()->m_stopTest = true;
        LogToFile("XTaskQueueTerminate with async wait: hr=%s", ConvertHR(hr).c_str());
        return LuaReturnHR(L, hr);
    }
    else
    {
        hr = XTaskQueueTerminate(queue, false, nullptr, [](void*)
        {
            CallLuaStringWithDefault(Data()->m_onTaskQueueTerminateWithAsyncWait, "common = require 'common'; common.cleanup()");
            LogToFile("StopTest\n");
            Data()->m_stopTest = true;
        });
    }

    LogToFile("XTaskQueueTerminate with async wait: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XTaskQueueSetCurrentProcessTaskQueue_Lua(lua_State *L)
{
    // CODE SNIPPET START: XTaskQueueSetCurrentProcessTaskQueue
    XTaskQueueHandle queue = nullptr;
    XTaskQueueSetCurrentProcessTaskQueue(queue);
    // CODE SNIPPET END

    LogToFile("XTaskQueueSetCurrentProcessTaskQueue");
    return LuaReturnHR(L, S_OK);
}

int XTaskQueueGetCurrentProcessTaskQueue_Lua(lua_State *L)
{
    // CODE SNIPPET START: XTaskQueueGetCurrentProcessTaskQueue
    XTaskQueueHandle queue = nullptr;
    XTaskQueueGetCurrentProcessTaskQueue(&queue);
    // CODE SNIPPET END

    LogToFile("XTaskQueueGetCurrentProcessTaskQueue 0x%0.8x", queue);
    return LuaReturnHR(L, S_OK);
}

void SetupAPIs_Async()
{
    lua_register(Data()->L, "XTaskQueueCreate", XTaskQueueCreate_Lua);
    lua_register(Data()->L, "XTaskQueueDuplicateHandle", XTaskQueueDuplicateHandle_Lua);
    lua_register(Data()->L, "XTaskQueueDispatch", XTaskQueueDispatch_Lua);
    lua_register(Data()->L, "XTaskQueueCloseHandle", XTaskQueueCloseHandle_Lua);
    lua_register(Data()->L, "XTaskQueueTerminate", XTaskQueueTerminate_Lua);
    lua_register(Data()->L, "XTaskQueueTerminateWithAsyncWait", XTaskQueueTerminateWithAsyncWait_Lua);
    lua_register(Data()->L, "XTaskQueueSetCurrentProcessTaskQueue", XTaskQueueSetCurrentProcessTaskQueue_Lua);
    lua_register(Data()->L, "XTaskQueueGetCurrentProcessTaskQueue", XTaskQueueGetCurrentProcessTaskQueue_Lua);

    //lua_register(Data()->L, "XTaskQueueGetPort", XTaskQueueGetPort_Lua);
    //lua_register(Data()->L, "XTaskQueueCreateComposite", XTaskQueueCreateComposite_Lua);
    //lua_register(Data()->L, "XTaskQueueSubmitCallback", XTaskQueueSubmitCallback_Lua);
    //lua_register(Data()->L, "XTaskQueueSubmitDelayedCallback", XTaskQueueSubmitDelayedCallback_Lua);
    //lua_register(Data()->L, "XTaskQueueRegisterWaiter", XTaskQueueRegisterWaiter_Lua);
    //lua_register(Data()->L, "XTaskQueueUnregisterWaiter", XTaskQueueUnregisterWaiter_Lua);
    //lua_register(Data()->L, "XTaskQueueRegisterMonitor", XTaskQueueRegisterMonitor_Lua);
    //lua_register(Data()->L, "XTaskQueueUnregisterMonitor", XTaskQueueUnregisterMonitor_Lua);
}


