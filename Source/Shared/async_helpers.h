// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "internal_errors.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

// Memhook aware function class with type erasure
template<typename T>
class Function;

template<typename Ret, typename... Args>
class Function<Ret(Args...)>
{
public:
    Function() noexcept = default;
    ~Function() = default;

    Function(std::nullptr_t) noexcept : m_callable{ nullptr }
    {
    }

    template <typename Functor>
    Function(Functor functor) noexcept
    {
        m_callable = UniquePtr<ICallable>{ MakeUnique<Callable<Functor>>(std::move(functor)).release() };
    }

    Function(const Function& rhs) noexcept
    {
        *this = rhs;
    }

    Function(Function&& rhs) noexcept
    {
        *this = std::move(rhs);
    }

    template <typename Functor>
    Function& operator=(Functor f) noexcept
    {
        m_callable = UniquePtr<ICallable>{ MakeUnique<Callable<Functor>>(std::move(f)).release() };
        return *this;
    }

    Function& operator=(const Function& rhs) noexcept
    {
        if (rhs.m_callable != nullptr)
        {
            m_callable = rhs.m_callable->Copy();
        }
        else
        {
            m_callable.reset();
        }
        return *this;
    }

    Function& operator=(Function&& rhs) noexcept
    {
        m_callable = std::move(rhs.m_callable);
        return *this;
    }

    Function& operator=(std::nullptr_t) noexcept
    {
        m_callable.reset();
        return *this;
    }

    Ret operator()(Args... args) const
    {
        if (m_callable != nullptr)
        {
            return (*m_callable)(args...);
        }
        else
        {
            return Ret();
        }
    }

    bool operator==(std::nullptr_t) const noexcept
    {
        return m_callable == nullptr;
    }

    bool operator!=(std::nullptr_t) const noexcept
    {
        return m_callable != nullptr;
    }

private:
    struct ICallable
    {
        virtual ~ICallable() = default;
        virtual Ret operator()(Args...) = 0;
        virtual UniquePtr<ICallable> Copy() = 0;
    };

    template <typename Functor>
    struct Callable : public ICallable
    {
        Callable(Functor functor) : m_functor{ std::move(functor) }
        {
        }

        ~Callable() override = default;

        Ret operator()(Args... args) override
        {
            return m_functor(args...);
        }

        UniquePtr<ICallable> Copy() override
        {
            return UniquePtr<ICallable>{ MakeUnique<Callable<Functor>>(m_functor).release() };
        }

        Functor m_functor;
    };

    UniquePtr<ICallable> m_callable{ nullptr };
};

template<typename... Args>
using xbox_live_callback = Function<void(Args...)>;

template<typename... Args>
using Callback = Function<void(Args...)>;

using AsyncWork = Function<void(void)>;

// RAII wrapper around XTaskQueueHandle
class TaskQueue
{
public:
    TaskQueue() noexcept;
    TaskQueue(XTaskQueueHandle handle) noexcept;
    TaskQueue(const TaskQueue& other) noexcept;
    TaskQueue(TaskQueue&& other) noexcept;
    TaskQueue& operator=(TaskQueue other) noexcept;
    ~TaskQueue() noexcept;

    TaskQueue DeriveWorkerQueue() const noexcept;
    static TaskQueue DeriveWorkerQueue(XTaskQueueHandle handle) noexcept;

    XTaskQueueHandle GetHandle() const noexcept;

    HRESULT Terminate(
        _In_ bool wait,
        _In_opt_ Callback<> queueTerminatedCallback = nullptr
    ) const noexcept;

    HRESULT RunWork(
        _In_ AsyncWork&& work,
        _In_ uint64_t delayInMs = 0
    ) const noexcept;

    HRESULT RunCompletion(
        _In_ AsyncWork&& work,
        _In_ uint64_t delayInMs = 0
    ) const noexcept;

private:
    HRESULT RunOnPort(
        _In_ XTaskQueuePort port,
        _In_ AsyncWork&& work,
        _In_ uint64_t delayInMs = 0
    ) const noexcept;

    XTaskQueueHandle m_handle{ nullptr };
};

