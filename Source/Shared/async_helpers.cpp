// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

TaskQueue::TaskQueue() noexcept
{
    auto state{ GlobalState::Get() };
    if (state)
    {
        *this = state->Queue();
    }
}

TaskQueue::TaskQueue(XTaskQueueHandle handle) noexcept
{
    if (handle)
    {
        auto hr = XTaskQueueDuplicateHandle(handle, &m_handle);
        assert(SUCCEEDED(hr));
        UNREFERENCED_PARAMETER(hr);
    }
}

TaskQueue::TaskQueue(const TaskQueue& other) noexcept
{
    if (other.m_handle)
    {
        auto hr = XTaskQueueDuplicateHandle(other.m_handle, &m_handle);
        assert(SUCCEEDED(hr));
        UNREFERENCED_PARAMETER(hr);
    }
}

TaskQueue::TaskQueue(TaskQueue&& other) noexcept
{
    m_handle = other.m_handle;
    other.m_handle = nullptr;
}

TaskQueue& TaskQueue::operator=(TaskQueue other) noexcept
{
    std::swap(other.m_handle, m_handle);
    return *this;
}

TaskQueue::~TaskQueue() noexcept
{
    if (m_handle)
    {
        XTaskQueueCloseHandle(m_handle);
    }
}

TaskQueue TaskQueue::DeriveWorkerQueue() const noexcept
{
    return DeriveWorkerQueue(m_handle);
}

XTaskQueueHandle TaskQueue::GetHandle() const noexcept
{
    return m_handle;
}

HRESULT TaskQueue::Terminate(
    _In_ bool wait,
    _In_opt_ Callback<> queueTerminatedCallback
) const noexcept
{
    auto context{ MakeUnique<Callback<>>(std::move(queueTerminatedCallback)) };

    HRESULT hr = XTaskQueueTerminate(m_handle, wait, context.get(),
        [](void* context)
    {
        // Be sure to retake ownership of the callback here
        UniquePtr<Callback<>> callback{ reinterpret_cast<Callback<>*>(context) };
        (*callback)();
    });

    RETURN_HR_IF_FAILED(hr);

    context.release();
    return S_OK;
}

HRESULT TaskQueue::RunWork(
    _In_ AsyncWork&& work,
    _In_ uint64_t delayInMs
) const noexcept
{
    return RunOnPort(XTaskQueuePort::Work, std::move(work), delayInMs);
}

HRESULT TaskQueue::RunCompletion(
    _In_ AsyncWork&& work,
    _In_ uint64_t delayInMs
) const noexcept
{
    return RunOnPort(XTaskQueuePort::Completion, std::move(work), delayInMs);
}

HRESULT TaskQueue::RunOnPort(
    _In_ XTaskQueuePort port,
    _In_ AsyncWork&& work,
    _In_ uint64_t delayInMs
) const noexcept
{
    auto context{ MakeUnique<AsyncWork>(std::move(work)) };

    HRESULT hr = XTaskQueueSubmitDelayedCallback(m_handle, port, static_cast<uint32_t>(delayInMs), context.get(),
        [](void* context, bool canceled)
    {
        UniquePtr<AsyncWork> work{ static_cast<AsyncWork*>(context) };
        if (!canceled)
        {
            (*work)();
        }
    });

    RETURN_HR_IF_FAILED(hr);

    context.release();
    return S_OK;
}

TaskQueue TaskQueue::DeriveWorkerQueue(XTaskQueueHandle handle) noexcept
{
    TaskQueue derivedQueue{ nullptr };
    TaskQueue queue{ handle };

    // If handle is null, derive a queue from XSAPI global queue
    if (!queue.m_handle)
    {
        queue = TaskQueue{};
    }

    // If queue is still null, try to derive from the process default queue
    if (!queue.m_handle)
    {
        TaskQueue processQueue{ nullptr };
        bool haveProcessQueue = XTaskQueueGetCurrentProcessTaskQueue(&processQueue.m_handle);
        if (haveProcessQueue)
        {
            queue = processQueue;
        }
    }

    assert(queue.m_handle);

    XTaskQueuePortHandle worker{ nullptr };
    auto hr = XTaskQueueGetPort(queue.m_handle, XTaskQueuePort::Work, &worker);
    assert(SUCCEEDED(hr));
    hr = XTaskQueueCreateComposite(worker, worker, &derivedQueue.m_handle);
    assert(SUCCEEDED(hr));
    UNREFERENCED_PARAMETER(hr);

    return derivedQueue;
}

PeriodicTask::PeriodicTask(
    const TaskQueue& queue,
    uint32_t interval,
    std::function<void()> task
) noexcept
    : m_queue{ queue.DeriveWorkerQueue() },
    m_interval{ interval },
    m_task{ std::move(task) }
{
}

