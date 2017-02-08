//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#include "call_buffer_timer.h"
#if !XSAPI_U
#include "ppltasks_extra.h"
#else
#include "ppltasks_extra_unix.h"
#endif

using namespace Concurrency::extras;

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

call_buffer_timer::call_buffer_timer() :
    m_bufferTimePerCall(30),
    m_previousTime(std::chrono::steady_clock::duration::zero()),
    m_isTaskInProgress(false),
    m_queuedTask(false)
{
}

call_buffer_timer::call_buffer_timer(
    _In_ std::function<void(const std::vector<string_t>&, const call_buffer_timer_completion_context&)> callback,
    _In_ std::chrono::seconds bufferTimePerCall
    ) :
    m_fCallback(std::move(callback)),
    m_bufferTimePerCall(std::move(bufferTimePerCall)),
    m_previousTime(std::chrono::steady_clock::duration::zero()),
    m_isTaskInProgress(false),
    m_queuedTask(false)
{
}

void
call_buffer_timer::fire()
{
    fire_helper();
}

void
call_buffer_timer::fire(
    _In_ const std::vector<string_t>& xboxUserIds,
    _In_ const call_buffer_timer_completion_context& usersAddedStruct
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

    std::weak_ptr<call_buffer_timer> thisWeak = shared_from_this();

    pplx::create_task([thisWeak, usersAddedStruct]()
    {
        std::shared_ptr<call_buffer_timer> pThis(thisWeak.lock());
        if (pThis == nullptr)
        {
            return;
        }

        std::lock_guard<std::mutex> lock(pThis->m_timerLock);
        pThis->fire_helper(usersAddedStruct);
    });
}

void
call_buffer_timer::fire_helper(
    _In_ const call_buffer_timer_completion_context& usersAddedStruct
)
{
    if (!m_isTaskInProgress)
    {
        std::chrono::milliseconds timeDiff = m_bufferTimePerCall - std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_previousTime);
        std::chrono::milliseconds timeRemaining = std::max<std::chrono::milliseconds>(std::chrono::milliseconds::zero(), timeDiff);
        auto& usersToCall = m_usersToCall;

        std::weak_ptr<call_buffer_timer> thisWeakPtr = shared_from_this();
        m_isTaskInProgress = true;
        m_previousTime = std::chrono::high_resolution_clock::now();
        create_delayed_task(
            timeRemaining,
            [thisWeakPtr, usersToCall, usersAddedStruct]()
        {
            std::shared_ptr<call_buffer_timer> pThis(thisWeakPtr.lock());
            if (pThis != nullptr)
            {
                std::lock_guard<std::mutex> lock(pThis->m_timerLock);
                pThis->m_isTaskInProgress = false;
                pThis->m_fCallback(usersToCall, usersAddedStruct);

                if (pThis->m_queuedTask)
                {
                    pThis->m_queuedTask = false;
                    pThis->fire_helper();
                }
            }
        });

        m_usersToCall.clear();
        m_usersToCallMap.clear();
    }
    else
    {
        m_queuedTask = true;
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
