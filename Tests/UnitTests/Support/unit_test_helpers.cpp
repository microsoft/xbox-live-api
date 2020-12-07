// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "UnitTestIncludes.h"
#include "unit_test_helpers.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

// Enable this flag to run UnitTests with a manual dispatch XTaskQueue where all XSAPI background work will
// be dispatched to a single thread.
#define USE_MANUAL_DISPATCH_QUEUE 0

// A simple, single threaded XTaskQueue dispatcher that will validate that XSAPI doesn't submit anything to or
// process anything on its background XTaskQueue after XblCleanup has completed
struct SingleThreadDispatcher : public ITaskQueueDispatcher
{
public:
    SingleThreadDispatcher()
    {
        XTaskQueueHandle queueHandle{ nullptr };
        HRESULT hr = XTaskQueueCreate(XTaskQueueDispatchMode::Manual, XTaskQueueDispatchMode::Manual, &queueHandle);
        assert(SUCCEEDED(hr));
        m_queue = TaskQueue{ queueHandle };

        hr = XTaskQueueRegisterMonitor(queueHandle, this, OnWorkSubmitted, &m_monitorToken);
        assert(SUCCEEDED(hr));

        m_dispatchThread = std::thread([this]
        {
            for (;;)
            {
                if (m_shutdown)
                {
                    constexpr uint32_t finalDispatchTimeout{ 2000 };
                    // After m_shutdown is signaled, wait an addition 2 seconds and ensure nothing is processed.
                    // XblCleanup has completed, so nothing more should be submitted to /processed on m_queue
                    auto processed = XTaskQueueDispatch(m_queue.GetHandle(), XTaskQueuePort::Work, finalDispatchTimeout);
                    UNREFERENCED_PARAMETER(processed);
                    assert(!processed);
                    break;
                }
                else
                {
                    constexpr uint32_t dispatchTimeout{ 10 };
                    XTaskQueueDispatch(m_queue.GetHandle(), XTaskQueuePort::Work, dispatchTimeout);
                }
            }
        });
    }

    ~SingleThreadDispatcher()
    {
        m_shutdown = true;
        m_dispatchThread.join();

        XTaskQueueUnregisterMonitor(m_queue.GetHandle(), m_monitorToken);
    }

    XTaskQueueHandle TaskQueueHandle() const
    {
        return m_queue.GetHandle();
    }

private:
    static void CALLBACK OnWorkSubmitted(void* context, XTaskQueueHandle queue, XTaskQueuePort)
    {
        UNREFERENCED_PARAMETER(context);
        UNREFERENCED_PARAMETER(queue);

        auto pThis{ static_cast<SingleThreadDispatcher*>(context) };
        assert(pThis);
        assert(queue == pThis->m_queue.GetHandle());

        // Nothing should be submitted to the queue after XblCleanup has completed
        assert(!pThis->m_shutdown);
    }

    TaskQueue m_queue;
    std::thread m_dispatchThread;
    std::atomic<bool> m_shutdown{ false };
    XTaskQueueRegistrationToken m_monitorToken{};
};

TestEnvironment::TestEnvironment() noexcept
{
    XblInitArgs args{};
    args.scid = MOCK_SCID;

#if USE_MANUAL_DISPATCH_QUEUE
    auto dispatcher = std::make_shared<SingleThreadDispatcher>();
    args.queue = dispatcher->TaskQueueHandle();
    m_dispatcher = dispatcher;
#endif

    // Use current directory for local storage path
    char pathArray[MAX_PATH + 1];
    GetCurrentDirectoryA(MAX_PATH + 1, pathArray);
    auto pathString = std::string{ pathArray } +'\\';
    args.localStoragePath = pathString.data();

    // Enable debug logging
    HCSettingsSetTraceLevel(HCTraceLevel::Verbose);
    HCTraceSetTraceToDebugger(true);

    VERIFY_SUCCEEDED(XblInitialize(&args));
}

TestEnvironment::TestEnvironment(const XblInitArgs* args)
{
    HCSettingsSetTraceLevel(HCTraceLevel::Verbose);
    HCTraceSetTraceToDebugger(true);

    VERIFY_SUCCEEDED(XblInitialize(args));
}

TestEnvironment::~TestEnvironment() noexcept
{
    XAsyncBlock asyncBlock{};
    VERIFY_SUCCEEDED(XblCleanupAsync(&asyncBlock));
    VERIFY_SUCCEEDED(XAsyncGetStatus(&asyncBlock, true));

    HCTraceSetTraceToDebugger(false);

    // Clean up Mock RTA & websocket handlers
    MockRtaService().SetSubscribeHandler(nullptr);
    system::MockWebsocket::SetConnectHandler(nullptr);
}

std::shared_ptr<XblContext> TestEnvironment::CreateMockXboxLiveContext(
    uint64_t xuid,
    const std::string& gamertag
) const noexcept
{
    auto context = XblContext::Make(std::move(CreateMockUser(xuid, gamertag)));
    VERIFY_SUCCEEDED(context->Initialize(GlobalState::Get()->RTAManager()));
    return context;
}

std::shared_ptr<xbox_live_context> TestEnvironment::CreateLegacyMockXboxLiveContext(
    uint64_t xuid,
    const std::string& gamertag
) const noexcept
{
    auto context = XblContext::Make(std::move(CreateMockUser(xuid, gamertag)));
    VERIFY_SUCCEEDED(context->Initialize(GlobalState::Get()->RTAManager()));
    return std::make_shared<xbox_live_context>(context.get());
}

system::MockRealTimeActivityService& TestEnvironment::MockRtaService() const noexcept
{
    return system::MockRealTimeActivityService::Instance();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END