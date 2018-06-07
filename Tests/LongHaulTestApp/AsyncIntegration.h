#pragma once

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
extern std::map<async_queue_handle_t, win32_handle> g_stopRequestedHandles;
extern std::map<async_queue_handle_t, win32_handle> g_workReadyHandles;

HANDLE InitializeAsync(_In_ async_queue_handle_t queue, _Out_ uint32_t* callbackToken);
HANDLE InitializeAsync(_In_ async_queue_handle_t queue, _In_ LPTHREAD_START_ROUTINE thread, _In_ LPVOID threadContext, _Out_ uint32_t* callbackToken);
void CleanupAsync(_In_ async_queue_handle_t queue, _In_ uint32_t callbackToken);
void DrainAsyncCompletionQueueUntilEmpty(_In_ async_queue_handle_t queue);
bool DrainAsyncCompletionQueue(_In_ async_queue_handle_t queue, _In_ uint32_t maxItemsToDrain);
bool DrainAsyncCompletionQueueWithTimeout(_In_ async_queue_handle_t queue, _In_ double stopAfterMilliseconds);