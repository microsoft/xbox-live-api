// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi-c/events_c.h"
#include "events_service_xsapi.h"
#include "xbox_live_context_internal.h"
#include <cll/Windows7PartA.h>

using namespace xbox::services;

NAMESPACE_MICROSOFT_XBOX_SERVICES_EVENTS_CPP_BEGIN

EventsService::EventsService(
    _In_ User user,
    _In_ const TaskQueue& queue
) :
    m_user{ std::move(user) },
    m_queue{ queue.DeriveWorkerQueue() }
{
    InitializeTenantSettings();
}

EventsService::~EventsService()
{
    assert(m_eventQueue);
    m_eventQueue->Cleanup();
}

HRESULT EventsService::Initialize()
{
    auto copyUserResult = m_user.Copy();
    RETURN_HR_IF_FAILED(copyUserResult.Hresult());
    m_eventQueue = MakeShared<EventQueue>(copyUserResult.ExtractPayload(), m_tenantSettings);
    m_eventQueue->Initialize();
    return ScheduleUpload();
}

HRESULT EventsService::WriteInGameEvent(
    _In_z_ const char* eventName,
    _In_opt_z_ const char* dimensions,
    _In_opt_z_ const char* measurements
)
{
    JsonDocument dimensionsJson;
    JsonDocument measurementsJson;

    std::regex regex("[A-Za-z]+[A-Za-z0-9_]*");
    bool matchFound = std::regex_match(eventName, regex);
    if (!matchFound)
    {
        LOG_DEBUG("Invalid event name");
        return E_INVALIDARG;
    }

    if (dimensions)
    {
        dimensionsJson.Parse(dimensions);
    }
    if (measurements)
    {
        measurementsJson.Parse(measurements);
    }

    if(dimensionsJson.HasParseError() || measurementsJson.HasParseError())
    {
        LOG_DEBUG("Unable to parse json string");
        return E_INVALIDARG;
    }

    return WriteInGameEventHelper(eventName, dimensionsJson, measurementsJson);
}

HRESULT EventsService::WriteInGameEventHelper(
    _In_ const xsapi_internal_string& eventName,
    _In_ const JsonValue& dimensions,
    _In_ const JsonValue& measurements
)
{
    return m_eventQueue->AddEvent(Event{ m_user.Xuid(), eventName, dimensions, measurements });
}

const std::string& EventsService::IKey()
{
    static std::string iKey;

    if (iKey.empty())
    {
        std::stringstream defaultIKey;
        defaultIKey << "P-XBL-T" << AppConfig::Instance()->TitleId();
        iKey = defaultIKey.str();
    }
    return iKey;
}

HRESULT EventsService::ScheduleUpload() noexcept
{
    auto hr = m_queue.RunWork([ weakThis = std::weak_ptr<EventsService>{ shared_from_this() } ]
        {
            auto state{ GlobalState::Get() };
            auto sharedThis{ weakThis.lock() };

            // Don't schedule another upload if XblCleanup has been called. Otherwise,
            // ensure lifetime of EventsService & GlobalState until the upload is complete
            // to make sure no client data is lost.
            if (sharedThis && state)
            {
                sharedThis->UploadAsync(AsyncContext<>{ [ sharedThis, state ]
                    {
                        // Schedule the next upload
                        sharedThis->ScheduleUpload();
                    }
                    });
            }
        },
        __min(std::pow(2, m_numRetryAttempts), 600) * m_minimumUploadIntervalInMs
    );

    if (FAILED(hr))
    {
        // Not much we can do if RunWork fails so just log the error and return
        LOGS_ERROR << __FUNCTION__ << " failed with HRESULT " << hr;
    }

    return hr;
}

