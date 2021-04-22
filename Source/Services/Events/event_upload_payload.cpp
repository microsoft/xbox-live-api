// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "events_service_xsapi.h"
#include "cll/ExceptionCodes.h"

#define DEVICE_TICKET_ID    "1"
#define USER_TICKET_ID      "2"

using namespace xbox::services::legacy;

NAMESPACE_MICROSOFT_XBOX_SERVICES_EVENTS_CPP_BEGIN

EventUploadPayload::EventUploadPayload(
    _In_ User&& user,
    _In_ std::shared_ptr<cll::CllTenantSettings> tenantSettings
) :
    m_user{ std::move(user) },
    m_tenantSettings{ tenantSettings }
{
}

HRESULT EventUploadPayload::AddEvent(_In_ const Event& event)
{
    if (m_events.size() >= static_cast<size_t>(m_tenantSettings->getMaxEventsPerPost()))
    {
        LOGS_DEBUG << "Cannot add more events to payload.";
        return E_FAIL;
    }

    m_events.push_back(event);
    return S_OK;
}

size_t EventUploadPayload::EventCount() const
{
    return m_events.size();
}

HRESULT EventUploadPayload::GetRequestData(
    _In_ AsyncContext<Result<const RequestData&>> async
)
{
    if (!m_requestData.requestBody.empty())
    {
        async.Complete(m_requestData);
        return S_OK;
    }

    return CreateTicketData({ async.Queue(),
        [
            sharedThis{ shared_from_this() },
            async
        ]
    (Result<std::vector<cll::TicketData>> result)
    {
        if (Succeeded(result))
        {
            cll::CllUploadRequestData cllRequestData;
            for (auto& event : sharedThis->m_events)
            {
                cll::CllEvent cllEvent;
                sharedThis->m_tenantSettings->populateEvent(
                    cllEvent,
                    EventsService::IKey(),
                    event.FullEventName(),
                    event.Data(),
                    event.Timestamp().to_string(xbox::services::datetime::date_format::ISO_8601).c_str(),
                    cll::LatencyNormal,
                    cll::PersistenceUnspecified,
                    cll::SensitivityUnspecified,
                    cll::SampleRate_Unspecified,
                    result.Payload(),
                    cll::CorrelationVector{}
                );

                sharedThis->m_tenantSettings->addEventToRequest(cllEvent, cllRequestData);
            }

            sharedThis->m_requestData.requestBody = cllRequestData.getRequestBody().data();
            for (const auto& header : cllRequestData.getHeaders())
            {
                sharedThis->m_requestData.headers[header.first.data()] = header.second.data();
            }
        }

        async.Complete(Result<const RequestData&>{ sharedThis->m_requestData, result.Hresult() });
    }
    });
}

size_t EventUploadPayload::ExtractEventsAndSerialize(
    _Inout_ xsapi_internal_vector<uint8_t>& serializedData,
    _In_ size_t targetDataSize,
    _Inout_ xbox::services::datetime& timestamp
)
{
    for (auto iter = m_events.begin(); iter != m_events.end() && serializedData.size() < targetDataSize; iter = m_events.erase(iter))
    {
        auto& event{ *iter };

        auto serializedEvent = event.Serialize();
        serializedData.insert(serializedData.end(), serializedEvent.begin(), serializedEvent.end());
        serializedData.push_back('\n');

        if (event.Timestamp() < timestamp)
        {
            timestamp = event.Timestamp();
        }
    }

    return m_events.size();
}

HRESULT EventUploadPayload::CreateTicketData(
    _In_ AsyncContext<Result<std::vector<cll::TicketData>>> async
)
{
    m_user.GetTokenAndSignature("POST", "https://vortex-win.data.microsoft.com", {}, nullptr, 0, false, { async.Queue(),
        [
            async,
            sharedThis{ shared_from_this() }
        ]
    (Result<TokenAndSignature> result)
    {
        if (Failed(result))
        {
            return async.Complete(result.Hresult());
        }
        
        cll::TicketData deviceTicketData {
            cll::TicketType::TicketTypeXauthDevice,
            DEVICE_TICKET_ID,
            result.Payload().token.data()
        };

        sharedThis->m_user.GetTokenAndSignature("POST", "https://vortex-events.xboxlive.com", {}, nullptr, 0, false, { async.Queue(),
            [
                async,
                deviceTicketData
            ]
        (Result<TokenAndSignature> result)
        {
            if (Failed(result))
            {
                return async.Complete(result.Hresult());
            }

            cll::TicketData userTicketData{
                cll::TicketType::TicketTypeXauthUser,
                USER_TICKET_ID,
                result.Payload().token.data()
            };

            return async.Complete(std::vector<cll::TicketData>{ deviceTicketData, userTicketData });
        }
        });
    }
    });

    return S_OK;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_EVENTS_CPP_END