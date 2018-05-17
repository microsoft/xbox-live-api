// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include <functional>
#include <vector>

namespace xbox { namespace services {

struct call_buffer_timer_completion_context
{
    call_buffer_timer_completion_context(
        _In_ uint32_t _context,
        _In_ size_t _numObjects,
        _In_ xbox_live_callback<xbox_live_result<void>> _callback = xbox_live_callback<xbox_live_result<void>>(nullptr)
        )
        : context(_context), numObjects(_numObjects), callback(std::move(_callback))
    {
    }
    uint32_t context;
    size_t numObjects;
    xbox_live_callback<xbox_live_result<void>> callback;
};

class call_buffer_timer : public std::enable_shared_from_this<call_buffer_timer>
{
public:
    call_buffer_timer();

    call_buffer_timer(
        _In_ xbox_live_callback<const xsapi_internal_vector<xsapi_internal_string>&, std::shared_ptr<call_buffer_timer_completion_context>> callback,
        _In_ std::chrono::seconds bufferTimePerCall,
        _In_opt_ async_queue_handle_t queue = nullptr
        );

    void fire();

    void fire(
        _In_ const xsapi_internal_vector<xsapi_internal_string>& xboxUserIds,
        _In_ std::shared_ptr<call_buffer_timer_completion_context> usersAddedStruct = nullptr
        );

private:
    void fire_helper(
        _In_ std::shared_ptr<call_buffer_timer_completion_context> usersAddedStruct = nullptr
        );

    bool m_isTaskInProgress;
    bool m_queuedTask;
    const std::chrono::seconds m_bufferTimePerCall;
#if _MSC_VER <= 1800 && !defined XSAPI_I
    std::chrono::system_clock::time_point m_previousTime;
#else
    std::chrono::time_point<std::chrono::steady_clock> m_previousTime;
#endif
    xsapi_internal_vector<xsapi_internal_string> m_usersToCall;
    xsapi_internal_unordered_map<xsapi_internal_string, bool> m_usersToCallMap;    // duplicating data to make lookup faster. SHould be a better way to do this
    xbox_live_callback<const xsapi_internal_vector<xsapi_internal_string>&, std::shared_ptr<call_buffer_timer_completion_context>> m_fCallback;
    std::mutex m_timerLock;
    async_queue_handle_t m_queue;
};

} }