std::shared_ptr<PeriodicTask> PeriodicTask::MakeAndRun(
    const TaskQueue& queue,
    uint32_t interval,
    std::function<void()> task
) noexcept
{
    auto periodicTask = std::shared_ptr<PeriodicTask>(
        new (Alloc(sizeof(PeriodicTask))) PeriodicTask{ queue, interval, std::move(task) },
        Deleter<PeriodicTask>(),
        Allocator<PeriodicTask>()
        );

    periodicTask->m_queue.RunWork([weakThis = std::weak_ptr<PeriodicTask>{ periodicTask }]
        {
            if (auto sharedThis{ weakThis.lock() })
            {
                sharedThis->Run();
            }
        });

    return periodicTask;
}

PeriodicTask::~PeriodicTask() noexcept
{
    m_queue.Terminate(false);
}

HRESULT PeriodicTask::ScheduleImmediately() noexcept
{
    std::lock_guard<std::mutex> lock{ mutex };
    // Every time the task is scheduled manually, skip the next scheduled run
    m_skipCount++;

    return m_queue.RunWork([weakThis = std::weak_ptr<PeriodicTask>{ shared_from_this() }]
        {
            if (auto sharedThis{ weakThis.lock() })
            {
                sharedThis->Run();
            }
        });
}

void PeriodicTask::Run() noexcept
{
    try
    {
        m_task();
    }
    catch (...)
    {
        LOGS_ERROR << __FUNCTION__ << " Failed unexpectedly with exception!";
        assert(false);
    }

    // Schedule the next run
    m_queue.RunWork([this, weakThis = std::weak_ptr<PeriodicTask>{ shared_from_this() }]
        {
            if (auto sharedThis{ weakThis.lock() })
            {
                std::unique_lock<std::mutex> lock{ mutex };
                if (m_skipCount > 0)
                {
                    LOGS_DEBUG << __FUNCTION__ << ": Skipping scheduled PeriodicTask, m_skipCount=" << m_skipCount;
                    --m_skipCount;
                }
                else
                {
                    lock.unlock();
                    Run();
                }
            }
        }, m_interval);
}

struct AsyncProviderContext
{
    AsyncProviderContext(
        AsyncProvider&& _provider,
        const char* _identityName,
        uint64_t _delay
    ) noexcept
        : provider{ std::move(_provider) },
        identityName{ _identityName },
        delay{ _delay }
    {
    }

    AsyncProvider provider;
    const char* identityName;
    uint64_t delay;
    std::weak_ptr<GlobalState> globalState;
};

HRESULT RunAsync(
    XAsyncBlock* async,
    const char* identityName,
    AsyncProvider&& provider,
    uint64_t delayInMs
) noexcept
{
    auto context = MakeUnique<AsyncProviderContext>(std::move(provider), identityName, delayInMs);

    HRESULT hr = XAsyncBegin(async, context.get(), nullptr, identityName,
        [](_In_ XAsyncOp op, _In_ const XAsyncProviderData* data) noexcept
    {
        auto context{ static_cast<AsyncProviderContext*>(data->context) };

        HC_TRACE_VERBOSE(XSAPI, "RunAsync::XAsyncOp::%s: IdentityName=%s", EnumName(op).data(), context->identityName);
        
        switch (op)

        {
        case XAsyncOp::Begin:
            try
            {
                // Should we invoke provider on XAsyncOp::Begin?
                RETURN_HR_IF_FAILED(XAsyncSchedule(data->async, static_cast<uint32_t>(context->delay)));
#if TRACK_ASYNC
                auto state = GlobalState::Get();
                if (state)
                {
                    std::lock_guard<std::mutex> lock{ state->asyncBlocksMutex };
                    state->asyncBlocks[data->async] = context->identityName;
                    context->globalState = state;
                }
                else
                {
                    HC_TRACE_VERBOSE(XSAPI, "XAsync operation running after GlobalState has been destroyed");
                    return E_UNEXPECTED;
                }
#endif
                return S_OK;
            }
            catch (...)
            {
                DISABLE_WARNING_PUSH;
                SUPPRESS_WARNING_UNNAMED_CUSTOM_OBJ;
                LOGS_ERROR << "Unexpected exception in " << __FUNCTION__ << ", completing XAsyncOperation.";
                DISABLE_WARNING_POP;
                return E_UNEXPECTED;
            }
        // For DoWork, GetResult, and Cancel, we have nothing to do. Invoke provider and handle any exceptions.
        case XAsyncOp::DoWork:
        case XAsyncOp::GetResult:
        case XAsyncOp::Cancel:
            try
            {
                return context->provider(op, data);
            }
            catch (...)
            {
                LOGS_ERROR << "Unexpected provider exception in " << __FUNCTION__ << ", completing XAsyncOperation.";
                return E_FAIL;
            }
        case XAsyncOp::Cleanup:
        {
#if TRACK_ASYNC
            if (auto state{ context->globalState.lock() })
            {
                std::lock_guard<std::mutex> lock{ state->asyncBlocksMutex };
                state->asyncBlocks.erase(data->async);
            }
#endif

            // Cleanup should only fail in catostrophic cases. Can't pass result to client 
            // at this point so die with exception.

            HRESULT hr = context->provider(op, data);
            Delete(context);
            return hr;
        }
        default:
        {
            assert(false);
            return S_OK;
        }
        }
    });

    RETURN_HR_IF_FAILED(hr);

    context.release();
    return S_OK;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