void EventsService::UploadAsync(
    AsyncContext<> async
) noexcept
{
    // If more than m_maximumUploadIntervalInMs has passed since the last upload, begin an upload
    // with whatever events are pending. Otherwise, wait until we have at least m_payloadMinimumEventCount
    // events and return immediately.
    uint64_t timeSinceLastUpload = std::chrono::duration_cast<std::chrono::milliseconds> (chrono_clock_t::now() - m_lastUploadAttempt).count();
    size_t payloadMinumumEventCount = timeSinceLastUpload > m_maximumUploadIntervalInMs ? 1 : m_payloadMinimumEventCount;

    std::shared_ptr<EventUploadPayload> payload = m_eventQueue->GetNextPayload(payloadMinumumEventCount);
    if (!payload)
    {
        return async.Complete();
    }

    HRESULT hr = UploadEventPayload(payload, AsyncContext<HRESULT>{ TaskQueue(),
        [
            sharedThis{ shared_from_this() },
            payload,
            async
        ]
    (HRESULT hr)
        {
            // Super naive implementation for now - on every failed upload switch to offline mode, on every successful upload
            // switch to normal mode. This will incur a lot of disk I/O so this needs to be tuned long term.
            switch (hr)
            {
            //Intentional fallthrough here for certain failure http statuses.
            //Failures that shouldn't be retried are functionally the same as successes,
            //in that we want to reset the backoff process and not requeue the payload.
            case HTTP_E_STATUS_BAD_REQUEST:
                //Retrying a 400 likely won't resolve the issue. Drop the request.
                //TODO: [natiskan] Write telemetry to keep track of failed 400s.
            case S_OK:
                sharedThis->m_numRetryAttempts = 0;
                sharedThis->m_eventQueue->SetMode(Mode::Normal);
                break;
            default:
                sharedThis->OnFailedPayload(payload);
                sharedThis->m_eventQueue->SetMode(Mode::Offline);
                break;
            }

            async.Complete();
        }
    });

    if (FAILED(hr))
    {
        OnFailedPayload(payload);
        return async.Complete();
    }
}

HRESULT EventsService::UploadEventPayload(
    std::shared_ptr<EventUploadPayload> payload,
    AsyncContext<HRESULT> async
)
{
    m_lastUploadAttempt = chrono_clock_t::now();

    return payload->GetRequestData({ async.Queue(),
        [
            weakThis = std::weak_ptr<EventsService>{ shared_from_this() },
            async
        ]
    (Result<const EventUploadPayload::RequestData&> result)
    {
        auto sharedThis{ weakThis.lock() };
        if (sharedThis && Succeeded(result))
        {

            Result<User> userResult = sharedThis->m_user.Copy();
            if (FAILED(userResult.Hresult()))
            {
                return async.Complete(userResult.Hresult());
            }

            auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
            auto hr = httpCall->Init(MakeShared<XboxLiveContextSettings>(), "POST", sharedThis->m_eventUploadUrl, xbox_live_api::events_upload);
            if (FAILED(hr))
            {
                return async.Complete(hr);
            }

            // Don't allow retries. We want to fail as fast as possible and the payload will
            // be retried by the EventsService later anyways.
            httpCall->SetRetryAllowed(false);
            //Explicitly allow retry of 401s with an updated token, as this can only retry once at most
            httpCall->SetAuthRetryAllowed(true);
            httpCall->SetTimeout(sharedThis->m_uploadTimeoutInSeconds);

            for (auto& header : result.Payload().headers)
            {
                httpCall->SetHeader(header.first, header.second);
            }

            httpCall->SetRequestBody(result.Payload().requestBody);

            httpCall->Perform(AsyncContext<HttpResult>{
                async.Queue().GetHandle(),
                [
                    async
                ]
            (HttpResult result)
            {
                HRESULT hr = result.Hresult();
                if (SUCCEEDED(hr))
                {
                    hr = result.Payload()->Result();
                    if (FAILED(hr))
                    {
                        HC_TRACE_INFORMATION(XSAPI, "Event upload failed with HTTP status %u", result.Payload()->HttpStatus());
                    }
                }
                async.Complete(hr);
            }
            });
        }
    }
    });
}

void xbox::services::events::EventsService::OnFailedPayload(std::shared_ptr<EventUploadPayload> failedPayload)
{
    m_eventQueue->RequeueFailedPayload(failedPayload);
    m_numRetryAttempts++;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_EVENTS_CPP_END