// PeriodicTask class. Periodically runs synchronous work at fixed intervals or when explicitly requested.
// Each time the task is run (manually or otherwise), it will be reschduled. Once started, a periodic
// task will continue to repeat for its lifetime.
class PeriodicTask : public std::enable_shared_from_this<PeriodicTask>
{
public:
    // Create a PeriodTask. The task will be immediately scheduled to the provided queue.
    static std::shared_ptr<PeriodicTask> MakeAndRun(
        const TaskQueue& queue,
        uint32_t interval,
        std::function<void()> task
    ) noexcept;

    PeriodicTask(const PeriodicTask&) = delete;
    PeriodicTask& operator=(PeriodicTask) = delete;
    ~PeriodicTask() noexcept;

    // Schedules task to the queue immediately.
    HRESULT ScheduleImmediately() noexcept;

private:
    PeriodicTask(
        const TaskQueue& queue,
        uint32_t interval,
        std::function<void()> work
    ) noexcept;

    void Run() noexcept;

    TaskQueue m_queue;
    uint32_t const m_interval;
    std::function<void()> const m_task;

    int32_t m_skipCount{ 0 };
    std::mutex mutex;
};

template<typename... Args>
class AsyncContext
{
public:
    AsyncContext() noexcept = default;

    AsyncContext(Function<void(Args...)>&& callback) noexcept
        : m_callback{ callback }
    {
    }

    AsyncContext(TaskQueue queue, Function<void(Args...)>&& callback) noexcept
        : m_queue{ std::move(queue) },
        m_callback{ callback }
    {
    }

    AsyncContext(XTaskQueueHandle queueHandle, Function<void(Args...)>&& callback) noexcept
        : m_queue{ queueHandle },
        m_callback{ callback }
    {
    }

    AsyncContext(TaskQueue queue) noexcept
        : m_queue{ std::move(queue) }
    {
    }

    AsyncContext(XAsyncBlock* asyncBlock) noexcept
    {
#if HC_PLATFORM_IS_MICROSOFT
        // Clang seems to compile this and assert even with no invalid uses of this template.
        // Keep this assert here to catch invalid uses on Microsoft platforms.
        static_assert(false, "Constructor only valid for Args... = <HRESULT>");
#endif
    }

    AsyncContext(const AsyncContext& other) = default;
    AsyncContext(AsyncContext&& other) = default;
    AsyncContext& operator=(const AsyncContext& other) = default;
    ~AsyncContext() = default;

    void Complete(Args... args) const noexcept
    {
        m_callback(args...);
    }

    const TaskQueue& Queue() const noexcept
    {
        return m_queue;
    }

    static AsyncContext<Args...> Collapse(
        Vector<AsyncContext<Args...>> _contexts
    ) noexcept
    {
        return AsyncContext<Args...>(
            [
                contexts{ std::move(_contexts) }
            ]
        (Args... args)
        {
            for (auto& context : contexts)
            {
                context.Complete(args...);
            }
        });
    }

private:
    TaskQueue m_queue{ nullptr };
    Function<void(Args...)> m_callback{ nullptr };
};

template<>
inline AsyncContext<HRESULT>::AsyncContext(XAsyncBlock* asyncBlock) noexcept
    : m_queue{ asyncBlock->queue }
{
    m_callback = [asyncBlock](HRESULT hr)
    {
        XAsyncComplete(asyncBlock, hr, 0);
    };
}

template<>
inline AsyncContext<Result<void>>::AsyncContext(XAsyncBlock* asyncBlock) noexcept
    : m_queue{ asyncBlock->queue }
{
    m_callback = [asyncBlock](Result<void> result)
    {
        XAsyncComplete(asyncBlock, result.Hresult(), 0);
    };
}

typedef Function<HRESULT(XAsyncOp, const XAsyncProviderData*)> AsyncProvider;

// Helper method for writing XAsync Providers.
// AsyncProvider type allows for capture enabled lambdas. XAsyncProvider context lifetime is managed automatically.
HRESULT RunAsync(
    XAsyncBlock* async,
    const char* identityName,
    AsyncProvider&& provider,
    uint64_t delayInMs = 0
) noexcept;

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
