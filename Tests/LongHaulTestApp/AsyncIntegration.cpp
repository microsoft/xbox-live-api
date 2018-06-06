#include "pch.h"
#include "AsyncIntegration.h"
#include "Utils/PerformanceCounters.h"

class win32_handle
{
public:
    win32_handle() : m_handle(nullptr)
    {
    }

    ~win32_handle()
    {
        if (m_handle != nullptr) CloseHandle(m_handle);
        m_handle = nullptr;
    }

    void set(HANDLE handle)
    {
        m_handle = handle;
    }

    HANDLE get() { return m_handle; }

private:
    HANDLE m_handle;
};


// Forwards
void CALLBACK HandleAsyncQueueCallback(
    _In_ void* context,
    _In_ async_queue_handle_t queue,
    _In_ AsyncQueueCallbackType type);
DWORD WINAPI BackgroundWorkThreadProc(LPVOID lpParam);

win32_handle g_stopRequestedHandle;
win32_handle g_workReadyHandle;
HANDLE g_hBackgroundWorkThread;

void InitializeAsync(_In_ async_queue_handle_t queue, _Out_ uint32_t* callbackToken)
{
    g_stopRequestedHandle.set(CreateEvent(nullptr, true, false, nullptr));
    g_workReadyHandle.set(CreateSemaphore(nullptr, 0, LONG_MAX, nullptr));

    AddAsyncQueueCallbackSubmitted(queue, nullptr, HandleAsyncQueueCallback, callbackToken);

    DuplicateAsyncQueueHandle(queue); // the BackgroundWorkThreadProc will call close
    g_hBackgroundWorkThread = CreateThread(nullptr, 0, BackgroundWorkThreadProc, queue, 0, nullptr);
}

void CleanupAsync(_In_ async_queue_handle_t queue, _In_ uint32_t callbackToken)
{
    RemoveAsyncQueueCallbackSubmitted(queue, callbackToken);
    CloseAsyncQueue(queue);
    SetEvent(g_stopRequestedHandle.get());
}

/// <summary>
/// Call this on the thread you want to dispatch async completions on
/// This will invoke the AsyncBlock's callback completion handler.
/// </summary>
void DrainAsyncCompletionQueueUntilEmpty(
    _In_ async_queue_handle_t queue)
{
    bool found = false;
    do
    {
        found = DispatchAsyncQueue(queue, AsyncQueueCallbackType_Completion, 0);
    } while (found);
}

/// <summary>
/// Call this on the thread you want to dispatch async completions on
/// This will invoke the AsyncBlock's callback completion handler.
///
/// If there's no more completion are ready to dispatch, it will early exit and returns false
/// otherwise it will dispatch up to maxItemsToDrain number of completions and returns true
/// </summary>
bool DrainAsyncCompletionQueue(
    _In_ async_queue_handle_t queue,
    _In_ uint32_t maxItemsToDrain)
{
    bool found = false;
    for( uint32_t i = maxItemsToDrain; i>0; i-- )
    {
        found = DispatchAsyncQueue(queue, AsyncQueueCallbackType_Completion, 0);
        if (!found)
            break;
    }

    return found;
}

/// <summary>
/// Call this on the thread you want to dispatch async completions on
/// This will invoke the AsyncBlock's callback completion handler.
///
/// If there's no more completion are ready to dispatch, it will early exit and returns false
/// otherwise it will dispatch until at least stopAfterMilliseconds has elapsed and returns true
/// </summary>
bool DrainAsyncCompletionQueueWithTimeout(
    _In_ async_queue_handle_t queue,
    _In_ double stopAfterMilliseconds)
{
    std::shared_ptr<performance_capture> timer = performance_counters::get_singleton_instance()->get_capture_instace(L"");
    bool found = false;
    do
    {
        timer->_Start();

        found = DispatchAsyncQueue(queue, AsyncQueueCallbackType_Completion, 0);
        if (!found)
            break;

        timer->_End();
        double timeTaken = timer->max_time();
        stopAfterMilliseconds -= timeTaken;

        if (stopAfterMilliseconds < 0)
            break;

    } while (found);

   return found;
}

/// <summary>
/// This callback will be invoked when background work of an async task is queued, 
/// and will be called from the thread that kicked off the async operation.
/// So it is best practice to not spend much time in this callback.  It is best to signal 
/// another thread to can DispatchAsyncQueue() as shown below in BackgroundWorkThreadProc().
/// 
/// This callback will also be invoked when completion of async task is queued,
/// from a background thread that handled the completion. 
/// In this implementation, we don't process this message as the game thread is calling 
/// DrainAsyncCompletionQueueUntilEmpty as part of of the game update loop.
/// </summary>
void CALLBACK HandleAsyncQueueCallback(
    _In_ void* context,
    _In_ async_queue_handle_t queue,
    _In_ AsyncQueueCallbackType type
    )
{
    UNREFERENCED_PARAMETER(context);
    UNREFERENCED_PARAMETER(queue);

    switch (type)
    {
    case AsyncQueueCallbackType::AsyncQueueCallbackType_Work:
        ReleaseSemaphore(g_workReadyHandle.get(), 1, nullptr);
        break;
    }
}


DWORD WINAPI BackgroundWorkThreadProc(LPVOID lpParam)
{
    HANDLE hEvents[2] =
    {
        g_workReadyHandle.get(),
        g_stopRequestedHandle.get()
    };

    async_queue_handle_t queue = static_cast<async_queue_handle_t>(lpParam);

    bool stop = false;
    while (!stop)
    {
        DWORD dwResult = WaitForMultipleObjectsEx(2, hEvents, false, INFINITE, false);
        switch (dwResult)
        {
        case WAIT_OBJECT_0: 
            // Background work is ready to be dispatched
            DispatchAsyncQueue(queue, AsyncQueueCallbackType_Work, 0);
            break;

        case WAIT_OBJECT_0 + 1:
        default:
            stop = true;
            break;
        }
    }

    CloseAsyncQueue(queue);
    return 0;
}

