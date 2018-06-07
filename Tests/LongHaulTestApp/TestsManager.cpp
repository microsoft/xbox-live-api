#include "pch.h"
#include "Tests.h"
#include "AsyncIntegration.h"

#define TEST_THREAD_ID 1

#define NUM_OF_TEST_THREADS 2
#define NUM_OF_BACKGROUND_THREADS_PER_GAME_THREAD 4

void TestsManager::StartTests(xbl_context_handle xboxContext)
{
    CreateSharedAsyncQueue(
        TEST_THREAD_ID,
        AsyncQueueDispatchMode::AsyncQueueDispatchMode_Manual,
        AsyncQueueDispatchMode::AsyncQueueDispatchMode_Manual,
        &m_queue);

    m_context = new ThreadContext{ xboxContext, m_queue };

    for (size_t i = 0; i < NUM_OF_BACKGROUND_THREADS_PER_GAME_THREAD; i++)
    {
        InitializeAsync(m_queue, BackgroundWorkThreadProc, m_context, &m_asyncQueueCallbackToken);
    }

    for (size_t i = 0; i < NUM_OF_TEST_THREADS; i++)
    {
        InitializeAsync(m_queue, TestsThreadProc, m_context, &m_asyncQueueCallbackToken);
    }

    CreateSharedAsyncQueue(
        TEST_THREAD_ID + 1,
        AsyncQueueDispatchMode::AsyncQueueDispatchMode_Manual,
        AsyncQueueDispatchMode::AsyncQueueDispatchMode_Manual,
        &m_queue2);

    InitializeAsync(m_queue2, BackgroundWorkThreadProc, m_context, &m_asyncQueueCallbackToken);
    InitializeAsync(m_queue2, TestsThreadProc, m_context, &m_asyncQueueCallbackToken);
}

bool g_runSocialManagerTests = true;
DWORD WINAPI TestsManager::TestsThreadProc(LPVOID lpParam)
{
    auto context = static_cast<ThreadContext*>(lpParam);

    HANDLE handles[2] =
    {
        g_workReadyHandles[context->queue].get(),
        g_stopRequestedHandles[context->queue].get()
    };

    bool runSocialManagerTests = g_runSocialManagerTests;
    g_runSocialManagerTests = false;

    Tests tests(context->xblContext, context->queue, runSocialManagerTests);

    bool stop = false;
    while (!stop)
    {
        try
        {
            tests.HandleTests();
        }
        catch (const std::exception& e)
        {
            tests.Log("[Error] " + std::string(e.what()));
        }

        DrainAsyncCompletionQueue(context->queue, 1);
        
        stop = WaitForSingleObject(g_stopRequestedHandles[context->queue].get(), 0) == WAIT_OBJECT_0;
    }

    CloseAsyncQueue(context->queue);
    return 0;
}
   
DWORD WINAPI TestsManager::BackgroundWorkThreadProc(LPVOID lpParam)
{
    auto context = static_cast<ThreadContext*>(lpParam);

    HANDLE handles[2] =
    {
        g_workReadyHandles[context->queue].get(),
        g_stopRequestedHandles[context->queue].get()
    };

    bool stop = false;
    while (!stop)
    {
        DWORD dwResult = WaitForMultipleObjectsEx(2, handles, false, INFINITE, false);
        switch (dwResult)
        {
        case WAIT_OBJECT_0:
            // Background work is ready to be dispatched
            DispatchAsyncQueue(context->queue, AsyncQueueCallbackType_Work, 0);
            break;

        case WAIT_OBJECT_0 + 1:
        default:
            stop = true;
            break;
        }
    }

    CloseAsyncQueue(context->queue);
    return 0;
}