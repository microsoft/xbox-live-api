//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#pragma once
#include <functional>
#include <vector>

namespace xbox { namespace services {

struct call_buffer_timer_completion_context
{
    call_buffer_timer_completion_context() : isNull(true), context(0), numObjects(0) {}
    bool isNull;
    uint32_t context;
    size_t numObjects;
    pplx::task_completion_event<xbox_live_result<void>> tce;
};

class call_buffer_timer : public std::enable_shared_from_this<call_buffer_timer>
{
public:
    call_buffer_timer() {};

    call_buffer_timer(
        std::function<void(const std::vector<string_t>&, const call_buffer_timer_completion_context&)> callback,
        _In_ std::chrono::seconds bufferTimePerCall
        );

    void fire();
    void fire(_In_ const std::vector<string_t>& xboxUserIds, _In_ const call_buffer_timer_completion_context& usersAddedStruct = call_buffer_timer_completion_context());

private:
    void fire_helper(_In_ const call_buffer_timer_completion_context& usersAddedStruct = call_buffer_timer_completion_context());

    bool m_isTaskInProgress;
    bool m_queuedTask;
    const std::chrono::seconds m_bufferTimePerCall;
#if _MSC_VER <= 1800 && !defined XSAPI_I
    std::chrono::system_clock::time_point m_previousTime;
#else
    std::chrono::time_point<std::chrono::steady_clock> m_previousTime;
#endif
    std::vector<string_t> m_usersToCall;
    std::unordered_map<string_t, bool> m_usersToCallMap;    // duplicating data to make lookup faster. SHould be a better way to do this
    std::function<void(const std::vector<string_t>&, const call_buffer_timer_completion_context&)> m_fCallback;
    std::mutex m_timerLock;
};

} }