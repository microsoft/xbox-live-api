// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "call_buffer_timer.h"
#if !XSAPI_U
#include "ppltasks_extra.h"
#else
#include "ppltasks_extra_unix.h"
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

using namespace xbox::services::system;

struct fire_context
{
    fire_context(
        _In_ std::weak_ptr<call_buffer_timer> _thisWeak,
        _In_ std::shared_ptr<call_buffer_timer_completion_context> _usersAddedStruct,
        _In_ xsapi_internal_vector<xsapi_internal_string> _usersToCall = xsapi_internal_vector<xsapi_internal_string>(),
        _In_ std::chrono::milliseconds _delay = std::chrono::milliseconds(0)
        )
        : thisWeak(std::move(_thisWeak)),
        usersAddedStruct(std::move(_usersAddedStruct)),
        usersToCall(std::move(_usersToCall)),
        delay(std::move(_delay))
    {
    }

    std::weak_ptr<call_buffer_timer> thisWeak;
    std::shared_ptr<call_buffer_timer_completion_context> usersAddedStruct;
    xsapi_internal_vector<xsapi_internal_string> usersToCall;
    std::chrono::milliseconds delay;
};

call_buffer_timer::call_buffer_timer() :
    m_bufferTimePerCall(30),
    m_previousTime(std::chrono::steady_clock::duration::zero()),
    m_isTaskInProgress(false),
    m_queuedTask(false),
    m_createThreads(false)
{
    m_queue = get_xsapi_singleton()->m_asyncQueue;
}

call_buffer_timer::call_buffer_timer(
    _In_ xbox_live_callback<const xsapi_internal_vector<xsapi_internal_string>&, std::shared_ptr<call_buffer_timer_completion_context>> callback,
    _In_ std::chrono::seconds bufferTimePerCall,
    _In_ async_queue_handle_t queue
    ) :
    m_fCallback(std::move(callback)),
    m_bufferTimePerCall(std::move(bufferTimePerCall)),
    m_previousTime(std::chrono::steady_clock::duration::zero()),
    m_isTaskInProgress(false),
    m_queuedTask(false),
    m_queue(queue)
{
    if (m_queue == nullptr)
    {
        m_queue = get_xsapi_singleton()->m_asyncQueue;
    }
}

void
call_buffer_timer::fire()
{
    std::lock_guard<std::mutex> lock(m_timerLock);
    fire_helper();
}

void
call_buffer_timer::fire(
    _In_ const xsapi_internal_vector<xsapi_internal_string>& xboxUserIds,
    _In_ std::shared_ptr<call_buffer_timer_completion_context> usersAddedStruct
)
{
    std::lock_guard<std::mutex> lock(m_timerLock);

    if (xboxUserIds.empty())
    {
        return;
    }

    if (m_usersToCall.capacity() < m_usersToCall.size() + xboxUserIds.size())
    {
        m_usersToCall.reserve((m_usersToCall.size() + xboxUserIds.size()) - m_usersToCall.capacity());
    }
    for (auto& xboxUserId : xboxUserIds)
    {
        if (m_usersToCallMap.find(xboxUserId) == m_usersToCallMap.end())
        {
            m_usersToCall.push_back(xboxUserId);
            m_usersToCallMap[xboxUserId] = true;
        }
    }

    auto context = utils::store_shared_ptr(xsapi_allocate_shared<fire_context>(shared_from_this(), usersAddedStruct));

    AsyncBlock* async = new (xsapi_memory::mem_alloc(sizeof(AsyncBlock))) AsyncBlock{};
    ZeroMemory(async, sizeof(async));
    BeginAsync(async, context, nullptr, __FUNCTION__,
        [](AsyncOp op, const AsyncProviderData* data)
    {
        if (op == AsyncOp_DoWork)
        {
            auto context = static_cast<fire_context*>(data->context);
            std::shared_ptr<call_buffer_timer> pThis(context->thisWeak.lock());
            if (pThis == nullptr)
            {
                return S_OK;
            }

            std::lock_guard<std::mutex> lock(pThis->m_timerLock);
            pThis->fire_helper(context->usersAddedStruct);
            return S_OK;
        }
        else if (op == AsyncOp_Cleanup)
        {
            utils::remove_shared_ptr<fire_context>(data->context);
            xsapi_memory::mem_free(data->async);
        }
        return S_OK;
    });
    ScheduleAsync(async, 0);
}

void
call_buffer_timer::fire_helper(
    _In_ std::shared_ptr<call_buffer_timer_completion_context> usersAddedStruct
)
{
    if (!m_isTaskInProgress)
    {
#if UWP_API || TV_API || UNIT_TEST_SERVICES
        std::chrono::milliseconds timeDiff = m_bufferTimePerCall - std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_previousTime);
        std::chrono::milliseconds timeRemaining = std::max<std::chrono::milliseconds>(std::chrono::milliseconds::zero(), timeDiff);
        auto& usersToCall = m_usersToCall;

        std::weak_ptr<call_buffer_timer> thisWeakPtr = shared_from_this();
        m_isTaskInProgress = true;
        m_previousTime = std::chrono::high_resolution_clock::now();

        auto contextSharedPtr = xsapi_allocate_shared<fire_context>(shared_from_this(), usersAddedStruct, usersToCall);
        contextSharedPtr->delay = timeRemaining;

        AsyncBlock* async = new (xsapi_memory::mem_alloc(sizeof(AsyncBlock))) AsyncBlock{};

        BeginAsync(async, utils::store_shared_ptr(contextSharedPtr), nullptr, __FUNCTION__,
            [](AsyncOp op, const AsyncProviderData* data)
        {
            if (op == AsyncOp_DoWork)
            {
                auto context = utils::remove_shared_ptr<fire_context>(data->context);

                std::shared_ptr<call_buffer_timer> pThis(context->thisWeak.lock());
                if (pThis != nullptr)
                {
                    {
                        std::lock_guard<std::mutex> lock(pThis->m_timerLock);
                        pThis->m_isTaskInProgress = false;
                    }

                    // no lock around this since it is never set after construction and can cause deadlock
                    pThis->m_fCallback(context->usersToCall, context->usersAddedStruct);

                    {
                        std::lock_guard<std::mutex> lock(pThis->m_timerLock);
                        if (pThis->m_queuedTask)
                        {
                            pThis->m_queuedTask = false;
                            pThis->fire_helper();
                        }
                    }
                }
            }
            else if (op == AsyncOp_Cleanup)
            {
                xsapi_memory::mem_free(data->async);
            }
            return S_OK;
        });
        ScheduleAsync(async, static_cast<uint32_t>(contextSharedPtr->delay.count()));
#else
        UNREFERENCED_PARAMETER(usersAddedStruct);
#endif

        m_usersToCall.clear();
        m_usersToCallMap.clear();
    }
    else
    {
        m_queuedTask = true;
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
