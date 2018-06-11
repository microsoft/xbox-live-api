// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/presence.h"
#include "xsapi/services.h"
#include "user_context.h"
#include "presence_internal.h"
#ifdef  __cplusplus_winrt
#include "Utils_WinRT.h"
#include "utils.h"

using namespace Windows::System::Threading;
#endif

#define PRESENCE_DELAY_IN_MILLISECONDS 60 * 1000

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_BEGIN

std::shared_ptr<presence_writer> 
presence_writer::get_presence_writer_singleton()
{
    return get_xsapi_singleton()->m_presenceWriterSingleton;
}

presence_writer::presence_writer() :
    m_writerInProgress(false),
    m_heartBeatDelayInMins(0),
    m_isCallInProgress(false)
{
}

#if UWP_API || UNIT_TEST_SERVICES
void presence_writer::start_timer(_In_ std::weak_ptr<presence_writer> thisWeakPtr)
{
    Windows::Foundation::TimeSpan delay = Microsoft::Xbox::Services::System::UtilsWinRT::ConvertSecondsToTimeSpan<std::chrono::seconds>(std::chrono::minutes(1));
    m_timer = ThreadPoolTimer::CreatePeriodicTimer(
        ref new TimerElapsedHandler([thisWeakPtr](ThreadPoolTimer^ source)
        {
            std::shared_ptr<presence_writer> pThis(thisWeakPtr.lock());
            if (pThis != nullptr)
            {
                pThis->handle_timer_trigger();
            }
        }),
        delay
    );
}
#else
void
presence_writer::start_timer(
    _In_ std::weak_ptr<presence_writer> thisWeakPtr)
{
    m_timerComplete = false;
    while (true)
    {
        if (m_timerComplete)
        {
            break;
        }
        int delayInMilliseconds = 60 * 1000; // call handle_timer_trigger() every minute
        utils::sleep(delayInMilliseconds);
        if (m_timerComplete)
        {
            break;
        }
        // Do write in the end so that it has the same logic as winrt timer
        handle_timer_trigger();
    }
}
#endif

void
presence_writer::start_writer(
    _In_ std::shared_ptr<presence_service_internal> presenceServiceImpl
    )
{
    bool startWriter = false;

    {
        std::lock_guard<std::mutex> guard(m_lock.get());
        if (!m_writerInProgress)
        {
            m_writerInProgress = true;
            startWriter = true;
        }

        xsapi_internal_string id = presenceServiceImpl->m_userContext->xbox_user_id();
        if (m_presenceServices.find(id) == m_presenceServices.end())
        {
            LOG_INFO("Add new presence service into writer");
            m_presenceServices.insert(std::make_pair(id, presenceServiceImpl));

            // Skip the first presence write, as we already did it as a part of sign in.
        }
        else
        {
            LOG_INFO("Presence service for the user already exsit, return");
            return;
        }
    }

    if (startWriter)
    {
        // Start timer that will write presence after certain time.
        std::weak_ptr<presence_writer> thisWeakPtr = shared_from_this();
        pplx::create_task([thisWeakPtr]()
        {
            std::shared_ptr<presence_writer> pThis(thisWeakPtr.lock());
            if (pThis != nullptr)
            {
                pThis->start_timer(thisWeakPtr);
            }
        });
    }
}

void
presence_writer::handle_timer_trigger()
{
    LOG_INFO("Start presence writer timer trigger");
    m_heartBeatDelayInMins--;
    if (m_heartBeatDelayInMins > 0)
    {
        return;
    }

    set_active_in_title();
}

void 
presence_writer::stop_writer(
    _In_ const xsapi_internal_string& xboxLiveUserId
    )
{
    std::lock_guard<std::mutex> guard(m_lock.get());
    if (m_writerInProgress)
    {
        auto presenceService = m_presenceServices.find(xboxLiveUserId);
        if (presenceService != m_presenceServices.end())
        {
            set_inactive_in_title(presenceService->second);
            m_presenceServices.erase(presenceService);
        }

        if (m_presenceServices.empty())
        {
            m_writerInProgress = false;
            m_heartBeatDelayInMins = 0;
        }

#ifdef __cplusplus_winrt
        if (m_timer)
        {
            m_timer->Cancel();
        }
#else
        m_timerComplete = true;
#endif
    }
}

void 
presence_writer::set_inactive_in_title(
    _In_ std::shared_ptr<presence_service_internal> presenceServiceImpl
    )
{
    // Set the presence to be not in this title
    try
    {
        presenceServiceImpl->set_presence(false, presence_data_internal(), get_xsapi_singleton()->m_asyncQueue, nullptr);
    }
    catch (...)
    {
        LOG_ERROR("Set presence inactive fail");
    }
}

struct set_active_in_title_context
{
    set_active_in_title_context(uint32_t _writeTaskCount) : writeTaskCount(_writeTaskCount), writeTasksComplete(0) {}

    const uint32_t writeTaskCount;
    std::atomic<uint32_t> writeTasksComplete;
};

void 
presence_writer::set_active_in_title()
{
    bool expected = false;
    if (m_isCallInProgress.compare_exchange_strong(expected, true))
    {
        LOG_INFO("Start presence writing.");

        std::lock_guard<std::mutex> guard(m_lock.get());

        auto context = xsapi_allocate_shared<set_active_in_title_context>(static_cast<uint32_t>(m_presenceServices.size()));
        std::weak_ptr<presence_writer> thisWeakPtr = shared_from_this();

        for (auto& presencePair : m_presenceServices)
        {
            auto& presenceService = presencePair.second;
            if (presenceService != nullptr)
            {
                presenceService->set_presence(
                    true,
                    presence_data_internal(), 
                    get_xsapi_singleton()->m_asyncQueue,
                    [context, thisWeakPtr](xbox_live_result<uint32_t> result)
                {
                    // only look at the last result
                    context->writeTasksComplete++;
                    if ((++context->writeTasksComplete) >= context->writeTaskCount)
                    {
                        std::shared_ptr<presence_writer> pThis(thisWeakPtr.lock());

                        pThis->m_isCallInProgress.store(false);
                        LOG_INFO("Presence writing finish.");

                        if (!result.err())
                        {
                            pThis->m_heartBeatDelayInMins = result.payload();
                        }
                        else
                        {
                            LOGS_ERROR << "Error detected on presence writing, using default interval for next write:"
                                << result.err() << ", msg:" << result.err_message();
                            // Ignore failures
                            pThis->m_heartBeatDelayInMins = s_defaultHeartBeatDelayInMins;
                        }
                    }
                });
            }
        }
    }
    else
    {
        LOG_INFO("Writting in progress, skip presence writing.");
    }

}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_END
